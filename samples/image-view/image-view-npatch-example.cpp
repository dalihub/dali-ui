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
#include <dali-ui-foundation/public-api/views/image/image-view.h>
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
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewNPatchController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.AddChildren({CreateImageArea(), CreateInfoLabel(), CreateSizeButtonRow(), CreateToggleRow()});
    return contents;
  }

  View CreateImageArea()
  {
    StackLayout area = StackLayout::New(StackOrientation::VERTICAL);
    area.SetRequestedWidth(MATCH_PARENT);
    area.SetRequestedHeight(WRAP_CONTENT);
    area.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    area.SetPadding(Extents(16, 16, 16, 16));
    mImage = ImageView::New(RESOURCES_DIR "button-up-1.9.png");
    mImage.SetRequestedWidth(SIZES[mSizeIndex].width);
    mImage.SetRequestedHeight(SIZES[mSizeIndex].height);
    mImage.SetNPatchBorderOnly(mNPatchBorderOnly);
    area.Add(mImage);
    return area;
  }

  View CreateInfoLabel()
  {
    mInfoLabel = Label::New(MakeInfoText());
    mInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mInfoLabel.SetRequestedHeight(40.0f);
    mInfoLabel.SetFontSize(13.0f);
    mInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mInfoLabel;
  }

  View CreateSizeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(80.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < SIZE_COUNT; ++i)
    {
      row.Add(CreateSizeButton(i));
    }
    return row;
  }

  View CreateToggleRow()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(80.0f);
    button.SetPadding(Extents(4, 4, 4, 4));
    button.SetBackgroundColor(UiColor(0x444444));
    mNPatchBorderOnlyLabel = Label::New("BORDER ONLY: OFF");
    mNPatchBorderOnlyLabel.SetRequestedWidth(MATCH_PARENT);
    mNPatchBorderOnlyLabel.SetRequestedHeight(MATCH_PARENT);
    mNPatchBorderOnlyLabel.SetFontSize(14.0f);
    mNPatchBorderOnlyLabel.SetTextColor(UiColor(0xFFFFFF));
    mNPatchBorderOnlyLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mNPatchBorderOnlyLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(mNPatchBorderOnlyLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageViewNPatchController::OnBorderOnlyClicked);
    return button;
  }

  View CreateSizeButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(index == mSizeIndex ? UiColor(0x4A90E2) : UiColor(0x333333));
    Label buttonLabel = Label::New(SIZES[index].label);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(14.0f);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageViewNPatchController::OnSizeButtonClicked);
    mSizeButtons[index] = button;
    return button;
  }

  void OnSizeButtonClicked(View clickedView, InputEvent /*event*/)
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

  void OnBorderOnlyClicked(View /*clickedView*/, InputEvent /*event*/)
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

  void OnKeyEvent(Window window, KeyEvent event)
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
