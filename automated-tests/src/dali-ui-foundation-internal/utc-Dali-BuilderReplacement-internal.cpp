/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-ui-foundation/integration-api/builder/json-parser.h>
#include <dali-ui-foundation/internal/builder/replacement.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui::Internal;

namespace
{

const TreeNode& ParseRoot(Dali::Ui::Integration::JsonParser& parser, const std::string& source)
{
  DALI_TEST_CHECK(parser.Parse(source));
  DALI_TEST_CHECK(parser.GetRoot());
  return *parser.GetRoot();
}

OptionalChild Child(const TreeNode& root, const char* name)
{
  const TreeNode* child = root.GetChild(name);
  return child ? OptionalChild(*child) : OptionalChild();
}

} // namespace

void utc_dali_builder_replacement_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_builder_replacement_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBuilderReplacementScalarTypesAndOverrideP(void)
{
  Dali::Ui::Integration::JsonParser parser = Dali::Ui::Integration::JsonParser::New();
  const std::string json = R"({
    "boolean":"{BOOLEAN}",
    "float":"{FLOAT}",
    "integer":"{INTEGER}",
    "text":"{TEXT}"
  })";
  const TreeNode& root = ParseRoot(parser, json);

  Property::Map defaults;
  defaults.Insert("BOOLEAN", true);
  defaults.Insert("FLOAT", 1.5f);
  defaults.Insert("INTEGER", 7);
  defaults.Insert("TEXT", "default");

  Property::Map overrides;
  overrides.Insert("INTEGER", 42);
  overrides.Insert("TEXT", "override");

  Replacement replacement(overrides, defaults);
  OptionalBoolean booleanValue = replacement.IsBoolean(*root.GetChild("boolean"));
  OptionalFloat floatValue = replacement.IsFloat(*root.GetChild("float"));
  OptionalInteger integerValue = replacement.IsInteger(*root.GetChild("integer"));
  OptionalString stringValue = replacement.IsString(*root.GetChild("text"));

  DALI_TEST_CHECK(booleanValue && *booleanValue);
  DALI_TEST_CHECK(floatValue);
  DALI_TEST_EQUALS(*floatValue, 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(integerValue);
  DALI_TEST_EQUALS(*integerValue, 42, TEST_LOCATION);
  DALI_TEST_CHECK(stringValue);
  DALI_TEST_EQUALS(*stringValue, "override", TEST_LOCATION);

  END_TEST;
}

