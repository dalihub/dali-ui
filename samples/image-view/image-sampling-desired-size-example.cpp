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
    Ui::SamplingMode::Type mode;
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
    mSizeIndex(0),
    mImageTypeIndexSampling(0),
    mImageTypeIndexDesired(0)
  {
    mApplication.InitSignal().Connect(this, &ImageSamplingController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageSamplingController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
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
  }

  View CreateSectionLabel(const char* text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(11.0f)
      .SetTextColor(UiColor(0x888888))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  // ── SamplingMode Section ───────────────────────────────────────────────

  View CreateSamplingImageTypeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(36.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    StackLayout typeButton = StackLayout::New(StackOrientation::VERTICAL)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetBackgroundColor(UiColor(0x1565C0))
                               .Children({
                                 Label::New(IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling])
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(MATCH_PARENT)
                                   .SetFontSize(12.0f)
                                   .SetTextColor(UiColor(0xFFFFFF))
                                   .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                   .SetVerticalTextAlignment(Text::Alignment::CENTER)
                                   .As(mSamplingImageTypeLabel),
                               });
    typeButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageSamplingController::OnSamplingImageTypeClicked);
    mSamplingImageTypeButton = typeButton;

    row.Add(typeButton);
    return row;
  }

  View CreateSamplingImage()
  {
    mSamplingImageContainer = StackLayout::New(StackOrientation::VERTICAL)
                                .SetRequestedWidth(MATCH_PARENT)
                                .SetRequestedHeight(WRAP_CONTENT)
                                .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
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
        ImageView::New(IMAGE_TYPE_URLS_SAMPLING[mImageTypeIndexSampling])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFittingMode(Ui::FittingMode::FILL)
          .SetDesiredWidth(64)
          .SetDesiredHeight(64)
          .SetSamplingMode(SAMPLINGS[mSamplingIndex].mode)
          .As(mSamplingImage);
        mSamplingImageContainer.Add(mSamplingImage);
        break;
      }
      case 1: // AnimatedImageView (GIF)
      {
        AnimatedImageView::New(IMAGE_TYPE_URLS_SAMPLING[mImageTypeIndexSampling])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFittingMode(Ui::FittingMode::FILL)
          .SetDesiredWidth(64)
          .SetDesiredHeight(64)
          .SetSamplingMode(SAMPLINGS[mSamplingIndex].mode)
          .SetLoopCount(-1)
          .Play()
          .As(mSamplingAnimatedImage);
        mSamplingImageContainer.Add(mSamplingAnimatedImage);
        break;
      }
    }
  }

  View CreateSamplingInfoLabel()
  {
    return Label::New(MakeSamplingInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mSamplingInfoLabel);
  }

  View CreateSamplingButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(72.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < SAMPLING_COUNT; ++i)
    {
      row.Add(CreateSamplingButton(i));
    }
    return row;
  }

  View CreateSamplingButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mSamplingIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(SAMPLINGS[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(11.0f)
                               .SetMultiLine(true)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageSamplingController::OnSamplingButtonClicked);
    mSamplingButtons[index] = button;
    return button;
  }

  // ── DesiredSize Section ───────────────────────────────────────────────

  View CreateDesiredSizeImageTypeRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(36.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    StackLayout typeButton = StackLayout::New(StackOrientation::VERTICAL)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetBackgroundColor(UiColor(0x1565C0))
                               .Children({
                                 Label::New(IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired])
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(MATCH_PARENT)
                                   .SetFontSize(12.0f)
                                   .SetTextColor(UiColor(0xFFFFFF))
                                   .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                   .SetVerticalTextAlignment(Text::Alignment::CENTER)
                                   .As(mDesiredImageTypeLabel),
                               });
    typeButton.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageSamplingController::OnDesiredSizeImageTypeClicked);
    mDesiredImageTypeButton = typeButton;

    row.Add(typeButton);
    return row;
  }

  View CreateDesiredSizeImage()
  {
    mDesiredSizeImageContainer = StackLayout::New(StackOrientation::VERTICAL)
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(WRAP_CONTENT)
                                   .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
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
        ImageView::New(IMAGE_TYPE_URLS_DESIRED[mImageTypeIndexDesired])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
          .SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth())
          .SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight())
          .As(mDesiredSizeImage);
        mDesiredSizeImageContainer.Add(mDesiredSizeImage);
        break;
      }
      case 1: // AnimatedImageView (GIF)
      {
        AnimatedImageView::New(IMAGE_TYPE_URLS_DESIRED[mImageTypeIndexDesired])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
          .SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth())
          .SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight())
          .SetLoopCount(-1)
          .Play()
          .As(mDesiredSizeAnimatedImage);
        mDesiredSizeImageContainer.Add(mDesiredSizeAnimatedImage);
        break;
      }
      case 2: // LottieAnimationView
      {
        LottieAnimationView::New(IMAGE_TYPE_URLS_DESIRED[mImageTypeIndexDesired])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetDesiredWidth(SIZES[mSizeIndex].size.GetWidth())
          .SetDesiredHeight(SIZES[mSizeIndex].size.GetHeight())
          .SetLoopCount(-1)
          .Play()
          .As(mDesiredSizeLottieView);
        mDesiredSizeImageContainer.Add(mDesiredSizeLottieView);
        break;
      }
    }
  }

  View CreateDesiredSizeInfoLabel()
  {
    return Label::New(MakeDesiredSizeInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mDesiredSizeInfoLabel);
  }

  View CreateDesiredSizeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(72.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < SIZE_COUNT; ++i)
    {
      row.Add(CreateDesiredSizeButton(i));
    }
    return row;
  }

  View CreateDesiredSizeButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mSizeIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(SIZES[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(13.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageSamplingController::OnDesiredSizeButtonClicked);
    mSizeButtons[index] = button;
    return button;
  }

  // ── Callbacks ───────────────────────────────────────────────────────────

  void OnSamplingImageTypeClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mImageTypeIndexSampling = (mImageTypeIndexSampling + 1) % IMAGE_TYPE_COUNT_SAMPLING;
    Label::DownCast(mSamplingImageTypeLabel).SetText(IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling]);
    CreateSamplingImageView();
    ApplySamplingMode();
    mSamplingInfoLabel.SetText(MakeSamplingInfoText());
    DALI_LOG_RELEASE_INFO("[Sampling] Image type changed to: %s\n", IMAGE_TYPE_NAMES_SAMPLING[mImageTypeIndexSampling]);
  }

  void OnDesiredSizeImageTypeClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mImageTypeIndexDesired = (mImageTypeIndexDesired + 1) % IMAGE_TYPE_COUNT_DESIRED;
    Label::DownCast(mDesiredImageTypeLabel).SetText(IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired]);
    CreateDesiredSizeImageView();
    ApplyDesiredSize();
    mDesiredSizeInfoLabel.SetText(MakeDesiredSizeInfoText());
    DALI_LOG_RELEASE_INFO("[DesiredSize] Image type changed to: %s\n", IMAGE_TYPE_NAMES_DESIRED[mImageTypeIndexDesired]);
  }

  void OnSamplingButtonClicked(View clickedView, const InputEvent& /*event*/)
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

  void OnDesiredSizeButtonClicked(View clickedView, const InputEvent& /*event*/)
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
  {"NEAREST",         Ui::SamplingMode::NEAREST},
  {"BOX_THEN\nNEAREST", Ui::SamplingMode::BOX_THEN_NEAREST},
  {"BOX_THEN\nLINEAR",  Ui::SamplingMode::BOX_THEN_LINEAR},
  {"LINEAR",          Ui::SamplingMode::LINEAR},
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