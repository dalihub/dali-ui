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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView SamplingMode + DesiredSize sample:
 *
 * [SamplingMode section] (ImageView / AnimatedImageView only)
 * - people-small-10.jpg displayed at full panel width so filtering artifacts are visible
 * - Buttons: NEAREST (pixelated) / BOX_THEN_NEAREST / BOX_THEN_LINEAR / LINEAR
 * - IMAGE TYPE button switches between ImageView and AnimatedImageView (GIF)
 *
 * [DesiredSize section] (ImageView / AnimatedImageView / LottieAnimationView)
 * - gallery-large-3.jpg loaded at different desired resolutions
 * - Buttons: FULL / HALF / QUARTER
 * - Reload() required because DesiredSize is a loader-time hint
 * - IMAGE TYPE button switches between all three view types
 *
 * Press Escape or Back to quit
 */
class ImageSamplingController : public ConnectionTracker
{
  static constexpr int SAMPLING_COUNT = 4;
  static constexpr int SIZE_COUNT     = 3;
  static constexpr int IMAGE_TYPE_COUNT_SAMPLING = 2; // ImageView, AnimatedImageView
  static constexpr int IMAGE_TYPE_COUNT_DESIRED   = 3; // ImageView, AnimatedImageView, LottieAnimationView

  struct SamplingEntry
  {
    const char*            name;
    Ui::Image::SamplingMode mode;
  };

  struct DesiredSizeEntry
  {
    const char*    name;
    ImageDimensions size;
  };

