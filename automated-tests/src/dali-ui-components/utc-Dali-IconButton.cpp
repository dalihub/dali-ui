/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <dali-ui-components/dali-ui-components.h>
#include <limits>
#include <dali-ui-components/public-api/styles/icon-button-style.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali-ui/ui-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_icon_button_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_icon_button_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliIconButtonStylePaddingValidationN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  const float invalid[] = {-3.0f, std::numeric_limits<float>::quiet_NaN(),
                           std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for(float value : invalid)
  {
    DALI_TEST_ASSERTION(IconButtonStyle::Builder().SetPadding(Insets(value, 0.0f, 0.0f, 0.0f)), "padding must be finite and non-negative");
    DALI_TEST_ASSERTION(IconButtonStyle::Builder().SetPadding(Insets(0.0f, value, 0.0f, 0.0f)), "padding must be finite and non-negative");
    DALI_TEST_ASSERTION(IconButtonStyle::Builder().SetPadding(Insets(0.0f, 0.0f, value, 0.0f)), "padding must be finite and non-negative");
    DALI_TEST_ASSERTION(IconButtonStyle::Builder().SetPadding(Insets(0.0f, 0.0f, 0.0f, value)), "padding must be finite and non-negative");
  }
  END_TEST;
}

int UtcDaliIconButtonStyleIconDimensionValidationN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  const float invalid[] = {-3.0f, std::numeric_limits<float>::quiet_NaN(),
                           std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for(float value : invalid)
  {
    DALI_TEST_ASSERTION(IconButtonStyle::Builder().SetIconWidth(value), "icon dimension must be finite and non-negative or a layout sentinel");
    DALI_TEST_ASSERTION(IconButtonStyle::Builder().SetIconHeight(value), "icon dimension must be finite and non-negative or a layout sentinel");
  }
  END_TEST;
}

int UtcDaliIconButtonRuntimeNumericValidationN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton button = IconButton::New();
  const float invalid[] = {-3.0f, std::numeric_limits<float>::quiet_NaN(),
                           std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for(float value : invalid)
  {
    DALI_TEST_ASSERTION(button.SetIconWidth(value), "icon dimension must be finite and non-negative or a layout sentinel");
    DALI_TEST_ASSERTION(button.SetIconHeight(value), "icon dimension must be finite and non-negative or a layout sentinel");
    DALI_TEST_ASSERTION(button.SetIconCornerRadius(Vector4(value, 0.0f, 0.0f, 0.0f)), "corner radius must be finite and non-negative");
    DALI_TEST_ASSERTION(button.SetIconCornerRadius(Vector4(0.0f, value, 0.0f, 0.0f)), "corner radius must be finite and non-negative");
    DALI_TEST_ASSERTION(button.SetIconCornerRadius(Vector4(0.0f, 0.0f, value, 0.0f)), "corner radius must be finite and non-negative");
    DALI_TEST_ASSERTION(button.SetIconCornerRadius(Vector4(0.0f, 0.0f, 0.0f, value)), "corner radius must be finite and non-negative");
  }
  END_TEST;
}

int UtcDaliIconButtonNumericBoundariesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  const float valid[] = {0.0f, 24.0f, WRAP_CONTENT, MATCH_PARENT};
  for(float value : valid)
  {
    auto style = IconButtonStyle::Builder().SetIconWidth(value).SetIconHeight(value).SetPadding(Insets(0.0f, 0.0f)).Build();
    IconButton button = IconButton::New(style);
    DALI_TEST_EQUALS(button.GetIconWidth(), value, TEST_LOCATION);
    DALI_TEST_EQUALS(button.GetIconHeight(), value, TEST_LOCATION);
    DALI_TEST_EQUALS(button.GetPadding(), Insets(0.0f, 0.0f), TEST_LOCATION);
    button.SetIconWidth(32.0f);
    button.SetIconHeight(32.0f);
    button.SetIconWidth(value);
    button.SetIconHeight(value);
    button.SetIconCornerRadius(Vector4::ZERO);
    DALI_TEST_EQUALS(button.GetIconWidth(), value, TEST_LOCATION);
    DALI_TEST_EQUALS(button.GetIconHeight(), value, TEST_LOCATION);
    DALI_TEST_EQUALS(button.GetIconCornerRadius(), Vector4::ZERO, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliIconButtonNewP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton        button = IconButton::New("icon.png");

  DALI_TEST_CHECK(button);
  DALI_TEST_CHECK(InteractiveView::DownCast(button));
  DALI_TEST_EQUALS(button.GetIconUrl(), std::string("icon.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconColor(), UiColor::ON_SURFACE_CONTAINER_HIGHEST, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonRuntimeIconPropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton        button = IconButton::New();

  button.SetIconUrl("updated-icon.png");
  button.SetIconWidth(24.0f);
  button.SetIconHeight(20.0f);
  button.SetIconColor(UiColor(Vector4(0.1f, 0.2f, 0.3f, 1.0f)));
  button.SetIconCornerRadius(Vector4(1.0f, 2.0f, 3.0f, 4.0f));

  DALI_TEST_EQUALS(button.GetIconUrl(), std::string("updated-icon.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconWidth(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconHeight(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconCornerRadius(), Vector4(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonArrangeIconP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton        button = IconButton::New("icon.png", IconButtonStyle::BackPreset());
  button.SetRequestedWidth(52.0f);
  button.SetRequestedHeight(52.0f);
  application.GetScene().Add(button);

  button.Measure(52.0f, 52.0f);
  button.Arrange(LayoutRect(0.0f, 0.0f, 52.0f, 52.0f));

  ImageView icon = ImageView::DownCast(button.GetChildAt(0u));
  DALI_TEST_CHECK(icon);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::POSITION_X), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::POSITION_Y), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::SIZE_WIDTH), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(icon.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonPresetStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  IconButtonStyle defaultStyle = IconButtonStyle::DefaultPreset();
  IconButtonStyle addStyle     = IconButtonStyle::AddPreset();
  IconButtonStyle backStyle    = IconButtonStyle::BackPreset();
  IconButtonStyle moreStyle    = IconButtonStyle::MorePreset();

  const std::string defaultUrl = defaultStyle.GetIconUrl().CStr();
  const std::string addUrl     = addStyle.GetIconUrl().CStr();
  DALI_TEST_EQUALS(defaultStyle.GetIconWidth(), 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultStyle.GetIconHeight(), 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(backStyle.GetIconWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(backStyle.GetIconHeight(), 40.0f, TEST_LOCATION);
  const std::string backUrl = backStyle.GetIconUrl().CStr();
  const std::string moreUrl = moreStyle.GetIconUrl().CStr();
  constexpr char    addSuffix[]     = "/components/ic_add.svg";
  constexpr char    backSuffix[]    = "/components/ic_navi_back.svg";
  constexpr char    moreSuffix[]    = "/components/ic_more.svg";
  DALI_TEST_EQUALS(defaultUrl, std::string(), TEST_LOCATION);
  DALI_TEST_CHECK(addUrl.size() >= sizeof(addSuffix) - 1u);
  DALI_TEST_CHECK(backUrl.size() >= sizeof(backSuffix) - 1u);
  DALI_TEST_CHECK(moreUrl.size() >= sizeof(moreSuffix) - 1u);
  DALI_TEST_EQUALS(addUrl.compare(addUrl.size() - (sizeof(addSuffix) - 1u), sizeof(addSuffix) - 1u, addSuffix), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(backUrl.compare(backUrl.size() - (sizeof(backSuffix) - 1u), sizeof(backSuffix) - 1u, backSuffix), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(moreUrl.compare(moreUrl.size() - (sizeof(moreSuffix) - 1u), sizeof(moreSuffix) - 1u, moreSuffix), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonNewWithPresetP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton        button = IconButton::New(IconButtonStyle::BackPreset());

  const std::string iconUrl = button.GetIconUrl().CStr();
  constexpr char    suffix[] = "/components/ic_navi_back.svg";
  DALI_TEST_CHECK(iconUrl.size() >= sizeof(suffix) - 1u);
  DALI_TEST_EQUALS(iconUrl.compare(iconUrl.size() - (sizeof(suffix) - 1u), sizeof(suffix) - 1u, suffix), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconWidth(), 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconHeight(), 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonNewWithCustomStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButtonStyle   style = IconButtonStyle::Builder()
                            .SetIconUrl("custom-icon.svg")
                            .SetIconWidth(28.0f)
                            .SetIconHeight(32.0f)
                            .Build();

  IconButton button = IconButton::New(style);

  DALI_TEST_EQUALS(button.GetIconUrl(), std::string("custom-icon.svg"), TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconWidth(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetIconHeight(), 32.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonStyleDefaultKeyP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  IconButtonStyle style = IconButtonStyle::Default();

  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetIconUrl(), std::string(), TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconWidth(), 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetIconHeight(), 56.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonDisabledAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton button = IconButton::New();
  auto* accessible = Dali::Accessibility::Accessible::Get(button);
  DALI_TEST_CHECK(accessible);
  int calls = 0;
  button.ClickedSignal().Connect(&application, [&calls](View, InputEvent) { ++calls; });
  Property::Map attributes;
  DALI_TEST_CHECK(button.HasAccessibilityState(Ui::Accessibility::State::ENABLED));
  button.SetEnabled(false);
  DALI_TEST_CHECK(!button.HasAccessibilityState(Ui::Accessibility::State::ENABLED));
  DALI_TEST_CHECK(!accessible->GetStates()[Dali::Integration::Accessibility::State::ENABLED]);
  button.DoAction("activate", attributes);
  DALI_TEST_EQUALS(calls, 0, TEST_LOCATION);
  button.SetEnabled(true);
  DALI_TEST_CHECK(button.HasAccessibilityState(Ui::Accessibility::State::ENABLED));
  DALI_TEST_CHECK(accessible->GetStates()[Dali::Integration::Accessibility::State::ENABLED]);
  button.DoAction("activate", attributes);
  DALI_TEST_EQUALS(calls, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonUnspecifiedSizeFallbackP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  IconButton button = IconButton::New(IconButtonStyle::Builder().Build());
  MeasuredSize size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 56.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonNaturalSizeAndUrlChangeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 32, 16);
  ImageUrl url = ImageUrl::New(texture);
  IconButton button = IconButton::New(IconButtonStyle::Builder().SetIconUrl(url.GetUrl()).Build());
  application.GetScene().Add(button);
  application.SendNotification();
  application.Render();
  MeasuredSize size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 32.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 16.0f, TEST_LOCATION);
  button.Arrange(LayoutRect(0.0f, 0.0f, size.width, size.height));
  DALI_TEST_EQUALS(button.GetChildAt(0u).GetProperty<float>(Actor::Property::SIZE_WIDTH), 32.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(button.GetChildAt(0u).GetProperty<float>(Actor::Property::SIZE_HEIGHT), 16.0f, TEST_LOCATION);

  Texture secondTexture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 12, 24);
  ImageUrl secondUrl = ImageUrl::New(secondTexture);
  button.SetIconUrl(secondUrl.GetUrl());
  application.SendNotification();
  application.Render();
  size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 24.0f, TEST_LOCATION);
  button.SetIconUrl("");
  size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 56.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonNaturalAspectAndExplicitZeroP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 32, 16);
  ImageUrl url = ImageUrl::New(texture);
  IconButton button = IconButton::New(IconButtonStyle::Builder().SetIconUrl(url.GetUrl()).SetIconWidth(48.0f).Build());
  application.GetScene().Add(button);
  application.SendNotification();
  application.Render();
  MeasuredSize size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 48.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 24.0f, TEST_LOCATION);
  button.SetIconWidth(WRAP_CONTENT);
  button.SetIconHeight(20.0f);
  size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 20.0f, TEST_LOCATION);
  button.SetIconWidth(0.0f);
  size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 20.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonStyleConfigurePreservesIndependentValuesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto original = IconButtonStyle::Builder().SetIconUrl("original.png").SetIconHeight(24.0f)
                    .SetIconColor(UiColor(0xFF0000u)).SetPadding(Insets(2.0f, 3.0f)).Build();
  auto changed = original.Configure().SetIconUrl("changed.png").SetIconHeight(30.0f).Build();
  DALI_TEST_EQUALS(original.GetIconUrl(), std::string("original.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetIconHeight(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(changed.GetIconUrl(), std::string("changed.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(changed.GetIconHeight(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(changed.GetIconWidth(), WRAP_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(changed.GetIconColor(), UiColor(0xFF0000u), TEST_LOCATION);
  DALI_TEST_EQUALS(changed.GetPadding(), Insets(2.0f, 3.0f), TEST_LOCATION);
  auto presetCopy = IconButtonStyle::BackPreset().Configure().Build();
  IconButton button = IconButton::New(presetCopy);
  auto size = button.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 52.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 52.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonAsyncNaturalSizeInvalidatesCacheP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  const std::string sourceFile = __FILE__;
  const std::string url = sourceFile.substr(0, sourceFile.find_last_of('/')) + "/../../../samples/image-view/res/svg-blocks.svg";
  IconButton button = IconButton::New(IconButtonStyle::Builder().SetIconUrl(Dali::String(url.c_str())).Build());
  button.SetRequestedWidth(300.0f);
  button.SetRequestedHeight(300.0f);
  application.GetScene().Add(button);
  ImageView icon = ImageView::DownCast(button.GetChildAt(0u));
  button.Measure(300.0f, 300.0f);
  MeasuredSize size = icon.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 56.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 56.0f, TEST_LOCATION);
  // SVG rasterization starts once the image has nonzero arranged bounds.
  button.Arrange(LayoutRect(0.0f, 0.0f, 300.0f, 300.0f));
  for(int attempt = 0; attempt < 4 && icon.GetLoadingStatus() == Ui::Visual::ResourceStatus::PREPARING; ++attempt)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    // LoadComplete requests layout before SVG rasterization can be scheduled.
    // The fixed parent keeps the child's measure constraints at 300x300.
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(icon.GetLoadingStatus() == Ui::Visual::ResourceStatus::READY);
  // The established SVG renderer double reports 100x100. The contract under
  // test is the component's cached fallback-to-natural transition, not SVG decoding.
  // No manual invalidation or changed constraint may make this second measure work.
  size = icon.Measure(300.0f, 300.0f);
  DALI_TEST_EQUALS(size.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliIconButtonNaturalSizeScaleAndPaddingP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto scaleManager = UiScaleManager::Get();
  const float originalScale = scaleManager.GetScale();
  const bool originalScalable = scaleManager.IsScalable();
  scaleManager.SetScalable(true);
  scaleManager.SetScale(2.0f);
  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 32, 16);
  ImageUrl url = ImageUrl::New(texture);
  IconButton button = IconButton::New(IconButtonStyle::Builder().SetIconUrl(url.GetUrl()).SetPadding(Insets(2.0f, 3.0f)).Build());
  application.GetWindow().Add(button);
  application.SendNotification();
  application.Render();
  const auto size = button.Measure(300.0f, 300.0f);
  button.Arrange(LayoutRect(0.0f, 0.0f, size.width, size.height));
  Actor icon = button.GetChildAt(0u);
  const float iconWidth = icon.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  const float iconHeight = icon.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  scaleManager.SetScale(originalScale);
  scaleManager.SetScalable(originalScalable);
  DALI_TEST_EQUALS(size.width, 72.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 44.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(iconWidth, 64.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(iconHeight, 32.0f, TEST_LOCATION);
  END_TEST;
}
