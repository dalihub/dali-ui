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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <test-gesture-generator.h>

#include <cmath>
#include <cstring>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

namespace Test
{
auto GetTimerCount() -> int;
void EmitGlobalTimerSignal();
} // namespace Test

void utc_dali_toast_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toast_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
constexpr uint32_t POST_FINISH_MS    = 600u;
constexpr uint32_t EXPAND_FINISH_MS  = 450u;
constexpr uint32_t DISMISS_FINISH_MS = 200u;

struct LifecycleData
{
  int                shownCount{0};
  int                hiddenCount{0};
  int                actionCount{0};
  Toast::ActionStage lastStage{Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE};
  Toast              lastSender;
};

void ObserveLifecycle(Toast toast, LifecycleData& data, ConnectionTrackerInterface* tracker)
{
  toast.ShownSignal().Connect(tracker, [&data](Toast sender)
  {
    ++data.shownCount;
    data.lastSender = sender;
  });
  toast.HiddenSignal().Connect(tracker, [&data](Toast sender)
  {
    ++data.hiddenCount;
    data.lastSender = sender;
  });
  toast.ActionButtonClickedSignal().Connect(tracker, [&data](Toast sender, Toast::ActionStage stage)
  {
    ++data.actionCount;
    data.lastStage  = stage;
    data.lastSender = sender;
  });
}

View GetContent(Toast toast)
{
  return View::DownCast(toast.GetChildAt(0u));
}

Label GetMessage(Toast toast)
{
  return Label::DownCast(GetContent(toast).GetChildAt(0u));
}

TextButton GetAction(Toast toast)
{
  return TextButton::DownCast(GetContent(toast).GetChildAt(1u));
}

void Advance(UiTestApplication& application, uint32_t milliseconds)
{
  application.SendNotification();
  application.Render(0u);
  application.Render(milliseconds);
  application.SendNotification();
}

void PressReturn(UiTestApplication& application, uint32_t time)
{
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, time, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, time + 20u, Dali::Integration::KeyEvent::UP, "", "", Device::Class::KEYBOARD, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);
}

float Width(Actor actor)
{
  return actor.GetProperty<Vector3>(Actor::Property::SIZE).width;
}

float Height(Actor actor)
{
  return actor.GetProperty<Vector3>(Actor::Property::SIZE).height;
}

float PositionX(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_X);
}

float PositionY(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_Y);
}

bool gUseArabicLocalization = false;

bool EqualsStringView(StringView value, const char* text)
{
  const std::size_t length = std::strlen(text);
  return value.Size() == length && value.Data() && std::strncmp(value.Data(), text, length) == 0;
}

bool OverrideToastLocalization(StringView resourceId, StringView, Dali::String& localized)
{
  if(EqualsStringView(resourceId, "IDS_TOAST_MESSAGE"))
  {
    localized = gUseArabicLocalization ? "تم حفظ الإعدادات" : "설정을 저장했습니다";
    return true;
  }
  if(EqualsStringView(resourceId, "IDS_TOAST_ACTION"))
  {
    localized = gUseArabicLocalization ? "تراجع" : "실행 취소";
    return true;
  }
  return false;
}

void ApplyToastMessage(BaseHandle target, const Dali::String& localized)
{
  Toast toast = Toast::DownCast(target);
  if(toast)
  {
    toast.SetText(localized);
  }
}

void ApplyToastAction(BaseHandle target, const Dali::String& localized)
{
  Toast toast = Toast::DownCast(target);
  if(toast)
  {
    toast.SetActionButtonText(localized);
  }
}
} // namespace

int UtcDaliToastNewCopyMoveDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             empty       = Toast::New();
  Toast             styled      = Toast::New(ToastStyle::Default());
  Toast             text        = Toast::New("Message");
  Toast             textStyle   = Toast::New("Styled", ToastStyle::Default());
  Toast             finite      = Toast::New("Short", Toast::Duration::SHORT);
  Toast             finiteStyle = Toast::New("Long", Toast::Duration::LONG, ToastStyle::Default());

  DALI_TEST_CHECK(empty && styled && text && textStyle && finite && finiteStyle);
  DALI_TEST_EQUALS(text.GetText(), Dali::String("Message"), TEST_LOCATION);
  DALI_TEST_EQUALS(finite.GetDuration(), Toast::Duration::SHORT, TEST_LOCATION);
  DALI_TEST_EQUALS(finiteStyle.GetDuration(), Toast::Duration::LONG, TEST_LOCATION);

  Toast copy(text);
  DALI_TEST_CHECK(copy == text);
  Toast moved(std::move(copy));
  DALI_TEST_CHECK(moved == text);
  DALI_TEST_CHECK(Toast::DownCast(BaseHandle(text)) == text);
  DALI_TEST_CHECK(!Toast::DownCast(BaseHandle(View::New())));
  DALI_TEST_CHECK(!Toast::DownCast(BaseHandle()));
  END_TEST;
}

