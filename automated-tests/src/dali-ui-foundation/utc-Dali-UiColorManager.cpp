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

#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

Vector4 gLastAppliedColor;
int     gApplyCallCount = 0;

void TestApplyFunc(const Vector4& color)
{
  gLastAppliedColor = color;
  gApplyCallCount++;
}

void TestApplyFunc2(const Vector4& color)
{
  // Second apply function for testing multiple bindings
}

bool OverridePrimary(StringView colorId, Vector4& outColor)
{
  if(colorId == "Primary")
  {
    outColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    return true;
  }
  return false;
}

bool OverrideAll(StringView colorId, Vector4& outColor)
{
  outColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
  return true;
}

} // namespace

void utc_dali_uicolormanager_startup(void)
{
  test_return_value = TET_UNDEF;
  gLastAppliedColor = Vector4::ZERO;
  gApplyCallCount   = 0;
}

void utc_dali_uicolormanager_cleanup(void)
{
  test_return_value = TET_PASS;
}

// === Constructor ===

int UtcDaliUiColorManagerConstructorP(void)
{
  UiTestApplication application;

  UiColorManager manager;
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

// === Get Singleton ===

int UtcDaliUiColorManagerGetP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  DALI_TEST_CHECK(manager);

  END_TEST;
}

int UtcDaliUiColorManagerGetSingletonP(void)
{
  UiTestApplication application;

  UiColorManager manager1 = UiColorManager::Get();
  UiColorManager manager2 = UiColorManager::Get();

  DALI_TEST_CHECK(manager1);
  DALI_TEST_CHECK(manager2);
  DALI_TEST_CHECK(manager1 == manager2);

  END_TEST;
}

// === Copy / Move ===

int UtcDaliUiColorManagerCopyConstructorP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  UiColorManager copy(manager);

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(manager == copy);

  END_TEST;
}

int UtcDaliUiColorManagerMoveConstructorP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  DALI_TEST_EQUALS(2, manager.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  UiColorManager moved(std::move(manager));
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

int UtcDaliUiColorManagerCopyAssignmentP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  UiColorManager copy;
  copy = manager;

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(manager == copy);

  END_TEST;
}

int UtcDaliUiColorManagerMoveAssignmentP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  UiColorManager moved;
  moved = std::move(manager);

  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

// === DownCast ===

int UtcDaliUiColorManagerDownCastP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  BaseHandle     handle(manager);

  UiColorManager downCasted = UiColorManager::DownCast(handle);
  DALI_TEST_CHECK(downCasted);

  END_TEST;
}

int UtcDaliUiColorManagerDownCastN(void)
{
  UiTestApplication application;

  BaseHandle handle;
  UiColorManager downCasted = UiColorManager::DownCast(handle);
  DALI_TEST_CHECK(!downCasted);

  END_TEST;
}

// === GetColor (by ID) ===

