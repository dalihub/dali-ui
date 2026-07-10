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
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const IMAGES[] = {
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "landscape-sample.jpg",
  RESOURCES_DIR "sample.jpg",
};
constexpr int IMAGE_COUNT = 3;

const char* const ANIMATED_IMAGES[] = {
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "animatedLoading.gif",
  RESOURCES_DIR "dog-anim.webp",
};
constexpr int ANIMATED_IMAGE_COUNT = 3;

const char* const LOTTIE_IMAGES[] = {
  RESOURCES_DIR "jolly_walker.json",
  RESOURCES_DIR "insta_camera.json",
  RESOURCES_DIR "confetti.json",
};
constexpr int LOTTIE_IMAGE_COUNT = 3;

struct RadiusCase
{
  const char*        label;
  Vector4            radius;
  CornerRadiusPolicy policy;
};

const RadiusCase RADIUS_CASES[] = {
  {"Normal", Vector4::ZERO, CornerRadiusPolicy::ABSOLUTE},
  {"Absolute 40", Vector4(40.0f, 40.0f, 40.0f, 40.0f), CornerRadiusPolicy::ABSOLUTE},
  {"Relative 0.25", Vector4(0.25f, 0.25f, 0.25f, 0.25f), CornerRadiusPolicy::RELATIVE},
  {"Mixed corners", Vector4(52.0f, 8.0f, 52.0f, 8.0f), CornerRadiusPolicy::ABSOLUTE},
};
constexpr int RADIUS_CASE_COUNT = sizeof(RADIUS_CASES) / sizeof(RADIUS_CASES[0]);
} // namespace

class ImageCornerRadiusController : public ConnectionTracker
{
public:
  explicit ImageCornerRadiusController(Application& application)
  : mApplication(application),
    mResourceIndex(0),
    mViewType(ViewType::IMAGE)
  {
    mApplication.InitSignal().Connect(this, &ImageCornerRadiusController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x202124));

