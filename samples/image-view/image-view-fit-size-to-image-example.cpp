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
 * ImageView FitSizeToImage sample:
 *
 * Direction: image natural size → view size.
 *
 * Uses a 640x360 (16:9) landscape image with MATCH_PARENT width and WRAP_CONTENT height.
 *
 * OFF: ImageView height stays 0 — only the gray container background is visible.
 * ON : After image loads, height = width * (360/640) ≈ 270px. The view expands to
 *      match the image's aspect ratio via a second layout pass
 *      (ResourceReady → InvalidateMeasure).
 *
 * @note Do NOT use together with SetImageLoadWithViewSize(true).
 *       ImageLoadWithViewSize causes GetNaturalSize to return the current view size
 *       instead of the image's true dimensions, making FitSizeToImage ineffective.
 *
 * Press Escape or Back to quit.
 */
class ImageViewFitSizeController : public ConnectionTracker
{
public:
  explicit ImageViewFitSizeController(Application& application)
  : mApplication(application),
    mFitSizeToImage(false)
  {
    mApplication.InitSignal().Connect(this, &ImageViewFitSizeController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));
    window.Add(CreateContents());
    window.KeyEventSignal().Connect(this, &ImageViewFitSizeController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateSectionLabel("FitSizeToImage  (ImageLoadWithViewSize is always OFF)"),
        CreateFitSizeContainer(),
        CreateFitSizeInfoLabel(),
        CreateFitSizeToggle(),
      });
  }

  View CreateFitSizeContainer()
  {
    // Uses a 640x360 (16:9) landscape image so the aspect-ratio calculation is meaningful.
    // Gray background shows the actual view bounds.
    //
    // FitSizeToImage=OFF → ImageView height stays 0 (WRAP_CONTENT before load),
    //                       only the gray container background is visible.
    // FitSizeToImage=ON  → After load, height = MATCH_PARENT_width * (360/640) ≈ 270px.
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetBackgroundColor(UiColor(0x444444))
      .Children({
        ImageView::New(RESOURCES_DIR "landscape-sample.jpg")
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .SetFitSizeToImage(mFitSizeToImage)
          .SetImageLoadWithViewSize(false)
          .As(mFitImage),
      });
  }

  View CreateFitSizeInfoLabel()
  {
    return Label::New(MakeFitSizeInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(32.0f)
      .SetFontSize(13.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mFitSizeInfoLabel);
  }

  View CreateFitSizeToggle()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(MATCH_PARENT)
                           .SetRequestedHeight(56.0f)
                           .SetViewPadding(Extents(4, 4, 4, 4))
                           .SetBackgroundColor(UiColor(0x444444))
                           .Children({
                             Label::New("FIT_SIZE: OFF")
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(13.0f)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER)
                               .As(mFitSizeLabel)
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImageViewFitSizeController::OnFitSizeToggleClicked);
    return button;
  }

  View CreateSectionLabel(const char* text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(28.0f)
      .SetFontSize(11.0f)
      .SetTextColor(UiColor(0x888888))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
  }

  void OnFitSizeToggleClicked(View /*clickedView*/, const InputEvent& /*event*/)
  {
    mFitSizeToImage = !mFitSizeToImage;
    mFitImage.SetFitSizeToImage(mFitSizeToImage);
    mFitSizeLabel.SetText(mFitSizeToImage ? "FIT_SIZE: ON" : "FIT_SIZE: OFF");
    mFitSizeInfoLabel.SetText(MakeFitSizeInfoText());
    DALI_LOG_RELEASE_INFO("[FitSizeToImage] FitSizeToImage=%d\n", mFitSizeToImage);
  }

  Dali::String MakeFitSizeInfoText() const
  {
    return Dali::String(mFitSizeToImage ? "FitSizeToImage: ON  — view height matches image aspect ratio"
                                        : "FitSizeToImage: OFF — view height stays 0 (gray area visible)");
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
  Ui::ImageView mFitImage;
  Label         mFitSizeInfoLabel;
  Label         mFitSizeLabel;
  bool          mFitSizeToImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewFitSizeController controller(application);
  application.MainLoop();
  return 0;
}
