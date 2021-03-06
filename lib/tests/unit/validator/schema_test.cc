#include "tests/test.hpp"

#include <cpp-pcp-client/validator/schema.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <cpp-pcp-client/valijson/rapidjson_adapter.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>
#pragma GCC diagnostic pop

using namespace PCPClient;

bool validateTest(lth_jc::JsonContainer document, Schema schema) {
    valijson::Validator validator { schema.getRaw() };
    valijson::adapters::RapidJsonAdapter adapted_document { document.getRaw() };
    valijson::ValidationResults validation_results;
    return validator.validate(adapted_document, &validation_results);
}

const std::string trivial_schema_txt {
    "{ \"title\": \"trivial\","
    "  \"type\": \"object\","
    "  \"properties\": {"
    "       \"index\": {"
    "           \"type\": \"integer\""
    "       }"
    "  },"
    "  \"required\": [\"index\"],"
    "  \"additionalProperties\" : true"
    "}"};

const std::string song_schema_txt {
    "{ \"title\": \"song\","
    "  \"type\": \"object\","
    "  \"properties\": {"
    "       \"artist\": {"
    "           \"type\": \"string\""
    "       },"
    "       \"title\": {"
    "           \"type\": \"string\""
    "       },"
    "       \"album\": {"
    "           \"type\": \"string\""
    "       },"
    "       \"year\": {"
    "           \"description\": \"release year\","
    "           \"type\": \"integer\","
    "           \"minimum\": 1950"
    "       }"
    "  },"
    "  \"required\": [\"artist\", \"title\"],"
    "  \"additionalProperties\" : false"
    "}"};

TEST_CASE("Schema::Schema", "[validation]") {
    SECTION("can instantiate an empty Schema") {
        REQUIRE_NOTHROW(Schema("foo"));
    }

    SECTION("can instantiate an empty Schema by setting the content type") {
        REQUIRE_NOTHROW(Schema("bar", ContentType::Json));
    }

    SECTION("can instantiate by parsing a JSON schema") {
        lth_jc::JsonContainer json_schema { "{\"spam\" : {\"type\" : \"object\"}}" };
        REQUIRE_NOTHROW(Schema("spam", json_schema));
    }

    SECTION("throw schema_error if parsing an invalid schema", "[validation]") {
        lth_jc::JsonContainer json_schema { "[\"un\", \"deux\"]" };
        REQUIRE_THROWS_AS(Schema("eggs", json_schema),
                          schema_error);
    }

    SECTION("instantiate correctly by parsing a JSON schema") {
        lth_jc::JsonContainer song_json_schema { song_schema_txt };
        Schema song_schema { "song", song_json_schema };

        SECTION("it validates valid data") {
            std::string good_song_txt {
                " { \"artist\": \"Zappa\","
                "   \"title\" : \"Bobby Brown\","
                "   \"album\" : \"Sheik Yerbouti\","
                "   \"year\"  : 1979 }" };
            lth_jc::JsonContainer good_song { good_song_txt };

            REQUIRE(validateTest(good_song, song_schema));
        }

        SECTION("it does not validate data with missing entries") {
            // Missing artist entry
            std::string bad_song_txt {
                " { \"title\" : \"Three Girl Rhumba\","
                "   \"album\" : \"Pink Flag\","
                "   \"year\"  : 1977"
                " }" };

            lth_jc::JsonContainer bad_song { bad_song_txt };
            REQUIRE_FALSE(validateTest(bad_song, song_schema));
        }

        SECTION("it does not validate data with invalid entries") {
            // Invalid title entry
            std::string bad_song_txt {
                " { \"artist\": \"Wire\","
                "   \"title\" : 12,"
                "   \"album\" : \"Pink Flag\","
                "   \"year\"  : 1977"
                " }" };

            lth_jc::JsonContainer bad_song { bad_song_txt };
            REQUIRE_FALSE(validateTest(bad_song, song_schema));
        }

        SECTION("it does not validate data with undefined entries") {
            // 'duration' entry is not defined
            std::string bad_song_txt {
                " { \"artist\"   : \"Wire\","
                "   \"title\"    : \"Ex Lion Tamer\","
                "   \"album\"    : \"Pink Flag\","
                "   \"year\"     : 1977,"
                "   \"duration\" : 138"
                " }" };

            lth_jc::JsonContainer bad_song { bad_song_txt };
            REQUIRE_FALSE(validateTest(bad_song, song_schema));
        }

        SECTION("it validates data with undefined entries when allowed") {
            lth_jc::JsonContainer json_schema { trivial_schema_txt };
            Schema schema { "parsed schema", json_schema };
            lth_jc::JsonContainer data {};

            // Missing index
            REQUIRE_FALSE(validateTest(data, schema));

            data.set<int>("index", 42);
            REQUIRE(validateTest(data, schema));

            data.set<std::string>("foo", "bar");
            REQUIRE(validateTest(data, schema));
        }
    }
}

