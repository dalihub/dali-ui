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
#include <dali-ui-foundation/public-api/animated-image-view.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const IMAGES[] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "people-small-10.jpg",
};
constexpr int IMAGE_COUNT = 4;

const char* const GIF_IMAGES[] = {
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "animatedLoading.gif",
};
constexpr int GIF_IMAGE_COUNT = 2;
} // namespace

/**
 * ImageView FittingMode sample:
 * - Shows images with the current FittingMode applied
 * - Tests: ImageView (JPG), ImageView (GIF), AnimatedImageView (GIF)
 * - 4 buttons at the bottom to select a FittingMode
 * - 1 button to cycle through images
 * - Active fitting mode button is highlighted
 * - Press Escape or Back to quit
 */
class ImageFittingModeController : public ConnectionTracker
{
  static constexpr int MODE_COUNT = 4;

  struct ModeEntry
  {
    Ui::Image::FittingMode mode;
    const char*           name;
  };

  static constexpr ModeEntry MODES[MODE_COUNT] = {
    {Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO, "FIT\nKEEP\nASPECT"},
    {Ui::Image::FittingMode::FILL, "FILL"},
    {Ui::Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO, "OVER\nFIT"},
    {Ui::Image::FittingMode::CENTER, "CENTER"},
  };

public:
  explicit ImageFittingModeController(Application& application)
  : mApplication(application),
    mActiveIndex(0),
    mImageIndex(0),
    mGifIndex(0)
  {
    mApplication.InitSignal().Connect(this, &ImageFittingModeController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageFittingModeController::OnKeyEvent);
  }

  View CreateContents()
  {
    // ImageView (JPG)
    ImageView::New(IMAGES[mImageIndex])
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetFittingMode(MODES[mActiveIndex].mode)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mImageView);

    // ImageView (GIF)
    ImageView::New(GIF_IMAGES[mGifIndex])
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetFittingMode(MODES[mActiveIndex].mode)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mGifImageView);

    // AnimatedImageView (GIF)
    AnimatedImageView::New(GIF_IMAGES[mGifIndex])
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetFittingMode(MODES[mActiveIndex].mode)
      .SetLoopCount(-1)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mAnimatedImageView);

    mAnimatedImageView.Play();

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateImageRow(),
        CreateButtonRow(),
      });
  }

  View CreateImageRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(4.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        CreateImageContainer("ImageView\n(JPG)", mImageView),
        CreateImageContainer("ImageView\n(GIF)", mGifImageView),
        CreateImageContainer("AnimatedImageView\n(GIF)", mAnimatedImageView),
      });
  }

  View CreateImageContainer(const char* label, View imageView)
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(2.0f)
      .SetRequestedWidth(WRAP_CONTENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        Label::New(label)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(32.0f)
          .SetFontSize(10.0f)
          .SetMultiLine(true)
          .SetTextColor(UiColor(0xAAAAAA))
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
        StackLayout::New(StackOrientation::HORIZONTAL)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
          .SetBackgroundColor(UiColor(0x2A2A2A))
          .Children({imageView}),
      });
  }

  View CreateButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetSpacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(100.0f)
                        .SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < MODE_COUNT; ++i)
    {
      row.Add(CreateModeButton(i));
    }

    row.Add(CreateSwapButton());

    return row;
  }

  View CreateModeButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mActiveIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(MODES[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetMultiLine(true)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageFittingModeController::OnModeButtonClicked);

    mButtons[index] = button;
    return button;
  }

  View CreateSwapButton()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(UiColor(0x2E7D32))
                           .Children({
                             Label::New("SWAP\nIMAGE")
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetMultiLine(true)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageFittingModeController::OnSwapButtonClicked);

    mSwapButton = button;
    return button;
  }

  void OnModeButtonClicked(View clickedView, InputEvent /*event*/)
  {
    for(int i = 0; i < MODE_COUNT; ++i)
    {
      if(mButtons[i] == clickedView)
      {
        SelectMode(i);
        return;
      }
    }
  }

  void OnSwapButtonClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mImageIndex = (mImageIndex + 1) % IMAGE_COUNT;
    mGifIndex = (mGifIndex + 1) % GIF_IMAGE_COUNT;

    mImageView.SetResourceUrl(IMAGES[mImageIndex]);
    mGifImageView.SetResourceUrl(GIF_IMAGES[mGifIndex]);
    mAnimatedImageView.SetResourceUrl(GIF_IMAGES[mGifIndex]);

    DALI_LOG_RELEASE_INFO("Images changed to: %s, %s\n", IMAGES[mImageIndex], GIF_IMAGES[mGifIndex]);
  }

  void SelectMode(int index)
  {
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x333333));
    mActiveIndex = index;
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x4A90E2));

    // Apply FittingMode to all image views
    mImageView.SetFittingMode(MODES[mActiveIndex].mode);
    mGifImageView.SetFittingMode(MODES[mActiveIndex].mode);
    mAnimatedImageView.SetFittingMode(MODES[mActiveIndex].mode);

    DALI_LOG_RELEASE_INFO("FittingMode changed to: %s\n", MODES[mActiveIndex].name);
  }

  void OnKeyEvent(KeyEvent event)
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
  Application&      mApplication;
  Ui::ImageView     mImageView;
  Ui::ImageView     mGifImageView;
  AnimatedImageView mAnimatedImageView;
  View              mButtons[MODE_COUNT];
  View              mSwapButton;
  int               mActiveIndex;
  int               mImageIndex;
  int               mGifIndex;
};

constexpr ImageFittingModeController::ModeEntry ImageFittingModeController::MODES[ImageFittingModeController::MODE_COUNT];

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageFittingModeController controller(application);
  application.MainLoop();
  return 0;
}