  static const SamplingEntry  SAMPLINGS[SAMPLING_COUNT];
  static const DesiredSizeEntry SIZES[SIZE_COUNT];
  static const char* IMAGE_TYPE_NAMES_SAMPLING[IMAGE_TYPE_COUNT_SAMPLING];
  static const char* IMAGE_TYPE_URLS_SAMPLING[IMAGE_TYPE_COUNT_SAMPLING];
  static const char* IMAGE_TYPE_NAMES_DESIRED[IMAGE_TYPE_COUNT_DESIRED];
  static const char* IMAGE_TYPE_URLS_DESIRED[IMAGE_TYPE_COUNT_DESIRED];

public:
  explicit ImageSamplingController(Application& application)
  : mApplication(application),
    mSamplingIndex(0),
    mImageTypeIndexSampling(0),
    mSizeIndex(0),
    mImageTypeIndexDesired(0)
  {
    mApplication.InitSignal().Connect(this, &ImageSamplingController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageSamplingController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.AddChildren({
      CreateSectionLabel("SamplingMode  —  small image displayed large (ImageView / AnimatedImageView)"),
      CreateSamplingImageTypeRow(),
      CreateSamplingImage(),
      CreateSamplingInfoLabel(),
      CreateSamplingButtonRow(),
      CreateSectionLabel("DesiredSize  —  large image loaded at reduced resolution (All types)"),
      CreateDesiredSizeImageTypeRow(),
      CreateDesiredSizeImage(),
      CreateDesiredSizeInfoLabel(),
      CreateDesiredSizeButtonRow(),
    });
    return contents;
  }

  View CreateSectionLabel(const char* text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(32.0f);
    label.SetFontSize(11.0f);
    label.SetTextColor(UiColor(0x888888));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  // ── SamplingMode Section ───────────────────────────────────────────────

  View CreateSamplingImageTypeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(36.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    StackLayout typeButton = StackLayout::New(StackOrientation::VERTICAL);
    typeButton.SetRequestedWidth(MATCH_PARENT);
    typeButton.SetRequestedHeight(MATCH_PARENT);
    typeButton.SetBackgroundColor(UiColor(0x1565C0));
    Label samplingImageTypeLabel = Label::New(IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling]);
    samplingImageTypeLabel.SetRequestedWidth(MATCH_PARENT);
    samplingImageTypeLabel.SetRequestedHeight(MATCH_PARENT);
    samplingImageTypeLabel.SetFontSize(12.0f);
    samplingImageTypeLabel.SetTextColor(UiColor(0xFFFFFF));
    samplingImageTypeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    samplingImageTypeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mSamplingImageTypeLabel = samplingImageTypeLabel;
    typeButton.Add(samplingImageTypeLabel);
    typeButton.AsInteractive().ClickedSignal().Connect(this, &ImageSamplingController::OnSamplingImageTypeClicked);
    mSamplingImageTypeButton = typeButton;

    row.Add(typeButton);
    return row;
  }

  View CreateSamplingImage()
  {
    mSamplingImageContainer = StackLayout::New(StackOrientation::VERTICAL);
    mSamplingImageContainer.SetRequestedWidth(MATCH_PARENT);
    mSamplingImageContainer.SetRequestedHeight(WRAP_CONTENT);
    mSamplingImageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    CreateSamplingImageView();
    return mSamplingImageContainer;
  }

  void CreateSamplingImageView()
  {
    mSamplingImageContainer.RemoveAllChildren();

    switch(mImageTypeIndexSampling)
    {
      case 0: // ImageView
      {
        mSamplingImage = ImageView::New(IMAGE_TYPE_URLS_SAMPLING[mImageTypeIndexSampling]);
        mSamplingImage.SetRequestedWidth(MATCH_PARENT);
        mSamplingImage.SetRequestedHeight(MATCH_PARENT);
        mSamplingImage.SetFittingMode(Ui::Image::FittingMode::FILL);
        mSamplingImage.SetDesiredWidth(64);
        mSamplingImage.SetDesiredHeight(64);
        mSamplingImage.SetSamplingMode(SAMPLINGS[mSamplingIndex].mode);
        mSamplingImageContainer.Add(mSamplingImage);
        break;
      }
      case 1: // AnimatedImageView (GIF)
      {
        mSamplingAnimatedImage = AnimatedImageView::New(IMAGE_TYPE_URLS_SAMPLING[mImageTypeIndexSampling]);
        mSamplingAnimatedImage.SetRequestedWidth(MATCH_PARENT);
        mSamplingAnimatedImage.SetRequestedHeight(MATCH_PARENT);
        mSamplingAnimatedImage.SetFittingMode(Ui::Image::FittingMode::FILL);
        mSamplingAnimatedImage.SetDesiredWidth(64);
        mSamplingAnimatedImage.SetDesiredHeight(64);
        mSamplingAnimatedImage.SetSamplingMode(SAMPLINGS[mSamplingIndex].mode);
        mSamplingAnimatedImage.SetLoopCount(-1);
        mSamplingAnimatedImage.Play();
        mSamplingImageContainer.Add(mSamplingAnimatedImage);
        break;
      }
    }
  }

  View CreateSamplingInfoLabel()
  {
    mSamplingInfoLabel = Label::New(MakeSamplingInfoText());
    mSamplingInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mSamplingInfoLabel.SetRequestedHeight(32.0f);
    mSamplingInfoLabel.SetFontSize(13.0f);
    mSamplingInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mSamplingInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mSamplingInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mSamplingInfoLabel;
  }

  View CreateSamplingButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(72.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < SAMPLING_COUNT; ++i)
    {
      row.Add(CreateSamplingButton(i));
    }
    return row;
  }