int UtcDaliUiColorManagerGetColorP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  DALI_TEST_EQUALS(manager.GetColor("Primary"), Vector4(0x38 / 255.0f, 0x7A / 255.0f, 0xFF / 255.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("OnPrimary"), Vector4(0xFC / 255.0f, 0xFC / 255.0f, 0xFF / 255.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("Background"), Vector4(0xF1 / 255.0f, 0xF1 / 255.0f, 0xF3 / 255.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("Surface"), Vector4(0xE4 / 255.0f, 0xE4 / 255.0f, 0xE7 / 255.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("OnSurface"), Vector4(0x4D / 255.0f, 0x4D / 255.0f, 0x52 / 255.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("Outline"), Vector4(0xB7 / 255.0f, 0xB7 / 255.0f, 0xBB / 255.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("Shadow"), Vector4(0.0f, 0.0f, 0.0f, 0.1f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("BlurSurface"), Vector4(0xFC / 255.0f, 0xFC / 255.0f, 0xFF / 255.0f, 0.8f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorManagerGetColorNotFoundN(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  Vector4 color = manager.GetColor("ThisColorDoesNotExist");
  DALI_TEST_EQUALS(color, Vector4::ZERO, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorManagerGetColorWithOutParamP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  Vector4 outColor;
  bool found = manager.GetColor("Primary", outColor);

  DALI_TEST_CHECK(found);
  DALI_TEST_CHECK(outColor != Vector4::ZERO);

  END_TEST;
}

int UtcDaliUiColorManagerGetColorWithOutParamN(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  Vector4 outColor(1.0f, 1.0f, 1.0f, 1.0f);
  bool found = manager.GetColor("NonExistentColor", outColor);

  DALI_TEST_CHECK(!found);

  END_TEST;
}

int UtcDaliUiColorManagerGetColorDefaultColorsP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  // Verify all default theme colors are available
  Vector4 primary;
  bool foundPrimary = manager.GetColor("Primary", primary);
  DALI_TEST_CHECK(foundPrimary);

  Vector4 background;
  bool foundBackground = manager.GetColor("Background", background);
  DALI_TEST_CHECK(foundBackground);

  Vector4 outline;
  bool foundOutline = manager.GetColor("Outline", outline);
  DALI_TEST_CHECK(foundOutline);

  END_TEST;
}

// === RegisterBinding / SetBindingColor ===

int UtcDaliUiColorManagerRegisterBindingTokenP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  UiColor color("Primary");
  manager.RegisterBinding(view, "TestColor", ColorCallback::New(TestApplyFunc));
  manager.SetBindingColor(view, "TestColor", color);

  // Verify binding was registered
  UiColor outColor;
  bool found = manager.GetBindingColor(view, "TestColor", outColor);
  DALI_TEST_CHECK(found);
  DALI_TEST_EQUALS(outColor.GetColorId(), "Primary", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorManagerSetBindingColorReplaceP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  // Register a token binding
  UiColor tokenColor("Primary");
  manager.RegisterBinding(view, "TestColor", ColorCallback::New(TestApplyFunc));
  manager.SetBindingColor(view, "TestColor", tokenColor);

  // Update with a different token color
  UiColor bgColor("Background");
  manager.SetBindingColor(view, "TestColor", bgColor);

  UiColor outColor;
  bool found = manager.GetBindingColor(view, "TestColor", outColor);
  DALI_TEST_CHECK(found);
  DALI_TEST_EQUALS(outColor.GetColorId(), "Background", TEST_LOCATION);

  END_TEST;
}

// === GetBindingColor ===

int UtcDaliUiColorManagerGetBindingColorP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  UiColor color("Primary");
  manager.RegisterBinding(view, "TestColor", ColorCallback::New(TestApplyFunc));
  manager.SetBindingColor(view, "TestColor", color);

  UiColor outColor;
  bool found = manager.GetBindingColor(view, "TestColor", outColor);

  DALI_TEST_CHECK(found);
  DALI_TEST_EQUALS(outColor.GetColorId(), "Primary", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorManagerGetBindingColorN(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  UiColor outColor;
  bool found = manager.GetBindingColor(view, "TestColor", outColor);

  DALI_TEST_CHECK(!found);

  END_TEST;
}

// === ClearBinding ===

int UtcDaliUiColorManagerClearBindingP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  UiColor color("Primary");
  manager.RegisterBinding(view, "TestColor", ColorCallback::New(TestApplyFunc));
  manager.SetBindingColor(view, "TestColor", color);

  manager.ClearBinding(view, "TestColor");

  // Verify binding was removed
  UiColor outColor;
  bool found = manager.GetBindingColor(view, "TestColor", outColor);
  DALI_TEST_CHECK(!found);

  END_TEST;
}

// === ClearBindings ===

int UtcDaliUiColorManagerClearBindingsP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  UiColor color1("Primary");
  UiColor color2("Background");

  manager.RegisterBinding(view, "TestColor1", ColorCallback::New(TestApplyFunc));
  manager.SetBindingColor(view, "TestColor1", color1);
  manager.RegisterBinding(view, "TestColor2", ColorCallback::New(TestApplyFunc2));
  manager.SetBindingColor(view, "TestColor2", color2);

  // Remove all bindings for this view
  manager.ClearBindings(view);

  // Verify all bindings were removed
  UiColor outColor;
  DALI_TEST_CHECK(!manager.GetBindingColor(view, "TestColor1", outColor));
  DALI_TEST_CHECK(!manager.GetBindingColor(view, "TestColor2", outColor));

  END_TEST;
}

int UtcDaliUiColorManagerClearBindingsNoBindingsN(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  // Should not throw even when no bindings exist
  manager.ClearBindings(view);

  DALI_TEST_CHECK(true);

  END_TEST;
}

// === SetColorOverride ===

int UtcDaliUiColorManagerSetColorOverrideP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  // Override "Primary" to red
  manager.SetColorOverride(OverridePrimary);

  Vector4 color = manager.GetColor("Primary");
  DALI_TEST_EQUALS(color, Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  // Cleanup
  manager.ClearColorOverride();

  END_TEST;
}

int UtcDaliUiColorManagerSetColorOverrideFallbackP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  // OverridePrimary only overrides "Primary", not "Background"
  manager.SetColorOverride(OverridePrimary);

  // "Background" should fall through to theme loader
  Vector4 background;
  bool found = manager.GetColor("Background", background);
  DALI_TEST_CHECK(found);
  DALI_TEST_CHECK(background != Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  // Cleanup
  manager.ClearColorOverride();

  END_TEST;
}

int UtcDaliUiColorManagerClearColorOverrideP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  // Get original theme color
  Vector4 originalPrimary = manager.GetColor("Primary");

  // Override to red
  manager.SetColorOverride(OverridePrimary);
  DALI_TEST_EQUALS(manager.GetColor("Primary"), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  // Clear override → should revert to theme value
  manager.ClearColorOverride();
  DALI_TEST_EQUALS(manager.GetColor("Primary"), originalPrimary, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorManagerSetColorOverrideRefreshP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();
  View view = View::New();

  // Bind view to "Primary" token color
  UiColor color("Primary");
  gApplyCallCount = 0;
  manager.RegisterBinding(view, "TestColor", ColorCallback::New(TestApplyFunc));
  manager.SetBindingColor(view, "TestColor", color);
  DALI_TEST_EQUALS(gApplyCallCount, 0, TEST_LOCATION);

  // Set override → should immediately refresh and call apply func
  gApplyCallCount = 0;
  manager.SetColorOverride(OverridePrimary);
  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedColor, Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  // Clear override → should refresh again with theme value
  gApplyCallCount = 0;
  manager.ClearColorOverride();
  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedColor != Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  END_TEST;
}

int UtcDaliUiColorManagerSetColorOverrideReplaceP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  // Set first override
  manager.SetColorOverride(OverridePrimary);
  DALI_TEST_EQUALS(manager.GetColor("Primary"), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  // Replace with override that returns black for everything
  manager.SetColorOverride(OverrideAll);
  DALI_TEST_EQUALS(manager.GetColor("Primary"), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetColor("Background"), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  // Cleanup
  manager.ClearColorOverride();

  END_TEST;
}

int UtcDaliUiColorManagerSetColorOverrideNullP(void)
{
  UiTestApplication application;

  UiColorManager manager = UiColorManager::Get();

  // Setting nullptr should act like ClearColorOverride
  manager.SetColorOverride(OverridePrimary);
  manager.SetColorOverride(nullptr);

  // Should use theme value
  Vector4 color = manager.GetColor("Primary");
  DALI_TEST_CHECK(color != Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  END_TEST;
}