int UtcDaliBuilderReplacementCompositeTypesP(void)
{
  Dali::Ui::Integration::JsonParser parser = Dali::Ui::Integration::JsonParser::New();
  const std::string json = R"({
    "vector2":"{VECTOR2}",
    "vector3":"{VECTOR3}",
    "vector4":"{VECTOR4}",
    "matrix":"{MATRIX}",
    "matrix3":"{MATRIX3}",
    "rect":"{RECT}",
    "extents":"{EXTENTS}",
    "insets":"{INSETS}"
  })";
  const TreeNode& root = ParseRoot(parser, json);

  Matrix matrix;
  matrix.SetIdentity();
  Matrix3 matrix3;
  matrix3.SetIdentity();

  Property::Map defaults;
  defaults.Insert("VECTOR2", Vector2(1.0f, 2.0f));
  defaults.Insert("VECTOR3", Vector3(1.0f, 2.0f, 3.0f));
  defaults.Insert("VECTOR4", Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  defaults.Insert("MATRIX", matrix);
  defaults.Insert("MATRIX3", matrix3);
  defaults.Insert("RECT", Rect<int>(1, 2, 3, 4));
  defaults.Insert("EXTENTS", Extents(1, 2, 3, 4));
  defaults.Insert("INSETS", Insets(1.0f, 2.0f, 3.0f, 4.0f));

  Replacement replacement(defaults);
  OptionalVector2 vector2 = replacement.IsVector2(*root.GetChild("vector2"));
  OptionalVector3 vector3 = replacement.IsVector3(*root.GetChild("vector3"));
  OptionalVector4 vector4 = replacement.IsVector4(*root.GetChild("vector4"));
  OptionalMatrix matrixValue = replacement.IsMatrix(*root.GetChild("matrix"));
  OptionalMatrix3 matrix3Value = replacement.IsMatrix3(*root.GetChild("matrix3"));
  OptionalRect rect = replacement.IsRect(*root.GetChild("rect"));
  OptionalExtents extents = replacement.IsExtents(*root.GetChild("extents"));
  OptionalInsets insets = replacement.IsInsets(*root.GetChild("insets"));

  DALI_TEST_CHECK(vector2);
  DALI_TEST_EQUALS(*vector2, Vector2(1.0f, 2.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(vector3);
  DALI_TEST_EQUALS(*vector3, Vector3(1.0f, 2.0f, 3.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(vector4);
  DALI_TEST_EQUALS(*vector4, Vector4(1.0f, 2.0f, 3.0f, 4.0f), 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(matrixValue && *matrixValue == matrix);
  DALI_TEST_CHECK(matrix3Value && *matrix3Value == matrix3);
  DALI_TEST_CHECK(rect && *rect == Rect<int>(1, 2, 3, 4));
  DALI_TEST_CHECK(extents && *extents == Extents(1, 2, 3, 4));
  DALI_TEST_CHECK(insets && *insets == Insets(1.0f, 2.0f, 3.0f, 4.0f));

  END_TEST;
}

int UtcDaliBuilderReplacementMapArrayAndPartialStringsP(void)
{
  Dali::Ui::Integration::JsonParser parser = Dali::Ui::Integration::JsonParser::New();
  const std::string json = R"({
    "map":"{MAP}",
    "array":"{ARRAY}",
    "partial":"assets/{DIRECTORY}/{FILE}",
    "unknown":"{UNKNOWN}",
    "wrong":"{INTEGER}"
  })";
  const TreeNode& root = ParseRoot(parser, json);

  Property::Map nestedMap;
  nestedMap.Insert("key", "value");
  Property::Array nestedArray;
  nestedArray.PushBack(1);
  nestedArray.PushBack(2);

  Property::Map defaults;
  defaults.Insert("MAP", nestedMap);
  defaults.Insert("ARRAY", nestedArray);
  defaults.Insert("DIRECTORY", "images");
  defaults.Insert("FILE", "icon.png");
  defaults.Insert("INTEGER", 9);

  Replacement replacement(defaults);
  Property::Value mapValue;
  Property::Value arrayValue;
  DALI_TEST_CHECK(replacement.IsMap(Child(root, "map"), mapValue));
  DALI_TEST_CHECK(replacement.IsArray(Child(root, "array"), arrayValue));
  DALI_TEST_EQUALS(mapValue.Get<Property::Map>().Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(arrayValue.Get<Property::Array>().Count(), 2u, TEST_LOCATION);

  OptionalString partial = replacement.IsString(*root.GetChild("partial"));
  OptionalString unknown = replacement.IsString(*root.GetChild("unknown"));
  OptionalString wrong = replacement.IsString(*root.GetChild("wrong"));
  DALI_TEST_CHECK(partial);
  DALI_TEST_EQUALS(*partial, "assets/images/icon.png", TEST_LOCATION);
  DALI_TEST_CHECK(unknown);
  DALI_TEST_EQUALS(*unknown, "{UNKNOWN}", TEST_LOCATION);
  DALI_TEST_CHECK(wrong);
  DALI_TEST_EQUALS(*wrong, "{INTEGER}", TEST_LOCATION);

  Property::Value unused;
  DALI_TEST_CHECK(!replacement.IsMap(OptionalChild(), unused));
  DALI_TEST_CHECK(!replacement.IsArray(OptionalChild(), unused));

  END_TEST;
}

int UtcDaliBuilderReplacementDirectNodesAndMissingChildrenP(void)
{
  Dali::Ui::Integration::JsonParser parser = Dali::Ui::Integration::JsonParser::New();
  const std::string json = R"({
    "boolean":true,
    "float":1.5,
    "integer":7,
    "text":"plain",
    "vector2":[1,2],
    "vector3":[1,2,3],
    "vector4":[1,2,3,4],
    "matrix":[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],
    "matrix3":[1,0,0,0,1,0,0,0,0,1],
    "rect":[1,2,3,4],
    "extents":[1,2,3,4],
    "insets":[1,2,3,4]
  })";
  const TreeNode& root = ParseRoot(parser, json);

  Replacement replacement;
  DALI_TEST_CHECK(replacement.IsBoolean(Child(root, "boolean")));
  DALI_TEST_CHECK(replacement.IsFloat(Child(root, "float")));
  DALI_TEST_CHECK(replacement.IsInteger(Child(root, "integer")));
  DALI_TEST_CHECK(replacement.IsString(Child(root, "text")));
  DALI_TEST_CHECK(replacement.IsVector2(Child(root, "vector2")));
  DALI_TEST_CHECK(replacement.IsVector3(Child(root, "vector3")));
  DALI_TEST_CHECK(replacement.IsVector4(Child(root, "vector4")));
  DALI_TEST_CHECK(replacement.IsMatrix(Child(root, "matrix")));
  DALI_TEST_CHECK(replacement.IsMatrix3(Child(root, "matrix3")));
  DALI_TEST_CHECK(replacement.IsRect(Child(root, "rect")));
  DALI_TEST_CHECK(replacement.IsExtents(Child(root, "extents")));
  DALI_TEST_CHECK(replacement.IsInsets(Child(root, "insets")));

  OptionalChild missing;
  DALI_TEST_CHECK(!replacement.IsBoolean(missing));
  DALI_TEST_CHECK(!replacement.IsFloat(missing));
  DALI_TEST_CHECK(!replacement.IsInteger(missing));
  DALI_TEST_CHECK(!replacement.IsString(missing));
  DALI_TEST_CHECK(!replacement.IsVector2(missing));
  DALI_TEST_CHECK(!replacement.IsVector3(missing));
  DALI_TEST_CHECK(!replacement.IsVector4(missing));
  DALI_TEST_CHECK(!replacement.IsMatrix(missing));
  DALI_TEST_CHECK(!replacement.IsMatrix3(missing));
  DALI_TEST_CHECK(!replacement.IsRect(missing));
  DALI_TEST_CHECK(!replacement.IsExtents(missing));
  DALI_TEST_CHECK(!replacement.IsInsets(missing));

  END_TEST;
}
