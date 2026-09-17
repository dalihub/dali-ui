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

#include <dali-ui-foundation/internal/views/view/view-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

void utc_dali_view_debug_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_debug_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewDebugPropertyTypesP(void)
{
  UiTestApplication application(UiConfig::New());
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "debugActor");
  actor.RegisterProperty("boolean", true);
  actor.RegisterProperty("float", 1.25f);
  actor.RegisterProperty("integer", 17);
  actor.RegisterProperty("vector2", Vector2(1.0f, 2.0f));
  actor.RegisterProperty("vector3", Vector3(1.0f, 2.0f, 3.0f));
  actor.RegisterProperty("vector4", Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  actor.RegisterProperty("matrix3", Matrix3::IDENTITY);
  actor.RegisterProperty("matrix", Matrix::IDENTITY);
  actor.RegisterProperty("rectangle", Rect<int>(1, 2, 3, 4), Property::READ_WRITE);
  actor.RegisterProperty("rotation", Quaternion(Radian(0.5f), Vector3::ZAXIS));
  actor.RegisterProperty("string", "debug-value", Property::READ_WRITE);
  Property::Array array;
  array.PushBack(true);
  array.PushBack(3.0f);
  actor.RegisterProperty("array", array, Property::READ_WRITE);
  Property::Map map;
  map.Insert("first", 1);
  map.Insert("second", Vector2::ONE);
  actor.RegisterProperty("map", map, Property::READ_WRITE);
  actor.RegisterProperty("extents", Extents(1u, 2u, 3u, 4u), Property::READ_WRITE);
  actor.RegisterProperty("insets", Insets(1, 2, 3, 4), Property::READ_WRITE);

  std::ostringstream properties;
  DumpProperties(properties, actor);
  DALI_TEST_CHECK(properties.str().find("debug-value") != std::string::npos);
  DALI_TEST_CHECK(properties.str().find("matrix3") != std::string::npos);

  const std::string dumped = DumpActor(actor);
  DALI_TEST_CHECK(dumped.find("debugActor") != std::string::npos);
  DALI_TEST_CHECK(dumped.find("rendererCount") != std::string::npos);
  END_TEST;
}

int UtcDaliViewDebugHierarchyP(void)
{
  UiTestApplication application(UiConfig::New());
  Actor root = Actor::New();
  root.SetProperty(Actor::Property::NAME, "rootActor");
  ImageView view = ImageView::New();
  view.SetProperty(Actor::Property::NAME, "debugView");
  view.SetProperty(ImageView::Property::IMAGE, Property::Map().Add("visualType", "COLOR").Add("mixColor", Color::RED));
  View child = View::New();
  child.SetProperty(Actor::Property::NAME, "childView");
  view.Add(child);
  root.Add(view);
  application.GetScene().Add(root);
  application.SendNotification();
  application.Render();

  const std::string viewDump = DumpView(GetImpl(view));
  DALI_TEST_CHECK(viewDump.find("debugView") != std::string::npos);

  std::ostringstream hierarchy;
  DumpViewHierarchy(hierarchy, root);
  DALI_TEST_CHECK(hierarchy.str().find("\"Actor\"") != std::string::npos);
  DALI_TEST_CHECK(hierarchy.str().find("\"View\"") != std::string::npos);
  DALI_TEST_CHECK(hierarchy.str().find("childView") != std::string::npos);
  END_TEST;
}
