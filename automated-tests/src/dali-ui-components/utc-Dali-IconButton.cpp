/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-components/public-api/styles/icon-button-style.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>

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
