/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <cstdint>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int         WINDOW_WIDTH  = 900;
constexpr int         WINDOW_HEIGHT = 720;
constexpr float       RELATIVE_LINE_HEIGHT = 1.6f;
constexpr const char* REMOTE_IMAGE =
  "https://www.w3.org/assets/logos/w3c-2025-transitional/w3c-72x48.png";
constexpr const char* FAILED_IMAGE = "https://127.0.0.1:1/image-span-failure.png";

Label NewTextLabel(const char* text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}

Label NewButton(const char* text)
{
  Label button = NewTextLabel(text, 14.0f, 0xF8FAFC);
  button.SetBackgroundColor(UiColor(0x475569));
  button.SetPadding(Extents(8u, 8u, 5u, 5u));
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetRequestedHeight(46.0f);
  button.SetCornerRadius(6.0f);
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  return button;
}

void AppendImage(Text::StyledTextBuilder& builder,
                 const char*              source,
                 const Vector2&           size,
                 float                    verticalOffset = 0.0f,
                 Text::ImageAttributes::InlineAlignment alignment =
                   Text::ImageAttributes::InlineAlignment::TEXT_CENTER)
{
  Text::ImageAttributes attributes(source, size);
  attributes.SetAlignment(alignment);
  attributes.SetVerticalOffset(verticalOffset);
  builder.PushSpan(Text::ImageSpan::New(attributes));
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  builder.PopSpan();
}

Text::StyledText BuildFieldText(float verticalOffset)
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
  builder.AppendText("Type and move the cursor before ");
  AppendImage(builder, RESOURCES_DIR "flag_kr.png", Vector2(64.0f, 38.0f), verticalOffset);
  builder.AppendText(" local, between images, ");
  AppendImage(builder, REMOTE_IMAGE, Vector2(72.0f, 48.0f), verticalOffset);
  builder.AppendText(" remote, and after this long scrolling tail.");
  return builder.Build();
}

Text::StyledText BuildEditorText(float verticalOffset)
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
  builder.AppendText("InputEditor multiline and wrapping. Edit before ");
  AppendImage(builder, RESOURCES_DIR "flag_us.png", Vector2(86.0f, 48.0f), verticalOffset);
  builder.AppendText(" and after the first image.\nA large replacement-only line:\n");
  AppendImage(builder, RESOURCES_DIR "flag_kr.png", Vector2(180.0f, 96.0f), verticalOffset);
  builder.AppendText("\nCursor boundaries around varied sizes: A ");
  AppendImage(builder,
              RESOURCES_DIR "flag_us.png",
              Vector2(34.0f, 22.0f),
              verticalOffset,
              Text::ImageAttributes::InlineAlignment::TEXT_BASELINE);
  builder.AppendText(" B ");
  AppendImage(builder,
              RESOURCES_DIR "flag_kr.png",
              Vector2(128.0f, 76.0f),
              verticalOffset,
              Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM);
  builder.AppendText(" C ");
  AppendImage(builder, RESOURCES_DIR "flag_ae.png", Vector2(52.0f, 36.0f), verticalOffset);
  builder.AppendText(" D.\nRTL boundaries: עברית ");
  AppendImage(builder, RESOURCES_DIR "flag_kr.png", Vector2(80.0f, 48.0f), verticalOffset);
  builder.AppendText(" العربية.\n");
  builder.AppendText("\nRemote image ");
  AppendImage(builder, REMOTE_IMAGE, Vector2(72.0f, 48.0f), verticalOffset);
  builder.AppendText(" followed by mixed direction: English العربية עברית.\nFailed resource keeps its reserved box: ");
  AppendImage(builder, FAILED_IMAGE, Vector2(72.0f, 48.0f), verticalOffset);
  builder.AppendText(". Continue typing and scroll vertically.");
  return builder.Build();
}
} // unnamed namespace

