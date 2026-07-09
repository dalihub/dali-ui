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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali/integration-api/debug.h>

#include <cstdio>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView SVG sample:
 * - Loads SVG resources through ImageView
 * - Cycles resource URL, desired rasterization size, and image color
 * - Press Escape or Back to quit
 */
class ImageViewSvgController : public ConnectionTracker
{
  static constexpr int SVG_COUNT  = 2;
  static constexpr int SIZE_COUNT = 4;
  static constexpr int TINT_COUNT = 4;

  struct SvgEntry
  {
    const char* name;
    const char* url;
  };

  struct SizeEntry
  {
    const char* name;
    int         size;
  };

  struct TintEntry
  {
    const char* name;
    UiColor     color;
  };

  static const SvgEntry  SVGS[SVG_COUNT];
  static const SizeEntry SIZES[SIZE_COUNT];
  static const TintEntry TINTS[TINT_COUNT];

public:
  explicit ImageViewSvgController(Application& application)
  : mApplication(application),
    mSvgIndex(0),
    mSizeIndex(0),
    mTintIndex(0),
    mLastLoadingStatus(-1)
  {
    mApplication.InitSignal().Connect(this, &ImageViewSvgController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x20242A));

    window.Add(CreateContents());
    window.KeyEventSignal().Connect(this, &ImageViewSvgController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Extents(12, 12, 12, 12));
    contents.SetSpacing(8.0f);
    contents.Add(CreateInfoLabel());
    contents.Add(CreateImageArea());
    contents.Add(CreateButtonRow());
    return contents;
  }

  View CreateInfoLabel()
  {
    mInfoLabel = Label::New(MakeInfoText());
    mInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mInfoLabel.SetRequestedHeight(40.0f);
    mInfoLabel.SetFontSize(13.0f);
    mInfoLabel.SetTextColor(UiColor(0xDDDDDD));
    mInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mInfoLabel;
  }

  View CreateImageArea()
  {
    StackLayout frame = StackLayout::New(StackOrientation::VERTICAL);
    frame.SetRequestedWidth(MATCH_PARENT);
    frame.SetRequestedHeight(WRAP_CONTENT);
    frame.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    frame.SetPadding(Extents(16, 16, 16, 16));
    frame.SetBackgroundColor(UiColor(0x30343B));

    mImage = ImageView::New(SVGS[mSvgIndex].url);
    mImage.SetRequestedWidth(MATCH_PARENT);
    mImage.SetRequestedHeight(MATCH_PARENT);
    mImage.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mImage.SetImageLoadWithViewSize(false);
    ApplyImageSettings();
    mImage.ResourceReadySignal().Connect(this, &ImageViewSvgController::OnResourceReady);

    frame.Add(mImage);
    return frame;
  }

  View CreateButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(72.0f);
    row.SetSpacing(8.0f);

    View svgButton = CreateButton(mSvgButtonLabel);
    svgButton.AsInteractive().ClickedSignal().Connect(this, &ImageViewSvgController::OnSvgButtonClicked);
    row.Add(svgButton);

    View sizeButton = CreateButton(mSizeButtonLabel);
    sizeButton.AsInteractive().ClickedSignal().Connect(this, &ImageViewSvgController::OnSizeButtonClicked);
    row.Add(sizeButton);

    View tintButton = CreateButton(mTintButtonLabel);
    tintButton.AsInteractive().ClickedSignal().Connect(this, &ImageViewSvgController::OnTintButtonClicked);
    row.Add(tintButton);

    UpdateLabels();
    return row;
  }

  View CreateButton(Label& buttonLabel)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetPadding(Extents(4, 4, 4, 4));
    button.SetBackgroundColor(UiColor(0x1565C0));

    buttonLabel = Label::New("");
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(13.0f);
    buttonLabel.SetMultiLine(true);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);
    return button;
  }

  void OnSvgButtonClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mSvgIndex           = (mSvgIndex + 1) % SVG_COUNT;
    mLastLoadingStatus = -1;
    mImage.SetResourceUrl(SVGS[mSvgIndex].url);
    ApplyImageSettings();
    UpdateLabels();
    DALI_LOG_RELEASE_INFO("[ImageViewSvg] SVG changed to: %s\n", SVGS[mSvgIndex].url);
  }

  void OnSizeButtonClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mSizeIndex          = (mSizeIndex + 1) % SIZE_COUNT;
    mLastLoadingStatus = -1;
    ApplyImageSettings();
    UpdateLabels();
    DALI_LOG_RELEASE_INFO("[ImageViewSvg] Desired size changed to: %d\n", SIZES[mSizeIndex].size);
  }

  void OnTintButtonClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mTintIndex = (mTintIndex + 1) % TINT_COUNT;
    ApplyImageSettings();
    UpdateLabels();
    DALI_LOG_RELEASE_INFO("[ImageViewSvg] Tint changed to: %s\n", TINTS[mTintIndex].name);
  }

  void ApplyImageSettings()
  {
    const int desiredSize = SIZES[mSizeIndex].size;
    mImage.SetDesiredWidth(desiredSize);
    mImage.SetDesiredHeight(desiredSize);
    mImage.SetImageColor(TINTS[mTintIndex].color);
  }

  void OnResourceReady(View view)
  {
    ImageView imageView = ImageView::DownCast(view);
    mLastLoadingStatus  = static_cast<int>(imageView.GetLoadingStatus());
    UpdateLabels();

    DALI_LOG_RELEASE_INFO("[ImageViewSvg] ResourceReadySignal fired. status=%d url=%s desired=%dx%d\n",
                          mLastLoadingStatus,
                          imageView.GetResourceUrl().CStr(),
                          imageView.GetDesiredWidth(),
                          imageView.GetDesiredHeight());
  }

  void UpdateLabels()
  {
    if(mInfoLabel)
    {
      mInfoLabel.SetText(MakeInfoText());
    }

    if(mSvgButtonLabel)
    {
      mSvgButtonLabel.SetText(MakeSvgButtonText());
    }

    if(mSizeButtonLabel)
    {
      mSizeButtonLabel.SetText(MakeSizeButtonText());
    }

    if(mTintButtonLabel)
    {
      mTintButtonLabel.SetText(MakeTintButtonText());
    }
  }

  Dali::String MakeInfoText() const
  {
    char text[160];
    std::snprintf(text,
                  sizeof(text),
                  "SVG: %s | Desired: %s | Tint: %s | Status: %d",
                  SVGS[mSvgIndex].name,
                  SIZES[mSizeIndex].name,
                  TINTS[mTintIndex].name,
                  mLastLoadingStatus);
    return Dali::String(text);
  }

  Dali::String MakeSvgButtonText() const
  {
    char text[64];
    std::snprintf(text, sizeof(text), "SVG\n%s", SVGS[mSvgIndex].name);
    return Dali::String(text);
  }

  Dali::String MakeSizeButtonText() const
  {
    char text[64];
    std::snprintf(text, sizeof(text), "SIZE\n%s", SIZES[mSizeIndex].name);
    return Dali::String(text);
  }

  Dali::String MakeTintButtonText() const
  {
    char text[64];
    std::snprintf(text, sizeof(text), "TINT\n%s", TINTS[mTintIndex].name);
    return Dali::String(text);
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
  Label         mSvgButtonLabel;
  Label         mSizeButtonLabel;
  Label         mTintButtonLabel;
  int           mSvgIndex;
  int           mSizeIndex;
  int           mTintIndex;
  int           mLastLoadingStatus;
};

const ImageViewSvgController::SvgEntry ImageViewSvgController::SVGS[ImageViewSvgController::SVG_COUNT] = {
  {"symbol", RESOURCES_DIR "svg-symbol.svg"},
  {"blocks", RESOURCES_DIR "svg-blocks.svg"},
};

const ImageViewSvgController::SizeEntry ImageViewSvgController::SIZES[ImageViewSvgController::SIZE_COUNT] = {
  {"unset", 0},
  {"128px", 128},
  {"256px", 256},
  {"512px", 512},
};

const ImageViewSvgController::TintEntry ImageViewSvgController::TINTS[ImageViewSvgController::TINT_COUNT] = {
  {"WHITE", UiColor(1.0f, 1.0f, 1.0f, 1.0f)},
  {"CYAN",  UiColor(0.0f, 0.9f, 1.0f, 1.0f)},
  {"PINK",  UiColor(1.0f, 0.35f, 0.65f, 1.0f)},
  {"DIM",   UiColor(0.45f, 0.45f, 0.45f, 1.0f)},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ImageViewSvgController controller(application);
  application.MainLoop();
  return 0;
}
