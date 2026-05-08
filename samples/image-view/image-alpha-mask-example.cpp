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
#include <dali-ui-foundation/public-api/animated-image-view.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * AlphaMask sample — tests ImageView and AnimatedImageView:
 * - Left panel : CropToMask=false — full image with mask alpha applied
 * - Right panel: CropToMask=true  — image cropped to mask bounding box
 * - IMAGE TYPE button: toggle between static (ImageView) and animated (AnimatedImageView)
 * - MASK buttons: switch between two mask shapes (CIRCLE, BUBBLE)
 * - MODE button: toggle MaskingMode between ON_RENDERING and ON_LOADING
 * - Press Escape or Back to quit
 */
class ImageAlphaMaskController : public ConnectionTracker
{
  // ── Constants ─────────────────────────────────────────────────────────────

  static constexpr int PANEL_CROP_OFF = 0;
  static constexpr int PANEL_CROP_ON  = 1;
  static constexpr int PANEL_COUNT    = 2;
  static constexpr int MASK_COUNT     = 2;
  static constexpr int IMAGE_TYPE_COUNT = 2;

  struct MaskEntry
  {
    const char* name;
    const char* url;
  };

  struct ImageTypeEntry
  {
    const char* name;
  };

  static const char* const    STATIC_URL;
  static const char* const    ANIMATED_URL;
  static const MaskEntry      MASKS[MASK_COUNT];
  static const ImageTypeEntry IMAGE_TYPES[IMAGE_TYPE_COUNT];

public:
  explicit ImageAlphaMaskController(Application& application)
  : mApplication(application),
    mMaskIndex(0),
    mImageTypeIndex(0),
    mMaskingOnLoading(false)
  {
    mApplication.InitSignal().Connect(this, &ImageAlphaMaskController::OnInit);
  }

private:
  // ── Initialization ─────────────────────────────────────────────────────────

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
                 .SetRequestedWidth(MATCH_PARENT)
                 .SetRequestedHeight(MATCH_PARENT)
                 .Children({
                   CreateCompareRow(),
                   CreateInfoLabel(),
                   CreateMaskButtonRow(),
                   CreateImageTypeButton(),
                   CreateModeToggle(),
                 }));

    window.KeyEventSignal().Connect(this, &ImageAlphaMaskController::OnKeyEvent);
  }

  // ── Widget builders ────────────────────────────────────────────────────────

  View CreateCompareRow()
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetSpacing(8.0f)
      .SetPadding(Extents(8, 8, 8, 8))
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .Children({
        CreatePanel("CROP: OFF", false, PANEL_CROP_OFF),
        CreatePanel("CROP: ON",  true,  PANEL_CROP_ON),
      });
  }

  View CreateSpacer()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  }

  View CreatePanel(const char* title, bool cropToMask, int panelIdx)
  {
    ImageView::New(STATIC_URL)
      .SetRequestedWidth(cropToMask ? WRAP_CONTENT : MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetAlphaMaskUrl(MASKS[mMaskIndex].url)
      .SetCropToMask(cropToMask)
      .SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING)
      .SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER))
      .As(mStaticImages[panelIdx]);

    AnimatedImageView::New(ANIMATED_URL)
      .SetRequestedWidth(cropToMask ? WRAP_CONTENT : MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetAlphaMaskUrl(MASKS[mMaskIndex].url)
      .SetCropToMask(cropToMask)
      .SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING)
      .SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER))
      .SetVisibility(false)
      .As(mAnimatedImages[panelIdx]);

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
        mStaticImages[panelIdx],
        mAnimatedImages[panelIdx],
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
                        .SetSpacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(60.0f)
                        .SetPadding(Extents(4, 4, 4, 4));
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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageAlphaMaskController::OnMaskButtonClicked);
    mMaskButtons[index] = button;
    return button;
  }

  View CreateImageTypeButton()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(MATCH_PARENT)
                           .SetRequestedHeight(60.0f)
                           .SetPadding(Extents(4, 4, 4, 4))
                           .SetBackgroundColor(UiColor(0x2E7D32))
                           .Children({
                             Label::New(MakeImageTypeText())
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(14.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER)
                               .As(mImageTypeLabel),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageAlphaMaskController::OnImageTypeToggleClicked);
    return button;
  }

  View CreateModeToggle()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(MATCH_PARENT)
                           .SetRequestedHeight(60.0f)
                           .SetPadding(Extents(4, 4, 4, 4))
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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageAlphaMaskController::OnModeToggleClicked);
    return button;
  }

  // ── Event handlers ─────────────────────────────────────────────────────────

  void OnMaskButtonClicked(View clickedView, InputEvent /*event*/)
  {
    for(int i = 0; i < MASK_COUNT; ++i)
    {
      if(mMaskButtons[i] == clickedView)
      {
        mMaskButtons[mMaskIndex].SetBackgroundColor(UiColor(0x333333));
        mMaskIndex = i;
        mMaskButtons[mMaskIndex].SetBackgroundColor(UiColor(0x4A90E2));

        ApplyMaskToAll();
        mInfoLabel.SetText(MakeInfoText());
        DALI_LOG_RELEASE_INFO("[AlphaMask] mask=%s\n", MASKS[mMaskIndex].name);
        return;
      }
    }
  }

  void OnImageTypeToggleClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mImageTypeIndex = (mImageTypeIndex + 1) % IMAGE_TYPE_COUNT;
    const bool showAnimated = (mImageTypeIndex == 1);

    for(int i = 0; i < PANEL_COUNT; ++i)
    {
      mStaticImages[i].SetVisibility(!showAnimated);
      mAnimatedImages[i].SetVisibility(showAnimated);
    }

    if(showAnimated)
    {
      for(int i = 0; i < PANEL_COUNT; ++i)
      {
        mAnimatedImages[i].Play();
      }
    }

    mImageTypeLabel.SetText(MakeImageTypeText());
    mInfoLabel.SetText(MakeInfoText());
    DALI_LOG_RELEASE_INFO("[AlphaMask] imageType=%s\n", IMAGE_TYPES[mImageTypeIndex].name);
  }

  void OnModeToggleClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mMaskingOnLoading = !mMaskingOnLoading;
    const auto mode   = mMaskingOnLoading ? Ui::Image::MaskingType::MASKING_ON_LOADING : Ui::Image::MaskingType::MASKING_ON_RENDERING;

    for(int i = 0; i < PANEL_COUNT; ++i)
    {
      mStaticImages[i].SetMaskingMode(mode);
      mStaticImages[i].Reload();
      mAnimatedImages[i].SetMaskingMode(mode);
      if(mImageTypeIndex == 1)
      {
        mAnimatedImages[i].Play();
      }
    }

    mModeLabel.SetText(mMaskingOnLoading ? "MODE: ON_LOADING" : "MODE: ON_RENDERING");
    mInfoLabel.SetText(MakeInfoText());
    DALI_LOG_RELEASE_INFO("[AlphaMask] MaskingMode=%s\n", mMaskingOnLoading ? "LOADING" : "RENDERING");
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

  // ── Helpers ─────────────────────────────────────────────────────────────────

  void ApplyMaskToAll()
  {
    for(int i = 0; i < PANEL_COUNT; ++i)
    {
      mStaticImages[i].SetAlphaMaskUrl(MASKS[mMaskIndex].url);
      mStaticImages[i].Reload();
      mAnimatedImages[i].SetAlphaMaskUrl(MASKS[mMaskIndex].url);
      if(mImageTypeIndex == 1)
      {
        mAnimatedImages[i].Play();
      }
    }
  }

  Dali::String MakeInfoText() const
  {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "Type: %s | Mask: %s | Mode: %s",
             IMAGE_TYPES[mImageTypeIndex].name,
             MASKS[mMaskIndex].name,
             mMaskingOnLoading ? "ON_LOADING" : "ON_RENDERING");
    return Dali::String(buf);
  }

  Dali::String MakeImageTypeText() const
  {
    char buf[64];
    snprintf(buf, sizeof(buf), "IMAGE TYPE: %s", IMAGE_TYPES[mImageTypeIndex].name);
    return Dali::String(buf);
  }

  // ── Members ─────────────────────────────────────────────────────────────────

  Application&     mApplication;
  Ui::ImageView    mStaticImages[PANEL_COUNT];
  AnimatedImageView mAnimatedImages[PANEL_COUNT];
  Label            mInfoLabel;
  Label            mModeLabel;
  Label            mImageTypeLabel;
  View             mMaskButtons[MASK_COUNT];
  int              mMaskIndex;
  int              mImageTypeIndex;
  bool             mMaskingOnLoading;
};

// ── Static data ───────────────────────────────────────────────────────────────

const char* const ImageAlphaMaskController::STATIC_URL   = RESOURCES_DIR "gallery-large-1.jpg";
const char* const ImageAlphaMaskController::ANIMATED_URL = RESOURCES_DIR "dali-logo-anim.gif";

const ImageAlphaMaskController::MaskEntry ImageAlphaMaskController::MASKS[ImageAlphaMaskController::MASK_COUNT] = {
  {"CIRCLE", RESOURCES_DIR "mask.png"},
  {"BUBBLE", RESOURCES_DIR "shape-bubble.png"},
};

const ImageAlphaMaskController::ImageTypeEntry ImageAlphaMaskController::IMAGE_TYPES[ImageAlphaMaskController::IMAGE_TYPE_COUNT] = {
  {"STATIC"},
  {"ANIMATED"},
};

// ── Entry point ───────────────────────────────────────────────────────────────

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageAlphaMaskController controller(application);
  application.MainLoop();
  return 0;
}
