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
constexpr float TITLE_HEIGHT        = 58.0f;
constexpr float PANEL_HEADER_HEIGHT = 52.0f;
constexpr float DESCRIPTION_HEIGHT  = 110.0f;
} // unnamed namespace

/**
 * Demonstrates the two common ways to supply ImageUrl to ImageView:
 *
 *  1. A path-based ImageUrl keeps a supported cached resource available.
 *  2. A PixelData-based ImageUrl owns the generated external texture.
 *
 * Both examples pass a temporary ImageUrl. ImageView stores its own handle, so
 * the application does not need to keep either temporary ImageUrl alive.
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

    const Dali::String imagePath = RESOURCES_DIR "gallery-large-3.jpg";

    // ImageView stores the temporary path-based ImageUrl before it is destroyed
    // at the end of this statement.
    mPathImageView = ImageView::New(ImageUrlUtils::GenerateUrl(imagePath));

    // PixelData can be discarded after GenerateUrl() uploads it. SetResourceUrl()
    // stores the temporary ImageUrl and therefore keeps the external texture alive.
    PixelData  pixelData       = SyncImageLoader::Load(imagePath);
    const bool pixelDataLoaded = static_cast<bool>(pixelData);
    mPixelDataImageView        = ImageView::New();
    if(pixelDataLoaded)
    {
      mPixelDataImageView.SetResourceUrl(ImageUrlUtils::GenerateUrl(pixelData));
      pixelData.Reset();
    }

    mWindow.Add(CreateContent(pixelDataLoaded ? nullptr : "PixelData load failed"));
  }

  View CreateContent(const char* pixelDataError)
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(6.0f);
    root.SetPadding(Extents(8, 8, 8, 8));

    Label title = CreateLabel("ImageView with ImageUrl", 22.0f, UiColor(0xFFFFFF));
    title.SetRequestedHeight(TITLE_HEIGHT);
    root.Add(title);

    StackLayout panels = StackLayout::New(StackOrientation::HORIZONTAL);
    panels.SetRequestedWidth(MATCH_PARENT);
    panels.SetRequestedHeight(MATCH_PARENT);
    panels.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    panels.SetSpacing(8.0f);
    panels.Add(CreatePanel("PATH-BASED ImageUrl",
                           "ImageView::New(GenerateUrl(path))\n"
                           "ImageView stores the temporary handle",
                           mPathImageView,
                           UiColor(0x16351F)));
    panels.Add(CreatePanel("PixelData-BASED ImageUrl",
                           pixelDataError ? pixelDataError
                                          : "SetResourceUrl(GenerateUrl(pixelData))\n"
                                            "PixelData and temporary handle can be discarded",
                           mPixelDataImageView,
                           UiColor(0x17304A)));
    root.Add(panels);

    Label help = CreateLabel("Escape or Back: quit", 14.0f, UiColor(0xBDBDBD));
    help.SetRequestedHeight(40.0f);
    root.Add(help);

    return root;
  }

  View CreatePanel(const char*    headerText,
                   const char*    descriptionText,
                   ImageView      imageView,
                   const UiColor& backgroundColor)
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetRequestedWidth(MATCH_PARENT);
    panel.SetRequestedHeight(MATCH_PARENT);
    panel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    panel.SetSpacing(6.0f);
    panel.SetPadding(Extents(6, 6, 6, 6));
    panel.SetBackgroundColor(backgroundColor);

    Label header = CreateLabel(headerText, 17.0f, UiColor(0x7CFF9A));
    header.SetRequestedHeight(PANEL_HEADER_HEIGHT);
    panel.Add(header);

    imageView.SetRequestedWidth(MATCH_PARENT);
    imageView.SetRequestedHeight(MATCH_PARENT);
    imageView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    imageView.SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    panel.Add(imageView);

    Label description = CreateLabel(descriptionText, 14.0f, UiColor(0xE0E0E0));
    description.SetRequestedHeight(DESCRIPTION_HEIGHT);
    panel.Add(description);

    return panel;
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
  ImageView    mPathImageView;
  ImageView    mPixelDataImageView;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewWithImageUrlController controller(application);
  application.MainLoop();
  return 0;
}
