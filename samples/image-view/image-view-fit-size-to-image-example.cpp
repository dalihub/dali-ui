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
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));
    window.Add(CreateContents());
    window.KeyEventSignal().Connect(this, &ImageViewFitSizeController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.AddChildren({
      CreateSectionLabel("FitSizeToImage  (ImageLoadWithViewSize is always OFF)"),
      CreateFitSizeContainer(),
      CreateFitSizeInfoLabel(),
      CreateFitSizeToggle(),
    });
    return contents;
  }

  View CreateFitSizeContainer()
  {
    // Uses a 640x360 (16:9) landscape image so the aspect-ratio calculation is meaningful.
    // Gray background shows the actual view bounds.
    //
    // FitSizeToImage=OFF → ImageView height stays 0 (WRAP_CONTENT before load),
    //                       only the gray container background is visible.
    // FitSizeToImage=ON  → After load, height = MATCH_PARENT_width * (360/640) ≈ 270px.
    mFitImage = ImageView::New(RESOURCES_DIR "landscape-sample.jpg");
    mFitImage.SetRequestedWidth(MATCH_PARENT);
    mFitImage.SetRequestedHeight(WRAP_CONTENT);
    mFitImage.SetFitSizeToImage(mFitSizeToImage);
    mFitImage.SetImageLoadWithViewSize(false);

    StackLayout container = StackLayout::New(StackOrientation::VERTICAL);
    container.SetRequestedWidth(MATCH_PARENT);
    container.SetRequestedHeight(WRAP_CONTENT);
    container.SetBackgroundColor(UiColor(0x444444));
    container.AddChildren({mFitImage});
    return container;
  }

  View CreateFitSizeInfoLabel()
  {
    mFitSizeInfoLabel = Label::New(MakeFitSizeInfoText());
    mFitSizeInfoLabel.SetRequestedWidth(MATCH_PARENT);
    mFitSizeInfoLabel.SetRequestedHeight(32.0f);
    mFitSizeInfoLabel.SetFontSize(13.0f);
    mFitSizeInfoLabel.SetTextColor(UiColor(0xCCCCCC));
    mFitSizeInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mFitSizeInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return mFitSizeInfoLabel;
  }

  View CreateFitSizeToggle()
  {
    mFitSizeLabel = Label::New("FIT_SIZE: OFF");
    mFitSizeLabel.SetRequestedWidth(MATCH_PARENT);
    mFitSizeLabel.SetRequestedHeight(MATCH_PARENT);
    mFitSizeLabel.SetFontSize(13.0f);
    mFitSizeLabel.SetTextColor(UiColor(0xFFFFFF));
    mFitSizeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mFitSizeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(56.0f);
    button.SetPadding(Extents(4, 4, 4, 4));
    button.SetBackgroundColor(UiColor(0x444444));
    button.AddChildren({mFitSizeLabel});

    button.AsInteractive().ClickedSignal().Connect(this, &ImageViewFitSizeController::OnFitSizeToggleClicked);
    return button;
  }

  View CreateSectionLabel(const char* text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(28.0f);
    label.SetFontSize(11.0f);
    label.SetTextColor(UiColor(0x888888));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  void OnFitSizeToggleClicked(View /*clickedView*/, InputEvent /*event*/)
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
