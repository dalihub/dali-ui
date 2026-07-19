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
constexpr const char* REMOTE_IMAGE =
  "https://www.w3.org/assets/logos/w3c-2025-transitional/w3c-72x48.png";
constexpr const char* FAILED_IMAGE = "https://127.0.0.1:1/image-span-failure.png";

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
  builder.AppendText("\uFFFC");
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
  builder.AppendText(" D.\nRTL boundaries: \xD7\xA2\xD7\x91\xD7\xA8\xD7\x99\xD7\xAA ");
  AppendImage(builder, RESOURCES_DIR "flag_kr.png", Vector2(80.0f, 48.0f), verticalOffset);
  builder.AppendText(" \xD8\xA7\xD9\x84\xD8\xB9\xD8\xB1\xD8\xA8\xD9\x8A\xD8\xA9.\n");
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

    Label title = Label::New("Editable ImageSpan: click a control and use normal cursor/edit/selection keys");
    title.SetFontSize(20.0f);
    title.SetTextColor(UiColor(0x0F172A));
    title.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(WRAP_CONTENT);

    CreateEditableControls();

    mStatus = Label::New();
    mStatus.SetFontSize(16.0f);
    mStatus.SetTextColor(UiColor(0x334155));
    mStatus.SetMultiLine(true);
    mStatus.SetRequestedWidth(MATCH_PARENT);
    mStatus.SetRequestedHeight(WRAP_CONTENT);

    mRoot.Add(title);
    mRoot.Add(mField);
    mRoot.Add(mEditor);
    mRoot.Add(mStatus);
    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &TextInputImageSpanController::OnKeyEvent);

    SetStyledContent();
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
    mField.SetRequestedHeight(92.0f);
    mField.SetMaximumLength(5000);

    mEditor = InputEditor::New();
    mEditor.SetFontSize(25.0f);
    mEditor.SetTextColor(UiColor(0x111827));
    mEditor.SetBackgroundColor(UiColor(0xFFFFFF));
    mEditor.SetPadding(Extents(14u, 14u, 12u, 12u));
    mEditor.SetLineWrapMode(Text::LineWrapMode::WORD);
    mEditor.SetTextOverflowMode(Text::OverflowMode::CLIP);
    mEditor.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mEditor.SetMaximumLength(5000);
  }

  void RecreateEditableControls()
  {
    mRoot.Remove(mField);
    mRoot.Remove(mEditor);
    mRoot.Remove(mStatus);
    mField.Reset();
    mEditor.Reset();

    CreateEditableControls();
    mRoot.Add(mField);
    mRoot.Add(mEditor);
    mRoot.Add(mStatus);
    SetStyledContent();
  }

  void SetStyledContent()
  {
    mField.SetStyledText(BuildFieldText(mVerticalOffset));
    mEditor.SetStyledText(BuildEditorText(mVerticalOffset));
    UpdateStatus("StyledText active");
  }

  void SetPlainContent()
  {
    mField.SetText("Plain InputField: StyledText and ImageSpan state cleared.");
    mEditor.SetText("Plain InputEditor\nSetText() clears every authored span and runtime image resource.");
    UpdateStatus("Plain text active");
  }

  void UpdateStatus(const char* state)
  {
    const char* alignment = mAlignment == Text::Alignment::START
                              ? "START"
                              : (mAlignment == Text::Alignment::CENTER ? "CENTER" : "END");
    std::string text      = std::string(state) +
                       " | R: reset StyledText | C: SetText clear | A: alignment | O: vertical offset | "
                       "T: 300ms set/clear/destroy stress | Esc: quit\n" +
                       "alignment=" + alignment + " offset=" + std::to_string(mVerticalOffset) +
                       " stress=" + (mStressEnabled ? "ON" : "OFF") +
                       " | Select across the 180x96 replacement to verify highlight < image < cursor.";
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

  void ToggleStress()
  {
    mStressEnabled = !mStressEnabled;
    if(mStressEnabled)
    {
      if(!mStressTimer)
      {
        mStressTimer = Timer::New(300u);
        mStressTimer.TickSignal().Connect(this, &TextInputImageSpanController::OnStressTick);
      }
      mStressTimer.Start();
    }
    else if(mStressTimer)
    {
      mStressTimer.Stop();
    }
    UpdateStatus("Lifecycle stress toggled");
  }

  bool OnStressTick()
  {
    mStressPhase = (mStressPhase + 1u) % 3u;
    if(mStressPhase == 0u)
    {
      SetStyledContent();
    }
    else if(mStressPhase == 1u)
    {
      SetPlainContent();
    }
    else
    {
      RecreateEditableControls();
    }
    return mStressEnabled;
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
      SetStyledContent();
    }
    else if(event.GetKeyString() == "c" || event.GetKeyString() == "C")
    {
      SetPlainContent();
    }
    else if(event.GetKeyString() == "a" || event.GetKeyString() == "A")
    {
      CycleAlignment();
    }
    else if(event.GetKeyString() == "o" || event.GetKeyString() == "O")
    {
      mVerticalOffset = mVerticalOffset == 0.0f ? -8.0f : 0.0f;
      SetStyledContent();
    }
    else if(event.GetKeyString() == "t" || event.GetKeyString() == "T")
    {
      ToggleStress();
    }
  }

private:
  Application&    mApplication;
  StackLayout     mRoot;
  InputField      mField;
  InputEditor     mEditor;
  Label           mStatus;
  Timer           mStressTimer;
  Text::Alignment mAlignment{Text::Alignment::START};
  float           mVerticalOffset{0.0f};
  bool            mStressEnabled{false};
  uint32_t        mStressPhase{0u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextInputImageSpanController controller(application);
  application.MainLoop();
  return 0;
}