  View CreateSamplingButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(index == mSamplingIndex ? UiColor(0x4A90E2) : UiColor(0x333333));
    Label buttonLabel = Label::New(SAMPLINGS[index].name);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(11.0f);
    buttonLabel.SetMultiLine(true);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageSamplingController::OnSamplingButtonClicked);
    mSamplingButtons[index] = button;
    return button;
  }

  // ── DesiredSize Section ───────────────────────────────────────────────

  View CreateDesiredSizeImageTypeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(36.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    StackLayout typeButton = StackLayout::New(StackOrientation::VERTICAL);
    typeButton.SetRequestedWidth(MATCH_PARENT);
    typeButton.SetRequestedHeight(MATCH_PARENT);
    typeButton.SetBackgroundColor(UiColor(0x1565C0));
    Label desiredImageTypeLabel = Label::New(IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired]);
    desiredImageTypeLabel.SetRequestedWidth(MATCH_PARENT);
    desiredImageTypeLabel.SetRequestedHeight(MATCH_PARENT);
    desiredImageTypeLabel.SetFontSize(12.0f);
    desiredImageTypeLabel.SetTextColor(UiColor(0xFFFFFF));
    desiredImageTypeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    desiredImageTypeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mDesiredImageTypeLabel = desiredImageTypeLabel;
    typeButton.Add(desiredImageTypeLabel);
    typeButton.AsInteractive().ClickedSignal().Connect(this, &ImageSamplingController::OnDesiredSizeImageTypeClicked);
    mDesiredImageTypeButton = typeButton;

    row.Add(typeButton);
    return row;
  }

  View CreateDesiredSizeImage()
  {
    mDesiredSizeImageContainer = StackLayout::New(StackOrientation::VERTICAL);
    mDesiredSizeImageContainer.SetRequestedWidth(MATCH_PARENT);
    mDesiredSizeImageContainer.SetRequestedHeight(WRAP_CONTENT);
    mDesiredSizeImageContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    CreateDesiredSizeImageView();
    return mDesiredSizeImageContainer;
  }

  void CreateDesiredSizeImageView()
  {
    mDesiredSizeImageContainer.RemoveAllChildren();

    switch(mImageTypeIndexDesired)
    {
      case 0: // ImageView
      {
        mDesiredSizeImage = ImageView::New(IMAGE_TYPE_URLS_DESIRED[mImageTypeIndexDesired]);
        mDesiredSizeImage.SetRequestedWidth(MATCH_PARENT);
        mDesiredSizeImage.SetRequestedHeight(MATCH_PARENT);
        mDesiredSizeImage.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
        mDesiredSizeImage.SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth());
        mDesiredSizeImage.SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight());
        mDesiredSizeImageContainer.Add(mDesiredSizeImage);
        break;
      }
      case 1: // AnimatedImageView (GIF)
      {
        mDesiredSizeAnimatedImage = AnimatedImageView::New(IMAGE_TYPE_URLS_DESIRED[mImageTypeIndexDesired]);
        mDesiredSizeAnimatedImage.SetRequestedWidth(MATCH_PARENT);
        mDesiredSizeAnimatedImage.SetRequestedHeight(MATCH_PARENT);
        mDesiredSizeAnimatedImage.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
        mDesiredSizeAnimatedImage.SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth());
        mDesiredSizeAnimatedImage.SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight());
        mDesiredSizeAnimatedImage.SetLoopCount(-1);
        mDesiredSizeAnimatedImage.Play();
        mDesiredSizeImageContainer.Add(mDesiredSizeAnimatedImage);
        break;
      }
      case 2: // LottieAnimationView
      {
        mDesiredSizeLottieView = LottieAnimationView::New(IMAGE_TYPE_URLS_DESIRED[mImageTypeIndexDesired]);
        mDesiredSizeLottieView.SetRequestedWidth(MATCH_PARENT);
        mDesiredSizeLottieView.SetRequestedHeight(MATCH_PARENT);
        mDesiredSizeLottieView.SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth());
        mDesiredSizeLottieView.SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight());
        mDesiredSizeLottieView.SetLoopCount(-1);
        mDesiredSizeLottieView.Play();
        mDesiredSizeImageContainer.Add(mDesiredSizeLottieView);
        break;
      }
    }
  }

  View CreateDesiredSizeInfoLabel()
  {
    mDesiredSizeInfoLabel = Label::New(MakeDesiredSizeInfoText());
    mDesiredSizeInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mDesiredSizeInfoLabel.SetRequestedHeight(32.0f);
    mDesiredSizeInfoLabel.SetFontSize(13.0f);
    mDesiredSizeInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mDesiredSizeInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mDesiredSizeInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mDesiredSizeInfoLabel;
  }

  View CreateDesiredSizeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(72.0f);
    row.SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < SIZE_COUNT; ++i)
    {
      row.Add(CreateDesiredSizeButton(i));
    }
    return row;
  }

  View CreateDesiredSizeButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(index == mSizeIndex ? UiColor(0x4A90E2) : UiColor(0x333333));
    Label buttonLabel = Label::New(SIZES[index].name);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(13.0f);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageSamplingController::OnDesiredSizeButtonClicked);
    mSizeButtons[index] = button;
    return button;
  }

  // ── Callbacks ───────────────────────────────────────────────────────────

  void OnSamplingImageTypeClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mImageTypeIndexSampling = (mImageTypeIndexSampling + 1) % IMAGE_TYPE_COUNT_SAMPLING;
    Label::DownCast(mSamplingImageTypeLabel).SetText(IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling]);
    CreateSamplingImageView();
    ApplySamplingMode();
    mSamplingInfoLabel.SetText(MakeSamplingInfoText());
    DALI_LOG_RELEASE_INFO("[Sampling] Image type changed to: %s\n", IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling]);
  }

  void OnDesiredSizeImageTypeClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mImageTypeIndexDesired = (mImageTypeIndexDesired + 1) % IMAGE_TYPE_COUNT_DESIRED;
    Label::DownCast(mDesiredImageTypeLabel).SetText(IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired]);
    CreateDesiredSizeImageView();
    ApplyDesiredSize();
    mDesiredSizeInfoLabel.SetText(MakeDesiredSizeInfoText());
    DALI_LOG_RELEASE_INFO("[DesiredSize] Image type changed to: %s\n", IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired]);
  }

  void OnSamplingButtonClicked(View clickedView, InputEvent /*event*/)
  {
    for(int i = 0; i < SAMPLING_COUNT; ++i)
    {
      if(mSamplingButtons[i] == clickedView)
      {
        mSamplingButtons[mSamplingIndex].SetBackgroundColor(UiColor(0x333333));
        mSamplingIndex = i;
        mSamplingButtons[mSamplingIndex].SetBackgroundColor(UiColor(0x4A90E2));
        ApplySamplingMode();
        mSamplingInfoLabel.SetText(MakeSamplingInfoText());
        DALI_LOG_RELEASE_INFO("[Sampling] mode=%s\n", SAMPLINGS[mSamplingIndex].name);
        return;
      }
    }
  }

  void OnDesiredSizeButtonClicked(View clickedView, InputEvent /*event*/)
  {
    for(int i = 0; i < SIZE_COUNT; ++i)
    {
      if(mSizeButtons[i] == clickedView)
      {
        mSizeButtons[mSizeIndex].SetBackgroundColor(UiColor(0x333333));
        mSizeIndex = i;
        mSizeButtons[mSizeIndex].SetBackgroundColor(UiColor(0x4A90E2));
        // DesiredSize is a loader hint — Reload() required to take effect
        ApplyDesiredSize();
        mDesiredSizeInfoLabel.SetText(MakeDesiredSizeInfoText());
        DALI_LOG_RELEASE_INFO("[DesiredSize] %s (%ux%u)\n",
                              SIZES[mSizeIndex].name,
                              SIZES[mSizeIndex].size.GetWidth(),
                              SIZES[mSizeIndex].size.GetHeight());
        return;
      }
    }
  }

  void ApplySamplingMode()
  {
    switch(mImageTypeIndexSampling)
    {
      case 0:
        if(mSamplingImage)
        {
          mSamplingImage.SetSamplingMode(SAMPLINGS[mSamplingIndex].mode);
          mSamplingImage.Reload();
        }
        break;
      case 1:
        if(mSamplingAnimatedImage)
        {
          mSamplingAnimatedImage.SetSamplingMode(SAMPLINGS[mSamplingIndex].mode);
        }
        break;
    }
  }

  void ApplyDesiredSize()
  {
    switch(mImageTypeIndexDesired)
    {
      case 0:
        if(mDesiredSizeImage)
        {
          mDesiredSizeImage.SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth());
          mDesiredSizeImage.SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight());
          mDesiredSizeImage.Reload();
        }
        break;
      case 1:
        if(mDesiredSizeAnimatedImage)
        {
          mDesiredSizeAnimatedImage.SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth());
          mDesiredSizeAnimatedImage.SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight());
        }
        break;
      case 2:
        if(mDesiredSizeLottieView)
        {
          mDesiredSizeLottieView.SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth());
          mDesiredSizeLottieView.SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight());
        }
        break;
    }
  }

  Dali::String MakeSamplingInfoText() const
  {
    char buf[128];
    snprintf(buf, sizeof(buf), "%s — %s", IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling], SAMPLINGS[mSamplingIndex].name);
    return Dali::String(buf);
  }

  Dali::String MakeDesiredSizeInfoText() const
  {
    char buf[128];
    const auto& s = SIZES[mSizeIndex].size;
    if(s.GetWidth() == 0 && s.GetHeight() == 0)
    {
      snprintf(buf, sizeof(buf), "%s — DesiredSize: FULL (no hint)", IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired]);
    }
    else
    {
      snprintf(buf, sizeof(buf), "%s — DesiredSize: %s (%ux%u)",
               IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired],
               SIZES[mSizeIndex].name, s.GetWidth(), s.GetHeight());
    }
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

  // SamplingMode section
  StackLayout         mSamplingImageContainer;
  Ui::ImageView       mSamplingImage;
  AnimatedImageView   mSamplingAnimatedImage;
  Label               mSamplingInfoLabel;
  View                mSamplingImageTypeButton;
  View                mSamplingImageTypeLabel;
  View                mSamplingButtons[SAMPLING_COUNT];
  int                 mSamplingIndex;
  int                 mImageTypeIndexSampling;

  // DesiredSize section
  StackLayout         mDesiredSizeImageContainer;
  Ui::ImageView       mDesiredSizeImage;
  AnimatedImageView   mDesiredSizeAnimatedImage;
  LottieAnimationView mDesiredSizeLottieView;
  Label               mDesiredSizeInfoLabel;
  View                mDesiredImageTypeButton;
  View                mDesiredImageTypeLabel;
  View                mSizeButtons[SIZE_COUNT];
  int                 mSizeIndex;
  int                 mImageTypeIndexDesired;
};

