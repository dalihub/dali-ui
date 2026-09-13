/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
constexpr uint32_t BLUR_RADIUS = 100u;
constexpr float    DOWNSCALE   = 0.25f;
const char* const  LEFT_IMAGE  = RESOURCES_DIR "gallery-medium-49.jpg";
const char* const  RIGHT_IMAGE = RESOURCES_DIR "gallery-medium-17.jpg";
} // unnamed namespace

class BlurEffectsGridController : public ConnectionTracker
{
public:
  explicit BlurEffectsGridController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &BlurEffectsGridController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetPositionSize(PositionSize(80, 60, 1280, 720));
    mWindow.SetBackgroundColor(Color::BLACK);

    const PositionSize windowSize = mWindow.GetPositionSize();
    const float        halfWidth  = windowSize.width * 0.5f;
    const float        halfHeight = windowSize.height * 0.5f;

    mRoot = CreateView(0.0f, 0.0f, windowSize.width, windowSize.height);
    mWindow.Add(mRoot);

    mGaussianOnceImage     = CreateImage(0.0f, 0.0f, halfWidth, halfHeight);
    mGaussianAlwaysImage   = CreateImage(halfWidth, 0.0f, halfWidth, halfHeight);
    mBackgroundOnceImage   = CreateImage(0.0f, halfHeight, halfWidth, halfHeight);
    mBackgroundAlwaysImage = CreateImage(halfWidth, halfHeight, halfWidth, halfHeight);
    mBackgroundOncePane    = CreateView(0.0f, halfHeight, halfWidth, halfHeight);
    mBackgroundAlwaysPane  = CreateView(halfWidth, halfHeight, halfWidth, halfHeight);

    mGaussianOnceImage.ResourceReadySignal().Connect(this, &BlurEffectsGridController::OnGaussianOnceReady);
    mGaussianAlwaysImage.ResourceReadySignal().Connect(this, &BlurEffectsGridController::OnGaussianAlwaysReady);
    mBackgroundOnceImage.ResourceReadySignal().Connect(this, &BlurEffectsGridController::OnBackgroundOnceReady);
    mBackgroundAlwaysImage.ResourceReadySignal().Connect(this, &BlurEffectsGridController::OnBackgroundAlwaysReady);

    mRoot.Add(mGaussianOnceImage);
    mRoot.Add(mGaussianAlwaysImage);
    mRoot.Add(mBackgroundOnceImage);
    mRoot.Add(mBackgroundAlwaysImage);

    mStopper = View::New();
    mRoot.Add(mStopper);
    mRoot.Add(mBackgroundOncePane);
    mRoot.Add(mBackgroundAlwaysPane);

    AddLabel("GaussianBlurEffect / Once", 0.0f, 0.0f, halfWidth);
    AddLabel("GaussianBlurEffect / Always", halfWidth, 0.0f, halfWidth);
    AddLabel("BackgroundBlurEffect / Once", 0.0f, halfHeight, halfWidth);
    AddLabel("BackgroundBlurEffect / Always", halfWidth, halfHeight, halfWidth);

    mGaussianOnceImage.SetResourceUrl(LEFT_IMAGE);
    mGaussianAlwaysImage.SetResourceUrl(RIGHT_IMAGE);
    mBackgroundOnceImage.SetResourceUrl(LEFT_IMAGE);
    mBackgroundAlwaysImage.SetResourceUrl(RIGHT_IMAGE);
  }

  static View CreateView(float x, float y, float width, float height)
  {
    View view = View::New();
    view.SetLayoutMode(LayoutMode::STANDALONE);
    view.SetRequestedX(x);
    view.SetRequestedY(y);
    view.SetRequestedWidth(width);
    view.SetRequestedHeight(height);
    view.SetParentOrigin(ParentOrigin::TOP_LEFT);
    view.SetPivot(Pivot::TOP_LEFT);
    return view;
  }

  static ImageView CreateImage(float x, float y, float width, float height)
  {
    ImageView image = ImageView::New();
    image.SetLayoutMode(LayoutMode::STANDALONE);
    image.SetRequestedX(x);
    image.SetRequestedY(y);
    image.SetRequestedWidth(width);
    image.SetRequestedHeight(height);
    image.SetParentOrigin(ParentOrigin::TOP_LEFT);
    image.SetPivot(Pivot::TOP_LEFT);
    image.SetFittingMode(Image::FittingMode::FILL);
    return image;
  }

  void AddLabel(const char* text, float x, float y, float width)
  {
    Label label = Label::New(text);
    label.SetLayoutMode(LayoutMode::STANDALONE);
    label.SetRequestedX(x);
    label.SetRequestedY(y);
    label.SetRequestedWidth(width);
    label.SetRequestedHeight(54.0f);
    label.SetParentOrigin(ParentOrigin::TOP_LEFT);
    label.SetPivot(Pivot::TOP_LEFT);
    label.SetFontSize(22.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.72f));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mRoot.Add(label);
  }

  static GaussianBlurEffect CreateGaussianEffect(bool blurOnce)
  {
    GaussianBlurEffect effect = GaussianBlurEffect::New(BLUR_RADIUS);
    effect.SetBlurOnce(blurOnce);
    effect.SetBlurDownscaleFactor(DOWNSCALE);
    return effect;
  }

  BackgroundBlurEffect CreateBackgroundEffect(bool blurOnce)
  {
    BackgroundBlurEffect effect = BackgroundBlurEffect::New(BLUR_RADIUS);
    effect.SetBlurOnce(blurOnce);
    effect.SetBlurDownscaleFactor(DOWNSCALE);
    effect.SetSourceView(mRoot);
    effect.SetStopperView(mStopper);
    return effect;
  }

  void OnGaussianOnceReady(View)
  {
    mGaussianOnceEffect = CreateGaussianEffect(true);
    mGaussianOnceImage.SetRenderEffect(mGaussianOnceEffect);
    mGaussianOnceEffect.Activate();
  }

  void OnGaussianAlwaysReady(View)
  {
    mGaussianAlwaysEffect = CreateGaussianEffect(false);
    mGaussianAlwaysImage.SetRenderEffect(mGaussianAlwaysEffect);
    mGaussianAlwaysEffect.Activate();
  }

  void OnBackgroundOnceReady(View)
  {
    mBackgroundOnceEffect = CreateBackgroundEffect(true);
    mBackgroundOncePane.SetRenderEffect(mBackgroundOnceEffect);
    mBackgroundOnceEffect.Activate();
  }

  void OnBackgroundAlwaysReady(View)
  {
    mBackgroundAlwaysEffect = CreateBackgroundEffect(false);
    mBackgroundAlwaysPane.SetRenderEffect(mBackgroundAlwaysEffect);
    mBackgroundAlwaysEffect.Activate();
  }

private:
  Application&         mApplication;
  Window               mWindow;
  View                 mRoot;
  View                 mStopper;
  View                 mBackgroundOncePane;
  View                 mBackgroundAlwaysPane;
  ImageView            mGaussianOnceImage;
  ImageView            mGaussianAlwaysImage;
  ImageView            mBackgroundOnceImage;
  ImageView            mBackgroundAlwaysImage;
  GaussianBlurEffect   mGaussianOnceEffect;
  GaussianBlurEffect   mGaussianAlwaysEffect;
  BackgroundBlurEffect mBackgroundOnceEffect;
  BackgroundBlurEffect mBackgroundAlwaysEffect;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  BlurEffectsGridController controller(application);
  application.MainLoop();
  return 0;
}
