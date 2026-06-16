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
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/dali.h>
#include <dali/public-api/events/key-event.h>

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t BLUR_RADIUS       = 40u;
constexpr float    BLUR_DOWNSCALE   = 0.25f;
constexpr float    IMAGE_SIZE       = 200.0f;
constexpr float    IMAGE_GAP_X      = 120.0f;
constexpr float    IMAGE_GAP_Y      = 90.0f;
constexpr float    BOARD_MOVE_X     = 180.0f;
constexpr float    BOARD_MOVE_Y     = 140.0f;
constexpr float    BOARD_MOVE_TIME  = 6.0f;
constexpr float    DITHER_STEP      = 5.0f / 255.0f;
constexpr float    DITHER_SCALE     = 0.2f;
constexpr float    INITIAL_DITHER   = 0.1f;
constexpr float    HELP_MARGIN      = 24.0f;
constexpr float    HELP_HEIGHT      = 44.0f;
constexpr int      IMAGE_COLUMNS    = 6;
constexpr int      IMAGE_ROWS       = 4;

const std::array<Vector4, 4> BACKGROUND_COLORS =
  {{
    Vector4(0.05f, 0.07f, 0.09f, 1.0f),
    Vector4(0.08f, 0.10f, 0.14f, 1.0f),
    Vector4(0.04f, 0.06f, 0.10f, 1.0f),
    Vector4(0.10f, 0.08f, 0.06f, 1.0f),
  }};

const char* const IMAGE_PATHS[] = {
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
};

constexpr unsigned int NUMBER_OF_IMAGES = sizeof(IMAGE_PATHS) / sizeof(IMAGE_PATHS[0]);
} // namespace

class BackgroundBlurDitherController : public ConnectionTracker
{
public:
  explicit BackgroundBlurDitherController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &BackgroundBlurDitherController::Create);
  }

  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(BACKGROUND_COLORS[0]);

    mRoot = View::New();
    mRoot.SetBackgroundColor(Color::TRANSPARENT);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    Window::WindowSize windowSize = mWindow.GetSize();
    mRoot.SetProperty(Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight()));
    mRoot.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mRoot.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mWindow.Add(mRoot);

    CreateMovingImageBoard();
    CreateFullscreenBlurPane();
    CreateStatusLabel();
    CreateHelpLabel();

    mWindow.KeyEventSignal().Connect(this, &BackgroundBlurDitherController::OnKeyEvent);
    mWindow.ResizeSignal().Connect(this, &BackgroundBlurDitherController::OnWindowResized);
  }

