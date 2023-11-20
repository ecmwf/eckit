/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <bitset>
#include <cstring>

#include "eckit/codec/Data.h"
#include "eckit/codec/codec.h"

#include "eckit/testing/Test.h"

namespace eckit::test {

// -------------------------------------------------------------------------------------------------------

struct UnencodableType {
    std::string _;
};

// -------------------------------------------------------------------------------------------------------

// Example type that can be encoded / decoded with eckit::codec
// The "operations" performed on this type are stored for unit-test purposes
class EncodableType {
public:
    using Operations = std::vector<std::string>;

    explicit EncodableType(std::string s, std::shared_ptr<Operations> operations = std::make_shared<Operations>()) :
        str(s), ops(operations) {
        ops->emplace_back("constructor");
    }

    EncodableType() :
        ops(std::make_shared<Operations>()) {}

    EncodableType(const EncodableType& other) :
        str(other.str), ops(other.ops) {
        // This constructor should not be called.
        ops->emplace_back("copy constructor");
    }

    EncodableType(EncodableType&& other) :
        str(std::move(other.str)), ops(other.ops) {
        ops->emplace_back("move constructor");
    }


    EncodableType& operator=(const EncodableType& rhs) {
        // This assignment should not be called.
        str = rhs.str;
        ops = rhs.ops;
        ops->emplace_back("assignment");
        return *this;
    }

    EncodableType& operator=(EncodableType&& rhs) {
        // This assignment should not be called.
        str = std::move(rhs.str);
        ops = rhs.ops;
        ops->emplace_back("move");
        return *this;
    }


    friend void encode_data(const EncodableType& in, codec::Data& out) {
        in.ops->emplace_back("encode_data");
        out.assign(in.str.data(), in.str.size());
    }

    friend size_t encode_metadata(const EncodableType& in, codec::Metadata& metadata) {
        in.ops->emplace_back("encode_metadata");
        metadata.set("type", "EncodableType");
        metadata.set("bytes", in.str.size());
        return in.str.size();
    }

    friend void decode(const codec::Metadata&, const codec::Data& b, EncodableType& out) {
        out.ops->emplace_back("decode");
        const auto* data = static_cast<const char*>(b.data());
        out.str          = std::string(data, data + b.size());
    }

