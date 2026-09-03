/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

class IconButtonExample : public ConnectionTracker
{
public:
  explicit IconButtonExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &IconButtonExample::Create);
  }

  void Create(Application application)
  {
    Components::UiConfig::New().Apply();
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(32.0f, 32.0f, 48.0f, 32.0f));
    root.SetSpacing(20.0f);

    Label title = Label::New("IconButton presets");
    title.SetFontSize(20.0f);
    title.SetTextColor(Color::BLACK);
    title.SetRequestedHeight(40.0f);
    root.Add(title);

    Label imageTitle = Label::New("Standalone ImageView (component SVG load check)");
    imageTitle.SetTextColor(Color::BLACK);
    root.Add(imageTitle);

    ImageView imageCheck = ImageView::New(IconButtonStyle::BackPreset().GetIconUrl());
    imageCheck.SetRequestedWidth(40.0f);
    imageCheck.SetRequestedHeight(40.0f);
    imageCheck.SetImageColor(UiColor::ON_SURFACE_CONTAINER_HIGHEST);
    imageCheck.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    root.Add(imageCheck);

    Label buttonTitle = Label::New("IconButton styles (enabled left / disabled right)");
    buttonTitle.SetTextColor(Color::BLACK);
    root.Add(buttonTitle);

    StackLayout buttonRow = StackLayout::New(StackOrientation::HORIZONTAL);
    buttonRow.SetRequestedWidth(MATCH_PARENT);
    buttonRow.SetRequestedHeight(96.0f);
    buttonRow.SetSpacing(16.0f);

    IconButton defaultButton = IconButton::New(IconButtonStyle::AddPreset());
    IconButton backButton    = IconButton::New(IconButtonStyle::BackPreset());
    IconButton moreButton    = IconButton::New(IconButtonStyle::MorePreset());
    IconButton disabledDefaultButton = IconButton::New(IconButtonStyle::AddPreset());
    IconButton disabledBackButton    = IconButton::New(IconButtonStyle::BackPreset());
    IconButton disabledMoreButton    = IconButton::New(IconButtonStyle::MorePreset());

    defaultButton.ClickedSignal().Connect(this, [this](View, InputEvent) { UpdateClickCount(mDefaultClickCount, "Add", ++mDefaultClicks); });
    backButton.ClickedSignal().Connect(this, [this](View, InputEvent) { UpdateClickCount(mBackClickCount, "Back", ++mBackClicks); });
    moreButton.ClickedSignal().Connect(this, [this](View, InputEvent) { UpdateClickCount(mMoreClickCount, "More", ++mMoreClicks); });

    AddButtonPair(buttonRow, defaultButton, disabledDefaultButton, mDefaultClickCount, "Add", 56.0f);
    AddButtonPair(buttonRow, backButton, disabledBackButton, mBackClickCount, "Back", 52.0f);
    AddButtonPair(buttonRow, moreButton, disabledMoreButton, mMoreClickCount, "More", 52.0f);
    root.Add(buttonRow);
    window.Add(root);
  }

private:
  void AddCounterLabel(StackLayout& parent, Label& counter, const char* name, uint32_t count)
  {
    counter = Label::New();
    counter.SetTextColor(Color::BLACK);
    UpdateClickCount(counter, name, count);
    parent.Add(counter);
  }

  void AddButtonPair(StackLayout& parent, IconButton& enabled, IconButton& disabled, Label& counter, const char* name, float size)
  {
    StackLayout column = StackLayout::New(StackOrientation::VERTICAL);
    column.SetRequestedWidth(size * 2.0f + 8.0f);
    column.SetRequestedHeight(96.0f);
    column.SetSpacing(4.0f);

    AddCounterLabel(column, counter, name, 0u);

    StackLayout pair = StackLayout::New(StackOrientation::HORIZONTAL);
    pair.SetRequestedWidth(size * 2.0f + 8.0f);
    pair.SetRequestedHeight(size);
    pair.SetSpacing(8.0f);
    enabled.SetRequestedWidth(size);
    enabled.SetRequestedHeight(size);
    disabled.SetRequestedWidth(size);
    disabled.SetRequestedHeight(size);
    disabled.SetEnabled(false);
    pair.Add(enabled);
    pair.Add(disabled);
    column.Add(pair);
    parent.Add(column);
  }

  void UpdateClickCount(Label& label, const char* name, uint32_t count)
  {
    const std::string text = std::string(name) + " clicks: " + std::to_string(count);
    label.SetText(Dali::String(text.c_str()));
  }

  Application& mApplication;
  Label        mDefaultClickCount;
  Label        mBackClickCount;
  Label        mMoreClickCount;
  uint32_t     mDefaultClicks{0u};
  uint32_t     mBackClicks{0u};
  uint32_t     mMoreClicks{0u};
};

int main(int argc, char** argv)
{
  Application       application = Application::New(&argc, &argv);
  IconButtonExample example(application);
  application.MainLoop();
  return 0;
}