const ImageSamplingController::SamplingEntry ImageSamplingController::SAMPLINGS[ImageSamplingController::SAMPLING_COUNT] = {
  {"NEAREST",         Ui::Image::SamplingMode::NEAREST},
  {"BOX_THEN\nNEAREST", Ui::Image::SamplingMode::BOX_THEN_NEAREST},
  {"BOX_THEN\nLINEAR",  Ui::Image::SamplingMode::BOX_THEN_LINEAR},
  {"LINEAR",          Ui::Image::SamplingMode::LINEAR},
};

const ImageSamplingController::DesiredSizeEntry ImageSamplingController::SIZES[ImageSamplingController::SIZE_COUNT] = {
  {"FULL",    ImageDimensions(0, 0)},
  {"128x128",    ImageDimensions(128, 128)},
  {"32x32", ImageDimensions(32, 32)},
};

const char* ImageSamplingController::IMAGE_TYPE_NAMES_SAMPLING[ImageSamplingController::IMAGE_TYPE_COUNT_SAMPLING] = {
  "ImageView (JPG)",
  "AnimatedImageView (GIF)",
};

const char* ImageSamplingController::IMAGE_TYPE_URLS_SAMPLING[ImageSamplingController::IMAGE_TYPE_COUNT_SAMPLING] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "dali-logo-anim.gif",
};

const char* ImageSamplingController::IMAGE_TYPE_NAMES_DESIRED[ImageSamplingController::IMAGE_TYPE_COUNT_DESIRED] = {
  "ImageView (JPG)",
  "AnimatedImageView (GIF)",
  "LottieAnimationView (JSON)",
};

const char* ImageSamplingController::IMAGE_TYPE_URLS_DESIRED[ImageSamplingController::IMAGE_TYPE_COUNT_DESIRED] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "jolly_walker.json",
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageSamplingController controller(application);
  application.MainLoop();
  return 0;
}
