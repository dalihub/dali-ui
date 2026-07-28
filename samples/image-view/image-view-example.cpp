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

namespace
{
const char* const REMOTE_IMAGE_URL = "https://raw.githubusercontent.com/dalihub/dali-test/refs/heads/master/visual-tests/remote-download/images/rockstar.jpg";
}

/**
 * ImageView basic sample:
 * - Loads local and remote images and connects ResourceReadySignal
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

    mLocalImage = ImageView::New(RESOURCES_DIR "gallery-large-3.jpg");
    mLocalImage.SetRequestedWidth(halfW - 4.0f);
    mLocalImage.SetRequestedHeight(halfH - 4.0f);
    mLocalImage.SetRequestedX(0.0f);
    mLocalImage.SetRequestedY(0.0f);
    window.Add(mLocalImage);

    mRemoteImage = ImageView::New(REMOTE_IMAGE_URL);
    mRemoteImage.SetRequestedWidth(halfW - 4.0f);
    mRemoteImage.SetRequestedHeight(halfH - 4.0f);
    mRemoteImage.SetRequestedX(halfW);
    mRemoteImage.SetRequestedY(0.0f);
    window.Add(mRemoteImage);

    mLocalImage.ResourceReadySignal().Connect(this, &ImageViewSampleController::OnResourceReady);
    mRemoteImage.ResourceReadySignal().Connect(this, &ImageViewSampleController::OnResourceReady);

    window.KeyEventSignal().Connect(this, &ImageViewSampleController::OnKeyEvent);
  }

  void OnResourceReady(View view)
  {
    ImageView imageView = ImageView::DownCast(view);
    DALI_LOG_RELEASE_INFO("[ImageView] ResourceReadySignal fired. url=%s status=%d\n",
                          imageView.GetResourceUrl().CStr(),
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
  Ui::ImageView mLocalImage;
  Ui::ImageView mRemoteImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewSampleController sample(application);
  application.MainLoop();
  return 0;
}
