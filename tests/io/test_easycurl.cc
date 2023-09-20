/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <array>
#include <cstdlib>
#include <string>
#include <thread>

#include "eckit/io/EasyCURL.h"
#include "eckit/value/Value.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

/*
 *
 * The following tests take advantage of the Python-based Mock REST API -- see mock/MockREST.py.
 * The Mock API is launched during test setup and serves as target for EasyCURL GET/POST/PUT/DELETE calls.
 *
 */

namespace {

const std::string BASE_URL = "http://127.0.0.1:49111";

class MockREST {
public:
    MockREST() {
        // Launch REST API in a (detached) thread
        std::thread api(MockREST::launch_rest_api);
        api.detach();

        // ... give the service some lead time
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // ... and way for the service initialization
        MockREST::wait_for_running_api();
    }

    ~MockREST() {
        // Gracefully, request REST API to shut down
        MockREST::shutdown_running_api();
    }

private:
    static void launch_rest_api() {
        std::system("MockREST.launcher.sh");
    }

    static void wait_for_running_api() {
        auto curl = EasyCURL();
        curl.verbose(true);
        for (size_t attempt = 0; attempt < 30; ++attempt) {
            try {
                auto response = curl.GET(BASE_URL + "/ping");
                if (response.code() == 200) {
                    // Got a ping! We're done...
                    return;
                }
            }
            catch (const SeriousBug& error) {
                // Unable to connect to server! Must retry...
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Enough! Let's give up...
        throw std::runtime_error("Unable to start Mock REST API");
    }

    static void shutdown_running_api() {
        auto curl = EasyCURL();
        try {
            auto response = curl.GET(BASE_URL + "/shutdown");
        }
        catch (const SeriousBug& error) {
            // Nothing to do...
        }
    }
};

namespace impl {

template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> make_array(T (&a)[N], std::index_sequence<I...>) {
    return {{std::move(a[I])...}};
}

template <typename T, size_t N>
std::array<T, N> make_array(const T (&values)[N]) {
    return make_array(values, std::make_index_sequence<N>{});
}

}  // namespace impl

std::string make_random_token(size_t length) {
    auto randomize = []() -> char {
        static std::array selected = impl::make_array("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
        return selected[rand() % (selected.size() - 1)];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randomize);
    return str;
}

std::string make_blob(std::string_view key, std::string_view value) {
    std::ostringstream oss;
    oss << R"({"key": ")" << key << R"(", "value": ")" << value << R"("})";
    return oss.str();
}

}  // namespace

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("EasyCURL GET (Successful operation)") {
    auto curl = EasyCURL();

    auto response = curl.GET(BASE_URL + "/ping");
    EXPECT(response.code() == 200);
    EXPECT(response.body() == "Hi!");
}

CASE("EasyCURL GET (Not Found)") {
    auto curl = EasyCURL();

    auto response = curl.GET(BASE_URL + "/ping__NOT-FOUND");
    EXPECT(response.code() == 404);
    EXPECT(response.body().find("Not Found") != std::string::npos);
}

CASE("EasyCURL GET (REST API, Successful operation)") {
    auto curl = EasyCURL();

    auto response = curl.GET(BASE_URL + "/blobs");
    EXPECT(response.code() == 200);
    EXPECT(response.body() == "{}\n");
}

CASE("EasyCURL GET (REST API, Insufficient permissions)") {
    auto curl     = EasyCURL();
    auto response = curl.GET(BASE_URL + "/insufficient-permissions");
    EXPECT(response.code() == 403);
    EXPECT(response.body() == "Forbidden");
}

CASE("EasyCURL PUT (REST API, Create+Update+Delete new entity)") {
    EasyCURLHeaders headers;
    headers["content-type"] = "application/json";

    auto curl = EasyCURL();
    curl.headers(headers);

    std::string key            = make_random_token(8);
    std::string original_value = make_random_token(64);
    {
        auto blob     = make_blob(key, original_value);
        auto response = curl.POST(BASE_URL + "/blob", blob);
        EXPECT(response.code() == 201);
        EXPECT(response.body().find(original_value) != std::string::npos);
    }
    {
        auto response = curl.GET(BASE_URL + "/blob/" + key + "/content");
        EXPECT(response.code() == 200);
        EXPECT(response.body() == original_value);
    }
    std::string updated_value = make_random_token(64);
    {
        auto blob     = make_blob(key, updated_value);
        auto response = curl.PUT(BASE_URL + "/blob", blob);
        EXPECT(response.code() == 200);
        EXPECT(response.body().find(updated_value) != std::string::npos);
    }
    {
        auto response = curl.GET(BASE_URL + "/blob/" + key + "/content");
        EXPECT(response.code() == 200);
        EXPECT(response.body() == updated_value);
    }
    {
        auto response = curl.DELETE(BASE_URL + "/blob/" + key);
        EXPECT(response.code() == 204);
        EXPECT(response.body().empty());
    }
    {
        auto response = curl.GET(BASE_URL + "/blob/" + key + "/content");
        EXPECT(response.code() == 404);
        EXPECT(response.body().find("Not Found") != std::string::npos);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    MockREST mock_rest_api_running_in_background;

    return run_tests(argc, argv);
}
