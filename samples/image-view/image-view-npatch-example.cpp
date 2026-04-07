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
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView N-Patch sample:
 * - Loads a .9.png image whose border regions are encoded in the file
 * - Shows the image stretched to various sizes to verify borders don't distort
 * - Toggle SetNPatchBorderOnly() to render only the border regions (hollow center)
 * - Manual border override row: shows SetNPatchBorder(Vector4) with explicit insets
 * - Press Escape or Back to quit
 */
class ImageViewNPatchController : public ConnectionTracker
{
  static constexpr int SIZE_COUNT = 3;

  struct SizeEntry
  {
    const char* label;
    float       width;
    float       height;
  };

  static const SizeEntry SIZES[SIZE_COUNT];

public:
  explicit ImageViewNPatchController(Application& application)
  : mApplication(application),
    mSizeIndex(1),
    mNPatchBorderOnly(false)
  {
    mApplication.InitSignal().Connect(this, &ImageViewNPatchController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewNPatchController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateImageArea(),
        CreateInfoLabel(),
        CreateSizeButtonRow(),
        CreateToggleRow(),
      });
  }

  View CreateImageArea()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetViewPadding(Extents(16, 16, 16, 16))
      .Children({
        ImageView::New(RESOURCES_DIR "button-up-1.9.png")
          .SetRequestedWidth(SIZES[mSizeIndex].width)
          .SetRequestedHeight(SIZES[mSizeIndex].height)
          .SetNPatchBorderOnly(mNPatchBorderOnly)
          .As(mImage),
      });
  }

  View CreateInfoLabel()
  {
    return Label::New(MakeInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(40.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mInfoLabel);
  }

  View CreateSizeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(80.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < SIZE_COUNT; ++i)
    {
      row.Add(CreateSizeButton(i));
    }
    return row;
  }

  View CreateToggleRow()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(MATCH_PARENT)
                           .SetRequestedHeight(80.0f)
                           .SetViewPadding(Extents(4, 4, 4, 4))
                           .SetBackgroundColor(UiColor(0x444444))
                           .Children({
                             Label::New("BORDER ONLY: OFF")
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER)
                               .As(mNPatchBorderOnlyLabel),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewNPatchController::OnBorderOnlyClicked);
    return button;
  }

  View CreateSizeButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mSizeIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(SIZES[index].label)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewNPatchController::OnSizeButtonClicked);
    mSizeButtons[index] = button;
    return button;
  }

  void OnSizeButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < SIZE_COUNT; ++i)
    {
      if(mSizeButtons[i] == clickedView)
      {
        mSizeButtons[mSizeIndex].SetBackgroundColor(UiColor(0x333333));
        mSizeIndex = i;
        mSizeButtons[mSizeIndex].SetBackgroundColor(UiColor(0x4A90E2));
        mImage.SetRequestedWidth(SIZES[mSizeIndex].width);
        mImage.SetRequestedHeight(SIZES[mSizeIndex].height);
        UpdateInfoLabel();
        DALI_LOG_RELEASE_INFO("[NPatch] size=%s (%.0fx%.0f)\n",
                              SIZES[mSizeIndex].label,
                              SIZES[mSizeIndex].width,
                              SIZES[mSizeIndex].height);
        return;
      }
    }
  }

  void OnBorderOnlyClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mNPatchBorderOnly = !mNPatchBorderOnly;
    mImage.SetNPatchBorderOnly(mNPatchBorderOnly);
    mNPatchBorderOnlyLabel.SetText(mNPatchBorderOnly ? "BORDER ONLY: ON" : "BORDER ONLY: OFF");
    UpdateInfoLabel();
    DALI_LOG_RELEASE_INFO("[NPatch] BorderOnly=%d\n", mNPatchBorderOnly);
  }

  void UpdateInfoLabel()
  {
    mInfoLabel.SetText(MakeInfoText());
  }

  Dali::String MakeInfoText() const
  {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "Size: %s (%.0f x %.0f) | BorderOnly: %s",
             SIZES[mSizeIndex].label,
             SIZES[mSizeIndex].width,
             SIZES[mSizeIndex].height,
             mNPatchBorderOnly ? "ON" : "OFF");
    return Dali::String(buf);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Ui::ImageView mImage;
  Label         mInfoLabel;
  Label         mNPatchBorderOnlyLabel;
  View          mSizeButtons[SIZE_COUNT];
  int           mSizeIndex;
  bool          mNPatchBorderOnly;
};

const ImageViewNPatchController::SizeEntry ImageViewNPatchController::SIZES[ImageViewNPatchController::SIZE_COUNT] = {
  {"SMALL",  120.0f, 60.0f},
  {"MEDIUM", 300.0f, 80.0f},
  {"LARGE",  500.0f, 120.0f},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewNPatchController controller(application);
  application.MainLoop();
  return 0;
}
