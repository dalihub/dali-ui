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
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
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

const char* const LOTTIE_IMAGES[] = {
  RESOURCES_DIR "jolly_walker.json",
  RESOURCES_DIR "insta_camera.json",
  RESOURCES_DIR "confetti.json",
};
constexpr int LOTTIE_IMAGE_COUNT = 3;
} // namespace

/**
 * ImageView FittingMode sample:
 * - Shows images with the current FittingMode applied
 * - Tests: ImageView (JPG), ImageView (GIF), AnimatedImageView (GIF), LottieAnimationView (JSON)
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
    mGifIndex(0),
    mLottieIndex(0)
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
    mImageView = ImageView::New(IMAGES[mImageIndex]);
    mImageView.SetRequestedWidth(MATCH_PARENT);
    mImageView.SetRequestedHeight(MATCH_PARENT);
    mImageView.SetFittingMode(MODES[mActiveIndex].mode);
    mImageView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    // ImageView (GIF)
    mGifImageView = ImageView::New(GIF_IMAGES[mGifIndex]);
    mGifImageView.SetRequestedWidth(MATCH_PARENT);
    mGifImageView.SetRequestedHeight(MATCH_PARENT);
    mGifImageView.SetFittingMode(MODES[mActiveIndex].mode);
    mGifImageView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    // AnimatedImageView (GIF)
    mAnimatedImageView = AnimatedImageView::New(GIF_IMAGES[mGifIndex]);
    mAnimatedImageView.SetRequestedWidth(MATCH_PARENT);
    mAnimatedImageView.SetRequestedHeight(MATCH_PARENT);
    mAnimatedImageView.SetFittingMode(MODES[mActiveIndex].mode);
    mAnimatedImageView.SetLoopCount(-1);
    mAnimatedImageView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    mAnimatedImageView.Play();

    // LottieAnimationView (JSON)
    mLottieView = LottieAnimationView::New(LOTTIE_IMAGES[mLottieIndex]);
    mLottieView.SetRequestedWidth(MATCH_PARENT);
    mLottieView.SetRequestedHeight(MATCH_PARENT);
    mLottieView.SetLoopCount(-1);
    mLottieView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    mLottieView.Play();

    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.Add(CreateImageRow());
    contents.Add(CreateButtonRow());
    return contents;
  }

  View CreateImageRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(300.0f);
    row.Add(CreateImageContainer("ImageView\n(JPG)", mImageView));
    row.Add(CreateImageContainer("ImageView\n(GIF)", mGifImageView));
    row.Add(CreateImageContainer("AnimatedImageView\n(GIF)", mAnimatedImageView));
    row.Add(CreateImageContainer("LottieAnimationView\n(always AspectRatio)", mLottieView));
    return row;
  }

  View CreateImageContainer(const char* label, View imageView)
  {
    StackLayout container = StackLayout::New(StackOrientation::VERTICAL);
    container.SetSpacing(2.0f);
    container.SetRequestedWidth(WRAP_CONTENT);
    container.SetRequestedHeight(MATCH_PARENT);
    container.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    Label title = Label::New(label);
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(32.0f);
    title.SetFontSize(10.0f);
    title.SetMultiLine(true);
    title.SetTextColor(UiColor(0xAAAAAA));
    title.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    title.SetVerticalTextAlignment(Text::Alignment::CENTER);

    StackLayout imageContainer = StackLayout::New(StackOrientation::HORIZONTAL);
    imageContainer.SetRequestedWidth(MATCH_PARENT);
    imageContainer.SetRequestedHeight(WRAP_CONTENT);
    imageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    imageContainer.SetBackgroundColor(UiColor(0x2A2A2A));
    imageContainer.Add(imageView);

    container.Add(title);
    container.Add(imageContainer);
    return container;
  }

  View CreateButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(100.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < MODE_COUNT; ++i)
    {
      row.Add(CreateModeButton(i));
    }

    row.Add(CreateSwapButton());

    return row;
  }

  View CreateModeButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(index == mActiveIndex ? UiColor(0x4A90E2) : UiColor(0x333333));

    Label buttonLabel = Label::New(MODES[index].name);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(14.0f);
    buttonLabel.SetMultiLine(true);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageFittingModeController::OnModeButtonClicked);

    mButtons[index] = button;
    return button;
  }

  View CreateSwapButton()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(0x2E7D32));

    Label buttonLabel = Label::New("SWAP\nIMAGE");
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(14.0f);
    buttonLabel.SetMultiLine(true);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageFittingModeController::OnSwapButtonClicked);

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
    mImageIndex  = (mImageIndex + 1) % IMAGE_COUNT;
    mGifIndex    = (mGifIndex + 1) % GIF_IMAGE_COUNT;
    mLottieIndex = (mLottieIndex + 1) % LOTTIE_IMAGE_COUNT;

    mImageView.SetResourceUrl(IMAGES[mImageIndex]);
    mGifImageView.SetResourceUrl(GIF_IMAGES[mGifIndex]);
    mAnimatedImageView.SetResourceUrl(GIF_IMAGES[mGifIndex]);
    mLottieView.SetResourceUrl(LOTTIE_IMAGES[mLottieIndex]);
    mLottieView.Play();

    DALI_LOG_RELEASE_INFO("Images changed to: %s, %s, %s\n", IMAGES[mImageIndex], GIF_IMAGES[mGifIndex], LOTTIE_IMAGES[mLottieIndex]);
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
  Application&        mApplication;
  Ui::ImageView       mImageView;
  Ui::ImageView       mGifImageView;
  AnimatedImageView   mAnimatedImageView;
  LottieAnimationView mLottieView;
  View                mButtons[MODE_COUNT];
  View                mSwapButton;
  int                 mActiveIndex;
  int                 mImageIndex;
  int                 mGifIndex;
  int                 mLottieIndex;
};

constexpr ImageFittingModeController::ModeEntry ImageFittingModeController::MODES[ImageFittingModeController::MODE_COUNT];

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ImageFittingModeController controller(application);
  application.MainLoop();
  return 0;
}