    window.Add(CreateContents());
    window.KeyEventSignal().Connect(this, &ImageCornerRadiusController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Extents(10, 10, 10, 10));
    contents.SetSpacing(8.0f);
    contents.Add(CreateTypeButton());
    contents.Add(CreateImageGrid());
    contents.Add(CreateSwapButton());
    return contents;
  }

  View CreateImageGrid()
  {
    mGridContainer = StackLayout::New(StackOrientation::HORIZONTAL);
    mGridContainer.SetRequestedWidth(MATCH_PARENT);
    mGridContainer.SetRequestedHeight(WRAP_CONTENT);
    mGridContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mGridContainer.SetSpacing(8.0f);

    PopulateGrid();
    return mGridContainer;
  }

  void PopulateGrid()
  {
    mGridContainer.RemoveAllChildren();
    for(int i = 0; i < RADIUS_CASE_COUNT; ++i)
    {
      mGridContainer.Add(CreateRadiusCard(i));
    }
  }

  View CreateRadiusCard(int radiusIndex)
  {
    StackLayout card = StackLayout::New(StackOrientation::VERTICAL);
    card.SetRequestedWidth(WRAP_CONTENT);
    card.SetRequestedHeight(MATCH_PARENT);
    card.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    card.SetBackgroundColor(UiColor(0x303134));
    card.SetPadding(Extents(8, 8, 8, 8));
    card.SetSpacing(6.0f);

    Label title = Label::New(RADIUS_CASES[radiusIndex].label);
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(28.0f);
    title.SetFontSize(11.0f);
    title.SetTextColor(UiColor(0xFFFFFF));
    title.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    title.SetVerticalTextAlignment(Text::Alignment::CENTER);
    card.Add(title);

    card.Add(CreateVisualView(radiusIndex));
    return card;
  }

  View CreateVisualView(int radiusIndex)
  {
    const RadiusCase& radiusCase = RADIUS_CASES[radiusIndex];

    switch(mViewType)
    {
      case ViewType::IMAGE:
      {
        mImages[radiusIndex] = ImageView::New(IMAGES[mResourceIndex % IMAGE_COUNT]);
        mImages[radiusIndex].SetRequestedWidth(MATCH_PARENT);
        mImages[radiusIndex].SetRequestedHeight(WRAP_CONTENT);
        mImages[radiusIndex].SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
        mImages[radiusIndex].SetFittingMode(Ui::Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO);
        mImages[radiusIndex].SetCornerRadius(radiusCase.radius);
        mImages[radiusIndex].SetCornerRadiusPolicy(radiusCase.policy);
        return mImages[radiusIndex];
      }
      case ViewType::ANIMATED_IMAGE:
      {
        mAnimatedImages[radiusIndex] = AnimatedImageView::New(ANIMATED_IMAGES[mResourceIndex % ANIMATED_IMAGE_COUNT]);
        mAnimatedImages[radiusIndex].SetRequestedWidth(MATCH_PARENT);
        mAnimatedImages[radiusIndex].SetRequestedHeight(WRAP_CONTENT);
        mAnimatedImages[radiusIndex].SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
        mAnimatedImages[radiusIndex].SetFittingMode(Ui::Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO);
        mAnimatedImages[radiusIndex].SetLoopCount(-1);
        mAnimatedImages[radiusIndex].SetCornerRadius(radiusCase.radius);
        mAnimatedImages[radiusIndex].SetCornerRadiusPolicy(radiusCase.policy);
        mAnimatedImages[radiusIndex].Play();
        return mAnimatedImages[radiusIndex];
      }
      case ViewType::LOTTIE_ANIMATION:
      {
        mLottieAnimations[radiusIndex] = LottieAnimationView::New(LOTTIE_IMAGES[mResourceIndex % LOTTIE_IMAGE_COUNT]);
        mLottieAnimations[radiusIndex].SetRequestedWidth(MATCH_PARENT);
        mLottieAnimations[radiusIndex].SetRequestedHeight(WRAP_CONTENT);
        mLottieAnimations[radiusIndex].SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
        mLottieAnimations[radiusIndex].SetLoopCount(-1);
        mLottieAnimations[radiusIndex].SetCornerRadius(radiusCase.radius);
        mLottieAnimations[radiusIndex].SetCornerRadiusPolicy(radiusCase.policy);
        mLottieAnimations[radiusIndex].Play();
        return mLottieAnimations[radiusIndex];
      }
    }

    return View();
  }

  View CreateTypeButton()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(48.0f);
    button.SetBackgroundColor(UiColor(0x5F6368));
    mTypeLabel = Label::New(GetViewTypeLabel());
    mTypeLabel.SetRequestedWidth(MATCH_PARENT);
    mTypeLabel.SetRequestedHeight(MATCH_PARENT);
    mTypeLabel.SetFontSize(15.0f);
    mTypeLabel.SetTextColor(UiColor(0xFFFFFF));
    mTypeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mTypeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(mTypeLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageCornerRadiusController::OnTypeButtonClicked);
    return button;
  }

  View CreateSwapButton()
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(64.0f);
    button.SetBackgroundColor(UiColor(0x1E88E5));
    Label buttonLabel = Label::New("SWAP RESOURCE");
    buttonLabel.SetRequestedWidth(MATCH_PARENT);
    buttonLabel.SetRequestedHeight(MATCH_PARENT);
    buttonLabel.SetFontSize(16.0f);
    buttonLabel.SetTextColor(UiColor(0xFFFFFF));
    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(buttonLabel);

    button.AsInteractive().ClickedSignal().Connect(this, &ImageCornerRadiusController::OnSwapButtonClicked);
    return button;
  }

  const char* GetViewTypeLabel() const
  {
    switch(mViewType)
    {
      case ViewType::IMAGE:
      {
        return "TYPE: ImageView";
      }
      case ViewType::ANIMATED_IMAGE:
      {
        return "TYPE: AnimatedImageView";
      }
      case ViewType::LOTTIE_ANIMATION:
      {
        return "TYPE: LottieAnimationView";
      }
    }
    return "";
  }

  void OnTypeButtonClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    mViewType = (mViewType + 1) % ViewType::COUNT;
    mTypeLabel.SetText(GetViewTypeLabel());
    PopulateGrid();
  }

  void OnSwapButtonClicked(View /*clickedView*/, InputEvent /*event*/)
  {
    ++mResourceIndex;
    for(int i = 0; i < RADIUS_CASE_COUNT; ++i)
    {
      switch(mViewType)
      {
        case ViewType::IMAGE:
        {
          mImages[i].SetResourceUrl(IMAGES[mResourceIndex % IMAGE_COUNT]);
          break;
        }
        case ViewType::ANIMATED_IMAGE:
        {
          mAnimatedImages[i].SetResourceUrl(ANIMATED_IMAGES[mResourceIndex % ANIMATED_IMAGE_COUNT]);
          mAnimatedImages[i].Play();
          break;
        }
        case ViewType::LOTTIE_ANIMATION:
        {
          mLottieAnimations[i].SetResourceUrl(LOTTIE_IMAGES[mResourceIndex % LOTTIE_IMAGE_COUNT]);
          mLottieAnimations[i].Play();
          break;
        }
      }
    }
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  struct ViewType
  {
    enum Type
    {
      IMAGE,
      ANIMATED_IMAGE,
      LOTTIE_ANIMATION,
      COUNT,
    };
  };

  Application& mApplication;
  int          mResourceIndex;
  int          mViewType;
  StackLayout  mGridContainer;
  Label        mTypeLabel;
  ImageView    mImages[RADIUS_CASE_COUNT];
  AnimatedImageView mAnimatedImages[RADIUS_CASE_COUNT];
  LottieAnimationView mLottieAnimations[RADIUS_CASE_COUNT];
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageCornerRadiusController sample(application);
  application.MainLoop();
  return 0;
}
