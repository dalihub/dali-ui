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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float TITLE_HEIGHT       = 58.0f;
constexpr float DESCRIPTION_HEIGHT = 110.0f;
} // unnamed namespace

/**
 * Demonstrates how to display PixelData through ImageUrl.
 *
 * ImageView stores the ImageUrl handle passed to New(), so the application
 * does not need to keep either the PixelData or a separate ImageUrl alive.
 */
class ImageViewWithImageUrlController : public ConnectionTracker
{
public:
  explicit ImageViewWithImageUrlController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageViewWithImageUrlController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(UiColor(0x171717));
    mWindow.KeyEventSignal().Connect(this, &ImageViewWithImageUrlController::OnKeyEvent);

    PixelData pixelData = SyncImageLoader::Load(RESOURCES_DIR "gallery-large-3.jpg");
    if(pixelData)
    {
      // The temporary ImageUrl is retained by ImageView. The PixelData and the
      // application's ImageUrl handle may both be discarded after this call.
      mImageView = ImageView::New(ImageUrlUtils::GenerateUrl(pixelData));
      pixelData.Reset();
    }
    else
    {
      mImageView = ImageView::New();
    }

    mWindow.Add(CreateContent(static_cast<bool>(mImageView.GetResourceUrl().Size())));
  }

  View CreateContent(bool imageLoaded)
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(8.0f);
    root.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    Label title = CreateLabel("ImageView with PixelData ImageUrl", 22.0f, UiColor(0xFFFFFF));
    title.SetRequestedHeight(TITLE_HEIGHT);
    root.Add(title);

    mImageView.SetRequestedWidth(MATCH_PARENT);
    mImageView.SetRequestedHeight(MATCH_PARENT);
    mImageView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mImageView.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    root.Add(mImageView);

    Label description = CreateLabel(imageLoaded
                                      ? "ImageView::New(ImageUrlUtils::GenerateUrl(pixelData))\n"
                                        "ImageView owns the ImageUrl handle"
                                      : "PixelData load failed",
                                    15.0f,
                                    imageLoaded ? UiColor(0x7CFF9A) : UiColor(0xFF8A80));
    description.SetRequestedHeight(DESCRIPTION_HEIGHT);
    root.Add(description);

    return root;
  }

  Label CreateLabel(const char* text, float fontSize, const UiColor& color)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetFontSize(fontSize);
    label.SetTextColor(color);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
  Window       mWindow;
  ImageView    mImageView;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewWithImageUrlController controller(application);
  application.MainLoop();
  return 0;
}