int UtcDaliToastStyleDefaultsAndBuilderP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ToastStyle        defaults = ToastStyle::Default();

  DALI_TEST_CHECK(defaults);
  DALI_TEST_EQUALS(defaults.GetInitialWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetInitialHeight(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMinimumWidth(), 72.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMinimumHeight(), 72.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetExpandedHeight(), 141.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMaximumHeight(), 182.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMaximumWidthRatio(), 0.68f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMaximumWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBottomOffsetRatio(), 0.05f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBottomOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetCornerRadius(), Vector4(36.0f, 36.0f, 36.0f, 36.0f), TEST_LOCATION);
  DALI_TEST_CHECK(defaults.GetCornerRadiusPolicy() == CornerRadiusPolicy::ABSOLUTE);
  DALI_TEST_EQUALS(defaults.GetPadding(), Insets(32.0f, 8.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetExpandedPadding(), Insets(32.0f, 32.0f, 16.0f, 16.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetItemSpacing(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetExpandedItemSpacing(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetFontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetFontFamily(), Dali::String("SamsungOneUI400"), TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBorderlineWidth(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBorderlineOffset(), -1.0f, TEST_LOCATION);
  DALI_TEST_CHECK(defaults.GetActionButtonStyle());

  const ShadowStack& shadows = defaults.GetShadow();
  DALI_TEST_EQUALS(shadows.GetShadowCount(), 2u, TEST_LOCATION);
  for(uint32_t index = 0u; index < shadows.GetShadowCount(); ++index)
  {
    DALI_TEST_CHECK(shadows.GetShadowAt(index).GetCutoutPolicy() == CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  }

  ToastStyle custom = defaults.Configure()
                        .SetInitialSize(12.0f, 13.0f)
                        .SetMinimumSize(80.0f, 81.0f)
                        .SetExpandedHeight(150.0f)
                        .SetMaximumHeight(200.0f)
                        .SetMaximumWidthRatio(0.75f)
                        .SetMaximumWidth(500.0f)
                        .SetBottomOffsetRatio(0.1f)
                        .SetBottomOffset(9.0f)
                        .SetCornerRadius(Vector4(7.0f, 7.0f, 7.0f, 7.0f))
                        .SetPadding(Insets(1.0f, 2.0f, 3.0f, 4.0f))
                        .SetExpandedPadding(Insets(5.0f, 6.0f, 7.0f, 8.0f))
                        .SetItemSpacing(11.0f)
                        .SetExpandedItemSpacing(12.0f)
                        .SetBackgroundColor(UiColor(Color::YELLOW))
                        .SetTextColor(UiColor(Color::RED))
                        .SetFontSize(20.0f)
                        .SetFontFamily("TestFont")
                        .SetBorderlineWidth(3.0f)
                        .SetBorderlineOffset(0.5f)
                        .Build();

  DALI_TEST_EQUALS(custom.GetInitialWidth(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetInitialHeight(), 13.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetMinimumWidth(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetMinimumHeight(), 81.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetExpandedHeight(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetMaximumHeight(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetMaximumWidthRatio(), 0.75f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetMaximumWidth(), 500.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBottomOffsetRatio(), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBottomOffset(), 9.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetItemSpacing(), 11.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetExpandedItemSpacing(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetFontFamily(), Dali::String("TestFont"), TEST_LOCATION);

  DALI_TEST_CHECK(ToastStyle::DownCast(defaults));
  DALI_TEST_CHECK(ToastStyle::StaticDownCast(UiStyle(custom)) == custom);
  DALI_TEST_CHECK(!ToastStyle::DownCast(BaseHandle(View::New())));

  Toast customToast = Toast::New("Styled", custom);
  DALI_TEST_EQUALS(customToast.GetBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(customToast.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(customToast.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetContent(customToast).GetPadding(), Insets(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastStyleValidationN(void)
{
  UiTestApplication application(Components::UiConfig::New());

  DALI_TEST_ASSERTION(ToastStyle::Builder().SetInitialWidth(0.0f), "ToastStyle initial width must be finite and positive");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetMaximumWidthRatio(1.1f), "ToastStyle maximum width ratio must be finite, positive, and no greater than one");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetPadding(Insets(-1.0f, 0.0f, 0.0f, 0.0f)), "ToastStyle padding must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetBorderlineOffset(2.0f), "ToastStyle borderline offset must be finite and between -1 and 1");
  DALI_TEST_ASSERTION(
    ToastStyle::Builder().SetExpandedHeight(200.0f).SetMaximumHeight(199.0f).Build(),
    "maximum height must be greater than or equal to expanded height");
  END_TEST;
}

int UtcDaliToastPropertiesAndContentP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast = Toast::New();

  DALI_TEST_EQUALS(toast.GetDuration(), Toast::Duration::INDEFINITE, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetActionButtonText(), Dali::String("Button"), TEST_LOCATION);

  toast.SetText("새 메시지");
  toast.SetDuration(321u);
  toast.SetTextColor(UiColor(Color::GREEN));
  toast.SetFontSize(19.0f);
  toast.SetFontFamily("TestFont");
  toast.SetActionButtonText("확인");
  toast.SetActionButtonTextColor(UiColor(Color::BLUE));
  toast.SetItemSpacing(17.0f);

  DALI_TEST_EQUALS(toast.GetText(), Dali::String("새 메시지"), TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetDuration(), 321u, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetTextColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetFontSize(), 19.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetFontFamily(), Dali::String("TestFont"), TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetActionButtonText(), Dali::String("확인"), TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetActionButtonTextColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetItemSpacing(), 17.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetMessage(toast).GetText(), Dali::String("새 메시지"), TEST_LOCATION);
  DALI_TEST_EQUALS(GetAction(toast).GetText(), Dali::String("확인"), TEST_LOCATION);

  toast.SetActionButtonText("");
  DALI_TEST_CHECK(!GetAction(toast).IsVisible());
  DALI_TEST_CHECK(GetAction(toast).IsAccessibilityHidden());
  DALI_TEST_ASSERTION(toast.SetFontSize(-1.0f), "font size must be finite and non-negative");
  DALI_TEST_ASSERTION(toast.SetItemSpacing(-1.0f), "item spacing must be finite and non-negative");
  END_TEST;
}

int UtcDaliToastEmptyWindowDoesNotReplacePresentationN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window  = application.GetWindow();
  Layer             overlay = window.GetOverlayLayer();
  Toast             toast   = Toast::New("Message");

  toast.Post(window);
  DALI_TEST_CHECK(toast.GetParent() == overlay);
  DALI_TEST_ASSERTION(toast.Post(Window()), "Toast::Post requires an initialized Window");
  DALI_TEST_CHECK(toast.GetParent() == overlay);
  END_TEST;
}

int UtcDaliToastPostShowDismissSignalsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window  = application.GetWindow();
  Layer             overlay = window.GetOverlayLayer();
  Toast             toast   = Toast::New("Message");
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  DALI_TEST_CHECK(toast.GetParent() == overlay);
  DALI_TEST_CHECK(toast.IsAccessibilityHidden());
  DALI_TEST_EQUALS(data.shownCount, 0, TEST_LOCATION);

  Advance(application, POST_FINISH_MS);
  DALI_TEST_EQUALS(data.shownCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.lastSender == toast);
  DALI_TEST_CHECK(!toast.IsAccessibilityHidden());
  DALI_TEST_EQUALS(toast.GetProperty<float>(Actor::Property::OPACITY), 1.0f, 0.001f, TEST_LOCATION);

  toast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!toast.GetParent());
  DALI_TEST_CHECK(toast.IsAccessibilityHidden());

  toast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastDurationTimerP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window             = application.GetWindow();
  const int         baselineTimerCount = Test::GetTimerCount();

  Toast indefinite = Toast::New("Indefinite");
  indefinite.Post(window);
  DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount, TEST_LOCATION);
  Test::EmitGlobalTimerSignal();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_CHECK(indefinite.GetParent());
  indefinite.Dismiss();
  Advance(application, DISMISS_FINISH_MS);

  Toast         finite = Toast::New("Finite", 321u);
  LifecycleData data;
  ObserveLifecycle(finite, data, &application);
  finite.Post(window);
  DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount + 1, TEST_LOCATION);
  Test::EmitGlobalTimerSignal();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!finite.GetParent());
  DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastDefaultContentFillsHeightAndCentersMessageP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast   = Toast::New("Message");
  View              content = GetContent(toast);
  Label             message = GetMessage(toast);

  DALI_TEST_CHECK(message);
  toast.Measure(320.0f, 72.0f);
  toast.Arrange(LayoutRect(0.0f, 0.0f, 320.0f, 72.0f));

  DALI_TEST_EQUALS(Height(content), Height(toast), 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(message.GetVerticalTextAlignment() == Text::Alignment::CENTER);
  END_TEST;
}

int UtcDaliToastResponsiveDefaultAndExpandedGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 600, 900));
  Toast toast = Toast::New(
    "This is a deliberately long Toast message that must wrap across multiple lines while remaining within the approved expanded height boundary.");

  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  const float defaultWidth  = Width(toast);
  const float defaultHeight = Height(toast);
  DALI_TEST_CHECK(defaultWidth <= 600.0f * 0.68f + 0.001f);
  DALI_TEST_CHECK(defaultHeight >= 72.0f);
  DALI_TEST_EQUALS(PositionX(toast), (600.0f - defaultWidth) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionY(toast), 900.0f - 45.0f - defaultHeight, 0.001f, TEST_LOCATION);

  Property::Map attributes;
  DALI_TEST_CHECK(GetAction(toast).DoAction("activate", attributes));
  Advance(application, EXPAND_FINISH_MS);

  DALI_TEST_EQUALS(Width(toast), 600.0f * 0.68f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(Height(toast) >= 141.0f);
  DALI_TEST_CHECK(Height(toast) <= 182.0f);
  DALI_TEST_EQUALS(PositionX(toast), (600.0f - Width(toast)) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionY(toast), 900.0f - 45.0f - Height(toast), 0.001f, TEST_LOCATION);

  window.SetPositionSize(PositionSize(0, 0, 500, 700));
  DALI_TEST_EQUALS(Width(toast), 500.0f * 0.68f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(toast), (500.0f - Width(toast)) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionY(toast), 700.0f - 35.0f - Height(toast), 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastTwoStageActionAndAccessibilityActivationP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast = Toast::New("Message");
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);
  toast.Post(application.GetWindow());
  Advance(application, POST_FINISH_MS);

  Property::Map attributes;
  DALI_TEST_CHECK(GetAction(toast).DoAction("activate", attributes));
  DALI_TEST_EQUALS(data.actionCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.lastStage == Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE);
  DALI_TEST_CHECK(toast.GetParent());
  Advance(application, EXPAND_FINISH_MS);

  DALI_TEST_CHECK(GetAction(toast).DoAction("activate", attributes));
  DALI_TEST_EQUALS(data.actionCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(data.lastStage == Toast::ActionStage::CONFIRM_IN_EXPANDED_MODE);
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!toast.GetParent());
  END_TEST;
}

int UtcDaliToastActionTouchAndEnterP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  Toast         touchToast = Toast::New("Touch");
  LifecycleData touchData;
  ObserveLifecycle(touchToast, touchData, &application);
  touchToast.Post(application.GetWindow());
  Advance(application, POST_FINISH_MS);
  TextButton  touchAction   = GetAction(touchToast);
  const float actionCenterX = PositionX(touchToast) + PositionX(touchAction) + Width(touchAction) * 0.5f;
  const float actionCenterY = PositionY(touchToast) + PositionY(touchAction) + Height(touchAction) * 0.5f;
  TestGenerateTap(application, actionCenterX, actionCenterY, 100u);
  DALI_TEST_EQUALS(touchData.actionCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(touchData.lastStage == Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE);

  Toast         keyToast = Toast::New("Keyboard");
  LifecycleData keyData;
  ObserveLifecycle(keyToast, keyData, &application);
  keyToast.Post(application.GetWindow());
  Advance(application, POST_FINISH_MS);
  TextButton keyAction = GetAction(keyToast);
  keyAction.SetFocusable(true);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(keyAction));
  application.SendNotification();
  application.Render();
  PressReturn(application, 300u);
  DALI_TEST_EQUALS(keyData.actionCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(keyData.lastStage == Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE);

  FocusManager::Get().ClearFocus();
  END_TEST;
}

int UtcDaliToastRepostAndCoexistenceP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window           = application.GetWindow();
  Layer             overlay          = window.GetOverlayLayer();
  const uint32_t    baselineChildren = overlay.GetChildCount();
  Toast             first            = Toast::New("First");
  Toast             second           = Toast::New("Second");
  LifecycleData     firstData;
  ObserveLifecycle(first, firstData, &application);

  first.Post(window);
  second.Post(window);
  DALI_TEST_EQUALS(overlay.GetChildCount(), baselineChildren + 2u, TEST_LOCATION);
  Advance(application, POST_FINISH_MS);
  DALI_TEST_EQUALS(firstData.shownCount, 1, TEST_LOCATION);

  first.Post(window);
  DALI_TEST_EQUALS(overlay.GetChildCount(), baselineChildren + 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(firstData.hiddenCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(first.GetParent() == overlay);
  DALI_TEST_CHECK(second.GetParent() == overlay);
  Advance(application, POST_FINISH_MS);
  DALI_TEST_EQUALS(firstData.shownCount, 2, TEST_LOCATION);

  first.Dismiss();
  second.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(overlay.GetChildCount(), baselineChildren, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast   = Toast::New("Network connected");
  View              content = GetContent(toast);
  Label             message = GetMessage(toast);
  TextButton        action  = GetAction(toast);

  DALI_TEST_CHECK(toast.GetAccessibilityRole() == UiAccessibility::Role::NOTIFICATION);
  DALI_TEST_CHECK(!toast.IsAccessibilityHighlightable());
  DALI_TEST_CHECK(toast.IsAccessibilityHidden());
  DALI_TEST_CHECK(!content.IsAccessibilityHidden());
  DALI_TEST_CHECK(message.IsAccessibilityHidden());
  DALI_TEST_CHECK(action.GetAccessibilityRole() == UiAccessibility::Role::BUTTON);
  DALI_TEST_CHECK(action.IsAccessibilityHighlightable());

  auto* accessible = Dali::Accessibility::Accessible::Get(toast);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Network connected Button"), TEST_LOCATION);
  toast.SetAccessibilityName("Connection notification");
  toast.SetText("Changed");
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Connection notification"), TEST_LOCATION);

  toast.SetAccessibilityName("");
  toast.SetActionButtonText("");
  DALI_TEST_EQUALS(accessible->GetName(), std::string("Changed"), TEST_LOCATION);
  DALI_TEST_CHECK(action.IsAccessibilityHidden());
  END_TEST;
}

int UtcDaliToastExternalDetachCleansResourcesAndEmitsOnceP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window             = application.GetWindow();
  Layer             overlay            = window.GetOverlayLayer();
  const int         baselineTimerCount = Test::GetTimerCount();
  Toast             toast              = Toast::New("Finite", Toast::Duration::SHORT);
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount + 1, TEST_LOCATION);
  overlay.Remove(toast);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount, TEST_LOCATION);
  DALI_TEST_CHECK(!toast.GetParent());

  overlay.Remove(toast);
  Test::EmitGlobalTimerSignal();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastHiddenCallbackCanReleaseLastApplicationHandleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast = Toast::New("Release safely");
  bool              hiddenCalled{false};

  toast.HiddenSignal().Connect(&application, [&toast, &hiddenCalled](Toast sender)
  {
    hiddenCalled = true;
    DALI_TEST_CHECK(sender);
    toast.Reset();
  });

  toast.Post(application.GetWindow());
  Advance(application, POST_FINISH_MS);
  toast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);

  DALI_TEST_CHECK(hiddenCalled);
  DALI_TEST_CHECK(!toast);
  END_TEST;
}

int UtcDaliToastRepeatedLifecycleResourceStabilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window             = application.GetWindow();
  Layer             overlay            = window.GetOverlayLayer();
  const uint32_t    baselineChildren   = overlay.GetChildCount();
  const int         baselineTimerCount = Test::GetTimerCount();
  Toast             toast              = Toast::New("Repeated", Toast::Duration::SHORT);
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  constexpr int ITERATIONS = 50;
  for(int iteration = 0; iteration < ITERATIONS; ++iteration)
  {
    toast.Post(window);
    DALI_TEST_EQUALS(overlay.GetChildCount(), baselineChildren + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount + 1, TEST_LOCATION);
    Advance(application, POST_FINISH_MS);

    toast.Dismiss();
    Advance(application, DISMISS_FINISH_MS);
    DALI_TEST_EQUALS(overlay.GetChildCount(), baselineChildren, TEST_LOCATION);
    DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount, TEST_LOCATION);
  }

  Test::EmitGlobalTimerSignal();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.shownCount, ITERATIONS, TEST_LOCATION);
  DALI_TEST_EQUALS(data.hiddenCount, ITERATIONS, TEST_LOCATION);
  DALI_TEST_EQUALS(data.actionCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(overlay.GetChildCount(), baselineChildren, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::GetTimerCount(), baselineTimerCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastMultiWindowIsolationAndRepostP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            firstWindow    = application.GetWindow();
  Window            secondWindow   = Window::New(PositionSize(20, 30, 480, 720), "ToastSecondaryWindow");
  Layer             firstOverlay   = firstWindow.GetOverlayLayer();
  Layer             secondOverlay  = secondWindow.GetOverlayLayer();
  const uint32_t    firstBaseline  = firstOverlay.GetChildCount();
  const uint32_t    secondBaseline = secondOverlay.GetChildCount();
  Toast             movingToast    = Toast::New("Moving");
  Toast             residentToast  = Toast::New("Resident");
  LifecycleData     movingData;
  ObserveLifecycle(movingToast, movingData, &application);

  movingToast.Post(firstWindow);
  residentToast.Post(secondWindow);
  DALI_TEST_CHECK(movingToast.GetParent() == firstOverlay);
  DALI_TEST_CHECK(residentToast.GetParent() == secondOverlay);
  DALI_TEST_EQUALS(firstOverlay.GetChildCount(), firstBaseline + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(secondOverlay.GetChildCount(), secondBaseline + 1u, TEST_LOCATION);
  Advance(application, POST_FINISH_MS);

  movingToast.Post(secondWindow);
  DALI_TEST_CHECK(movingToast.GetParent() == secondOverlay);
  DALI_TEST_CHECK(residentToast.GetParent() == secondOverlay);
  DALI_TEST_EQUALS(firstOverlay.GetChildCount(), firstBaseline, TEST_LOCATION);
  DALI_TEST_EQUALS(secondOverlay.GetChildCount(), secondBaseline + 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(movingData.hiddenCount, 0, TEST_LOCATION);
  Advance(application, POST_FINISH_MS);
  DALI_TEST_EQUALS(movingData.shownCount, 2, TEST_LOCATION);

  movingToast.Dismiss();
  residentToast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(firstOverlay.GetChildCount(), firstBaseline, TEST_LOCATION);
  DALI_TEST_EQUALS(secondOverlay.GetChildCount(), secondBaseline, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastUnicodeRtlAndLocalizationBindingP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            ltrWindow = application.GetWindow();
  Window            rtlWindow = Window::New(PositionSize(0, 0, 600, 900), "ToastRtlWindow");
  Toast             ltrToast  = Toast::New("설정을 저장했습니다");
  Toast             rtlToast  = Toast::New("설정을 저장했습니다");
  ltrToast.SetActionButtonText("실행 취소");
  rtlToast.SetActionButtonText("실행 취소");
  rtlToast.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);

  ltrToast.Post(ltrWindow);
  rtlToast.Post(rtlWindow);
  Advance(application, POST_FINISH_MS);

  const View       ltrContent  = GetContent(ltrToast);
  const View       rtlContent  = GetContent(rtlToast);
  const TextButton ltrAction   = GetAction(ltrToast);
  const TextButton rtlAction   = GetAction(rtlToast);
  const float      expectedRtl = Width(rtlContent) - PositionX(ltrAction) - Width(ltrAction);
  DALI_TEST_CHECK(rtlToast.GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(Width(rtlContent), Width(ltrContent), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(rtlAction), expectedRtl, 0.001f, TEST_LOCATION);

  UiLocalizationManager localization = UiLocalizationManager::Get();
  gUseArabicLocalization             = false;
  localization.SetLocalizedStringOverride(OverrideToastLocalization);
  localization.SetBindingResource(rtlToast, "ToastMessage", "IDS_TOAST_MESSAGE", LocalizedStringCallback::New(ApplyToastMessage));
  localization.SetBindingResource(rtlToast, "ToastAction", "IDS_TOAST_ACTION", LocalizedStringCallback::New(ApplyToastAction));
  DALI_TEST_EQUALS(rtlToast.GetText(), Dali::String("설정을 저장했습니다"), TEST_LOCATION);
  DALI_TEST_EQUALS(rtlToast.GetActionButtonText(), Dali::String("실행 취소"), TEST_LOCATION);

  gUseArabicLocalization = true;
  localization.RefreshBindings();
  DALI_TEST_EQUALS(rtlToast.GetText(), Dali::String("تم حفظ الإعدادات"), TEST_LOCATION);
  DALI_TEST_EQUALS(rtlToast.GetActionButtonText(), Dali::String("تراجع"), TEST_LOCATION);

  localization.ClearBindings(rtlToast);
  localization.ClearLocalizedStringOverride();
  gUseArabicLocalization = false;
  ltrToast.Dismiss();
  rtlToast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  END_TEST;
}
