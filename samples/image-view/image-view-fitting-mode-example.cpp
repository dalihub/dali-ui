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
} // namespace

/**
 * ImageView FittingMode sample:
 * - Shows an image with the current FittingMode applied
 * - 4 buttons at the bottom to select a FittingMode
 * - 1 button to cycle through images
 * - Active fitting mode button is highlighted
 * - Press Escape or Back to quit
 */
class ImageViewFittingModeController : public ConnectionTracker
{
  static constexpr int MODE_COUNT = 4;

  struct ModeEntry
  {
    Ui::FittingMode::Type mode;
    const char*           name;
  };

  static constexpr ModeEntry MODES[MODE_COUNT] = {
    {Ui::FittingMode::FIT_KEEP_ASPECT_RATIO, "FIT\nKEEP\nASPECT"},
    {Ui::FittingMode::FILL, "FILL"},
    {Ui::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO, "OVER\nFIT"},
    {Ui::FittingMode::CENTER, "CENTER"},
  };

public:
  explicit ImageViewFittingModeController(Application& application)
  : mApplication(application),
    mActiveIndex(0),
    mImageIndex(0)
  {
    mApplication.InitSignal().Connect(this, &ImageViewFittingModeController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImageViewFittingModeController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        ImageView::New(IMAGES[mImageIndex])
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .SetFittingMode(MODES[mActiveIndex].mode)
          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
          .As(mImage),
        CreateButtonRow(),
      });
  }

  View CreateButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .Spacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(100.0f)
                        .SetViewPadding(Extents(4, 4, 4, 4));

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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewFittingModeController::OnModeButtonClicked);

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

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewFittingModeController::OnSwapButtonClicked);

    mSwapButton = button;
    return button;
  }

  void OnModeButtonClicked(View clickedView, const InputEvent& /*event*/)
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

  void OnSwapButtonClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mImageIndex = (mImageIndex + 1) % IMAGE_COUNT;
    mImage.SetResourceUrl(IMAGES[mImageIndex]);
    DALI_LOG_RELEASE_INFO("Image changed to: %s\n", IMAGES[mImageIndex]);
  }

  void SelectMode(int index)
  {
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x333333));
    mActiveIndex = index;
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x4A90E2));
    mImage.SetFittingMode(MODES[mActiveIndex].mode);

    DALI_LOG_RELEASE_INFO("FittingMode changed to: %s\n", MODES[mActiveIndex].name);
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
  View          mButtons[MODE_COUNT];
  View          mSwapButton;
  int           mActiveIndex;
  int           mImageIndex;
};

constexpr ImageViewFittingModeController::ModeEntry ImageViewFittingModeController::MODES[ImageViewFittingModeController::MODE_COUNT];

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewFittingModeController controller(application);
  application.MainLoop();
  return 0;
}
