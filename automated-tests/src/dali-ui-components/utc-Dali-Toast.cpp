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
#include <dali-ui-components/internal/toast-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/public-api/adaptor-framework/input-method-context.h>

#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

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
constexpr uint32_t POST_FINISH_MS    = 400u;
constexpr uint32_t DISMISS_FINISH_MS = 400u;

struct LifecycleData
{
  int   shownCount{0};
  int   hiddenCount{0};
  Toast lastSender;
};

class ScopedUiScale
{
public:
  explicit ScopedUiScale(float scale)
  : mManager(UiScaleManager::Get()),
    mOriginalScale(mManager.GetScale())
  {
    mManager.SetScale(scale);
  }

  ~ScopedUiScale()
  {
    mManager.SetScale(mOriginalScale);
  }

  UiScaleManager GetManager() const
  {
    return mManager;
  }

private:
  UiScaleManager mManager;
  float          mOriginalScale;
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
}

Label GetMessage(Toast toast)
{
  return Label::DownCast(toast.GetChildAt(0u));
}

void Advance(UiTestApplication& application, uint32_t milliseconds)
{
  application.SendNotification();
  application.Render(0u);
  application.Render(milliseconds);
  application.SendNotification();
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
  DALI_TEST_EQUALS(empty.GetDuration(), Toast::Duration::SHORT, TEST_LOCATION);
  DALI_TEST_EQUALS(text.GetText(), Dali::String("Message"), TEST_LOCATION);
  DALI_TEST_EQUALS(finite.GetDuration(), Toast::Duration::SHORT, TEST_LOCATION);

  Toast copy(text);
  Toast moved(std::move(copy));
  DALI_TEST_CHECK(moved == text);
  DALI_TEST_CHECK(Toast::DownCast(BaseHandle(text)) == text);
  DALI_TEST_CHECK(!Toast::DownCast(BaseHandle(View::New())));
  END_TEST;
}

int UtcDaliToastInvalidArgumentsN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast = Toast::New("Message");

  DALI_TEST_ASSERTION(Toast::New(ToastStyle()), "ToastStyle must be initialized");
  DALI_TEST_ASSERTION(toast.SetDuration(0u), "Toast duration must be greater than zero");
  DALI_TEST_ASSERTION(toast.Post(Window()), "Toast::Post requires an initialized Window");
  END_TEST;
}

