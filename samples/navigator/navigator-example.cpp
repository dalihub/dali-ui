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

// Interactive sample for Ui::Navigator (+ DialogContainer + AlertDialog).
//
//   - Page Anim On/Off     : toggles page transition animations
//   - Modal Anim On/Off    : toggles modal transition animations
//   - Push Page / Pop Page : Navigator::Push / Navigator::Pop
//   - Fade Page            : Navigator::SetPageTransitionSpec for one page
//   - Show Dialog          : Navigator::PushModal with modal default animation
//   - Per-modal Custom Ani : Navigator::SetPageModalTransitionSpec for one modal
//                            view. Tap an action button OR tap the scrim to
//                            dismiss it.
//   - Back / Escape        : Navigator::NavigateBack (dismisses modal, else pops;
//                            quits when nothing is left to go back to)

#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <functional>
#include <memory>
#include <string>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::View;

namespace
{
const UiColor PAGE_COLORS[] = {UiColor(0x1565C0u), UiColor(0x2E7D32u), UiColor(0x6A1B9Au), UiColor(0xEF6C00u), UiColor(0xC62828u)};
constexpr int PAGE_COLOR_COUNT = 5;
constexpr float PAGE_SLIDE_TRAVEL = 96.0f;
constexpr float PAGE_WOBBLE_TRAVEL = 72.0f;
constexpr float PAGE_WOBBLE_VERTICAL_TRAVEL = 28.0f;
constexpr float MODAL_DROP_TRAVEL = 80.0f;
constexpr float MODAL_BOUNCE_TRAVEL = 120.0f;
constexpr float MODAL_SHAKE_X_TRAVEL = 110.0f;
constexpr float MODAL_SHAKE_Y_TRAVEL = 84.0f;
} // namespace

class NavigatorExample : public ConnectionTracker
{
public:
  explicit NavigatorExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &NavigatorExample::Create);
  }

  ~NavigatorExample() = default;

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Extents(24, 24, 24, 24));

    root.Add(MakeText("Navigator Sample", 24.0f, 48.0f));

    mNavigator = Navigator::New();
    mNavigator.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mNavigator.TransitionFinishedSignal().Connect(this, &NavigatorExample::OnTransitionFinished);
#ifdef DALI_UI_NAVIGATOR_HAS_PAGE_ANIMATION_SWITCH
    mNavigator.SetPageTransitionAnimationEnabled(mPageAnimationEnabled);
#endif
    mNavigator.SetTransitionSpec(CreateSlideTransitionSpec());
#ifdef DALI_UI_NAVIGATOR_HAS_MODAL_TRANSITION_SPEC
    mNavigator.SetModalTransitionSpec(CreateModalDropTransitionSpec());
#endif
#ifdef DALI_UI_NAVIGATOR_HAS_MODAL_ANIMATION_SWITCH
    mNavigator.SetModalTransitionAnimationEnabled(mModalAnimationEnabled);
#endif
    root.Add(mNavigator);

    mStatus = MakeText("", 18.0f, 36.0f);
    root.Add(mStatus);

    StackLayout navButtons = MakeButtonRow();
    navButtons.Add(MakeButton("Page Anim On/Off", UiColor(0x455A64u), [this]() { TogglePageAnimation(); }));
    navButtons.Add(MakeButton("Push Page", UiColor(0x3367D6u), [this]() { PushPage(); }));
    navButtons.Add(MakeButton("Per-page Custom Ani", UiColor(0x7B1FA2u), [this]() { PushPage(true); }));
    navButtons.Add(MakeButton("Pop Page", UiColor(0x5F6368u), [this]() { mNavigator.Pop(mPageAnimationEnabled); UpdateStatus(); }));
    root.Add(navButtons);

    StackLayout modalButtons = MakeButtonRow();
    modalButtons.Add(MakeButton("Modal Anim On/Off", UiColor(0x37474Fu), [this]() { ToggleModalAnimation(); }));
    modalButtons.Add(MakeButton("Show Dialog", UiColor(0x00897Bu), [this]() { ShowDialog(false); }));
    modalButtons.Add(MakeButton("Per-modal Custom Ani", UiColor(0xC2185Bu), [this]() { ShowDialog(true); }));
    root.Add(modalButtons);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &NavigatorExample::OnKeyEvent);

    PushPage(); // start with one page
  }

