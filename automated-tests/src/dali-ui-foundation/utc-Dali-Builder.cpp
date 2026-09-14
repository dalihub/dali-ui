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
 *
 */

#include <dali-ui-foundation/integration-api/builder/builder.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_builder_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_builder_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* BUILDER_JSON = R"JSON(
{
  "constants": {
    "ROOT_NAME": "root-from-constant",
    "WIDTH": 240.0
  },
  "config": {
    "assetPath": "{DALI_IMAGE_DIR}icons",
    "quality": 3
  },
  "mappings": {
    "hidden": false,
    "mappedColor": [0.1, 0.2, 0.3, 1.0],
    "nested": { "enabled": "<hidden>", "color": "<mappedColor>" }
  },
  "templates": {
    "baseActor": {
      "type": "Actor",
      "name": "base",
      "size": [100.0, 80.0, 0.0],
      "actors": [
        { "type": "Actor", "name": "baseChild" }
      ]
    },
    "rootActor": {
      "type": "baseActor",
      "name": "{ROOT_NAME}",
      "visible": "<hidden>",
      "actors": [
        { "type": "Actor", "name": "directChild", "position": [1.0, 2.0, 3.0] }
      ]
    },
    "missingType": { "name": "invalid" }
  },
  "styles": {
    "baseStyle": {
      "visible": false,
      "color": [1.0, 0.0, 0.0, 1.0]
    },
    "derivedStyle": {
      "inherit": ["baseStyle"],
      "size": [320.0, 180.0, 0.0],
      "actors": {
        "directChild": { "visible": false }
      }
    }
  },
  "stage": [
    { "type": "Actor", "name": "stageActor" }
  ],
  "customSection": [
    { "type": "Actor", "name": "customActor" }
  ],
  "paths": {
    "generated": {
      "points": [[0.0, 0.0, 0.0], [10.0, 20.0, 0.0], [30.0, 40.0, 0.0]],
      "curvature": 0.35
    },
    "explicit": {
      "points": [[0.0, 0.0, 0.0], [10.0, 10.0, 0.0]],
      "controlPoints": [[2.0, 2.0, 0.0], [8.0, 8.0, 0.0]]
    },
    "invalid": {}
  },
  "constrainers": {
    "path": {
      "type": "PathConstrainer",
      "points": [[0.0, 0.0, 0.0], [10.0, 20.0, 0.0]],
      "controlPoints": [[2.0, 4.0, 0.0], [8.0, 16.0, 0.0]],
      "forward": [1.0, 0.0, 0.0]
    },
    "linear": {
      "type": "LinearConstrainer",
      "value": [0.0, 1.0, 0.0],
      "progress": [0.0, 0.5, 1.0]
    },
    "wrong": { "type": "Actor" },
    "untyped": {}
  }
}
)JSON";
} // namespace

