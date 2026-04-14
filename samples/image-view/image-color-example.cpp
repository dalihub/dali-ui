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
#include <dali-ui-foundation/public-api/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageColor sample for ImageView / AnimatedImageView / LottieAnimationView:
 * - SetImageColor() applies an RGBA color as a per-pixel multiplier on the image
 * - WHITE (0xFFFFFF) means no tint — the image appears as original
 * - Preset buttons demonstrate: no tint, red/green/blue channels, desaturate (gray), dim
 * - Alpha column shows partial transparency via the alpha component
 * - IMAGE TYPE button switches between ImageView, AnimatedImageView (GIF), and LottieAnimationView
 * - Press Escape or Back to quit
 */
class ImageColorController : public ConnectionTracker
{
  static constexpr int COLOR_COUNT = 6;
  static constexpr int IMAGE_TYPE_COUNT = 3;

  struct ColorEntry
  {
    const char* name;
    UiColor     color;
  };

  static const ColorEntry COLORS[COLOR_COUNT];
  static const char*      IMAGE_TYPE_NAMES[IMAGE_TYPE_COUNT];
  static const char*      IMAGE_URLS[IMAGE_TYPE_COUNT];

public:
  explicit ImageColorController(Application& application)
  : mApplication(application),
    mColorIndex(0),
    mImageTypeIndex(0)
  {
    mApplication.InitSignal().Connect(this, &ImageColorController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageColorController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateImageTypeRow(),
        CreateImageArea(),
        CreateInfoLabel(),
        CreateColorButtonRow(),
      });
  }

  View CreateImageTypeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(44.0f)
                        .SetPadding(Extents(4, 4, 4, 4));

    StackLayout typeButton = StackLayout::New(StackOrientation::VERTICAL)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetBackgroundColor(UiColor(0x1565C0))
                               .Children({
                                 Label::New(IMAGE_TYPE_NAMES[mImageTypeIndex])
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(MATCH_PARENT)
                                   .SetFontSize(13.0f)
                                   .SetTextColor(UiColor(0xFFFFFF))
                                   .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                   .SetVerticalTextAlignment(Text::Alignment::CENTER)
                                   .As(mImageTypeLabel),
                               });
    typeButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageColorController::OnImageTypeClicked);
    mImageTypeButton = typeButton;

    row.Add(typeButton);
    return row;
  }

  View CreateImageArea()
  {
    // Create initial ImageView
    mImageContainer = StackLayout::New(StackOrientation::VERTICAL)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(WRAP_CONTENT)
                        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    CreateImageView();
    return mImageContainer;
  }

  void CreateImageView()
  {
    mImageContainer.RemoveAllChildren();

    switch(mImageTypeIndex)
    {
      case 0: // ImageView
      {
        ImageView::New(IMAGE_URLS[mImageTypeIndex])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
          .SetImageColor(COLORS[mColorIndex].color)
          .As(mImageView);
        mImageContainer.Add(mImageView);
        break;
      }
      case 1: // AnimatedImageView (GIF)
      {
        AnimatedImageView::New(IMAGE_URLS[mImageTypeIndex])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
          .SetImageColor(COLORS[mColorIndex].color)
          .SetLoopCount(-1)
          .Play()
          .As(mAnimatedImageView);
        mImageContainer.Add(mAnimatedImageView);
        break;
      }
      case 2: // LottieAnimationView
      {
        LottieAnimationView::New(IMAGE_URLS[mImageTypeIndex])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetImageColor(COLORS[mColorIndex].color)
          .SetLoopCount(-1)
          .Play()
          .As(mLottieView);
        mImageContainer.Add(mLottieView);
        break;
      }
    }
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

  View CreateColorButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(100.0f)
                        .SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < COLOR_COUNT; ++i)
    {
      row.Add(CreateColorButton(i));
    }
    return row;
  }

  View CreateColorButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mColorIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(COLORS[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(13.0f)
                               .SetMultiLine(true)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageColorController::OnColorButtonClicked);
    mColorButtons[index] = button;
    return button;
  }

  void OnImageTypeClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mImageTypeIndex = (mImageTypeIndex + 1) % IMAGE_TYPE_COUNT;
    Label::DownCast(mImageTypeLabel).SetText(IMAGE_TYPE_NAMES[mImageTypeIndex]);
    CreateImageView();
    ApplyCurrentColor();
    mInfoLabel.SetText(MakeInfoText());

    DALI_LOG_RELEASE_INFO("[ImageColor] Image type changed to: %s\n", IMAGE_TYPE_NAMES[mImageTypeIndex]);
  }

  void OnColorButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < COLOR_COUNT; ++i)
    {
      if(mColorButtons[i] == clickedView)
      {
        mColorButtons[mColorIndex].SetBackgroundColor(UiColor(0x333333));
        mColorIndex = i;
        mColorButtons[mColorIndex].SetBackgroundColor(UiColor(0x4A90E2));
        ApplyCurrentColor();
        mInfoLabel.SetText(MakeInfoText());

        DALI_LOG_RELEASE_INFO("[ImageColor] color changed to: %s\n", COLORS[mColorIndex].name);
        return;
      }
    }
  }

  void ApplyCurrentColor()
  {
    const UiColor& color = COLORS[mColorIndex].color;
    switch(mImageTypeIndex)
    {
      case 0:
        if(mImageView)
          mImageView.SetImageColor(color);
        break;
      case 1:
        if(mAnimatedImageView)
          mAnimatedImageView.SetImageColor(color);
        break;
      case 2:
        if(mLottieView)
          mLottieView.SetImageColor(color);
        break;
    }
  }

  Dali::String MakeInfoText() const
  {
    char buf[128];
    snprintf(buf, sizeof(buf), "%s — %s", IMAGE_TYPE_NAMES[mImageTypeIndex], COLORS[mColorIndex].name);
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
  Application&        mApplication;
  StackLayout         mImageContainer;
  Ui::ImageView       mImageView;
  AnimatedImageView   mAnimatedImageView;
  LottieAnimationView mLottieView;
  Label               mInfoLabel;
  View                mImageTypeButton;
  View                mImageTypeLabel;
  View                mColorButtons[COLOR_COUNT];
  int                 mColorIndex;
  int                 mImageTypeIndex;
};

const ImageColorController::ColorEntry ImageColorController::COLORS[ImageColorController::COLOR_COUNT] = {
  {"ORIGINAL", UiColor(1.0f, 1.0f, 1.0f, 1.0f)},
  {"RED",      UiColor(1.0f, 0.0f, 0.0f, 1.0f)},
  {"GREEN",    UiColor(0.0f, 1.0f, 0.0f, 1.0f)},
  {"BLUE",     UiColor(0.0f, 0.0f, 1.0f, 1.0f)},
  {"GRAY",     UiColor(0.5f, 0.5f, 0.5f, 1.0f)},
  {"50%\nALPHA", UiColor(1.0f, 1.0f, 1.0f, 0.5f)},
};

const char* ImageColorController::IMAGE_TYPE_NAMES[ImageColorController::IMAGE_TYPE_COUNT] = {
  "ImageView (JPG)",
  "AnimatedImageView (GIF)",
  "LottieAnimationView (JSON)",
};

const char* ImageColorController::IMAGE_URLS[ImageColorController::IMAGE_TYPE_COUNT] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "jolly_walker.json",
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageColorController controller(application);
  application.MainLoop();
  return 0;
}