private:
  void PushPage(bool usePageFade = false)
  {
    View page = View::New();
    page.SetBackgroundColor(PAGE_COLORS[(mNextPageNumber - 1) % PAGE_COLOR_COUNT]);

    Label label = Label::New(("Page " + std::to_string(mNextPageNumber)).c_str());
    label.SetFontSize(28.0f);
    label.SetTextColor(UiColor(0xFFFFFFu));
    label.SetRequestedPositionX(24.0f);
    label.SetRequestedPositionY(24.0f);
    page.Add(label);

    ++mNextPageNumber;
    if(usePageFade)
    {
      mNavigator.SetPageTransitionSpec(page, CreateFadePageTransitionSpec());
    }
    mNavigator.Push(page, mPageAnimationEnabled); // Navigator sizes the page to fill itself
    UpdateStatus();
  }

  std::shared_ptr<NavigationTransitionSpec> CreateSlideTransitionSpec() const
  {
    auto    spec   = std::make_shared<NavigationTransitionSpec>();
    spec->duration = 0.42f;

    // Incoming pages (enter/popEnter) set an off-screen start via SetProperty, but the
    // page is arranged this frame and ViewImpl::OnArrange then overwrites POSITION_X with
    // the layout bounds (0). AnimateTo would read that clobbered value as its start and
    // not move, so drive POSITION_X with an explicit key-frame animation whose progress-0
    // value is authoritative and survives the arrange. Outgoing pages (exit/popExit) are
    // already arranged and stable at 0, so AnimateTo from the current value is correct.
    // Opacity is never touched by layout, so AnimateTo is used for it everywhere.
    spec->enter = [](Animation& anim, View view) {
      Dali::Ui::Extension::SetPositionX(view, PAGE_SLIDE_TRAVEL);
      view.SetProperty(Actor::Property::OPACITY, 0.0f);
      KeyFrames slideX = KeyFrames::New();
      slideX.Add(0.0f, PAGE_SLIDE_TRAVEL);
      slideX.Add(1.0f, 0.0f);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_X), slideX, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_OUT);
    };
    spec->exit = [](Animation& anim, View view) {
      anim.AnimateTo(Property(view, Actor::Property::POSITION_X), -PAGE_SLIDE_TRAVEL, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.82f, AlphaFunction::EASE_OUT);
    };
    spec->popEnter = [](Animation& anim, View view) {
      Dali::Ui::Extension::SetPositionX(view, -PAGE_SLIDE_TRAVEL);
      view.SetProperty(Actor::Property::OPACITY, 0.82f);
      KeyFrames slideX = KeyFrames::New();
      slideX.Add(0.0f, -PAGE_SLIDE_TRAVEL);
      slideX.Add(1.0f, 0.0f);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_X), slideX, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_OUT);
    };
    spec->popExit = [](Animation& anim, View view) {
      anim.AnimateTo(Property(view, Actor::Property::POSITION_X), PAGE_SLIDE_TRAVEL, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f, AlphaFunction::EASE_OUT);
    };
    spec->snapIncoming = [](View view) {
      Dali::Ui::Extension::SetPositionX(view, 0.0f);
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };
    spec->snapOutgoing = [](View view) {
      Dali::Ui::Extension::SetPositionX(view, 0.0f);
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };

    return spec;
  }

  std::shared_ptr<NavigationTransitionSpec> CreateFadePageTransitionSpec() const
  {
    auto    spec   = std::make_shared<NavigationTransitionSpec>();
    spec->duration = 0.58f;

    spec->enter = [](Animation& anim, View view) {
      KeyFrames wobbleX = KeyFrames::New();
      wobbleX.Add(0.0f, PAGE_WOBBLE_TRAVEL);
      wobbleX.Add(0.45f, -PAGE_WOBBLE_TRAVEL * 0.42f);
      wobbleX.Add(0.72f, PAGE_WOBBLE_TRAVEL * 0.18f);
      wobbleX.Add(1.0f, 0.0f);

      KeyFrames wobbleY = KeyFrames::New();
      wobbleY.Add(0.0f, -PAGE_WOBBLE_VERTICAL_TRAVEL);
      wobbleY.Add(0.32f, PAGE_WOBBLE_VERTICAL_TRAVEL * 0.65f);
      wobbleY.Add(0.64f, -PAGE_WOBBLE_VERTICAL_TRAVEL * 0.28f);
      wobbleY.Add(1.0f, 0.0f);

      view.SetProperty(Actor::Property::OPACITY, 0.0f);
      Dali::Ui::Extension::SetPositionX(view, PAGE_WOBBLE_TRAVEL);
      Dali::Ui::Extension::SetPositionY(view, -PAGE_WOBBLE_VERTICAL_TRAVEL);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_X), wobbleX, AlphaFunction::EASE_OUT);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_Y), wobbleY, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_OUT, TimePeriod(0.0f, 0.34f));
    };
    spec->popExit = [](Animation& anim, View view) {
      KeyFrames wobbleX = KeyFrames::New();
      wobbleX.Add(0.0f, 0.0f);
      wobbleX.Add(0.35f, -PAGE_WOBBLE_TRAVEL * 0.28f);
      wobbleX.Add(1.0f, PAGE_WOBBLE_TRAVEL);

      KeyFrames wobbleY = KeyFrames::New();
      wobbleY.Add(0.0f, 0.0f);
      wobbleY.Add(0.45f, PAGE_WOBBLE_VERTICAL_TRAVEL * 0.45f);
      wobbleY.Add(1.0f, -PAGE_WOBBLE_VERTICAL_TRAVEL * 0.75f);

      anim.AnimateBetween(Property(view, Actor::Property::POSITION_X), wobbleX, AlphaFunction::EASE_OUT);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_Y), wobbleY, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f, AlphaFunction::EASE_OUT, TimePeriod(0.0f, 0.28f));
    };
    spec->snapIncoming = [](View view) {
      Dali::Ui::Extension::SetPositionX(view, 0.0f);
      Dali::Ui::Extension::SetPositionY(view, 0.0f);
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };
    spec->snapOutgoing = [](View view) {
      Dali::Ui::Extension::SetPositionX(view, 0.0f);
      Dali::Ui::Extension::SetPositionY(view, 0.0f);
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };

    return spec;
  }

  std::shared_ptr<NavigationTransitionSpec> CreateModalDropTransitionSpec() const
  {
    auto    spec   = std::make_shared<NavigationTransitionSpec>();
    spec->duration = 0.32f;

    // Incoming modal drives POSITION_Y with a key-frame animation for the same reason as
    // the slide spec (the just-arranged modal would otherwise have its POSITION_Y clobbered
    // to 0 by ViewImpl::OnArrange). popExit animates the already-arranged modal from its
    // current position, so AnimateTo is correct there.
    spec->enter = [](Animation& anim, View view) {
      Dali::Ui::Extension::SetPositionY(view, MODAL_DROP_TRAVEL);
      view.SetProperty(Actor::Property::OPACITY, 0.0f);
      KeyFrames dropY = KeyFrames::New();
      dropY.Add(0.0f, MODAL_DROP_TRAVEL);
      dropY.Add(1.0f, 0.0f);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_Y), dropY, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_OUT);
    };
    spec->popExit = [](Animation& anim, View view) {
      anim.AnimateTo(Property(view, Actor::Property::POSITION_Y), MODAL_DROP_TRAVEL, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f, AlphaFunction::EASE_OUT);
    };
    spec->snapIncoming = [](View view) {
      Dali::Ui::Extension::SetPositionY(view, 0.0f);
      view.SetProperty(Actor::Property::SCALE, Vector3(1.0f, 1.0f, 1.0f));
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };
    spec->snapOutgoing = [](View view) {
      Dali::Ui::Extension::SetPositionY(view, 0.0f);
      view.SetProperty(Actor::Property::SCALE, Vector3(1.0f, 1.0f, 1.0f));
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };

    return spec;
  }

  std::shared_ptr<NavigationTransitionSpec> CreatePerModalBounceTransitionSpec() const
  {
    auto    spec   = std::make_shared<NavigationTransitionSpec>();
    spec->duration = 0.78f;

    spec->enter = [](Animation& anim, View view) {
      KeyFrames shakeX = KeyFrames::New();
      shakeX.Add(0.0f, -MODAL_SHAKE_X_TRAVEL);
      shakeX.Add(0.18f, MODAL_SHAKE_X_TRAVEL * 0.92f);
      shakeX.Add(0.34f, -MODAL_SHAKE_X_TRAVEL * 0.74f);
      shakeX.Add(0.52f, MODAL_SHAKE_X_TRAVEL * 0.48f);
      shakeX.Add(0.70f, -MODAL_SHAKE_X_TRAVEL * 0.24f);
      shakeX.Add(0.86f, MODAL_SHAKE_X_TRAVEL * 0.10f);
      shakeX.Add(1.0f, 0.0f);

      KeyFrames shakeY = KeyFrames::New();
      shakeY.Add(0.0f, -MODAL_BOUNCE_TRAVEL);
      shakeY.Add(0.18f, MODAL_SHAKE_Y_TRAVEL * 0.68f);
      shakeY.Add(0.34f, -MODAL_SHAKE_Y_TRAVEL * 0.54f);
      shakeY.Add(0.52f, MODAL_SHAKE_Y_TRAVEL * 0.36f);
      shakeY.Add(0.70f, -MODAL_SHAKE_Y_TRAVEL * 0.22f);
      shakeY.Add(0.86f, MODAL_SHAKE_Y_TRAVEL * 0.10f);
      shakeY.Add(1.0f, 0.0f);

      KeyFrames scale = KeyFrames::New();
      scale.Add(0.0f, Vector3(0.62f, 0.62f, 1.0f));
      scale.Add(0.18f, Vector3(1.18f, 1.18f, 1.0f));
      scale.Add(0.34f, Vector3(0.88f, 0.88f, 1.0f));
      scale.Add(0.52f, Vector3(1.10f, 1.10f, 1.0f));
      scale.Add(0.70f, Vector3(0.96f, 0.96f, 1.0f));
      scale.Add(1.0f, Vector3(1.0f, 1.0f, 1.0f));

      Dali::Ui::Extension::SetPositionX(view, -MODAL_SHAKE_X_TRAVEL);
      Dali::Ui::Extension::SetPositionY(view, -MODAL_BOUNCE_TRAVEL);
      view.SetProperty(Actor::Property::SCALE, Vector3(0.62f, 0.62f, 1.0f));
      view.SetProperty(Actor::Property::OPACITY, 0.0f);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_X), shakeX, AlphaFunction::EASE_OUT);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_Y), shakeY, AlphaFunction::EASE_OUT);
      anim.AnimateBetween(Property(view, Actor::Property::SCALE), scale, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f, AlphaFunction::EASE_OUT, TimePeriod(0.0f, 0.18f));
    };
    spec->popExit = [](Animation& anim, View view) {
      KeyFrames shakeX = KeyFrames::New();
      shakeX.Add(0.0f, 0.0f);
      shakeX.Add(0.18f, MODAL_SHAKE_X_TRAVEL * 0.58f);
      shakeX.Add(0.36f, -MODAL_SHAKE_X_TRAVEL * 0.76f);
      shakeX.Add(0.56f, MODAL_SHAKE_X_TRAVEL * 0.92f);
      shakeX.Add(1.0f, MODAL_SHAKE_X_TRAVEL * 1.15f);

      KeyFrames shakeY = KeyFrames::New();
      shakeY.Add(0.0f, 0.0f);
      shakeY.Add(0.18f, -MODAL_SHAKE_Y_TRAVEL * 0.38f);
      shakeY.Add(0.36f, MODAL_SHAKE_Y_TRAVEL * 0.34f);
      shakeY.Add(0.56f, -MODAL_SHAKE_Y_TRAVEL * 0.28f);
      shakeY.Add(1.0f, MODAL_BOUNCE_TRAVEL);

      anim.AnimateBetween(Property(view, Actor::Property::POSITION_X), shakeX, AlphaFunction::EASE_OUT);
      anim.AnimateBetween(Property(view, Actor::Property::POSITION_Y), shakeY, AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.66f, 0.66f, 1.0f), AlphaFunction::EASE_OUT);
      anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f, AlphaFunction::EASE_OUT, TimePeriod(0.08f, 0.30f));
    };
    spec->snapIncoming = [](View view) {
      Dali::Ui::Extension::SetPositionX(view, 0.0f);
      Dali::Ui::Extension::SetPositionY(view, 0.0f);
      view.SetProperty(Actor::Property::SCALE, Vector3(1.0f, 1.0f, 1.0f));
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };
    spec->snapOutgoing = [](View view) {
      Dali::Ui::Extension::SetPositionX(view, 0.0f);
      Dali::Ui::Extension::SetPositionY(view, 0.0f);
      view.SetProperty(Actor::Property::SCALE, Vector3(1.0f, 1.0f, 1.0f));
      view.SetProperty(Actor::Property::OPACITY, 1.0f);
    };

    return spec;
  }

  void TogglePageAnimation()
  {
    mPageAnimationEnabled = !mPageAnimationEnabled;
#ifdef DALI_UI_NAVIGATOR_HAS_PAGE_ANIMATION_SWITCH
    mNavigator.SetPageTransitionAnimationEnabled(mPageAnimationEnabled);
#endif
    UpdateStatus();
  }

  void ToggleModalAnimation()
  {
    mModalAnimationEnabled = !mModalAnimationEnabled;
#ifdef DALI_UI_NAVIGATOR_HAS_MODAL_ANIMATION_SWITCH
    mNavigator.SetModalTransitionAnimationEnabled(mModalAnimationEnabled);
#endif
    UpdateStatus();
  }

  void ShowDialog(bool usePerModalAnimation)
  {
    AlertDialog alert = AlertDialog::New();
    alert.SetBackgroundColor(UiColor(0xFFFFFFu));
    alert.SetSpacing(8.0f);
    alert.SetTitle("Delete item?");
    alert.SetMessage("This action cannot be undone.");
    alert.SetActionButtons({{"Cancel", [this]() { mNavigator.PopModal(mModalAnimationEnabled); }},
                            {"Delete", [this]() { mNavigator.PopModal(mModalAnimationEnabled); }}});

    // Center the card over the scrim.
    const float dialogWidth  = 600.0f;
    const float dialogHeight = 340.0f;
    alert.SetLayoutParams(AbsoluteLayoutParams::New()
                            .SetBounds(LayoutRect(0.5f, 0.5f, dialogWidth, dialogHeight))
                            .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));

    DialogContainer container = DialogContainer::New();
    container.SetModalContent(alert);

    if(usePerModalAnimation)
    {
#ifdef DALI_UI_NAVIGATOR_HAS_MODAL_TRANSITION_SPEC
      mNavigator.SetPageModalTransitionSpec(container, CreatePerModalBounceTransitionSpec());
#else
      // Compatibility path for running the sample against an older installed
      // dali-ui-components package. Current Navigator builds use the modal-only
      // API above so page and modal transition specs stay separated.
      mNavigator.SetPageTransitionSpec(container, CreatePerModalBounceTransitionSpec());
#endif
    }

    mNavigator.PushModal(container, mModalAnimationEnabled);
    UpdateStatus();
  }

  void OnTransitionFinished(Navigator /*navigator*/)
  {
    UpdateStatus();
  }

  void UpdateStatus()
  {
    const std::string text = "nav pages: " + std::to_string(mNavigator.GetNavigationStackCount()) +
                             "   modals: " + std::to_string(mNavigator.GetModalStackCount()) +
                             "   page animation: " + (mPageAnimationEnabled ? "on" : "off") +
                             "   modal animation: " + (mModalAnimationEnabled ? "on" : "off");
    mStatus.SetText(text.c_str());
  }

  Label MakeText(const char* text, float fontSize, float height)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(height);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(0x202124u));
    label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    return label;
  }

  StackLayout MakeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(72.0f);
    row.SetSpacing(8.0f);
    row.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    return row;
  }

  InteractiveView MakeButton(const char* text, const UiColor& color, std::function<void()> onClicked)
  {
    InteractiveView button = InteractiveView::New();
    button.SetBackgroundColor(color);
    button.SetCornerRadius(12.0f);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    Label label = Label::New(text);
    label.SetFontSize(13.0f);
    label.SetTextColor(UiColor(0xFFFFFFu));
    label.SetRequestedPositionX(10.0f);
    label.SetRequestedPositionY(28.0f);
    button.Add(label);

    button.ConnectClickedSignal(this, [onClicked](View, InputEvent) { onClicked(); });
    return button;
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        if(mNavigator.GetModalStackCount() > 0u)
        {
          mNavigator.PopModal(mModalAnimationEnabled);
        }
        else if(mNavigator.GetNavigationStackCount() > 1u)
        {
          mNavigator.Pop(mPageAnimationEnabled);
        }
        else
        {
          mApplication.Quit();
        }
      }
    }
  }

  Application& mApplication;
  Navigator    mNavigator;
  Label        mStatus;
  int          mNextPageNumber{1};
  bool         mPageAnimationEnabled{true};
  bool         mModalAnimationEnabled{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  NavigatorExample test(application);
  application.MainLoop();
  return 0;
}
