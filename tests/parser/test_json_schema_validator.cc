/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/testing/Test.h"

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>


using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_json_schema_validator_basic") {
    // Define a simple schema
    nlohmann::json schema = R"({
        "$schema": "http://json-schema.org/draft-07/schema#",
        "type": "object",
        "properties": {
            "name": { "type": "string" },
            "age": { "type": "integer", "minimum": 0 }
        },
        "required": ["name"]
    })"_json;

    // Create validator
    nlohmann::json_schema::json_validator validator;
    validator.set_root_schema(schema);

    SECTION("valid document") {
        nlohmann::json doc = R"({
            "name": "John Doe",
            "age": 30
        })"_json;

        EXPECT_NO_THROW(validator.validate(doc));
    }

    SECTION("missing required field") {
        nlohmann::json doc = R"({
            "age": 30
        })"_json;

        EXPECT_THROWS_AS(validator.validate(doc), std::exception);
    }

    SECTION("wrong type") {
        nlohmann::json doc = R"({
            "name": 123,
            "age": 30
        })"_json;

        EXPECT_THROWS_AS(validator.validate(doc), std::exception);
    }

    SECTION("negative age") {
        nlohmann::json doc = R"({
            "name": "Jane Doe",
            "age": -5
        })"_json;

        EXPECT_THROWS_AS(validator.validate(doc), std::exception);
    }
}


CASE("test_json_schema_validator_array") {
    nlohmann::json schema = R"({
        "$schema": "http://json-schema.org/draft-07/schema#",
        "type": "array",
        "items": { "type": "number" },
        "minItems": 1
    })"_json;

    nlohmann::json_schema::json_validator validator;
    validator.set_root_schema(schema);

    SECTION("valid array") {
        nlohmann::json doc = R"([1.0, 2.5, 3.14])"_json;
        EXPECT_NO_THROW(validator.validate(doc));
    }

    SECTION("empty array fails minItems") {
        nlohmann::json doc = R"([])"_json;
        EXPECT_THROWS_AS(validator.validate(doc), std::exception);
    }

    SECTION("array with wrong type") {
        nlohmann::json doc = R"([1, "two", 3])"_json;
        EXPECT_THROWS_AS(validator.validate(doc), std::exception);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
