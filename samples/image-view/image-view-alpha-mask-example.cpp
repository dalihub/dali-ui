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
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView AlphaMask sample:
 * - Left panel : CropToMask=false — full image with mask alpha applied
 * - Right panel: CropToMask=true  — image cropped to mask bounding box
 * - Buttons to switch between two mask shapes (CIRCLE, BUBBLE)
 * - Toggle MaskingMode between ON_RENDERING and ON_LOADING
 * - Press Escape or Back to quit
 */
class ImageViewAlphaMaskController : public ConnectionTracker
{
  static constexpr int MASK_COUNT = 2;

  struct MaskEntry
  {
    const char* name;
    const char* url;
  };

  static const MaskEntry MASKS[MASK_COUNT];

public:
  explicit ImageViewAlphaMaskController(Application& application)
  : mApplication(application),
    mMaskIndex(0),
    mMaskingOnLoading(false)
  {
    mApplication.InitSignal().Connect(this, &ImageViewAlphaMaskController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewAlphaMaskController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateCompareRow(),
        CreateInfoLabel(),
        CreateMaskButtonRow(),
        CreateModeToggle(),
      });
  }

  // Two ImageViews side by side: left=CropToMask OFF, right=CropToMask ON
  View CreateCompareRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .Spacing(8.0f)
      .SetViewPadding(Extents(8, 8, 8, 8))
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        CreatePanel("CROP: OFF", false, mImageOff),
        CreatePanel("CROP: ON",  true,  mImageOn),
      });
  }

  View CreateSpacer()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  }

  View CreatePanel(const char* title, bool cropToMask, Ui::ImageView& outImage)
  {
    ImageView::New(RESOURCES_DIR "gallery-large-1.jpg")
      .SetRequestedWidth(cropToMask ? WRAP_CONTENT : MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetAlphaMaskUrl(MASKS[mMaskIndex].url)
      .SetCropToMask(cropToMask)
      .SetMaskingMode(Ui::MaskingType::MASKING_ON_RENDERING)
      .SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER))
      .As(outImage);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(WRAP_CONTENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .SetBackgroundColor(UiColor(0x2A2A2A))
      .Children({
        Label::New(title)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(36.0f)
          .SetFontSize(14.0f)
          .SetTextColor(UiColor(0xFFFFFF))
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        CreateSpacer(),
        outImage,
        CreateSpacer(),
      });
  }

  View CreateInfoLabel()
  {
    return Label::New(MakeInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(36.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mInfoLabel);
  }

  View CreateMaskButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(80.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < MASK_COUNT; ++i)
    {
      row.Add(CreateMaskButton(i));
    }
    return row;
  }

  View CreateMaskButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mMaskIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(MASKS[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewAlphaMaskController::OnMaskButtonClicked);
    mMaskButtons[index] = button;
    return button;
  }

  View CreateModeToggle()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(MATCH_PARENT)
                           .SetRequestedHeight(80.0f)
                           .SetViewPadding(Extents(4, 4, 4, 4))
                           .SetBackgroundColor(UiColor(0x444444))
                           .Children({
                             Label::New("MODE: ON_RENDERING")
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER)
                               .As(mModeLabel),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewAlphaMaskController::OnModeToggleClicked);
    return button;
  }

  void OnMaskButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < MASK_COUNT; ++i)
    {
      if(mMaskButtons[i] == clickedView)
      {
        mMaskButtons[mMaskIndex].SetBackgroundColor(UiColor(0x333333));
        mMaskIndex = i;
        mMaskButtons[mMaskIndex].SetBackgroundColor(UiColor(0x4A90E2));

        mImageOff.SetAlphaMaskUrl(MASKS[mMaskIndex].url);
        mImageOff.Reload();
        mImageOn.SetAlphaMaskUrl(MASKS[mMaskIndex].url);
        mImageOn.Reload();

        mInfoLabel.SetText(MakeInfoText());
        DALI_LOG_RELEASE_INFO("[AlphaMask] mask=%s\n", MASKS[mMaskIndex].name);
        return;
      }
    }
  }

  void OnModeToggleClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mMaskingOnLoading = !mMaskingOnLoading;
    auto mode = mMaskingOnLoading ? Ui::MaskingType::MASKING_ON_LOADING : Ui::MaskingType::MASKING_ON_RENDERING;

    mImageOff.SetMaskingMode(mode);
    mImageOff.Reload();
    mImageOn.SetMaskingMode(mode);
    mImageOn.Reload();

    mModeLabel.SetText(mMaskingOnLoading ? "MODE: ON_LOADING" : "MODE: ON_RENDERING");
    mInfoLabel.SetText(MakeInfoText());
    DALI_LOG_RELEASE_INFO("[AlphaMask] MaskingMode=%s\n", mMaskingOnLoading ? "LOADING" : "RENDERING");
  }

  Dali::String MakeInfoText() const
  {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "Mask: %s | Mode: %s",
             MASKS[mMaskIndex].name,
             mMaskingOnLoading ? "ON_LOADING" : "ON_RENDERING");
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
  Ui::ImageView mImageOff; // CropToMask=false
  Ui::ImageView mImageOn;  // CropToMask=true
  Label         mInfoLabel;
  Label         mModeLabel;
  View          mMaskButtons[MASK_COUNT];
  int           mMaskIndex;
  bool          mMaskingOnLoading;
};

const ImageViewAlphaMaskController::MaskEntry ImageViewAlphaMaskController::MASKS[ImageViewAlphaMaskController::MASK_COUNT] = {
  {"CIRCLE", RESOURCES_DIR "mask.png"},
  {"BUBBLE", RESOURCES_DIR "shape-bubble.png"},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewAlphaMaskController controller(application);
  application.MainLoop();
  return 0;
}
