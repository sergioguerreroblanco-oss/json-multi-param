/**
 * @file        test_param_set_basic.cpp
 * @author      Sergio Guerrero Blanco
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       Unit tests for ParamSet core behavior.
 *
 * @details
 * Covers:
 *  - Schema definition and typed set/get
 *  - Constraint enforcement
 *  - JSON serialization/deserialization (typed JSON values)
 *  - Compact-string serialization/deserialization (determinism + escaping)
 *  - Ergonomics helpers such as get_or()
 */

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "param_set.h"

namespace
{

/**
 * @class ParamSetBasicFixture
 * @brief Test fixture providing small reusable schemas and helper constraints.
 */
class ParamSetBasicFixture : public ::testing::Test
{
   protected:
    static jsonmp::Constraints<int> speed_constraints()
    {
        jsonmp::Constraints<int> c;
        c.has_min = true;
        c.min     = 0;
        c.has_max = true;
        c.max     = 200;
        return c;
    }

    static jsonmp::Constraints<std::string> mode_constraints()
    {
        jsonmp::Constraints<std::string> c;
        c.has_allowed = true;
        c.allowed_values.push_back("AUTO");
        c.allowed_values.push_back("MANUAL");
        return c;
    }

    static jsonmp::ParamSet make_basic_schema()
    {
        jsonmp::ParamSet ps;
        ps.addInt("speed", 0, speed_constraints());
        ps.addDouble("temperature_limit", 60.0, {});
        ps.addString("mode", "AUTO", mode_constraints());
        ps.addBool("enabled", true, {});
        return ps;
    }
};

}  // namespace

/*****************************************************************************/
/* Basic typed set/get */
/*****************************************************************************/

/**
 * @test ParamSetBasic_AddSetGetIntAndString
 * @brief Verifies typed add/set/get for basic types.
 */
TEST_F(ParamSetBasicFixture, AddSetGetIntAndString)
{
    // GIVEN: a ParamSet with int and string params
    jsonmp::ParamSet ps;
    ps.addInt("speed", 0, {});
    ps.addString("mode", "AUTO", {});

    // WHEN: assigning values using typed set()
    std::string error;
    ASSERT_TRUE(ps.set<int>("speed", 120, error)) << error;
    ASSERT_TRUE(ps.set<std::string>("mode", "MANUAL", error)) << error;

    // THEN: typed get() returns the assigned values
    EXPECT_EQ(ps.get<int>("speed"), 120);
    EXPECT_EQ(ps.get<std::string>("mode"), "MANUAL");
}

/*****************************************************************************/
/* Constraints */
/*****************************************************************************/

/**
 * @test ParamSetBasic_IntMinMax
 * @brief Validates min/max constraint behavior for int.
 */
TEST_F(ParamSetBasicFixture, IntMinMax)
{
    // GIVEN: a constrained int parameter
    jsonmp::ParamSet ps;
    ps.addInt("speed", 0, speed_constraints());

    std::string error;

    // WHEN/THEN: values inside range succeed
    ASSERT_TRUE(ps.set<int>("speed", 0, error)) << error;
    ASSERT_TRUE(ps.set<int>("speed", 200, error)) << error;

    // WHEN/THEN: values outside range fail
    EXPECT_FALSE(ps.set<int>("speed", -1, error));
    EXPECT_FALSE(error.empty());

    EXPECT_FALSE(ps.set<int>("speed", 201, error));
    EXPECT_FALSE(error.empty());
}

/**
 * @test ParamSetBasic_StringAllowedValues
 * @brief Validates allowed-values constraint behavior for std::string.
 */
TEST_F(ParamSetBasicFixture, StringAllowedValues)
{
    // GIVEN: a constrained string parameter
    jsonmp::ParamSet ps;
    ps.addString("mode", "AUTO", mode_constraints());

    std::string error;

    // WHEN/THEN: allowed values succeed
    ASSERT_TRUE(ps.set<std::string>("mode", "AUTO", error)) << error;
    ASSERT_TRUE(ps.set<std::string>("mode", "MANUAL", error)) << error;

    // WHEN/THEN: unknown value fails
    EXPECT_FALSE(ps.set<std::string>("mode", "INVALID", error));
    EXPECT_FALSE(error.empty());
}

/*****************************************************************************/
/* JSON serialization */
/*****************************************************************************/

/**
 * @test ParamSetBasic_JsonRoundTrip
 * @brief Serializes to JSON and deserializes back into an equivalent schema.
 */
