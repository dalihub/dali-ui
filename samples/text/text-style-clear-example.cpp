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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;
constexpr float ITEM_PADDING  = 10.0f;
constexpr float FONT_SIZE     = 24.0f;

const UiColor BACKGROUND_COLOR(0xF2F2F2);

Text::Shadow CreateShadow(const Vector2& offset, uint32_t color, float blurRadius)
{
  Text::Shadow shadow;
  shadow.SetColor(UiColor(color));
  shadow.SetOffset(offset);
  shadow.SetBlurRadius(blurRadius);
  return shadow;
}

Text::Outline CreateOutline(uint32_t color, float width, float blurRadius = 0.0f)
{
  Text::Outline outline;
  outline.SetColor(UiColor(color));
  outline.SetWidth(width);
  outline.SetBlurRadius(blurRadius);
  return outline;
}
} // namespace

class TextStyleClearController : public ConnectionTracker
{
public:
  explicit TextStyleClearController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextStyleClearController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    Label guide = CreateBaseLabel("1 Set shadow  2 Zero-offset shadow  3 Clear shadow\n4 Set outline  5 Zero-width outline  6 Clear outline\n7 Toggle sync/async");
    guide.SetFontSize(18.0f);
    guide.SetTextColor(UiColor(0x202020));
    guide.SetMultiLine(true);
    guide.SetRequestedHeight(WRAP_CONTENT);

    mModeLabel = CreateBaseLabel("");
    mModeLabel.SetFontSize(18.0f);
    UpdateModeLabel();

    mDefaultLabel = CreateBaseLabel("Base: no style -> plain text");

    mShadowNonZeroLabel = CreateBaseLabel("");
    ApplyShadowWithOffset();

    mShadowZeroLabel = CreateBaseLabel("");
    ApplyShadowZeroOffset();

    mClearShadowLabel = CreateBaseLabel("");
    ApplyClearedShadow();

    mOutlineLabel = CreateBaseLabel("");
    ApplyOutlineWithWidth();

    mOutlineZeroWidthLabel = CreateBaseLabel("");
    ApplyOutlineZeroWidth();

    mClearOutlineLabel = CreateBaseLabel("");
    ApplyClearedOutline();

    root.AddChildren({
      guide,
      mModeLabel,
      mDefaultLabel,
      mShadowNonZeroLabel,
      mShadowZeroLabel,
      mClearShadowLabel,
      mOutlineLabel,
      mOutlineZeroWidthLabel,
      mClearOutlineLabel,
    });
    window.Add(root);

    ConnectAsyncRenderFinishedSignal(mDefaultLabel);
    ConnectAsyncRenderFinishedSignal(mShadowNonZeroLabel);
    ConnectAsyncRenderFinishedSignal(mShadowZeroLabel);
    ConnectAsyncRenderFinishedSignal(mClearShadowLabel);
    ConnectAsyncRenderFinishedSignal(mOutlineLabel);
    ConnectAsyncRenderFinishedSignal(mOutlineZeroWidthLabel);
    ConnectAsyncRenderFinishedSignal(mClearOutlineLabel);

