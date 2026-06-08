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

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.AddChildren({CreateCompareRow(), CreateInfoLabel(), CreateMaskButtonRow(), CreateImageTypeButton(), CreateModeToggle()});
    window.Add(root);

    window.KeyEventSignal().Connect(this, &ImageAlphaMaskController::OnKeyEvent);
  }

  // ── Widget builders ────────────────────────────────────────────────────────

  View CreateCompareRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(WRAP_CONTENT);
    row.SetSpacing(8.0f);
    row.SetPadding(Extents(8, 8, 8, 8));
    row.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    row.AddChildren({CreatePanel("CROP: OFF", false, PANEL_CROP_OFF), CreatePanel("CROP: ON", true, PANEL_CROP_ON)});
    return row;
  }

  View CreateSpacer()
  {
    StackLayout spacer = StackLayout::New(StackOrientation::VERTICAL);
    spacer.SetRequestedWidth(MATCH_PARENT);
    spacer.SetRequestedHeight(WRAP_CONTENT);
    spacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    return spacer;
  }

  View CreatePanel(const char* title, bool cropToMask, int panelIdx)
  {
    mStaticImages[panelIdx] = ImageView::New(STATIC_URL);
    mStaticImages[panelIdx].SetRequestedWidth(cropToMask ? WRAP_CONTENT : MATCH_PARENT);
    mStaticImages[panelIdx].SetRequestedHeight(WRAP_CONTENT);
    mStaticImages[panelIdx].SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    mStaticImages[panelIdx].SetAlphaMaskUrl(MASKS[mMaskIndex].url);
    mStaticImages[panelIdx].SetCropToMask(cropToMask);
    mStaticImages[panelIdx].SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING);
    mStaticImages[panelIdx].SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));

    mAnimatedImages[panelIdx] = AnimatedImageView::New(ANIMATED_URL);
    mAnimatedImages[panelIdx].SetRequestedWidth(cropToMask ? WRAP_CONTENT : MATCH_PARENT);
    mAnimatedImages[panelIdx].SetRequestedHeight(WRAP_CONTENT);
    mAnimatedImages[panelIdx].SetAlphaMaskUrl(MASKS[mMaskIndex].url);
    mAnimatedImages[panelIdx].SetCropToMask(cropToMask);
    mAnimatedImages[panelIdx].SetMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING);
    mAnimatedImages[panelIdx].SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    mAnimatedImages[panelIdx].SetVisibility(false);

    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetRequestedWidth(WRAP_CONTENT);
    panel.SetRequestedHeight(MATCH_PARENT);
    panel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    panel.SetBackgroundColor(UiColor(0x2A2A2A));

    Label titleLabel = Label::New(title);
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(36.0f);
    titleLabel.SetFontSize(14.0f);
    titleLabel.SetTextColor(UiColor(0xFFFFFF));
    titleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    panel.AddChildren({titleLabel, CreateSpacer(), mStaticImages[panelIdx], mAnimatedImages[panelIdx], CreateSpacer()});
    return panel;
  }

  View CreateInfoLabel()
  {
    mInfoLabel = Label::New(MakeInfoText());
    mInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mInfoLabel.SetRequestedHeight(36.0f);
    mInfoLabel.SetFontSize(13.0f);
    mInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mInfoLabel;
  }

  View CreateMaskButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(60.0f);
    row.SetPadding(Extents(4, 4, 4, 4));
    for(int i = 0; i < MASK_COUNT; ++i)
    {
      row.Add(CreateMaskButton(i));
    }
    return row;
  }

  View CreateMaskButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(index == mMaskIndex ? UiColor(0x4A90E2) : UiColor(0x333333));
    Label buttonLabel = Label::New(MASKS[index].name);
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(14.0f);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageAlphaMaskController::OnMaskButtonClicked);
    mMaskButtons[index] = button;
    return button;
  }

  View CreateImageTypeButton()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(60.0f);
    button.SetPadding(Extents(4, 4, 4, 4));
    button.SetBackgroundColor(UiColor(0x2E7D32));
    mImageTypeLabel = Label::New(MakeImageTypeText());
    mImageTypeLabel.SetRequestedWidth(MATCH_PARENT);
    mImageTypeLabel.SetRequestedHeight(MATCH_PARENT);
    mImageTypeLabel.SetFontSize(14.0f);
    mImageTypeLabel.SetTextColor(UiColor(0xFFFFFF));
    mImageTypeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mImageTypeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(mImageTypeLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageAlphaMaskController::OnImageTypeToggleClicked);
    return button;
  }

  View CreateModeToggle()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(60.0f);
    button.SetPadding(Extents(4, 4, 4, 4));
    button.SetBackgroundColor(UiColor(0x444444));
    mModeLabel = Label::New("MODE: ON_RENDERING");
    mModeLabel.SetRequestedWidth(MATCH_PARENT);
    mModeLabel.SetRequestedHeight(MATCH_PARENT);
    mModeLabel.SetFontSize(14.0f);
    mModeLabel.SetTextColor(UiColor(0xFFFFFF));
    mModeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mModeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(mModeLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageAlphaMaskController::OnModeToggleClicked);
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
