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
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView basic sample:
 * - Loads an image and connects ResourceReadySignal
 * - Press Escape or Back to quit
 */
class ImageViewSampleController : public ConnectionTracker
{
public:
  explicit ImageViewSampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageViewSampleController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window     = application.GetWindow();
    auto   posSize    = window.GetPositionSize();
    float  halfW      = posSize.width * 0.5f;
    float  halfH      = posSize.height * 0.5f;
    window.SetBackgroundColor(Color::DARK_GRAY);

    mImage = ImageView::New(RESOURCES_DIR "gallery-large-3.jpg");
    mImage.SetRequestedWidth(halfW - 4.0f);
    mImage.SetRequestedHeight(halfH - 4.0f);
    mImage.SetRequestedPositionX(0.0f);
    mImage.SetRequestedPositionY(0.0f);
    window.Add(mImage);

    mImage.ResourceReadySignal().Connect(this, &ImageViewSampleController::OnResourceReady);

    window.KeyEventSignal().Connect(this, &ImageViewSampleController::OnKeyEvent);
  }

  void OnResourceReady(View view)
  {
    ImageView imageView = ImageView::DownCast(view);
    DALI_LOG_RELEASE_INFO("[ImageView] ResourceReadySignal fired. status=%d\n",
                          static_cast<int>(imageView.GetLoadingStatus()));
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
  Ui::ImageView mImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewSampleController sample(application);
  application.MainLoop();
  return 0;
}
