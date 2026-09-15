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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-test-suite-utils.h>
#include "../../../samples/dialog/dialog-transition.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
void Finish(UiTestApplication& application)
{
  application.SendNotification();
  application.Render(1500);
  application.SendNotification();
  application.Render();
}

AlertDialog NewCenteredDialog()
{
  auto dialog = AlertDialog::New();
  dialog.SetRequestedWidth(440.0f);
  dialog.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.5f, 0.5f, WRAP_CONTENT, WRAP_CONTENT))
    .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  dialog.SetTitle("Transition test");
  dialog.SetMessage("The scrim stays in place while the dialog moves.");
  dialog.AddActionButton("Close");
  return dialog;
}
}

int UtcDaliDialogTransitionSampleModes(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  navigator.SetRequestedWidth(480.0f);
  navigator.SetRequestedHeight(800.0f);
  application.GetScene().Add(navigator);
  Finish(application);
  for(auto mode : {DialogTransitionSample::Mode::FADE, DialogTransitionSample::Mode::SLIDE, DialogTransitionSample::Mode::SCALE})
  {
    auto dialog = NewCenteredDialog();
    // Obtain the reference from the real layout, independently of the helper.
    DialogPostOptions options;
    options.animated = false;
    DALI_TEST_CHECK(dialog.Post(navigator, options));
    Finish(application);
    const auto expectedPosition = dialog.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
    dialog.Dismiss(false);
    const float restingY = DialogTransitionSample::CenteredY(dialog, Vector2(480.0f, 800.0f));
    DALI_TEST_EQUALS(restingY, expectedPosition.y, 0.01f, TEST_LOCATION);
    options.animated = true;
    options.transitionSpec = DialogTransitionSample::CreateSpec(&tracker, mode, 1.0f, restingY);
    DALI_TEST_CHECK(dialog.Post(navigator, options));
    auto container = DialogContainer::DownCast(dialog.GetParent());
    auto scrim = container.GetScrim();
    application.SendNotification();
    application.Render(250);
    const auto position = dialog.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
    const auto scale = dialog.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
    const float opacity = dialog.GetCurrentProperty<float>(Actor::Property::OPACITY);
    if(mode == DialogTransitionSample::Mode::SLIDE)
    {
      DALI_TEST_CHECK(position.y > restingY && position.y < restingY + 80.0f);
      DALI_TEST_EQUALS(opacity, 1.0f, 0.001f, TEST_LOCATION);
    }
    else if(mode == DialogTransitionSample::Mode::SCALE)
    {
      DALI_TEST_CHECK(scale.x > 0.8f && scale.x < 1.0f);
    }
    else
    {
      DALI_TEST_CHECK(opacity > 0.0f && opacity < 1.0f);
      DALI_TEST_EQUALS(scale, Vector3::ONE, 0.001f, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(scrim.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::ZERO, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(container.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, 0.001f, TEST_LOCATION);
    const float scrimOpacity = scrim.GetCurrentProperty<float>(Actor::Property::OPACITY);
    DALI_TEST_CHECK(scrimOpacity > 0.0f && scrimOpacity < 1.0f);
    Finish(application);
    DALI_TEST_EQUALS(dialog.GetCurrentProperty<Vector3>(Actor::Property::POSITION), expectedPosition, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(dialog.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, 0.001f, TEST_LOCATION);
    dialog.Dismiss(true);
    application.SendNotification();
    application.Render(250);
    if(mode == DialogTransitionSample::Mode::SLIDE)
    {
      DALI_TEST_CHECK(dialog.GetCurrentProperty<Vector3>(Actor::Property::POSITION).y > restingY);
    }
    DALI_TEST_CHECK(dialog.IsPosted());
    Finish(application);
    DALI_TEST_CHECK(!dialog.IsPosted());
    options.animated = false;
    DALI_TEST_CHECK(dialog.Post(navigator, options));
    Finish(application);
    DALI_TEST_EQUALS(dialog.GetCurrentProperty<Vector3>(Actor::Property::POSITION), expectedPosition, 0.01f, TEST_LOCATION);
    dialog.Dismiss(false);
  }
  END_TEST;
}

int UtcDaliDialogTransitionSampleTimingAndNoScrim(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  navigator.SetRequestedWidth(480.0f);
  navigator.SetRequestedHeight(800.0f);
  application.GetScene().Add(navigator);
  Finish(application);
  for(bool animated : {false, true})
  {
    for(float duration : {0.35f, 1.4f})
    {
      auto dialog = NewCenteredDialog();
      // First Post, without a prior layout, must use the same centered target.
      const float restingY = DialogTransitionSample::CenteredY(dialog, Vector2(480.0f, 800.0f));
      unsigned shown = 0u;
      unsigned hidden = 0u;
      ConnectionTracker signals;
      dialog.ShownSignal().Connect(&signals, [&](Dialog) { ++shown; });
      dialog.HiddenSignal().Connect(&signals, [&](Dialog, DialogDismissReason) { ++hidden; });
      DialogPostOptions options;
      options.animated = animated;
      options.containerStyle = DialogContainerStyle::NoScrimPreset();
      options.transitionSpec = DialogTransitionSample::CreateSpec(&tracker, DialogTransitionSample::Mode::SLIDE, duration, restingY);
      navigator.SetModalTransitionAnimationEnabled(animated);
      DALI_TEST_CHECK(dialog.Post(navigator, options));
      auto scrim = DialogContainer::DownCast(dialog.GetParent()).GetScrim();
      DALI_TEST_CHECK(!scrim.GetRenderEffect());
      application.SendNotification();
      application.Render(500);
      application.SendNotification();
      DALI_TEST_EQUALS(shown, (animated && duration > 0.5f) ? 0u : 1u, TEST_LOCATION);
      Finish(application);
      DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
      DALI_TEST_EQUALS(dialog.GetCurrentProperty<Vector3>(Actor::Property::POSITION).y,
                       (800.0f - dialog.GetCurrentProperty<Vector3>(Actor::Property::SIZE).y) * 0.5f, 0.01f, TEST_LOCATION);
      // Back uses Navigator's animation flag, just like scrim dismissal.
      DALI_TEST_CHECK(navigator.NavigateBack());
      DALI_TEST_EQUALS(dialog.IsPosted(), animated, TEST_LOCATION);
      Finish(application);
      DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
      DALI_TEST_CHECK(!dialog.IsPosted());
    }
  }
  END_TEST;
}
