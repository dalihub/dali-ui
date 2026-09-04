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
constexpr uint32_t BLUR_RADIUS               = 40u;
constexpr float    CONFIGURED_DOWNSCALE      = 0.25f;
constexpr float    ANIMATION_DURATION_SECONDS = 2.5f;
const char* const  IMAGE_URL                 = RESOURCES_DIR "gallery-medium-49.jpg";

constexpr float WINDOW_WIDTH  = 1280.0f;
constexpr float WINDOW_HEIGHT = 720.0f;
constexpr float CARD_TOP      = 132.0f;
constexpr float CARD_WIDTH    = 580.0f;
constexpr float CARD_HEIGHT   = 430.0f;
constexpr float LEFT_X        = 40.0f;
constexpr float RIGHT_X       = 660.0f;

View NewView(float x, float y, float width, float height, const Vector4& color = Color::TRANSPARENT)
{
  View view = View::New();
  view.SetLayoutMode(LayoutMode::STANDALONE);
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetBackgroundColor(color);
  return view;
}

ImageView NewImage(float x, float y, float width, float height)
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

Label NewLabel(const char* text, float x, float y, float width, float height, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedX(x);
  label.SetRequestedY(y);
  label.SetRequestedWidth(width);
  label.SetRequestedHeight(height);
  label.SetParentOrigin(ParentOrigin::TOP_LEFT);
  label.SetPivot(Pivot::TOP_LEFT);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // unnamed namespace

class BlurStrengthAnimationController : public ConnectionTracker
{
public:
  explicit BlurStrengthAnimationController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &BlurStrengthAnimationController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetPositionSize(PositionSize(80, 60, static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT)));
    mWindow.SetBackgroundColor(UiColor(0x08111F));

    mRoot = NewView(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, UiColor(0x08111F));
    mWindow.Add(mRoot);

    Label title = NewLabel("Blur Strength Animation", 40.0f, 24.0f, 1200.0f, 48.0f, 32.0f, UiColor(0xF8FAFC));
    Label subtitle = NewLabel("Strength 0 ↔ 1  |  requested downscale 0.25  |  animation buffers 1.0", 40.0f, 72.0f, 1200.0f, 34.0f, 17.0f, UiColor(0x94A3B8));
    mRoot.Add(title);
    mRoot.Add(subtitle);

    AddCardBackground(LEFT_X);
    AddCardBackground(RIGHT_X);

    mGaussianImage = NewImage(LEFT_X + 12.0f, CARD_TOP + 12.0f, CARD_WIDTH - 24.0f, CARD_HEIGHT - 24.0f);
    mBackgroundImage = NewImage(RIGHT_X + 12.0f, CARD_TOP + 12.0f, CARD_WIDTH - 24.0f, CARD_HEIGHT - 24.0f);
    mGaussianImage.ResourceReadySignal().Connect(this, &BlurStrengthAnimationController::OnGaussianImageReady);
    mBackgroundImage.ResourceReadySignal().Connect(this, &BlurStrengthAnimationController::OnBackgroundImageReady);
    mRoot.Add(mGaussianImage);
    mRoot.Add(mBackgroundImage);

    // Everything below this stopper is captured by BackgroundBlurEffect.
    mStopper = View::New();
    mRoot.Add(mStopper);

    mBackgroundPane = NewView(RIGHT_X + 12.0f, CARD_TOP + 12.0f, CARD_WIDTH - 24.0f, CARD_HEIGHT - 24.0f);
    mRoot.Add(mBackgroundPane);

    AddCardLabel("GaussianBlurEffect", LEFT_X);
    AddCardLabel("BackgroundBlurEffect", RIGHT_X);

    View status = NewView(40.0f, 590.0f, 1200.0f, 92.0f, UiColor(0x111C2E));
    status.SetCornerRadius(16.0f);
    mRoot.Add(status);

    mDirectionLabel = NewLabel("Preparing effects...", 68.0f, 601.0f, 1144.0f, 36.0f, 18.0f, UiColor(0xE2E8F0));
    Label detail = NewLabel("At Strength 0, blur rendering is bypassed. A new animation automatically reactivates it at full resolution.",
                            68.0f, 637.0f, 1144.0f, 30.0f, 14.0f, UiColor(0x7DD3FC));
    mRoot.Add(mDirectionLabel);
    mRoot.Add(detail);

    mGaussianImage.SetResourceUrl(IMAGE_URL);
    mBackgroundImage.SetResourceUrl(IMAGE_URL);
  }

  void AddCardBackground(float x)
  {
    View card = NewView(x, CARD_TOP, CARD_WIDTH, CARD_HEIGHT, UiColor(0x182338));
    card.SetCornerRadius(18.0f);
    mRoot.Add(card);
  }

  void AddCardLabel(const char* text, float x)
  {
    Label label = NewLabel(text, x + 12.0f, CARD_TOP + 12.0f, CARD_WIDTH - 24.0f, 52.0f, 21.0f, UiColor(0xFFFFFF));
    label.SetBackgroundColor(Vector4(0.02f, 0.04f, 0.08f, 0.76f));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mRoot.Add(label);
  }

  void OnGaussianImageReady(View)
  {
    if(mGaussianReady)
    {
      return;
    }

    mGaussianReady  = true;
    mGaussianEffect = GaussianBlurEffect::New(BLUR_RADIUS);
    mGaussianEffect.SetBlurDownscaleFactor(CONFIGURED_DOWNSCALE);
    mGaussianImage.SetRenderEffect(mGaussianEffect);
    mGaussianEffect.Activate();
    StartWhenReady();
  }

  void OnBackgroundImageReady(View)
  {
    if(mBackgroundReady)
    {
      return;
    }

    mBackgroundReady  = true;
    mBackgroundEffect = BackgroundBlurEffect::New(BLUR_RADIUS);
    mBackgroundEffect.SetBlurDownscaleFactor(CONFIGURED_DOWNSCALE);
    mBackgroundEffect.SetSourceView(mRoot);
    mBackgroundEffect.SetStopperView(mStopper);
    mBackgroundPane.SetRenderEffect(mBackgroundEffect);
    mBackgroundEffect.Activate();
    StartWhenReady();
  }

  void StartWhenReady()
  {
    if(mGaussianReady && mBackgroundReady && !mAnimation)
    {
      StartAnimation(0.0f, 1.0f);
    }
  }

  void StartAnimation(float fromValue, float toValue)
  {
    mAnimation = Animation::New(ANIMATION_DURATION_SECONDS);
    const AlphaFunction alpha(AlphaFunction::BuiltinFunction::EASE_IN_OUT_SINE);
    const TimePeriod    period(0.0f, ANIMATION_DURATION_SECONDS);

    mGaussianEffect.AddBlurStrengthAnimation(mAnimation, alpha, period, fromValue, toValue);
    mBackgroundEffect.AddBlurStrengthAnimation(mAnimation, alpha, period, fromValue, toValue);
    mAnimation.FinishedSignal().Connect(this, &BlurStrengthAnimationController::OnAnimationFinished);

    mAnimatingToBlur = toValue > fromValue;
    mDirectionLabel.SetText(mAnimatingToBlur
                              ? "LIVE  •  Strength 0 → 1  •  clear to blurred"
                              : "LIVE  •  Strength 1 → 0  •  blurred to clear");
    mAnimation.Play();
  }

  void OnAnimationFinished(Animation)
  {
    if(mAnimatingToBlur)
    {
      StartAnimation(1.0f, 0.0f);
    }
    else
    {
      StartAnimation(0.0f, 1.0f);
    }
  }

private:
  Application&         mApplication;
  Window               mWindow;
  View                 mRoot;
  View                 mStopper;
  View                 mBackgroundPane;
  ImageView            mGaussianImage;
  ImageView            mBackgroundImage;
  Label                mDirectionLabel;
  GaussianBlurEffect   mGaussianEffect;
  BackgroundBlurEffect mBackgroundEffect;
  Animation            mAnimation;
  bool                 mGaussianReady{false};
  bool                 mBackgroundReady{false};
  bool                 mAnimatingToBlur{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  BlurStrengthAnimationController controller(application);
  application.MainLoop();
  return 0;
}