    window.KeyEventSignal().Connect(this, &TextStyleClearController::OnKeyEvent);
  }

  Label CreateBaseLabel(const char* text)
  {
    Label label = Label::New(text);
    label.SetBackgroundColor(BACKGROUND_COLOR);
    label.SetPadding(Extents(ITEM_PADDING, ITEM_PADDING, ITEM_PADDING, ITEM_PADDING));
    label.SetFontSize(FONT_SIZE);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetTextColor(UiColor(0x202020));
    return label;
  }

  void ConnectAsyncRenderFinishedSignal(Label label)
  {
    label.AsyncRenderFinishedSignal().Connect(this, &TextStyleClearController::OnAsyncRenderFinished);
  }

  void SetAsyncRenderingForAll(bool enabled)
  {
    mDefaultLabel.SetAsyncRendering(enabled);
    mShadowNonZeroLabel.SetAsyncRendering(enabled);
    mShadowZeroLabel.SetAsyncRendering(enabled);
    mClearShadowLabel.SetAsyncRendering(enabled);
    mOutlineLabel.SetAsyncRendering(enabled);
    mOutlineZeroWidthLabel.SetAsyncRendering(enabled);
    mClearOutlineLabel.SetAsyncRendering(enabled);
  }

  void ToggleAsyncRendering()
  {
    mAsyncRendering = !mAsyncRendering;
    SetAsyncRenderingForAll(mAsyncRendering);
    UpdateModeLabel();
  }

  void UpdateModeLabel()
  {
    mModeLabel.SetText(mAsyncRendering ? "Mode: ASYNC rendering -> waiting for async render" : "Mode: SYNC rendering");
  }

  void OnAsyncRenderFinished(View, float, float)
  {
    if(mAsyncRendering)
    {
      mModeLabel.SetText("Mode: ASYNC rendering -> async render finished");
    }
  }

  void ApplyShadowWithOffset()
  {
    mShadowNonZeroLabel.SetText("Shadow: offset=(4,4), blur=2 -> visible shadow");
    mShadowNonZeroLabel.SetTextShadow(CreateShadow(Vector2(4.0f, 4.0f), 0xCC3300, 2.0f));
  }

  void ApplyShadowZeroOffset()
  {
    mShadowZeroLabel.SetText("Shadow: offset=(0,0), blur=4 -> enabled, no displacement");
    mShadowZeroLabel.SetTextShadow(CreateShadow(Vector2::ZERO, 0x0055CC, 4.0f));
  }

  void ApplyClearedShadow()
  {
    mClearShadowLabel.SetText("Shadow: set then ClearTextShadow() -> no shadow");
    mClearShadowLabel.SetTextShadow(CreateShadow(Vector2(4.0f, 4.0f), 0xCC3300, 2.0f));
    mClearShadowLabel.ClearTextShadow();
  }

  void ClearShadowRows()
  {
    mShadowNonZeroLabel.SetText("Shadow: ClearTextShadow() -> no shadow");
    mShadowNonZeroLabel.ClearTextShadow();
    mShadowZeroLabel.SetText("Shadow: ClearTextShadow() -> no shadow");
    mShadowZeroLabel.ClearTextShadow();
    ApplyClearedShadow();
  }

  void ApplyOutlineWithWidth()
  {
    mOutlineLabel.SetText("Outline: width=2, blur=0 -> visible outline");
    mOutlineLabel.SetTextOutline(CreateOutline(0x008855, 2.0f));
  }

  void ApplyOutlineZeroWidth()
  {
    mOutlineZeroWidthLabel.SetText("Outline: width=0, blur=4 -> no outline expected");
    mOutlineZeroWidthLabel.SetTextOutline(CreateOutline(0x8844CC, 0.0f, 4.0f));
  }

  void ApplyClearedOutline()
  {
    mClearOutlineLabel.SetText("Outline: set then ClearTextOutline() -> no outline");
    mClearOutlineLabel.SetTextOutline(CreateOutline(0x0066CC, 2.0f));
    mClearOutlineLabel.ClearTextOutline();
  }

  void ClearOutlineRows()
  {
    mOutlineLabel.SetText("Outline: cleared after width=2 -> no outline");
    mOutlineLabel.ClearTextOutline();
    mOutlineZeroWidthLabel.SetText("Outline: cleared after width=0, blur=4 -> no outline");
    mOutlineZeroWidthLabel.ClearTextOutline();
    ApplyClearedOutline();
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
      ApplyShadowWithOffset();
      UpdateModeLabel();
    }
    else if(event.GetKeyName() == "2")
    {
      ApplyShadowZeroOffset();
      UpdateModeLabel();
    }
    else if(event.GetKeyName() == "3")
    {
      ClearShadowRows();
      UpdateModeLabel();
    }
    else if(event.GetKeyName() == "4")
    {
      ApplyOutlineWithWidth();
      UpdateModeLabel();
    }
    else if(event.GetKeyName() == "5")
    {
      ApplyOutlineZeroWidth();
      UpdateModeLabel();
    }
    else if(event.GetKeyName() == "6")
    {
      ClearOutlineRows();
      UpdateModeLabel();
    }
    else if(event.GetKeyName() == "7")
    {
      ToggleAsyncRendering();
    }
  }

private:
  Application& mApplication;
  bool         mAsyncRendering{false};
  Label        mModeLabel;
  Label        mDefaultLabel;
  Label        mShadowNonZeroLabel;
  Label        mShadowZeroLabel;
  Label        mClearShadowLabel;
  Label        mOutlineLabel;
  Label        mOutlineZeroWidthLabel;
  Label        mClearOutlineLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextStyleClearController controller(application);
  application.MainLoop();
  return 0;
}
