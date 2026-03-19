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
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_uicolor_startup(void)
{
  test_return_value = TET_UNDEF;
  UiConfig::New().Apply();
}

void utc_dali_uicolor_cleanup(void)
{
  test_return_value = TET_PASS;
}

// === Default Constructor ===

int UtcDaliUiColorDefaultConstructorP(void)
{
  TestApplication application;

  UiColor color;

  DALI_TEST_CHECK(!color.HasColorId());
  DALI_TEST_EQUALS(color.GetColorId(), String(), TEST_LOCATION);

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved, Vector4::ZERO, TEST_LOCATION);

  END_TEST;
}

// === RGBA Constructor ===

int UtcDaliUiColorRgbaConstructorP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 1.0f);

  DALI_TEST_CHECK(!color.HasColorId());
  DALI_TEST_EQUALS(color.GetColorId(), String(), TEST_LOCATION);

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorRgbaConstructorDefaultAlphaP(void)
{
  TestApplication application;

  UiColor color(0.5f, 0.5f, 0.5f);

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.a, 1.0f, TEST_LOCATION);

  END_TEST;
}

// === Vector4 Constructor ===

int UtcDaliUiColorVector4ConstructorP(void)
{
  TestApplication application;

  Vector4 input(0.2f, 0.4f, 0.6f, 0.8f);
  UiColor color(input);

  DALI_TEST_CHECK(!color.HasColorId());

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved, input, TEST_LOCATION);

  END_TEST;
}

// === String ID Constructor ===

