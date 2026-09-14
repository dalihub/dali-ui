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
#include <dali-ui-test-suite-utils.h>

#include <sstream>
#include <string>

using namespace Dali;
using Dali::Ui::Integration::JsonParser;
using Dali::Ui::Integration::TreeNode;

void utc_dali_json_parser_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_json_parser_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliJsonParserParsesAllJsonValueTypesP(void)
{
  JsonParser parser = JsonParser::New();
  DALI_TEST_CHECK(parser.Parse(
    R"({
      "string":"line\nquote:\" unicode:\u0041 {TOKEN}",
      "integer":-42,
      "float":6.25e-2,
      "true":true,
      "false":false,
      "null":null,
      "array":[1,2.5,"three",false,null],
      "object":{"nested":"value"}
    })"));

  const TreeNode* root = parser.GetRoot();
  DALI_TEST_CHECK(root);
  DALI_TEST_EQUALS(root->GetType(), TreeNode::OBJECT, TEST_LOCATION);
  DALI_TEST_EQUALS(root->Size(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(root->GetChild("string")->GetString(), "line\nquote:\" unicode:A {TOKEN}", TEST_LOCATION);
  DALI_TEST_EQUALS(root->GetChild("integer")->GetInteger(), -42, TEST_LOCATION);
  DALI_TEST_EQUALS(root->GetChild("float")->GetFloat(), 0.0625f, 0.0001f, TEST_LOCATION);
  DALI_TEST_CHECK(root->GetChild("true")->GetBoolean());
  DALI_TEST_CHECK(!root->GetChild("false")->GetBoolean());
  DALI_TEST_EQUALS(root->GetChild("null")->GetType(), TreeNode::IS_NULL, TEST_LOCATION);
  DALI_TEST_EQUALS(root->Count("array"), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(root->Count("missing"), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(root->GetChildIgnoreCase("OBJECT"));
  DALI_TEST_CHECK(!root->GetChildIgnoreCase("absent"));
  DALI_TEST_EQUALS(root->Find("nested")->GetString(), "value", TEST_LOCATION);
  DALI_TEST_CHECK(!root->Find("absent"));

  END_TEST;
}

int UtcDaliJsonParserIteratesMergesPacksAndWritesP(void)
{
  JsonParser parser = JsonParser::New();
  DALI_TEST_CHECK(parser.Parse(R"({"first":1,"shared":"old"})"));
  DALI_TEST_CHECK(parser.Parse(R"({"second":2,"shared":"new"})"));
  DALI_TEST_CHECK(!parser.ParseError());
  DALI_TEST_EQUALS(parser.GetErrorDescription(), "No Error", TEST_LOCATION);
  DALI_TEST_EQUALS(parser.GetErrorPosition(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(parser.GetErrorLineNumber(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(parser.GetErrorColumn(), 0, TEST_LOCATION);

  const TreeNode* root = parser.GetRoot();
  DALI_TEST_EQUALS(root->GetChild("first")->GetInteger(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(root->GetChild("second")->GetInteger(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(root->GetChild("shared")->GetString(), "new", TEST_LOCATION);

  size_t count = 0u;
  for(auto iter = root->CBegin(); iter != root->CEnd(); iter++)
  {
    auto entry = *iter;
    DALI_TEST_CHECK(entry.first);
    ++count;
  }
  DALI_TEST_EQUALS(count, root->Size(), TEST_LOCATION);

  parser.Pack();
  std::ostringstream output;
  parser.Write(output, 2);
  DALI_TEST_CHECK(output.str().find("\"second\"") != std::string::npos);
  DALI_TEST_CHECK(output.str().find("\"new\"") != std::string::npos);

  END_TEST;
}

int UtcDaliJsonParserCopiesTreeAndDowncastsP(void)
{
  JsonParser parser = JsonParser::New();
  DALI_TEST_CHECK(parser.Parse(R"({"outer":{"inner":[1,2,3]}})"));

  JsonParser copy = JsonParser::New(*parser.GetRoot());
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetRoot() != parser.GetRoot());
  DALI_TEST_EQUALS(copy.GetRoot()->Count("outer"), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetRoot()->Find("inner")->Size(), 3u, TEST_LOCATION);

  BaseHandle handle = copy;
  DALI_TEST_CHECK(JsonParser::DownCast(handle));
  DALI_TEST_CHECK(!JsonParser::DownCast(BaseHandle()));

  END_TEST;
}

int UtcDaliJsonParserReportsRepresentativeSyntaxErrorsP(void)
{
  const char* invalidSources[] = {
    "",
    "{",
    R"({"key"})",
    R"({"key":tru})",
    R"({"key":01})",
    R"({"key":"unterminated})",
    R"({"key":[1,]})",
    R"({"key":@})",
  };

  for(const char* source : invalidSources)
  {
    JsonParser parser = JsonParser::New();
    DALI_TEST_CHECK(!parser.Parse(source));
    DALI_TEST_CHECK(parser.ParseError());
    DALI_TEST_CHECK(!parser.GetRoot());
    DALI_TEST_CHECK(!parser.GetErrorDescription().empty());
    DALI_TEST_CHECK(parser.GetErrorPosition() >= 0);
    DALI_TEST_CHECK(parser.GetErrorLineNumber() >= 0);
    DALI_TEST_CHECK(parser.GetErrorColumn() >= 0);
  }

  END_TEST;
}
