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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView SamplingMode + DesiredSize sample:
 *
 * [SamplingMode section]
 * - people-small-10.jpg displayed at full panel width so filtering artifacts are visible
 * - Buttons: NEAREST (pixelated) / BOX_THEN_NEAREST / BOX_THEN_LINEAR / LINEAR
 *
 * [DesiredSize section]
 * - gallery-large-3.jpg loaded at different desired resolutions
 * - Buttons: FULL / HALF / QUARTER
 * - Reload() required because DesiredSize is a loader-time hint
 *
 * Press Escape or Back to quit
 */
class ImageViewSamplingController : public ConnectionTracker
{
  static constexpr int SAMPLING_COUNT = 4;
  static constexpr int SIZE_COUNT     = 3;

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

public:
  explicit ImageViewSamplingController(Application& application)
  : mApplication(application),
    mSamplingIndex(0),
    mSizeIndex(0)
  {
    mApplication.InitSignal().Connect(this, &ImageViewSamplingController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewSamplingController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateSectionLabel("SamplingMode  —  small image displayed large"),
        CreateSamplingImage(),
        CreateSamplingInfoLabel(),
        CreateSamplingButtonRow(),
        CreateSectionLabel("DesiredSize  —  large image loaded at reduced resolution"),
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
      .SetFontSize(12.0f)
      .SetTextColor(UiColor(0x888888))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  View CreateSamplingImage()
  {
    // Load gallery-large-3.jpg at a small DesiredSize so the loader downscales it.
    // Displaying the result large makes the sampling algorithm difference clearly visible:
    // NEAREST looks blocky/pixelated, BOX_THEN_LINEAR looks smooth.
    return ImageView::New(RESOURCES_DIR "gallery-large-3.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FILL)
      .SetDesiredSize(ImageDimensions(64, 64))
      .SetSamplingMode(SAMPLINGS[mSamplingIndex].mode)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mSamplingImage);
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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewSamplingController::OnSamplingButtonClicked);
    mSamplingButtons[index] = button;
    return button;
  }

  View CreateDesiredSizeImage()
  {
    return ImageView::New(RESOURCES_DIR "gallery-large-3.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetDesiredSize(SIZES[mSizeIndex].size)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mDesiredSizeImage);
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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewSamplingController::OnDesiredSizeButtonClicked);
    mSizeButtons[index] = button;
    return button;
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
        mSamplingImage.SetSamplingMode(SAMPLINGS[mSamplingIndex].mode);
        mSamplingImage.Reload();
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
        mDesiredSizeImage.SetDesiredSize(SIZES[mSizeIndex].size);
        mDesiredSizeImage.Reload();
        mDesiredSizeInfoLabel.SetText(MakeDesiredSizeInfoText());
        DALI_LOG_RELEASE_INFO("[DesiredSize] %s (%ux%u)\n",
                              SIZES[mSizeIndex].name,
                              SIZES[mSizeIndex].size.GetWidth(),
                              SIZES[mSizeIndex].size.GetHeight());
        return;
      }
    }
  }

  Dali::String MakeSamplingInfoText() const
  {
    return Dali::String(SAMPLINGS[mSamplingIndex].name);
  }

  Dali::String MakeDesiredSizeInfoText() const
  {
    char buf[64];
    const auto& s = SIZES[mSizeIndex].size;
    if(s.GetWidth() == 0 && s.GetHeight() == 0)
    {
      snprintf(buf, sizeof(buf), "DesiredSize: FULL (no hint)");
    }
    else
    {
      snprintf(buf, sizeof(buf), "DesiredSize: %s (%ux%u)", SIZES[mSizeIndex].name, s.GetWidth(), s.GetHeight());
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
  Ui::ImageView mSamplingImage;
  Ui::ImageView mDesiredSizeImage;
  Label         mSamplingInfoLabel;
  Label         mDesiredSizeInfoLabel;
  View          mSamplingButtons[SAMPLING_COUNT];
  View          mSizeButtons[SIZE_COUNT];
  int           mSamplingIndex;
  int           mSizeIndex;
};

const ImageViewSamplingController::SamplingEntry ImageViewSamplingController::SAMPLINGS[ImageViewSamplingController::SAMPLING_COUNT] = {
  {"NEAREST",         Ui::SamplingMode::NEAREST},
  {"BOX_THEN\nNEAREST", Ui::SamplingMode::BOX_THEN_NEAREST},
  {"BOX_THEN\nLINEAR",  Ui::SamplingMode::BOX_THEN_LINEAR},
  {"LINEAR",          Ui::SamplingMode::LINEAR},
};

const ImageViewSamplingController::DesiredSizeEntry ImageViewSamplingController::SIZES[ImageViewSamplingController::SIZE_COUNT] = {
  {"FULL",    ImageDimensions(0, 0)},
  {"128x128",    ImageDimensions(128, 128)},
  {"32x32", ImageDimensions(32, 32)},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewSamplingController controller(application);
  application.MainLoop();
  return 0;
}