private:
  void CreateMovingImageBoard()
  {
    Window::WindowSize windowSize = mWindow.GetSize();

    mImageBoard = View::New();
    mImageBoard.SetLayoutMode(LayoutMode::STANDALONE);
    mImageBoard.SetRequestedWidth(MATCH_PARENT);
    mImageBoard.SetRequestedHeight(MATCH_PARENT);
    mImageBoard.SetRequestedPositionX(0.0f);
    mImageBoard.SetRequestedPositionY(0.0f);
    mImageBoard.SetBackgroundColor(Color::TRANSPARENT);
    mImageBoard.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mImageBoard.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mImageBoard.SetProperty(Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight()));
    mRoot.Add(mImageBoard);

    const float strideX     = IMAGE_SIZE + IMAGE_GAP_X;
    const float strideY     = IMAGE_SIZE + IMAGE_GAP_Y;
    const float boardWidth  = IMAGE_SIZE + strideX * static_cast<float>(IMAGE_COLUMNS - 1);
    const float boardHeight = IMAGE_SIZE + strideY * static_cast<float>(IMAGE_ROWS - 1);
    const float startX      = (static_cast<float>(windowSize.GetWidth()) - boardWidth) * 0.5f;
    const float startY      = (static_cast<float>(windowSize.GetHeight()) - boardHeight) * 0.5f;

    unsigned int imageIndex = 0u;
    for(int row = 0; row < IMAGE_ROWS; ++row)
    {
      for(int column = 0; column < IMAGE_COLUMNS; ++column)
      {
        ImageView image = ImageView::New(IMAGE_PATHS[imageIndex]);
        image.SetLayoutMode(LayoutMode::STANDALONE);
        image.SetRequestedWidth(IMAGE_SIZE);
        image.SetRequestedHeight(IMAGE_SIZE);
        image.SetRequestedPositionX(startX + strideX * column);
        image.SetRequestedPositionY(startY + strideY * row);
        image.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
        image.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
        mImageBoard.Add(image);

        imageIndex = (imageIndex + 1u) % NUMBER_OF_IMAGES;
      }
    }
  }

  void CreateFullscreenBlurPane()
  {
    Vector2 windowSize = mWindow.GetSize();

    mBlurPane = View::New();
    mBlurPane.SetLayoutMode(LayoutMode::STANDALONE);
    mBlurPane.SetRequestedWidth(MATCH_PARENT);
    mBlurPane.SetRequestedHeight(MATCH_PARENT);
    mBlurPane.SetRequestedPositionX(0.0f);
    mBlurPane.SetRequestedPositionY(0.0f);
    mBlurPane.SetBackgroundColor(Color::TRANSPARENT);
    mBlurPane.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mBlurPane.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mBlurPane.SetProperty(Actor::Property::SIZE, windowSize);
    mBlurPane.SetProperty(Actor::Property::WIDTH_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mBlurPane.SetProperty(Actor::Property::HEIGHT_RESIZE_POLICY, ResizePolicy::FILL_TO_PARENT);
    mRoot.Add(mBlurPane);

    mBackgroundBlur = BackgroundBlurEffect::New(BLUR_RADIUS);
    mBackgroundBlur.SetBlurDownscaleFactor(BLUR_DOWNSCALE);
    mBackgroundBlur.SetDitherNoiseStrength(mDitherNoiseStrength);
    mBackgroundBlur.SetBlurOnce(false);
    mBlurPane.SetRenderEffect(mBackgroundBlur);
  }

  void CreateStatusLabel()
  {
    mStatusLabel = Label::New();
    mStatusLabel.SetLayoutMode(LayoutMode::STANDALONE);
    mStatusLabel.SetRequestedWidth(260.0f);
    mStatusLabel.SetRequestedHeight(40.0f);
    mStatusLabel.SetRequestedPositionX(static_cast<float>(mWindow.GetSize().GetWidth()) - 284.0f);
    mStatusLabel.SetRequestedPositionY(24.0f);
    mStatusLabel.SetFontSize(14.0f);
    mStatusLabel.SetTextColor(UiColor(0xFFFFFF));
    mStatusLabel.SetHorizontalTextAlignment(Text::Alignment::END);
    mStatusLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mStatusLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mStatusLabel.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mRoot.Add(mStatusLabel);

    UpdateStatusLabel();
  }

  void CreateHelpLabel()
  {
    Window::WindowSize windowSize = mWindow.GetSize();

    mHelpLabel = Label::New("1 Blur  2 Speed  3/4 Radius -/+  5 BG  6 Animation  7/8 Noise -/+  Esc/Back Quit");
    mHelpLabel.SetLayoutMode(LayoutMode::STANDALONE);
    mHelpLabel.SetRequestedWidth(static_cast<float>(windowSize.GetWidth()) - HELP_MARGIN * 2.0f);
    mHelpLabel.SetRequestedHeight(HELP_HEIGHT);
    mHelpLabel.SetRequestedPositionX(HELP_MARGIN);
    mHelpLabel.SetRequestedPositionY(static_cast<float>(windowSize.GetHeight()) - HELP_HEIGHT - HELP_MARGIN);
    mHelpLabel.SetFontSize(15.0f);
    mHelpLabel.SetTextColor(UiColor(0xFFE66D));
    mHelpLabel.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.68f));
    mHelpLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mHelpLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mHelpLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mHelpLabel.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mRoot.Add(mHelpLabel);
  }

  void UpdateStatusLabel()
  {
    std::ostringstream oss;
    oss << "Noise " << static_cast<int>(mDitherNoiseStrength * 255.0f + 0.5f)
        << "/255  " << std::fixed << std::setprecision(3) << mDitherNoiseStrength
        << " -> " << std::setprecision(4) << mDitherNoiseStrength * DITHER_SCALE;
    mStatusLabel.SetText(oss.str().c_str());
  }

  void StartBoardAnimation()
  {
    if(mBoardAnimation)
    {
      mBoardAnimation.Stop();
      mBoardAnimation.Clear();
    }

    KeyFrames positions = KeyFrames::New();
    positions.Add(0.00f, Vector3(-BOARD_MOVE_X, -BOARD_MOVE_Y, 0.0f));
    positions.Add(0.25f, Vector3(BOARD_MOVE_X, -BOARD_MOVE_Y, 0.0f));
    positions.Add(0.50f, Vector3(BOARD_MOVE_X, BOARD_MOVE_Y, 0.0f));
    positions.Add(0.75f, Vector3(-BOARD_MOVE_X, BOARD_MOVE_Y, 0.0f));
    positions.Add(1.00f, Vector3(-BOARD_MOVE_X, -BOARD_MOVE_Y, 0.0f));

    mBoardAnimation = Animation::New(BOARD_MOVE_TIME / mSpeedMultiplier);
    mBoardAnimation.AnimateBetween(Property(mImageBoard, Actor::Property::POSITION), positions, AlphaFunction::LINEAR);
    mBoardAnimation.SetLooping(true);
    mBoardAnimation.Play();
  }

  void OnWindowResized(Window windowHandle, Window::WindowSize windowSize)
  {
    mRoot.SetProperty(Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight()));
    mImageBoard.SetProperty(Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight()));
    mBlurPane.SetProperty(Actor::Property::SIZE, Vector2(windowSize.GetWidth(), windowSize.GetHeight()));
    mStatusLabel.SetRequestedPositionX(static_cast<float>(windowSize.GetWidth()) - 284.0f);
    mHelpLabel.SetRequestedWidth(static_cast<float>(windowSize.GetWidth()) - HELP_MARGIN * 2.0f);
    mHelpLabel.SetRequestedPositionY(static_cast<float>(windowSize.GetHeight()) - HELP_HEIGHT - HELP_MARGIN);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "1")
    {
      mBlurVisible = !mBlurVisible;
      mBlurPane.SetProperty(Actor::Property::VISIBLE, mBlurVisible);
    }
    else if(event.GetKeyName() == "2")
    {
      mSpeedMultiplier = mSpeedMultiplier < 4.0f ? mSpeedMultiplier * 2.0f : 0.5f;
      if(mAnimationEnabled)
      {
        StartBoardAnimation();
      }
    }
    else if(event.GetKeyName() == "3")
    {
      if(mCurrentBlurRadius > 10u)
      {
        mCurrentBlurRadius -= 10u;
        mBackgroundBlur.SetBlurRadius(mCurrentBlurRadius);
      }
    }
    else if(event.GetKeyName() == "4")
    {
      mCurrentBlurRadius += 10u;
      mBackgroundBlur.SetBlurRadius(mCurrentBlurRadius);
    }
    else if(event.GetKeyName() == "5")
    {
      mBackgroundColorIndex = (mBackgroundColorIndex + 1u) % BACKGROUND_COLORS.size();
      mWindow.SetBackgroundColor(BACKGROUND_COLORS[mBackgroundColorIndex]);
    }
    else if(event.GetKeyName() == "6")
    {
      mAnimationEnabled = !mAnimationEnabled;
      if(mAnimationEnabled)
      {
        StartBoardAnimation();
      }
      else if(mBoardAnimation)
      {
        mBoardAnimation.Stop();
      }
    }
    else if(event.GetKeyName() == "7")
    {
      mDitherNoiseStrength = std::max(0.0f, mDitherNoiseStrength - DITHER_STEP);
      mBackgroundBlur.SetDitherNoiseStrength(mDitherNoiseStrength);
      UpdateStatusLabel();
    }
    else if(event.GetKeyName() == "8")
    {
      mDitherNoiseStrength = std::min(1.0f, mDitherNoiseStrength + DITHER_STEP);
      mBackgroundBlur.SetDitherNoiseStrength(mDitherNoiseStrength);
      UpdateStatusLabel();
    }
  }

private:
  Application& mApplication;

  Window mWindow;
  View   mRoot;
  View   mImageBoard;

  View                 mBlurPane;
  Label                mStatusLabel;
  Label                mHelpLabel;
  BackgroundBlurEffect mBackgroundBlur;
  Animation            mBoardAnimation;

  uint32_t mCurrentBlurRadius{BLUR_RADIUS};
  uint32_t mBackgroundColorIndex{0u};
  float    mSpeedMultiplier{1.0f};
  float    mDitherNoiseStrength{INITIAL_DITHER};
  bool     mBlurVisible{true};
  bool     mAnimationEnabled{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  BackgroundBlurDitherController controller(application);
  application.MainLoop();
  return 0;
}