int UtcDaliBuilderConstantsTemplatesAndStylesP(void)
{
  UiTestApplication            application(UiConfig::New());
  Dali::Ui::Integration::Builder builder = Dali::Ui::Integration::Builder::New();
  Property::Map               additions;
  Property::Value             overrideName("override-root");
  Property::Map               overrides;

  builder.LoadFromString(BUILDER_JSON);
  additions["EXTRA"] = 17;
  builder.AddConstants(additions);
  builder.AddConstant("ROOT_NAME", Property::Value("updated-root"));

  DALI_TEST_CHECK(builder.GetConstants().Count() >= 6u);
  DALI_TEST_EQUALS(builder.GetConstant("EXTRA").Get<int>(), 17, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetConfigurations().Find("quality")->Get<int>(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetConstant("unknown").GetType(), Property::NONE, TEST_LOCATION);

  Actor actor = Actor::DownCast(builder.Create("rootActor"));
  DALI_TEST_CHECK(actor);
  DALI_TEST_EQUALS(actor.GetProperty<Dali::String>(Actor::Property::NAME), Dali::String("updated-root"), TEST_LOCATION);
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_EQUALS(actor.GetChildCount(), 2u, TEST_LOCATION);

  overrides["ROOT_NAME"] = overrideName;
  Actor overridden = Actor::DownCast(builder.Create("rootActor", overrides));
  DALI_TEST_EQUALS(overridden.GetProperty<Dali::String>(Actor::Property::NAME), Dali::String("updated-root"), TEST_LOCATION);

  DALI_TEST_CHECK(builder.ApplyStyle("DERIVEDSTYLE", actor));
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(320.0f, 180.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_CHECK(!actor.FindChildByName("directChild").GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_CHECK(!builder.ApplyStyle("unknown", actor));
  DALI_TEST_CHECK(!builder.Create("unknown"));
  DALI_TEST_CHECK(!builder.Create("missingType"));
  END_TEST;
}

int UtcDaliBuilderCreateAndApplyJsonP(void)
{
  UiTestApplication     application(UiConfig::New());
  Dali::Ui::Integration::Builder builder = Dali::Ui::Integration::Builder::New();
  Actor                actor   = Actor::DownCast(builder.CreateFromJson(
    R"JSON({"type":"Actor","name":"inline","position":[4.0,5.0,6.0]})JSON"));

  DALI_TEST_CHECK(actor);
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(4.0f, 5.0f, 6.0f), TEST_LOCATION);
  DALI_TEST_CHECK(builder.ApplyFromJson(actor, R"JSON({"visible":false,"size":[9.0,8.0,7.0]})JSON"));
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_EQUALS(actor.GetProperty<Vector3>(Actor::Property::SIZE), Vector3(9.0f, 8.0f, 7.0f), TEST_LOCATION);
  DALI_TEST_CHECK(!builder.CreateFromJson("{"));
  DALI_TEST_CHECK(!builder.ApplyFromJson(actor, "{"));
  END_TEST;
}

int UtcDaliBuilderAddActorsP(void)
{
  UiTestApplication     application(UiConfig::New());
  Dali::Ui::Integration::Builder builder = Dali::Ui::Integration::Builder::New();
  Actor                parent  = Actor::New();

  builder.LoadFromString(BUILDER_JSON);
  builder.AddActors(parent);
  builder.AddActors("customSection", parent);
  builder.AddActors("unknownSection", parent);

  DALI_TEST_EQUALS(parent.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(parent.FindChildByName("stageActor"));
  DALI_TEST_CHECK(parent.FindChildByName("customActor"));
  END_TEST;
}

int UtcDaliBuilderPathsAndConstrainersP(void)
{
  UiTestApplication     application(UiConfig::New());
  Dali::Ui::Integration::Builder builder = Dali::Ui::Integration::Builder::New();

  builder.LoadFromString(BUILDER_JSON);

  Path generated = builder.GetPath("generated");
  Path explicitPath = builder.GetPath("explicit");
  DALI_TEST_CHECK(generated);
  DALI_TEST_CHECK(explicitPath);
  DALI_TEST_CHECK(builder.GetPath("generated") == generated);
  DALI_TEST_CHECK(!builder.GetPath("invalid"));
  DALI_TEST_CHECK(!builder.GetPath("unknown"));

  PathConstrainer path = builder.GetPathConstrainer("path");
  DALI_TEST_CHECK(path);
  DALI_TEST_CHECK(builder.GetPathConstrainer("path") == path);
  DALI_TEST_CHECK(!builder.GetPathConstrainer("wrong"));
  DALI_TEST_CHECK(!builder.GetPathConstrainer("untyped"));
  DALI_TEST_CHECK(!builder.GetPathConstrainer("unknown"));

  LinearConstrainer linear = builder.GetLinearConstrainer("linear");
  DALI_TEST_CHECK(linear);
  DALI_TEST_CHECK(builder.GetLinearConstrainer("linear") == linear);
  DALI_TEST_CHECK(!builder.GetLinearConstrainer("wrong"));
  DALI_TEST_CHECK(!builder.GetLinearConstrainer("untyped"));
  DALI_TEST_CHECK(!builder.GetLinearConstrainer("unknown"));
  END_TEST;
}