class TextInputImageSpanController : public ConnectionTracker
{
public:
  explicit TextInputImageSpanController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextInputImageSpanController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xF1F5F9));

    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetPadding(Extents(24u, 24u, 20u, 20u));
    mRoot.SetSpacing(12.0f);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);

    Label title = NewTextLabel("Editable ImageSpan", 24.0f, 0x0F172A);
    title.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    Label guide = NewTextLabel("Click a control, then move the cursor, edit, and select across each image.", 16.0f, 0x475569);
    guide.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    CreateEditableControls();

    Label fieldTitle  = NewTextLabel("InputField — single line", 17.0f, 0x1E293B);
    Label editorTitle = NewTextLabel("InputEditor — multiline", 17.0f, 0x1E293B);

    mFieldCard = StackLayout::New(StackOrientation::VERTICAL);
    mFieldCard.SetPadding(Extents(2u, 2u, 2u, 2u));
    mFieldCard.SetRequestedWidth(MATCH_PARENT);
    mFieldCard.SetRequestedHeight(94.0f);
    mFieldCard.SetBackgroundColor(UiColor(0xCBD5E1));
    mFieldCard.Add(mField);

    mEditorCard = StackLayout::New(StackOrientation::VERTICAL);
    mEditorCard.SetPadding(Extents(2u, 2u, 2u, 2u));
    mEditorCard.SetRequestedWidth(MATCH_PARENT);
    mEditorCard.SetBackgroundColor(UiColor(0xCBD5E1));
    mEditorCard.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mEditorCard.Add(mEditor);

    StackLayout contentToolbar = StackLayout::New(StackOrientation::HORIZONTAL);
    contentToolbar.SetSpacing(6.0f);
    contentToolbar.SetRequestedWidth(MATCH_PARENT);
    contentToolbar.SetRequestedHeight(46.0f);
    StackLayout settingToolbar = StackLayout::New(StackOrientation::HORIZONTAL);
    settingToolbar.SetSpacing(6.0f);
    settingToolbar.SetRequestedWidth(MATCH_PARENT);
    settingToolbar.SetRequestedHeight(46.0f);
    Label resetButton = NewButton("Reset StyledText");
    Label plainButton = NewButton("Set Plain Text");
    mAlignmentButton  = NewButton("");
    mOffsetButton     = NewButton("");
    mLineHeightButton = NewButton("");
    mLifecycleButton  = NewButton("");
    for(Label button : {resetButton, plainButton, mLifecycleButton})
    {
      contentToolbar.Add(button);
    }
    for(Label button : {mAlignmentButton, mOffsetButton, mLineHeightButton})
    {
      settingToolbar.Add(button);
    }
    resetButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { SetStyledContent("StyledText restored"); });
    plainButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { SetPlainContent("Plain text set"); });
    mAlignmentButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleAlignment(); });
    mOffsetButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleVerticalOffset(); });
    mLineHeightButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleLineHeight(); });
    mLifecycleButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleLifecycle(); });

    mStatus = NewTextLabel("", 13.0f, 0xE2E8F0);
    mStatus.SetBackgroundColor(UiColor(0x1E293B));
    mStatus.SetPadding(Extents(12u, 12u, 8u, 8u));
    mStatus.SetRequestedHeight(172.0f);
    mStatus.SetCornerRadius(7.0f);

    mRoot.Add(title);
    mRoot.Add(guide);
    mRoot.Add(fieldTitle);
    mRoot.Add(mFieldCard);
    mRoot.Add(editorTitle);
    mRoot.Add(mEditorCard);
    mRoot.Add(contentToolbar);
    mRoot.Add(settingToolbar);
    mRoot.Add(mStatus);
    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &TextInputImageSpanController::OnKeyEvent);

    SetStyledContent("StyledText active");
  }

  void CreateEditableControls()
  {
    mField = InputField::New();
    mField.SetFontSize(27.0f);
    mField.SetTextColor(UiColor(0x111827));
    mField.SetBackgroundColor(UiColor(0xFFFFFF));
    mField.SetPadding(Extents(14u, 14u, 10u, 10u));
    mField.SetTextOverflowMode(Text::OverflowMode::CLIP);
    mField.SetRequestedWidth(MATCH_PARENT);
    mField.SetRequestedHeight(MATCH_PARENT);
    mField.SetMaximumLength(5000);
    mField.SetCornerRadius(6.0f);

    mEditor = InputEditor::New();
    mEditor.SetFontSize(25.0f);
    mEditor.SetTextColor(UiColor(0x111827));
    mEditor.SetBackgroundColor(UiColor(0xFFFFFF));
    mEditor.SetPadding(Extents(14u, 14u, 12u, 12u));
    mEditor.SetLineWrapMode(Text::LineWrapMode::WORD);
    mEditor.SetTextOverflowMode(Text::OverflowMode::CLIP);
    ApplyLineHeight();
    mEditor.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mEditor.SetMaximumLength(5000);
    mEditor.SetCornerRadius(6.0f);
  }

  void RecreateEditableControls()
  {
    mFieldCard.Remove(mField, RemovePolicy::IMMEDIATE);
    mEditorCard.Remove(mEditor, RemovePolicy::IMMEDIATE);
    mField.Reset();
    mEditor.Reset();

    CreateEditableControls();
    mFieldCard.Add(mField);
    mEditorCard.Add(mEditor);
    SetStyledContent("Editable controls recreated");
  }

  void SetStyledContent(const char* action)
  {
    mField.SetStyledText(BuildFieldText(mVerticalOffset));
    mEditor.SetStyledText(BuildEditorText(mVerticalOffset));
    mStyledContent = true;
    UpdateStatus(action);
  }

  void SetPlainContent(const char* action)
  {
    mField.SetText("Plain InputField: StyledText and ImageSpan state cleared.");
    mEditor.SetText("Plain InputEditor\nSetText() clears every authored span and runtime image resource.");
    mStyledContent = false;
    UpdateStatus(action);
  }

  void UpdateStatus(const char* state)
  {
    const char* alignment = mAlignment == Text::Alignment::START
                              ? "START"
                              : (mAlignment == Text::Alignment::CENTER ? "CENTER" : "END");
    mAlignmentButton.SetText((std::string("Alignment: ") + alignment).c_str());
    mOffsetButton.SetText((std::string("Vertical Offset: ") + (mVerticalOffset == 0.0f ? "0" : "-8")).c_str());
    mLineHeightButton.SetText((std::string("Line Height: ") + (mRelativeLineHeightEnabled ? "RELATIVE 1.6" : "AUTO")).c_str());
    mLifecycleButton.SetText((std::string("Lifecycle: ") + (mLifecycleEnabled ? "RUNNING" : "STOPPED")).c_str());

    std::string text = std::string("Content: ") + (mStyledContent ? "StyledText" : "Plain") +
                       "\nHorizontal alignment: " + alignment +
                       "\nVertical offset: " + std::to_string(static_cast<int>(mVerticalOffset)) +
                       "\nLine height: " + (mRelativeLineHeightEnabled ? "RELATIVE 1.6" : "AUTO") +
                       "\nLifecycle: " + (mLifecycleEnabled ? "RUNNING (300 ms set / clear / recreate)" : "STOPPED") +
                       "\nLast action: " + state +
                       "\nTry: move the cursor, select through an image, then use Backspace/Delete at its boundary."
                       "\nShortcuts: R, C, A, O, H, T.";
    mStatus.SetText(text.c_str());
  }

  void CycleAlignment()
  {
    mAlignment = mAlignment == Text::Alignment::START
                   ? Text::Alignment::CENTER
                   : (mAlignment == Text::Alignment::CENTER ? Text::Alignment::END : Text::Alignment::START);
    mField.SetHorizontalTextAlignment(mAlignment);
    mEditor.SetHorizontalTextAlignment(mAlignment);
    UpdateStatus("Alignment changed");
  }

  void ToggleVerticalOffset()
  {
    mVerticalOffset = mVerticalOffset == 0.0f ? -8.0f : 0.0f;
    SetStyledContent("StyledText rebuilt with a new vertical offset");
  }

  void ApplyLineHeight()
  {
    mEditor.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    mEditor.SetLineHeight(mRelativeLineHeightEnabled ? RELATIVE_LINE_HEIGHT : Text::LINE_HEIGHT_AUTO);
  }

  void ToggleLineHeight()
  {
    mRelativeLineHeightEnabled = !mRelativeLineHeightEnabled;
    ApplyLineHeight();
    UpdateStatus("Line height changed");
  }

  void ToggleLifecycle()
  {
    mLifecycleEnabled = !mLifecycleEnabled;
    if(mLifecycleEnabled)
    {
      if(!mLifecycleTimer)
      {
        mLifecycleTimer = Timer::New(300u);
        mLifecycleTimer.TickSignal().Connect(this, &TextInputImageSpanController::OnLifecycleTick);
      }
      mLifecycleTimer.Start();
    }
    else if(mLifecycleTimer)
    {
      mLifecycleTimer.Stop();
    }
    UpdateStatus("Lifecycle playback toggled");
  }

  bool OnLifecycleTick()
  {
    mLifecyclePhase = (mLifecyclePhase + 1u) % 3u;
    if(mLifecyclePhase == 0u)
    {
      SetStyledContent("Lifecycle set StyledText");
    }
    else if(mLifecyclePhase == 1u)
    {
      SetPlainContent("Lifecycle set plain text");
    }
    else
    {
      RecreateEditableControls();
    }
    return mLifecycleEnabled;
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyString() == "r" || event.GetKeyString() == "R")
    {
      SetStyledContent("StyledText restored");
    }
    else if(event.GetKeyString() == "c" || event.GetKeyString() == "C")
    {
      SetPlainContent("Plain text set");
    }
    else if(event.GetKeyString() == "a" || event.GetKeyString() == "A")
    {
      CycleAlignment();
    }
    else if(event.GetKeyString() == "o" || event.GetKeyString() == "O")
    {
      ToggleVerticalOffset();
    }
    else if(event.GetKeyString() == "h" || event.GetKeyString() == "H")
    {
      ToggleLineHeight();
    }
    else if(event.GetKeyString() == "t" || event.GetKeyString() == "T")
    {
      ToggleLifecycle();
    }
  }

private:
  Application&    mApplication;
  StackLayout     mRoot;
  StackLayout     mFieldCard;
  StackLayout     mEditorCard;
  InputField      mField;
  InputEditor     mEditor;
  Label           mStatus;
  Label           mAlignmentButton;
  Label           mOffsetButton;
  Label           mLineHeightButton;
  Label           mLifecycleButton;
  Timer           mLifecycleTimer;
  Text::Alignment mAlignment{Text::Alignment::START};
  float           mVerticalOffset{0.0f};
  bool            mRelativeLineHeightEnabled{false};
  bool            mStyledContent{true};
  bool            mLifecycleEnabled{false};
  uint32_t        mLifecyclePhase{0u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextInputImageSpanController controller(application);
  application.MainLoop();
  return 0;
}