    const std::vector<std::string>& operations() const { return *ops; }

private:
    std::string str;
    mutable std::shared_ptr<std::vector<std::string>> ops;
};

// -------------------------------------------------------------------------------------------------------

CASE("test exceptions") {
    EXPECT(not(codec::is_interpretable<UnencodableType, codec::ArrayReference>()));
    EXPECT(not codec::is_encodable<UnencodableType>());
    EXPECT(not codec::can_encode_metadata<UnencodableType>());
    EXPECT(not codec::can_encode_data<UnencodableType>());

    UnencodableType in;
    codec::Data data;
    codec::Metadata metadata;

    EXPECT_THROWS_AS(codec::ref(in, codec::tag::disable_static_assert()), codec::NotEncodable);
    EXPECT_THROWS_AS(codec::copy(in, codec::tag::disable_static_assert()), codec::NotEncodable);
    EXPECT_THROWS_AS(codec::encode(in, metadata, data, codec::tag::disable_static_assert()), codec::NotEncodable);
}

// -------------------------------------------------------------------------------------------------------

CASE("encoding test::EncodableType") {
    static_assert(not codec::is_interpretable<EncodableType, codec::ArrayReference>());
    static_assert(codec::is_encodable<EncodableType>());
    static_assert(codec::is_decodable<EncodableType>());

    const std::string encoded_string{"encoded string"};
    EncodableType in(encoded_string);
    codec::Data data;
    codec::Metadata metadata;
    EXPECT_NO_THROW(encode(in, metadata, data));

    EXPECT(metadata.type() == "EncodableType");
    EXPECT(data.size() == encoded_string.size());
    EXPECT(::memcmp(data, encoded_string.data(), encoded_string.size()) == 0);
}


// -------------------------------------------------------------------------------------------------------

CASE("encoding codec::types::ArrayView") {
    static_assert(not codec::is_interpretable<codec::ArrayReference, codec::ArrayReference>());
    static_assert(codec::can_encode_data<codec::ArrayReference>());
    static_assert(codec::can_encode_metadata<codec::ArrayReference>());
    static_assert(codec::is_encodable<codec::ArrayReference>());
}

// -------------------------------------------------------------------------------------------------------

template <typename T>
void assert_StdVector() {
    static_assert(codec::is_interpretable<std::vector<T>, codec::ArrayReference>());
    static_assert(not codec::can_encode_data<std::vector<T>>());
    static_assert(not codec::can_encode_metadata<std::vector<T>>());
    static_assert(not codec::is_encodable<std::vector<T>>());
}

template <typename T>
void encode_StdVector() {
    std::vector<T> in{1, 2, 3, 4, 5};

    codec::ArrayReference interpreted;
    interprete(in, interpreted);

    codec::Data data;
    codec::Metadata metadata;

    encode(interpreted, metadata, data);

    EXPECT(data.size() == in.size() * sizeof(T));
    EXPECT(::memcmp(in.data(), data.data(), data.size()) == 0);
    EXPECT(metadata.type() == "array");
    EXPECT(metadata.getString("datatype") == codec::DataType::str<T>());
}

CASE("encoding std::vector") {
    assert_StdVector<int>();
    assert_StdVector<float>();
    assert_StdVector<double>();
    assert_StdVector<long>();
    assert_StdVector<std::byte>();

    encode_StdVector<int>();
    encode_StdVector<float>();
    encode_StdVector<double>();
    encode_StdVector<long>();

    {
        using T = std::byte;
        std::bitset<8> bits;
        std::vector<T> in;
        in.resize(5);
        size_t n{0};
        for (auto& byte : in) {
            bits.set(n++, true);
            byte = *reinterpret_cast<std::byte*>(&bits);
        }
        codec::ArrayReference interpreted;
        interprete(in, interpreted);

        codec::Data data;
        codec::Metadata metadata;

        encode(interpreted, metadata, data);

        EXPECT(data.size() == in.size() * sizeof(T));
        EXPECT(::memcmp(in.data(), data.data(), data.size()) == 0);
        EXPECT(metadata.type() == "array");
        EXPECT(metadata.getString("datatype") == codec::DataType::str<T>());
    }
}

// -------------------------------------------------------------------------------------------------------


template <typename T>
void assert_StdArray() {
    static_assert(codec::is_interpretable<std::array<T, 5>, codec::ArrayReference>());
    static_assert(not codec::can_encode_data<std::array<T, 5>>());
    static_assert(not codec::can_encode_metadata<std::array<T, 5>>());
    static_assert(not codec::is_encodable<std::array<T, 5>>());
}

template <typename T>
void encode_StdArray() {
    std::array<T, 5> in{1, 2, 3, 4, 5};

    codec::ArrayReference interpreted;
    interprete(in, interpreted);

    codec::Data data;
    codec::Metadata metadata;

    encode(interpreted, metadata, data);

    EXPECT(data.size() == in.size() * sizeof(T));
    EXPECT(::memcmp(in.data(), data.data(), data.size()) == 0);
    EXPECT(metadata.type() == "array");
    EXPECT(metadata.getString("datatype") == codec::DataType::str<T>());
}

CASE("encoding std::array") {
    assert_StdArray<int>();
    assert_StdArray<float>();
    assert_StdArray<double>();
    assert_StdArray<long>();
    assert_StdArray<std::byte>();

    encode_StdVector<int>();
    encode_StdVector<float>();
    encode_StdVector<double>();
    encode_StdVector<long>();

    {
        using T = std::byte;
        std::bitset<8> bits;
        std::vector<T> in;
        in.resize(5);
        size_t n{0};
        for (auto& byte : in) {
            bits.set(n++, true);
            byte = *reinterpret_cast<std::byte*>(&bits);
        }
        codec::ArrayReference interpreted;
        interprete(in, interpreted);

        codec::Data data;
        codec::Metadata metadata;

        encode(interpreted, metadata, data);

        EXPECT(data.size() == in.size() * sizeof(T));
        EXPECT(::memcmp(in.data(), data.data(), data.size()) == 0);
        EXPECT(metadata.type() == "array");
        EXPECT(metadata.getString("datatype") == codec::DataType::str<T>());
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("test Encoder") {
    SECTION("default constructor") {
        codec::Encoder encoder;
        EXPECT(encoder == false);
        codec::Metadata metadata;
        codec::Data data;
        EXPECT_THROWS_AS(encode(encoder, metadata, data), eckit::AssertionFailed);
    }

    SECTION("Encoder via reference") {
        codec::Encoder encoder;
        auto ops = std::make_shared<EncodableType::Operations>();

        EncodableType encodable("string", ops);
        EXPECT_EQUAL(ops->size(), 1);
        EXPECT_EQUAL(ops->back(), "constructor");

        codec::ref(encodable);
        EXPECT_EQUAL(ops->size(), 1);
        EXPECT_EQUAL(ops->back(), "constructor");

        encoder = codec::Encoder{codec::ref(encodable)};
        EXPECT_EQUAL(ops->size(), 2);
        EXPECT_EQUAL(ops->back(), "encode_metadata");

        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT_EQUAL(ops->size(), 3);
        EXPECT_EQUAL(ops->back(), "encode_data");
    }

    SECTION("Encoder via copy") {
        codec::Encoder encoder;
        auto ops = std::make_shared<EncodableType::Operations>();

        EncodableType encodable("string", ops);
        EXPECT_EQUAL(ops->size(), 1);
        EXPECT_EQUAL(ops->back(), "constructor");


        encoder = codec::Encoder{codec::copy(encodable)};
        EXPECT_EQUAL(ops->size(), 3);
        EXPECT_EQUAL(ops->at(1), "encode_metadata");
        EXPECT_EQUAL(ops->at(2), "encode_data");

        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT_EQUAL(ops->size(), 3);
        EXPECT_EQUAL(ops->at(2), "encode_data");
    }

    SECTION("Encoder via move") {
        codec::Encoder encoder;
        auto ops = std::make_shared<EncodableType::Operations>();

        EncodableType encodable("string", ops);
        EXPECT_EQUAL(ops->size(), 1);
        EXPECT_EQUAL(ops->back(), "constructor");

        encoder = codec::Encoder{std::move(encodable)};
        EXPECT_EQUAL(ops->size(), 3);
        EXPECT_EQUAL(ops->at(1), "move constructor");
        EXPECT_EQUAL(ops->at(2), "encode_metadata");

        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT_EQUAL(ops->size(), 4);
        EXPECT_EQUAL(ops->at(3), "encode_data");
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("Encoder for std::vector") {
    SECTION("ref") {
        using T = double;
        std::vector<T> v{1, 2, 3, 4, 5, 6, 7, 8};

        codec::Encoder encoder(codec::ref(v));

        // We can only encode with reference to original vector (no copies were made)
        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT(data.size() == v.size() * sizeof(T));
        EXPECT(::memcmp(data, v.data(), data.size()) == 0);
    }

    SECTION("copy") {
        using T = double;
        std::vector<T> v{1, 2, 3, 4, 5, 6, 7, 8};

        codec::Encoder encoder;
        {
            std::vector<T> scoped = v;
            encoder               = codec::Encoder(codec::copy(scoped));
            scoped.assign(scoped.size(), 0);  // zero out before destruction
        }

        // We can now encode with scoped vector destroyed
        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT(data.size() == v.size() * sizeof(T));
        EXPECT(::memcmp(data, v.data(), data.size()) == 0);
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("Encoder for std::array") {
    SECTION("ref") {
        using T = double;
        std::array<T, 8> v{1, 2, 3, 4, 5, 6, 7, 8};

        codec::Encoder encoder(codec::ref(v));

        // We can only encode with reference to original vector (no copies were made)
        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT(data.size() == v.size() * sizeof(T));
        EXPECT(::memcmp(data, v.data(), data.size()) == 0);
    }

    SECTION("copy") {
        using T = double;
        std::array<T, 8> v{1, 2, 3, 4, 5, 6, 7, 8};

        codec::Encoder encoder;
        {
            std::array<T, 8> scoped = v;
            encoder                 = codec::Encoder(codec::copy(scoped));
            std::fill(std::begin(scoped), std::end(scoped), 0);  // zero out before destruction
        }

        // We can now encode with scoped vector destroyed
        codec::Metadata metadata;
        codec::Data data;
        encode(encoder, metadata, data);
        EXPECT(data.size() == v.size() * sizeof(T));
        EXPECT(::memcmp(data, v.data(), data.size()) == 0);
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("Encoder of encoder") {
    using T = double;
    std::vector<T> v{1, 2, 3, 4, 5, 6, 7, 8};

    codec::Encoder encoder(codec::ref(v));
    codec::Encoder encoder_of_encoder(codec::ref(encoder));

    codec::Metadata metadata;
    codec::Data data;
    encode(encoder_of_encoder, metadata, data);
    EXPECT(data.size() == v.size() * sizeof(T));
    EXPECT(::memcmp(data, v.data(), data.size()) == 0);
}

// -------------------------------------------------------------------------------------------------------

/// Helper class to be used in testing decoding of arrays.
template <typename T>
struct EncodedArray {
    codec::Data data;
    codec::Metadata metadata;

    EncodedArray() :
        in{1, 2, 3, 4, 5, 6, 7, 8} {
        encode(in, metadata, data);
    }

    friend bool operator==(const std::vector<T>& lhs, const EncodedArray<T>& rhs) {
        if (lhs.size() != rhs.in.size()) {
            return false;
        }
        return ::memcmp(lhs.data(), rhs.in.data(), rhs.in.size() * sizeof(T)) == 0;
    }
    friend bool operator==(const std::array<T, 8>& lhs, const EncodedArray<T>& rhs) {
        if (lhs.size() != rhs.in.size()) {
            return false;
        }
        return ::memcmp(lhs.data(), rhs.in.data(), rhs.in.size() * sizeof(T)) == 0;
    }

private:
    std::vector<T> in;
};

template <>
struct EncodedArray<std::byte> {
    using T = std::byte;
    codec::Data data;
    codec::Metadata metadata;

    EncodedArray() {
        std::bitset<8> bits;
        in.resize(5);
        size_t n{0};
        for (auto& byte : in) {
            bits.set(n++, true);
            byte = *reinterpret_cast<std::byte*>(&bits);
        }
        encode(in, metadata, data);
    }

    friend bool operator==(const std::vector<T>& lhs, const EncodedArray<T>& rhs) {
        if (lhs.size() != rhs.in.size()) {
            return false;
        }
        return ::memcmp(lhs.data(), rhs.in.data(), rhs.in.size() * sizeof(T)) == 0;
    }

private:
    std::vector<std::byte> in;
};

// -------------------------------------------------------------------------------------------------------

CASE("Decoding to std::vector") {
    using T = double;
    EncodedArray<T> encoded;
    std::vector<T> out;

    SECTION("decode std::vector directly") {
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, out));
        EXPECT(out == encoded);
    }

    SECTION("decode using rvalue codec::Decoder (type erasure)") {
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(out)));
        EXPECT(out == encoded);
    }

    SECTION("decode using lvalue codec::Decoder (type erasure)") {
        codec::Decoder decoder(out);
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(out)));
        EXPECT(out == encoded);
    }

    SECTION("decode using decoder of decoder") {
        codec::Decoder decoder(out);
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(decoder)));
        EXPECT(out == encoded);
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("Decoding to std::array") {
    using T = double;
    EncodedArray<T> encoded;
    std::array<T, 8> out;

    SECTION("decode std::vector directly") {
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, out));
        EXPECT(out == encoded);
    }

    SECTION("decode using rvalue codec::Decoder (type erasure)") {
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(out)));
        EXPECT(out == encoded);
    }

    SECTION("decode using lvalue codec::Decoder (type erasure)") {
        codec::Decoder decoder(out);
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(out)));
        EXPECT(out == encoded);
    }

    SECTION("decode using decoder of decoder") {
        codec::Decoder decoder(out);
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(decoder)));
        EXPECT(out == encoded);
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("Encode/Decode byte array") {
    using T = std::byte;
    EncodedArray<T> encoded;
    std::vector<T> out;

    auto validate = [&]() {
        EXPECT(out == encoded);

        auto to_byte = [](const char* str) { return std::byte(std::bitset<8>(str).to_ulong()); };
        EXPECT(out[0] == to_byte("00000001"));
        EXPECT(out[1] == to_byte("00000011"));
        EXPECT(out[2] == to_byte("00000111"));
        EXPECT(out[3] == to_byte("00001111"));
        EXPECT(out[4] == to_byte("00011111"));
    };

    SECTION("decode directly") {
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, out));
        validate();
    }

    SECTION("decode using rvalue codec::Decoder (type erasure)") {
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(out)));
        validate();
    }

    SECTION("decode using lvalue codec::Decoder (type erasure)") {
        codec::Decoder decoder(out);
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(out)));
        validate();
    }

    SECTION("decode using decoder of decoder") {
        codec::Decoder decoder(out);
        EXPECT_NO_THROW(decode(encoded.metadata, encoded.data, codec::Decoder(decoder)));
        validate();
    }
}

// -------------------------------------------------------------------------------------------------------

CASE("Encode/Decode string") {
    std::string in{"short string"};
    codec::Metadata metadata;
    codec::Data data;
    encode(in, metadata, data);
    EXPECT(data.size() == 0);

    std::string out;
    decode(metadata, data, out);
    EXPECT(out == in);
}

// -------------------------------------------------------------------------------------------------------

template <typename T>
void test_encode_decode_scalar() {
    T in{std::numeric_limits<T>::max()};
    T out;
    codec::Metadata metadata;
    codec::Data data;
    encode(in, metadata, data);
    EXPECT(data.size() == 0);

    decode(metadata, data, out);
    EXPECT(out == in);
}

CASE("Encode/Decode scalar") {
    // bit identical encoding via Base64 string within the metadata!
    SECTION("int32") {
        test_encode_decode_scalar<std::int32_t>();
    }
    SECTION("int64") {
        test_encode_decode_scalar<std::int64_t>();
    }
    SECTION("real32") {
        test_encode_decode_scalar<float>();
    }
    SECTION("real64") {
        test_encode_decode_scalar<double>();
    }
    SECTION("uint64") {
        test_encode_decode_scalar<std::uint64_t>();
    }
}

// -------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