int UtcDaliToastStyleDefaultsAndBuilderP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ToastStyle        defaults = ToastStyle::Default();

  DALI_TEST_EQUALS(defaults.GetMaximumWidthRatio(), 0.68f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMaximumHeight(), 112.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBottomOffsetRatio(), 0.05f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetItemSpacing(), 16.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetCornerRadius(), Vector4(36.0f, 36.0f, 36.0f, 36.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetPadding(), Insets(32.0f, 32.0f, 16.0f, 16.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetIconWidth(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetIconHeight(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetIconCornerRadius(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetIconColor().GetRgba(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBorderlineWidth(), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetBorderlineOffset(), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetFontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetFontFamily(), Dali::String("SamsungOneUI400"), TEST_LOCATION);
  DALI_TEST_CHECK(defaults.GetBackgroundColor() == UiColor("SurfaceFixed"));
  DALI_TEST_CHECK(defaults.GetTextColor() == UiColor("OnSurfaceContainerFixedVariantBright"));
  DALI_TEST_CHECK(defaults.GetBorderlineColor() == UiColor("OutlineACC"));
  DALI_TEST_CHECK(defaults.GetShadow().GetShadowCount() > 0u);

  ToastStyle custom = defaults.Configure()
                        .SetMaximumWidthRatio(0.75f)
                        .SetMaximumHeight(96.0f)
                        .SetBottomOffsetRatio(0.08f)
                        .SetItemSpacing(12.0f)
                        .SetCornerRadius(Vector4(20.0f, 20.0f, 20.0f, 20.0f))
                        .SetPadding(Insets(20.0f, 20.0f, 12.0f, 12.0f))
                        .SetBackgroundColor(UiColor(Color::YELLOW))
                        .SetTextColor(UiColor(Color::RED))
                        .SetFontSize(20.0f)
                        .SetFontFamily("TestFont")
                        .SetIconWidth(28.0f)
                        .SetIconHeight(30.0f)
                        .SetIconCornerRadius(Vector4(4.0f, 4.0f, 4.0f, 4.0f))
                        .SetIconColor(UiColor(Color::GREEN))
                        .SetShadow(ShadowStack{})
                        .SetBorderlineWidth(3.0f)
                        .SetBorderlineOffset(0.0f)
                        .SetBorderlineColor(UiColor(Color::BLUE))
                        .Build();

  DALI_TEST_EQUALS(custom.GetMaximumWidthRatio(), 0.75f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetMaximumHeight(), 96.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBottomOffsetRatio(), 0.08f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetItemSpacing(), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetCornerRadius(), Vector4(20.0f, 20.0f, 20.0f, 20.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetPadding(), Insets(20.0f, 20.0f, 12.0f, 12.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetFontSize(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetFontFamily(), Dali::String("TestFont"), TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetIconWidth(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetIconHeight(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetIconCornerRadius(), Vector4(4.0f, 4.0f, 4.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetIconColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetShadow().GetShadowCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBorderlineWidth(), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBorderlineOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(custom.GetBorderlineColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_CHECK(ToastStyle::DownCast(defaults));
  DALI_TEST_CHECK(!ToastStyle::DownCast(BaseHandle(View::New())));
  DALI_TEST_CHECK(ToastStyle::StaticDownCast(UiStyle(custom)) == custom);
  END_TEST;
}

int UtcDaliToastStyleValidationN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetMaximumWidthRatio(0.0f), "maximum width ratio must be finite, positive, and no greater than one");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetMaximumWidthRatio(1.1f), "maximum width ratio must be finite, positive, and no greater than one");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetMaximumWidthRatio(std::numeric_limits<float>::quiet_NaN()), "maximum width ratio must be finite, positive, and no greater than one");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetMaximumHeight(-1.0f), "maximum height must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetBottomOffsetRatio(0.0f), "bottom offset ratio must be finite, positive, and no greater than one");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetBottomOffsetRatio(1.1f), "bottom offset ratio must be finite, positive, and no greater than one");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetItemSpacing(-1.0f), "item spacing must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetCornerRadius(Vector4(-1.0f, 0.0f, 0.0f, 0.0f)), "corner radius must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetCornerRadius(Vector4(std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f)), "corner radius must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetPadding(Insets(-1.0f, 0.0f, 0.0f, 0.0f)), "padding must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetFontSize(-1.0f), "font size must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetFontSize(std::numeric_limits<float>::quiet_NaN()), "font size must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetIconWidth(-1.0f), "icon width must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetIconHeight(-1.0f), "icon height must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetIconCornerRadius(Vector4(-1.0f, 0.0f, 0.0f, 0.0f)), "icon corner radius must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetBorderlineWidth(-1.0f), "borderline width must be finite and non-negative");
  DALI_TEST_ASSERTION(ToastStyle::Builder().SetBorderlineOffset(std::numeric_limits<float>::infinity()), "borderline offset must be finite");
  END_TEST;
}

int UtcDaliToastMessageOnlyContentP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ToastStyle        style = ToastStyle::Default()
                       .Configure()
                       .SetCornerRadius(Vector4(12.0f, 12.0f, 12.0f, 12.0f))
                       .SetBackgroundColor(UiColor(Color::BLUE))
                       .SetTextColor(UiColor(Color::RED))
                       .SetFontFamily("TestFont")
                       .SetBorderlineWidth(3.0f)
                       .SetBorderlineOffset(0.0f)
                       .SetBorderlineColor(UiColor(Color::GREEN))
                       .Build();
  Toast toast   = Toast::New("Simple message", style);
  Label message = GetMessage(toast);

  DALI_TEST_CHECK(message);
  DALI_TEST_EQUALS(toast.GetChildCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetCornerRadius(), Vector4(12.0f, 12.0f, 12.0f, 12.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(message.GetText(), Dali::String("Simple message"), TEST_LOCATION);
  DALI_TEST_EQUALS(message.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(message.GetFontFamily(), std::string("TestFont"), TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetBorderlineWidth(), 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetBorderlineOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetBorderlineColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_CHECK(message.IsMultiLine());
  DALI_TEST_EQUALS(message.GetMaximumLines(), Text::MAXIMUM_LINES_UNLIMITED, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastWrapsLongMessageWithinMaximumHeightP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ScopedUiScale     scale(2.0f);
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 1200, 1800));
  Toast toast = Toast::New("Toast messages can wrap across multiple lines. Keep them concise so they remain easy to read before the timeout.");

  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  Label       message        = GetMessage(toast);
  const float messageWidth   = message.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float messageHeight  = message.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float requiredHeight = message.GetHeightForWidth(messageWidth);

  DALI_TEST_CHECK(message.GetLineCount() > 1);
  DALI_TEST_CHECK(messageHeight <= 160.0f + 0.001f);
  DALI_TEST_CHECK(requiredHeight > messageHeight);
  DALI_TEST_EQUALS(message.GetTextOverflowMode(), Text::OverflowMode::ELLIPSIS, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastPreservesExplicitMessageLineBreaksP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ScopedUiScale     scale(2.0f);
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 1200, 1800));
  ToastStyle style = ToastStyle::Default().Configure().SetMaximumHeight(200.0f).Build();
  Toast      toast = Toast::New("A\nB\nC\nD", style);

  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  Label message = GetMessage(toast);
  DALI_TEST_EQUALS(message.GetMaximumLines(), Text::MAXIMUM_LINES_UNLIMITED, TEST_LOCATION);
  DALI_TEST_EQUALS(message.GetLineCount(), 4, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastOptionalIconP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast = Toast::New("Saved");

  DALI_TEST_EQUALS(toast.GetIconResourceUrl(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetIconColor().GetRgba(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_CHECK(!toast.IsIconSynchronousLoading());
  DALI_TEST_EQUALS(toast.GetChildCount(), 1u, TEST_LOCATION);

  toast.SetIconResourceUrl("icon.png");
  ImageView icon = ImageView::DownCast(toast.GetChildAt(0u));
  DALI_TEST_CHECK(icon);
  DALI_TEST_CHECK(Label::DownCast(toast.GetChildAt(1u)));
  DALI_TEST_EQUALS(toast.GetChildCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetResourceUrl(), Dali::String("icon.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetRequestedWidth(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetRequestedHeight(), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetCornerRadius(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_CHECK(icon.IsVisible());

  toast.SetIconColor(UiColor(Color::GREEN));
  toast.SetIconSynchronousLoading(true);
  DALI_TEST_EQUALS(toast.GetIconColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_CHECK(toast.IsIconSynchronousLoading());
  DALI_TEST_EQUALS(icon.GetImageColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_CHECK(icon.IsSynchronousLoading());

  toast.Post(application.GetWindow());
  Advance(application, POST_FINISH_MS);
  const Label message = Label::DownCast(toast.GetChildAt(1u));
  DALI_TEST_EQUALS(message.GetProperty<float>(Actor::Property::POSITION_X),
                   icon.GetProperty<float>(Actor::Property::POSITION_X) + icon.GetProperty<float>(Actor::Property::SIZE_WIDTH) + 16.0f,
                   0.001f,
                   TEST_LOCATION);

  toast.SetIconResourceUrl("");
  Advance(application, 1u);
  DALI_TEST_EQUALS(toast.GetIconResourceUrl(), Dali::String(), TEST_LOCATION);
  DALI_TEST_CHECK(!icon.IsVisible());
  const float toastWidth = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  DALI_TEST_EQUALS(message.GetProperty<float>(Actor::Property::POSITION_X), 32.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(message.GetProperty<float>(Actor::Property::SIZE_WIDTH), toastWidth - 64.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastIconVariantReservesIconSlotWithLongTextP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 600, 900));
  Toast toast = Toast::New("A long Toast message must leave a distinct icon slot before the text instead of consuming the complete content width");
  toast.SetIconResourceUrl("icon.png");

  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  const ImageView icon         = ImageView::DownCast(toast.GetChildAt(0u));
  const Label     message      = Label::DownCast(toast.GetChildAt(1u));
  const float     iconX        = icon.GetProperty<float>(Actor::Property::POSITION_X);
  const float     iconWidth    = icon.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float     messageX     = message.GetProperty<float>(Actor::Property::POSITION_X);
  const float     messageWidth = message.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float     toastWidth   = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);

  DALI_TEST_EQUALS(iconWidth, 36.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(messageX, iconX + 36.0f + 16.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(messageX + messageWidth, toastWidth - 32.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastStackLayoutMirrorsContentInRtlP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 600, 900));
  ToastStyle style = ToastStyle::Default()
                       .Configure()
                       .SetPadding(Insets(24.0f, 48.0f, 16.0f, 16.0f))
                       .Build();

  Toast ltr = Toast::New("Saved", style);
  ltr.SetIconResourceUrl("icon.png");
  ltr.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  ltr.Post(window);
  Advance(application, POST_FINISH_MS);

  ImageView   ltrIcon     = ImageView::DownCast(ltr.GetChildAt(0u));
  Label       ltrMessage  = Label::DownCast(ltr.GetChildAt(1u));
  const float ltrIconX    = ltrIcon.GetProperty<float>(Actor::Property::POSITION_X);
  const float ltrMessageX = ltrMessage.GetProperty<float>(Actor::Property::POSITION_X);
  DALI_TEST_EQUALS(ltrIconX, 24.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ltrMessageX, ltrIconX + 36.0f + 16.0f, 0.001f, TEST_LOCATION);

  Toast rtl = Toast::New("Saved", style);
  rtl.SetIconResourceUrl("icon.png");
  rtl.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  rtl.Post(window);
  Advance(application, POST_FINISH_MS);

  ImageView   rtlIcon         = ImageView::DownCast(rtl.GetChildAt(0u));
  Label       rtlMessage      = Label::DownCast(rtl.GetChildAt(1u));
  const float rtlWidth        = rtl.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float rtlIconX        = rtlIcon.GetProperty<float>(Actor::Property::POSITION_X);
  const float rtlMessageX     = rtlMessage.GetProperty<float>(Actor::Property::POSITION_X);
  const float rtlMessageWidth = rtlMessage.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  DALI_TEST_EQUALS(rtlIconX, rtlWidth - 24.0f - 36.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(rtlMessageX, 48.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(rtlMessageX + rtlMessageWidth + 16.0f, rtlIconX, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(rtlMessageX < rtlIconX);
  END_TEST;
}

int UtcDaliToastStackLayoutUsesEffectiveUiScaleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ScopedUiScale     scale(2.0f);

  Window window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 800, 1000));
  Toast toast = Toast::New("Saved");
  toast.SetIconResourceUrl("icon.png");
  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  ImageView   icon        = ImageView::DownCast(toast.GetChildAt(0u));
  Label       message     = Label::DownCast(toast.GetChildAt(1u));
  const float iconX       = icon.GetProperty<float>(Actor::Property::POSITION_X);
  const float iconWidth   = icon.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float messageX    = message.GetProperty<float>(Actor::Property::POSITION_X);
  const float toastX      = toast.GetProperty<float>(Actor::Property::POSITION_X);
  const float toastY      = toast.GetProperty<float>(Actor::Property::POSITION_Y);
  const float toastWidth  = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float toastHeight = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);

  DALI_TEST_EQUALS(iconX, 64.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(iconWidth, 72.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(messageX - iconX - iconWidth, 32.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toastX, (800.0f - toastWidth) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toastY, 1000.0f - 50.0f - toastHeight, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(toastHeight <= 224.0f + 0.001f);

  toast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  END_TEST;
}

int UtcDaliToastStackLayoutRespondsToRuntimeUiScaleChangeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ScopedUiScale     scale(1.0f);
  UiScaleManager    scaleManager = scale.GetManager();

  Window window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 800, 1000));
  Toast toast = Toast::New("Saved");
  toast.SetIconResourceUrl("icon.png");
  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  scaleManager.SetScale(2.0f);
  Advance(application, 1u);
  DALI_TEST_CHECK(toast.GetPropertyIndex("viewEffectiveScale") != Property::INVALID_INDEX);
  application.RunIdles();
  Advance(application, 1u);

  ImageView   icon        = ImageView::DownCast(toast.GetChildAt(0u));
  Label       message     = Label::DownCast(toast.GetChildAt(1u));
  const float iconX       = icon.GetProperty<float>(Actor::Property::POSITION_X);
  const float iconWidth   = icon.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float messageX    = message.GetProperty<float>(Actor::Property::POSITION_X);
  const float toastX      = toast.GetProperty<float>(Actor::Property::POSITION_X);
  const float toastY      = toast.GetProperty<float>(Actor::Property::POSITION_Y);
  const float toastWidth  = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float toastHeight = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);

  DALI_TEST_EQUALS(iconX, 64.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(iconWidth, 72.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(messageX - iconX - iconWidth, 32.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toastX, (800.0f - toastWidth) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toastY, 1000.0f - 50.0f - toastHeight, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastRepostUsesScaleChangedWhileDetachedP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ScopedUiScale     scale(1.0f);
  UiScaleManager    scaleManager = scale.GetManager();

  Window window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 800, 1000));
  Toast toast = Toast::New("Saved");
  toast.SetIconResourceUrl("icon.png");
  toast.Post(window);
  Advance(application, POST_FINISH_MS);
  toast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_CHECK(!toast.GetParent());

  scaleManager.SetScale(2.0f);
  toast.Post(window);

  ImageView   icon       = ImageView::DownCast(toast.GetChildAt(0u));
  const float toastX     = toast.GetProperty<float>(Actor::Property::POSITION_X);
  const float toastWidth = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::SIZE_WIDTH), 72.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toastX, (800.0f - toastWidth) * 0.5f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastStackLayoutClampsIconContentInTinyWindowP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 40, 80));
  Toast toast = Toast::New("Saved");
  toast.SetIconResourceUrl("icon.png");
  toast.Post(window);
  Advance(application, POST_FINISH_MS);

  ImageView   icon          = ImageView::DownCast(toast.GetChildAt(0u));
  Label       message       = Label::DownCast(toast.GetChildAt(1u));
  const float toastWidth    = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float toastHeight   = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float iconX         = icon.GetProperty<float>(Actor::Property::POSITION_X);
  const float iconY         = icon.GetProperty<float>(Actor::Property::POSITION_Y);
  const float iconWidth     = icon.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float iconHeight    = icon.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float messageX      = message.GetProperty<float>(Actor::Property::POSITION_X);
  const float messageY      = message.GetProperty<float>(Actor::Property::POSITION_Y);
  const float messageWidth  = message.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float messageHeight = message.GetProperty<float>(Actor::Property::SIZE_HEIGHT);

  DALI_TEST_CHECK(iconX >= 0.0f && iconX + iconWidth <= toastWidth + 0.001f);
  DALI_TEST_CHECK(iconY >= 0.0f && iconY + iconHeight <= toastHeight + 0.001f);
  DALI_TEST_CHECK(messageX >= 0.0f && messageX + messageWidth <= toastWidth + 0.001f);
  DALI_TEST_CHECK(messageY >= 0.0f && messageY + messageHeight <= toastHeight + 0.001f);
  END_TEST;
}

int UtcDaliToastPostDismissAndTimeoutP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window  = application.GetWindow();
  Layer             overlay = window.GetOverlayLayer();
  Toast             toast   = Toast::New("Saved", 321u);
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  DALI_TEST_CHECK(toast.GetParent() == overlay);
  DALI_TEST_CHECK(toast.IsAccessibilityHidden());
  DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);
  Advance(application, POST_FINISH_MS);
  DALI_TEST_EQUALS(data.shownCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!toast.IsAccessibilityHidden());
  DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

  Test::EmitGlobalTimerSignal();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_CHECK(!toast.GetParent());
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.lastSender == toast);

  toast.Post(window);
  Advance(application, POST_FINISH_MS);
  toast.Dismiss();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.shownCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(data.hiddenCount, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastDurationStartsWhenPostedP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Toast             toast = Toast::New("Fast", 1u);
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(application.GetWindow());
  DALI_TEST_EQUALS(Test::GetTimerCount(), 1, TEST_LOCATION);

  Test::EmitGlobalTimerSignal();
  Advance(application, DISMISS_FINISH_MS);
  DALI_TEST_EQUALS(data.shownCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastOneUiMotionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 600, 900));
  Toast         toast = Toast::New("Saved");
  LifecycleData data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  application.SendNotification();
  application.Render(0u);
  const float height   = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float restingY = 900.0f - 45.0f - height;
  const float edgeY    = 900.0f - height;
  DALI_TEST_EQUALS(toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y), edgeY, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.0f, 0.001f, TEST_LOCATION);

  application.Render(151u);
  const float postingY = toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
  DALI_TEST_CHECK(postingY > restingY && postingY < edgeY);
  DALI_TEST_CHECK(postingY < (restingY + edgeY) * 0.5f);
  DALI_TEST_CHECK(toast.GetCurrentProperty<float>(Actor::Property::OPACITY) > 0.5f);
  DALI_TEST_EQUALS(data.shownCount, 0, TEST_LOCATION);

  application.Render(151u);
  application.SendNotification();
  DALI_TEST_EQUALS(toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y), restingY, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetCurrentProperty<float>(Actor::Property::OPACITY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(data.shownCount, 1, TEST_LOCATION);

  toast.Dismiss();
  application.SendNotification();
  application.Render(151u);
  const float dismissingY = toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
  DALI_TEST_CHECK(dismissingY > restingY && dismissingY < edgeY);
  DALI_TEST_CHECK(dismissingY > (restingY + edgeY) * 0.5f);
  DALI_TEST_CHECK(toast.GetCurrentProperty<float>(Actor::Property::OPACITY) < 0.5f);
  application.Render(151u);
  application.SendNotification();
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastInputPanelOverlapGeometryP(void)
{
  const PositionSize windowBounds(100, 200, 600, 900);

  DALI_TEST_EQUALS(Dali::Ui::Internal::CalculateToastInputPanelOverlap(windowBounds, BoundsInteger(), Dali::InputMethodContext::State::WILL_SHOW), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::CalculateToastInputPanelOverlap(windowBounds, BoundsInteger(100, 800, 600, 300), Dali::InputMethodContext::State::HIDE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::CalculateToastInputPanelOverlap(windowBounds, BoundsInteger(100, 800, 600, 300), Dali::InputMethodContext::State::SHOW), 300.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::CalculateToastInputPanelOverlap(windowBounds, BoundsInteger(800, 800, 200, 300), Dali::InputMethodContext::State::SHOW), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::CalculateToastInputPanelOverlap(windowBounds, BoundsInteger(100, 1200, 600, 300), Dali::InputMethodContext::State::SHOW), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastContentChangeDuringPostingRestartsMotionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 600, 900));
  Toast         toast = Toast::New("Saved");
  LifecycleData data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  application.SendNotification();
  application.Render(100u);
  toast.SetIconResourceUrl("icon.png");
  application.SendNotification();
  application.Render(0u);

  const float height   = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const float restingY = 900.0f - 45.0f - height;
  const float edgeY    = 900.0f - height;
  DALI_TEST_EQUALS(toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y), edgeY, 0.001f, TEST_LOCATION);

  application.Render(151u);
  const float postingY = toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
  DALI_TEST_CHECK(postingY > restingY && postingY < edgeY);
  application.Render(151u);
  application.SendNotification();
  DALI_TEST_EQUALS(toast.GetCurrentProperty<float>(Actor::Property::POSITION_Y), restingY, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(data.shownCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastResponsiveGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  window.SetPositionSize(PositionSize(0, 0, 600, 900));
  ToastStyle style = ToastStyle::Default().Configure().SetMaximumWidthRatio(0.5f).SetBottomOffsetRatio(0.05f).Build();
  Toast      toast = Toast::New("A message that wraps when the available width is constrained", style);

  toast.Post(window);
  Advance(application, POST_FINISH_MS);
  float width  = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  float height = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  DALI_TEST_CHECK(width <= 300.0f + 0.001f);
  DALI_TEST_CHECK(height <= 112.0f + 0.001f);
  DALI_TEST_EQUALS(toast.GetProperty<float>(Actor::Property::POSITION_X), (600.0f - width) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetProperty<float>(Actor::Property::POSITION_Y), 900.0f - 45.0f - height, 0.001f, TEST_LOCATION);

  window.SetPositionSize(PositionSize(0, 0, 500, 700));
  width  = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  height = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  DALI_TEST_CHECK(width <= 250.0f + 0.001f);
  DALI_TEST_EQUALS(toast.GetProperty<float>(Actor::Property::POSITION_X), (500.0f - width) * 0.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(toast.GetProperty<float>(Actor::Property::POSITION_Y), 700.0f - 35.0f - height, 0.001f, TEST_LOCATION);

  window.SetPositionSize(PositionSize(0, 0, 40, 80));
  width                     = toast.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  height                    = toast.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  const Label message       = GetMessage(toast);
  const float messageX      = message.GetProperty<float>(Actor::Property::POSITION_X);
  const float messageY      = message.GetProperty<float>(Actor::Property::POSITION_Y);
  const float messageWidth  = message.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float messageHeight = message.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  DALI_TEST_CHECK(width <= 20.0f + 0.001f);
  DALI_TEST_CHECK(height <= 76.0f + 0.001f);
  DALI_TEST_CHECK(messageX >= 0.0f && messageX + messageWidth <= width + 0.001f);
  DALI_TEST_CHECK(messageY >= 0.0f && messageY + messageHeight <= height + 0.001f);
  DALI_TEST_EQUALS(toast.GetProperty<float>(Actor::Property::POSITION_Y), 80.0f - 4.0f - height, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastExternalRemovalEmitsHiddenP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window  = application.GetWindow();
  Layer             overlay = window.GetOverlayLayer();
  Toast             toast   = Toast::New("Removed externally");
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  Advance(application, POST_FINISH_MS);
  overlay.Remove(toast);
  application.SendNotification();

  DALI_TEST_EQUALS(data.shownCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(data.hiddenCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!toast.GetParent());
  END_TEST;
}

int UtcDaliToastRepostDoesNotEmitHiddenForSupersededPresentationP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Window            window = application.GetWindow();
  Toast             toast  = Toast::New("First");
  LifecycleData     data;
  ObserveLifecycle(toast, data, &application);

  toast.Post(window);
  toast.SetText("Second");
  toast.Post(window);
  Advance(application, POST_FINISH_MS);
  DALI_TEST_EQUALS(data.shownCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(data.hiddenCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetMessage(toast).GetText(), Dali::String("Second"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliToastLocalizationBindingP(void)
{
  UiTestApplication     application(Components::UiConfig::New());
  UiLocalizationManager manager = UiLocalizationManager::Get();
  manager.SetLocalizedStringOverride(OverrideToastLocalization);
  Toast toast = Toast::New();
  manager.SetBindingResource(toast, "ToastText", "IDS_TOAST_MESSAGE", LocalizedStringCallback::New(&ApplyToastMessage));

  manager.RefreshBindings();
  DALI_TEST_EQUALS(toast.GetText(), Dali::String("설정을 저장했습니다"), TEST_LOCATION);
  gUseArabicLocalization = true;
  manager.RefreshBindings();
  DALI_TEST_EQUALS(toast.GetText(), Dali::String("تم حفظ الإعدادات"), TEST_LOCATION);

  gUseArabicLocalization = false;
  manager.ClearBindings(toast);
  manager.ClearLocalizedStringOverride();
  END_TEST;
}
