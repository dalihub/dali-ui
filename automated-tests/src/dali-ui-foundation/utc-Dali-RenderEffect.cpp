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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

int UtcDaliRenderEffectDitherNoiseStrengthP(void)
{
  UiTestApplication application;
  tet_infoline("UtcDaliRenderEffectDitherNoiseStrengthP");

  BackgroundBlurEffect backgroundBlur = BackgroundBlurEffect::New();
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 0.1f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetDitherNoiseStrength(0.5f);
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 0.5f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetDitherNoiseStrength(-1.0f);
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  backgroundBlur.SetDitherNoiseStrength(2.0f);
  DALI_TEST_EQUALS(backgroundBlur.GetDitherNoiseStrength(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  GaussianBlurEffect gaussianBlur = GaussianBlurEffect::New();
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 0.1f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetDitherNoiseStrength(0.25f);
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 0.25f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetDitherNoiseStrength(-1.0f);
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  gaussianBlur.SetDitherNoiseStrength(2.0f);
  DALI_TEST_EQUALS(gaussianBlur.GetDitherNoiseStrength(), 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}