TEST_F(ParamSetBasicFixture, JsonRoundTrip)
{
    // GIVEN: a filled ParamSet
    auto tx = make_basic_schema();

    std::string error;
    ASSERT_TRUE(tx.set<int>("speed", 120, error)) << error;
    ASSERT_TRUE(tx.set<double>("temperature_limit", 60.0, error)) << error;
    ASSERT_TRUE(tx.set<std::string>("mode", "MANUAL", error)) << error;
    ASSERT_TRUE(tx.set<bool>("enabled", true, error)) << error;

    // WHEN: serialize to JSON
    const auto j = tx.to_json();

    // THEN: deserialize into another ParamSet with the same schema
    auto rx = make_basic_schema();
    ASSERT_TRUE(rx.from_json(j, error)) << error;

    EXPECT_EQ(rx.get<int>("speed"), 120);
    EXPECT_DOUBLE_EQ(rx.get<double>("temperature_limit"), 60.0);
    EXPECT_EQ(rx.get<std::string>("mode"), "MANUAL");
    EXPECT_EQ(rx.get<bool>("enabled"), true);
}

/**
 * @test ParamSetBasic_JsonTypesAreTyped
 * @brief Ensures JSON output uses proper JSON types (number/bool/string).
 */
TEST_F(ParamSetBasicFixture, JsonTypesAreTyped)
{
    auto ps = make_basic_schema();

    std::string error;
    ASSERT_TRUE(ps.set<int>("speed", 120, error)) << error;
    ASSERT_TRUE(ps.set<double>("temperature_limit", 60.0, error)) << error;
    ASSERT_TRUE(ps.set<std::string>("mode", "MANUAL", error)) << error;
    ASSERT_TRUE(ps.set<bool>("enabled", true, error)) << error;

    const auto j = ps.to_json();

    EXPECT_TRUE(j.at("speed").is_number_integer());
    EXPECT_TRUE(j.at("temperature_limit").is_number_float() ||
                j.at("temperature_limit").is_number());
    EXPECT_TRUE(j.at("mode").is_string());
    EXPECT_TRUE(j.at("enabled").is_boolean());
}

/*****************************************************************************/
/* Compact string serialization */
/*****************************************************************************/

/**
 * @test ParamSetBasic_CompactStringIsDeterministic
 * @brief Ensures compact serialization orders keys lexicographically.
 */
TEST_F(ParamSetBasicFixture, CompactStringIsDeterministic)
{
    jsonmp::ParamSet ps;
    ps.addInt("b", 0, {});
    ps.addInt("a", 0, {});
    ps.addInt("c", 0, {});

    std::string error;
    ASSERT_TRUE(ps.set<int>("b", 2, error)) << error;
    ASSERT_TRUE(ps.set<int>("a", 1, error)) << error;
    ASSERT_TRUE(ps.set<int>("c", 3, error)) << error;

    const std::string compact = ps.to_compact_string();

    // Expect alphabetical order: a, b, c
    EXPECT_EQ(compact, "a=1;b=2;c=3");
}

/**
 * @test ParamSetBasic_CompactStringRoundTripWithEscapes
 * @brief Ensures compact serialization/deserialization supports escaped separators.
 */
TEST_F(ParamSetBasicFixture, CompactStringRoundTripWithEscapes)
{
    jsonmp::ParamSet tx;
    tx.addString("note", "", {});

    std::string       error;
    const std::string original = R"(a=b; c=\path\file; end)";

    ASSERT_TRUE(tx.set<std::string>("note", original, error)) << error;

    const std::string compact = tx.to_compact_string();

    jsonmp::ParamSet rx;
    rx.addString("note", "", {});

    ASSERT_TRUE(rx.from_compact_string(compact, error)) << error;
    EXPECT_EQ(rx.get<std::string>("note"), original);
}

/*****************************************************************************/
/* Ergonomics */
/*****************************************************************************/

/**
 * @test ParamSetBasic_GetOrReturnsFallbackWhenMissing
 * @brief Ensures get_or returns fallback when a key is missing.
 */
TEST_F(ParamSetBasicFixture, GetOrReturnsFallbackWhenMissing)
{
    jsonmp::ParamSet ps;
    ps.addInt("speed", 10, {});

    EXPECT_EQ(ps.get_or<int>("speed", 99), 10);
    EXPECT_EQ(ps.get_or<int>("missing", 99), 99);
}

/**
 * @test ParamSetBasic_GetOrThrowsOnTypeMismatch
 * @brief Ensures get_or throws when the key exists but types mismatch.
 */
TEST_F(ParamSetBasicFixture, GetOrThrowsOnTypeMismatch)
{
    jsonmp::ParamSet ps;
    ps.addInt("speed", 10, {});

    EXPECT_THROW((ps.get_or<std::string>("speed", "x")), std::runtime_error);
}
