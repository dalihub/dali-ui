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

#include <dali-ui-components/internal/window/window-frame-style-resolver.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

constexpr float EPSILON = 0.001f;

int UtcDaliWindowFrameStyleSetShadowSelectsColor(void)
{
  WindowFrameStyle style;
  style.SetShadow(Dali::Ui::Shadow());
  style.SetShadowOutsets(Dali::Ui::Insets(4.0f, 8.0f, 12.0f, 16.0f));

  const ResolvedWindowFrameStyle resolved = ResolveWindowFrameStyle(style, false);
  DALI_TEST_CHECK(style.GetShadowSource() == WindowFrameShadowSource::COLOR);
  DALI_TEST_CHECK(resolved.shadowSource == WindowFrameShadowSource::COLOR);
  DALI_TEST_CHECK(resolved.shadowVisible);
  DALI_TEST_EQUALS(resolved.shadowOutsets.start, 4.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.end, 8.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.top, 12.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.bottom, 16.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameStyleMaximizedHidesClientEffect(void)
{
  WindowFrameStyle style;
  style.SetShadow(Dali::Ui::Shadow());
  style.SetShadowOutsets(Dali::Ui::Insets(12.0f, 12.0f, 12.0f, 12.0f));

  const ResolvedWindowFrameStyle resolved = ResolveWindowFrameStyle(style, true);
  DALI_TEST_CHECK(resolved.shadowSource == WindowFrameShadowSource::COLOR);
  DALI_TEST_CHECK(!resolved.shadowVisible);
  DALI_TEST_EQUALS(resolved.shadowOutsets.start, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.end, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.top, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.bottom, 0.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameStyleClearShadowDisablesClientGeometry(void)
{
  WindowFrameStyle style;
  style.SetShadow(Dali::Ui::Shadow());
  style.SetShadowOutsets(Dali::Ui::Insets(20.0f, 20.0f, 20.0f, 20.0f));
  style.ClearShadow();

  const ResolvedWindowFrameStyle resolved = ResolveWindowFrameStyle(style, false);
  DALI_TEST_CHECK(style.GetShadowSource() == WindowFrameShadowSource::NONE);
  DALI_TEST_CHECK(resolved.shadowSource == WindowFrameShadowSource::NONE);
  DALI_TEST_CHECK(!resolved.shadowVisible);
  DALI_TEST_EQUALS(resolved.shadowOutsets.start, 0.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameStyleSetShadowImageUrlSelectsImage(void)
{
  WindowFrameStyle style;
  style.SetShadowImageBorder(Dali::Ui::Insets(1.0f, 3.0f, 2.0f, 4.0f));

  const ResolvedWindowFrameStyle missing = ResolveWindowFrameStyle(style, false);
  DALI_TEST_CHECK(missing.shadowSource == WindowFrameShadowSource::NONE);

  style.SetShadowImageUrl("shadow.9.png");
  const ResolvedWindowFrameStyle ready = ResolveWindowFrameStyle(style, false);
  DALI_TEST_CHECK(style.GetShadowSource() == WindowFrameShadowSource::IMAGE);
  DALI_TEST_CHECK(ready.shadowSource == WindowFrameShadowSource::IMAGE);

  const Dali::Ui::Insets border = style.GetShadowImageBorder();
  DALI_TEST_EQUALS(border.start, 1.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(border.end, 3.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(border.top, 2.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(border.bottom, 4.0f, EPSILON, TEST_LOCATION);

  const Dali::Vector4 nPatchBorder = ToNPatchBorder(border);
  DALI_TEST_EQUALS(nPatchBorder.x, 1.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(nPatchBorder.y, 2.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(nPatchBorder.z, 3.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(nPatchBorder.w, 4.0f, EPSILON, TEST_LOCATION);

  style.SetShadowImageUrl("");
  DALI_TEST_CHECK(style.GetShadowSource() == WindowFrameShadowSource::NONE);
  END_TEST;
}

int UtcDaliWindowFrameStyleNegativeOutsetsClamp(void)
{
  WindowFrameStyle style;
  style.SetShadow(Dali::Ui::Shadow());
  style.SetShadowOutsets(Dali::Ui::Insets(-1.0f, 2.0f, -3.0f, 4.0f));

  const ResolvedWindowFrameStyle resolved = ResolveWindowFrameStyle(style, false);
  DALI_TEST_EQUALS(resolved.shadowOutsets.start, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.end, 2.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.top, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(resolved.shadowOutsets.bottom, 4.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}