TEST_CASE("Schema::getName", "[validation]") {
    SECTION("the user can set during construction and get the schema name") {
        Schema schema { "a very nice name" };
        REQUIRE(schema.getName() == "a very nice name");
    }
}

TEST_CASE("Schema::addConstraint(type)", "[validation]") {
    SECTION("it throws a schem_error for parsed schemas") {
        lth_jc::JsonContainer json_schema { trivial_schema_txt };
        Schema parsed_schema { "parsed schema", json_schema };
        REQUIRE_THROWS_AS(parsed_schema.addConstraint("foo", TypeConstraint::Int),
                          schema_error);
    }

    Schema schema { "spam" };

    SECTION("it creates an interger constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : 2}" };
        lth_jc::JsonContainer bad_data { "{\"foo\" : \"two\"}" };
        schema.addConstraint("foo", TypeConstraint::Int, true);

        REQUIRE(validateTest(data, schema));
        REQUIRE_FALSE(validateTest(bad_data, schema));
    }

    SECTION("it creates an string constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : \"bar\"}" };
        schema.addConstraint("foo", TypeConstraint::String, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates an double constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : 0.0 }" };
        schema.addConstraint("foo", TypeConstraint::Double, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates an boolean constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : true }" };
        schema.addConstraint("foo", TypeConstraint::Bool, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates an array constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : []}" };
        schema.addConstraint("foo", TypeConstraint::Array, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates an string constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : {}}" };
        schema.addConstraint("foo", TypeConstraint::Object, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates an string constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : null}" };
        schema.addConstraint("foo", TypeConstraint::Null, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates an any constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : 1}" };
        schema.addConstraint("foo", TypeConstraint::Any, true);
        REQUIRE(validateTest(data, schema));
    }

   SECTION("it creates optional constraint") {
       lth_jc::JsonContainer data { "{}" };
        schema.addConstraint("foo", TypeConstraint::Int, false);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it creates multiple constraint") {
        lth_jc::JsonContainer data { "{\"foo\" : \"bar\","
                             " \"baz\" : 1 }" };
        schema.addConstraint("foo", TypeConstraint::String, true);
        schema.addConstraint("baz", TypeConstraint::Int, true);
        REQUIRE(validateTest(data, schema));
    }

    SECTION("it can be modified after use") {
        lth_jc::JsonContainer data { "{\"foo\" : \"bar\"}" };
        schema.addConstraint("foo", TypeConstraint::String, true);

        REQUIRE(validateTest(data, schema));

        schema.addConstraint("baz", TypeConstraint::Int, true);
        REQUIRE_FALSE(validateTest(data, schema));

        lth_jc::JsonContainer data2 { "{\"foo\" : \"bar\","
                              "\"baz\" : 1 }" };

        REQUIRE(validateTest(data2, schema));
    }

    SECTION("it throws a schema_error if type is not Object") {
        Schema schema { "eggs", TypeConstraint::String };
        REQUIRE_THROWS_AS(schema.addConstraint("baz", TypeConstraint::Int, true),
                          schema_error);
    }
}

TEST_CASE("Schema::addConstraint(subschema)", "[validation]") {
    SECTION("it throws a schema_error for parsed schemas") {
        Schema subschema { "subschema" };
        subschema.addConstraint("foo", TypeConstraint::String, true);

        lth_jc::JsonContainer json_schema { trivial_schema_txt };
        Schema parsed_schema { "parsed schema", json_schema };
        REQUIRE_THROWS_AS(parsed_schema.addConstraint("foo", subschema, true),
                          schema_error);
    }

    SECTION("it throws a schema_error if type is not Object") {
        Schema subschema { "subschema" };
        subschema.addConstraint("foo", TypeConstraint::String, true);

        Schema schema { "actual schema", TypeConstraint::Array };
        REQUIRE_THROWS_AS(schema.addConstraint("foo", subschema, true),
                          schema_error);
    }

    Schema schema { "spam" };

    SECTION("it creates a sub schema constraint") {
        lth_jc::JsonContainer data {"{\"root\" : "
                                "{\"foo\" : \"bar\","
                                "\"baz\" : 1 }}" };
        Schema subschema { "subschema" };
        subschema.addConstraint("foo", TypeConstraint::String, true);
        subschema.addConstraint("baz", TypeConstraint::Int, true);
        schema.addConstraint("root", subschema, true);
        REQUIRE(validateTest(data, schema));
    }
}

TEST_CASE("Schema::getContentType()", "[validation]") {
    Schema schema { "eggs", ContentType::Binary };
    REQUIRE(schema.getContentType() == ContentType::Binary);
}
