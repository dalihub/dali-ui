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
 * ImageView ImageColor sample:
 * - SetImageColor() applies an RGBA color as a per-pixel multiplier on the image
 * - WHITE (0xFFFFFF) means no tint — the image appears as original
 * - Preset buttons demonstrate: no tint, red/green/blue channels, desaturate (gray), dim
 * - Alpha column shows partial transparency via the alpha component
 * - Press Escape or Back to quit
 */
class ImageViewImageColorController : public ConnectionTracker
{
  static constexpr int COLOR_COUNT = 6;

  struct ColorEntry
  {
    const char* name;
    UiColor     color;
  };

  static const ColorEntry COLORS[COLOR_COUNT];

public:
  explicit ImageViewImageColorController(Application& application)
  : mApplication(application),
    mColorIndex(0)
  {
    mApplication.InitSignal().Connect(this, &ImageViewImageColorController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewImageColorController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateImageArea(),
        CreateInfoLabel(),
        CreateColorButtonRow(),
      });
  }

  View CreateImageArea()
  {
    return ImageView::New(RESOURCES_DIR "gallery-large-3.jpg")
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFittingMode(Ui::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetImageColor(COLORS[mColorIndex].color)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
      .As(mImage);
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
                        .SetViewPadding(Extents(4, 4, 4, 4));

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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewImageColorController::OnColorButtonClicked);
    mButtons[index] = button;
    return button;
  }

  void OnColorButtonClicked(View clickedView, const InputEvent& /*event*/)
  {
    for(int i = 0; i < COLOR_COUNT; ++i)
    {
      if(mButtons[i] == clickedView)
      {
        mButtons[mColorIndex].SetBackgroundColor(UiColor(0x333333));
        mColorIndex = i;
        mButtons[mColorIndex].SetBackgroundColor(UiColor(0x4A90E2));
        mImage.SetImageColor(COLORS[mColorIndex].color);
        mInfoLabel.SetText(MakeInfoText());

        DALI_LOG_RELEASE_INFO("[ImageColor] color changed to: %s\n", COLORS[mColorIndex].name);
        return;
      }
    }
  }

  Dali::String MakeInfoText() const
  {
    return Dali::String(COLORS[mColorIndex].name);
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
  Ui::ImageView mImage;
  Label         mInfoLabel;
  View          mButtons[COLOR_COUNT];
  int           mColorIndex;
};

const ImageViewImageColorController::ColorEntry ImageViewImageColorController::COLORS[ImageViewImageColorController::COLOR_COUNT] = {
  {"ORIGINAL", UiColor(1.0f, 1.0f, 1.0f, 1.0f)},
  {"RED",      UiColor(1.0f, 0.0f, 0.0f, 1.0f)},
  {"GREEN",    UiColor(0.0f, 1.0f, 0.0f, 1.0f)},
  {"BLUE",     UiColor(0.0f, 0.0f, 1.0f, 1.0f)},
  {"GRAY",     UiColor(0.5f, 0.5f, 0.5f, 1.0f)},
  {"50%\nALPHA", UiColor(1.0f, 1.0f, 1.0f, 0.5f)},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewImageColorController controller(application);
  application.MainLoop();
  return 0;
}