int UtcDaliUiColorStringIdConstructorP(void)
{
  TestApplication application;

  String id("TestColorId");
  UiColor color(id);

  DALI_TEST_CHECK(color.HasColorId());
  DALI_TEST_EQUALS(color.GetColorId(), "TestColorId", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorStringIdMoveConstructorP(void)
{
  TestApplication application;

  String id("MovedColorId");
  UiColor color(std::move(id));

  DALI_TEST_CHECK(color.HasColorId());
  DALI_TEST_EQUALS(color.GetColorId(), "MovedColorId", TEST_LOCATION);

  END_TEST;
}

// === Hex RGB Constructor ===

int UtcDaliUiColorHexConstructorP(void)
{
  TestApplication application;

  // 0xFF0000 = pure red
  UiColor color(0xFF0000u);

  DALI_TEST_CHECK(!color.HasColorId());

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorHexConstructorWithAlphaP(void)
{
  TestApplication application;

  // 0x00FF00 = pure green, alpha 0.5
  UiColor color(0x00FF00u, 0.5f);

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 0.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorHexConstructorMixedP(void)
{
  TestApplication application;

  // 0x387AFF = the default "Primary" color
  UiColor color(0x387AFFu);

  Vector4 resolved = color.Resolve();
  DALI_TEST_EQUALS(resolved.r, 0x38 / 255.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0x7A / 255.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0xFF / 255.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 1.0f, TEST_LOCATION);

  END_TEST;
}

// === Copy / Move Semantics ===

int UtcDaliUiColorCopyConstructorRgbaP(void)
{
  TestApplication application;

  UiColor original(1.0f, 0.5f, 0.25f, 0.75f);
  UiColor copy(original);

  DALI_TEST_CHECK(!copy.HasColorId());
  DALI_TEST_EQUALS(copy.Resolve(), original.Resolve(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorCopyConstructorTokenP(void)
{
  TestApplication application;

  UiColor original("CopyTestId");
  UiColor copy(original);

  DALI_TEST_CHECK(copy.HasColorId());
  DALI_TEST_EQUALS(copy.GetColorId(), "CopyTestId", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorMoveConstructorP(void)
{
  TestApplication application;

  UiColor original(0.1f, 0.2f, 0.3f, 0.4f);
  Vector4 originalResolved = original.Resolve();

  UiColor moved(std::move(original));

  DALI_TEST_EQUALS(moved.Resolve(), originalResolved, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorCopyAssignmentP(void)
{
  TestApplication application;

  UiColor original(0.5f, 0.6f, 0.7f, 0.8f);
  UiColor copy;
  copy = original;

  DALI_TEST_EQUALS(copy.Resolve(), original.Resolve(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorMoveAssignmentP(void)
{
  TestApplication application;

  UiColor original(0.1f, 0.2f, 0.3f, 0.4f);
  Vector4 originalResolved = original.Resolve();

  UiColor moved;
  moved = std::move(original);

  DALI_TEST_EQUALS(moved.Resolve(), originalResolved, TEST_LOCATION);

  END_TEST;
}

// === HasColorId / GetColorId ===

int UtcDaliUiColorHasColorIdRgbaN(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f);
  DALI_TEST_CHECK(!color.HasColorId());

  END_TEST;
}

int UtcDaliUiColorHasColorIdTokenP(void)
{
  TestApplication application;

  UiColor color("SomeToken");
  DALI_TEST_CHECK(color.HasColorId());

  END_TEST;
}

int UtcDaliUiColorGetColorIdRgbaN(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f);
  DALI_TEST_EQUALS(color.GetColorId(), String(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorGetColorIdTokenP(void)
{
  TestApplication application;

  UiColor color("MyColorId");
  DALI_TEST_EQUALS(color.GetColorId(), "MyColorId", TEST_LOCATION);

  END_TEST;
}

// === Resolve ===

int UtcDaliUiColorResolveRgbaP(void)
{
  TestApplication application;

  UiColor color(0.1f, 0.2f, 0.3f, 0.4f);
  Vector4 resolved = color.Resolve();

  DALI_TEST_EQUALS(resolved, Vector4(0.1f, 0.2f, 0.3f, 0.4f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorResolveTokenP(void)
{
  TestApplication application;

  // "Primary" is a known default theme color (0x387AFF)
  UiColor color = UiColor::PRIMARY;
  Vector4 resolved = color.Resolve();

  // Should resolve to non-zero (the default theme provides "Primary")
  DALI_TEST_CHECK(resolved != Vector4::ZERO);

  END_TEST;
}

int UtcDaliUiColorResolveUnknownTokenN(void)
{
  TestApplication application;

  UiColor color("NonExistentColorId12345");
  Vector4 resolved = color.Resolve();

  // Unknown token should fallback to ZERO
  DALI_TEST_EQUALS(resolved, Vector4::ZERO, TEST_LOCATION);

  END_TEST;
}

// === Implicit Conversion to Vector4 ===

int UtcDaliUiColorImplicitConversionP(void)
{
  TestApplication application;

  UiColor color(0.5f, 0.6f, 0.7f, 0.8f);
  Vector4 vec = color; // implicit conversion

  DALI_TEST_EQUALS(vec, Vector4(0.5f, 0.6f, 0.7f, 0.8f), TEST_LOCATION);

  END_TEST;
}

// === ScaleAlpha ===

int UtcDaliUiColorScaleAlphaRgbaP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 0.8f);
  UiColor result = color.ScaleAlpha(0.5f);

  Vector4 resolved = result.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 0.4f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorScaleAlphaTokenP(void)
{
  TestApplication application;

  UiColor color("Primary");
  UiColor result = color.ScaleAlpha(0.5f);

  // The result should still have a color ID
  DALI_TEST_CHECK(result.HasColorId());
  DALI_TEST_EQUALS(result.GetColorId(), "Primary", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorScaleAlphaDoesNotModifyOriginalP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 1.0f);
  UiColor result = color.ScaleAlpha(0.5f);

  // Original should be unchanged
  DALI_TEST_EQUALS(color.Resolve().a, 1.0f, TEST_LOCATION);
  // Result should have new alpha
  DALI_TEST_EQUALS(result.Resolve().a, 0.5f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorScaleAlphaClampP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 1.0f);
  UiColor result = color.ScaleAlpha(2.0f);

  // Should clamp to 1.0
  DALI_TEST_CHECK(result.Resolve().a <= 1.0f);

  END_TEST;
}

// === WithAlpha ===

int UtcDaliUiColorWithAlphaRgbaP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 1.0f);
  UiColor result = color.WithAlpha(0.3f);

  Vector4 resolved = result.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.g, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.b, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 0.3f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorWithAlphaTokenP(void)
{
  TestApplication application;

  UiColor color("Primary");
  UiColor result = color.WithAlpha(0.7f);

  DALI_TEST_CHECK(result.HasColorId());
  DALI_TEST_EQUALS(result.GetColorId(), "Primary", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorWithAlphaDoesNotModifyOriginalP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 1.0f);
  UiColor result = color.WithAlpha(0.2f);

  // Original unchanged
  DALI_TEST_EQUALS(color.Resolve().a, 1.0f, TEST_LOCATION);
  // Result has new alpha
  DALI_TEST_EQUALS(result.Resolve().a, 0.2f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorWithAlphaClampP(void)
{
  TestApplication application;

  UiColor color(1.0f, 0.0f, 0.0f, 1.0f);

  // Negative should clamp to 0
  UiColor result = color.WithAlpha(-1.0f);
  DALI_TEST_EQUALS(result.Resolve().a, 0.0f, TEST_LOCATION);

  // Over 1 should clamp to 1
  result = color.WithAlpha(5.0f);
  DALI_TEST_EQUALS(result.Resolve().a, 1.0f, TEST_LOCATION);

  END_TEST;
}

// === Alpha Chaining ===

int UtcDaliUiColorWithAlphaThenScaleAlphaTokenP(void)
{
  TestApplication application;

  // WithAlpha fixes alpha to 0.5, then ScaleAlpha multiplies that fixed value by 0.2
  // Expected: With mode preserved, final alpha = 0.5 * 0.2 = 0.1 (token's original alpha ignored)
  UiColor result = UiColor::PRIMARY.WithAlpha(0.5f).ScaleAlpha(0.2f);

  DALI_TEST_CHECK(result.HasColorId());

  Vector4 resolved = result.Resolve();
  DALI_TEST_CHECK(resolved != Vector4::ZERO);
  DALI_TEST_EQUALS(resolved.a, 0.1f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorWithAlphaThenScaleAlphaRgbaP(void)
{
  TestApplication application;

  // RGBA case: WithAlpha sets alpha to 0.5, ScaleAlpha multiplies by 0.2
  UiColor result = UiColor(1.0f, 0.0f, 0.0f, 1.0f).WithAlpha(0.5f).ScaleAlpha(0.2f);

  Vector4 resolved = result.Resolve();
  DALI_TEST_EQUALS(resolved.r, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.a, 0.1f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorScaleAlphaChainTokenP(void)
{
  TestApplication application;

  // ScaleAlpha chaining: 1.0 * 0.5 * 0.2 = 0.1, then resolved.a = token_alpha * 0.1
  UiColor result = UiColor::PRIMARY.ScaleAlpha(0.5f).ScaleAlpha(0.2f);

  DALI_TEST_CHECK(result.HasColorId());

  Vector4 primaryResolved = UiColor::PRIMARY.Resolve();
  Vector4 resolved        = result.Resolve();
  DALI_TEST_EQUALS(resolved.a, primaryResolved.a * 0.5f * 0.2f, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorScaleAlphaThenWithAlphaTokenP(void)
{
  TestApplication application;

  // ScaleAlpha then WithAlpha: WithAlpha should override to With mode
  UiColor result = UiColor::PRIMARY.ScaleAlpha(0.5f).WithAlpha(0.3f);

  DALI_TEST_CHECK(result.HasColorId());

  Vector4 resolved = result.Resolve();
  DALI_TEST_EQUALS(resolved.a, 0.3f, 0.001f, TEST_LOCATION);

  END_TEST;
}

// === Predefined Colors ===

int UtcDaliUiColorPredefinedPrimaryP(void)
{
  TestApplication application;

  DALI_TEST_CHECK(UiColor::PRIMARY.HasColorId());
  DALI_TEST_EQUALS(UiColor::PRIMARY.GetColorId(), "Primary", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorPredefinedBackgroundP(void)
{
  TestApplication application;

  DALI_TEST_CHECK(UiColor::BACKGROUND.HasColorId());
  DALI_TEST_EQUALS(UiColor::BACKGROUND.GetColorId(), "Background", TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiColorPredefinedOutlineP(void)
{
  TestApplication application;

  DALI_TEST_CHECK(UiColor::OUTLINE.HasColorId());
  DALI_TEST_EQUALS(UiColor::OUTLINE.GetColorId(), "Outline", TEST_LOCATION);

  END_TEST;
}

// === Size Check ===

int UtcDaliUiColorSizeP(void)
{
  TestApplication application;

  // UiColor should be exactly 24 bytes for ABI stability
  DALI_TEST_EQUALS(sizeof(UiColor), 24u, TEST_LOCATION);

  END_TEST;
}
