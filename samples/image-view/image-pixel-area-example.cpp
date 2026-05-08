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
#include <dali-ui-foundation/public-api/animated-image-view.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/events/key-event.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView PixelArea sample:
 * - Displays a sub-region of the image using SetPixelArea()
 * - PixelArea is specified as normalized (x, y, width, height) in [0, 1]
 * - Buttons select preset regions: FULL, four quadrants, and a center zoom
 * - A label shows the currently active region name
 * - "GIF" button toggles between a static JPG and an animated GIF to verify
 *   that PixelArea works on both
 * - "ANIMATE" button runs a looping PixelArea animation (pan + zoom)
 * - Press Escape or Back to quit
 */
class ImagePixelAreaController : public ConnectionTracker
{
  static constexpr int AREA_COUNT = 6;

  struct AreaEntry
  {
    const char* name;
    Vector4     area; // (x, y, width, height) normalized
  };

  static const AreaEntry AREAS[AREA_COUNT];

  static const char* const JPG_URL;
  static const char* const GIF_URL;

public:
  explicit ImagePixelAreaController(Application& application)
  : mApplication(application),
    mActiveIndex(0),
    mUsingGif(false),
    mAnimating(false)
  {
    mApplication.InitSignal().Connect(this, &ImagePixelAreaController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x1A1A1A));

    window.Add(CreateContents());

    window.KeyEventSignal().Connect(this, &ImagePixelAreaController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .Children({
        CreateImageArea(),
        CreateInfoLabel(),
        CreateAreaButtonRow(),
        CreateExtraButtonRow(),
      });
  }

  View CreateImageArea()
  {
    mImageContainer = StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    mImage = ImageView::New(JPG_URL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO)
      .SetPixelArea(AREAS[mActiveIndex].area);

    mImageContainer.Add(mImage);
    return mImageContainer;
  }

  View CreateInfoLabel()
  {
    return Label::New(MakeInfoText())
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(40.0f)
      .SetFontSize(14.0f)
      .SetTextColor(UiColor(0xCCCCCC))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .As(mInfoLabel);
  }

  View CreateAreaButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetSpacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(80.0f)
                        .SetPadding(Extents(4, 4, 4, 4));

    for(int i = 0; i < AREA_COUNT; ++i)
    {
      row.Add(CreateAreaButton(i));
    }

    return row;
  }

  View CreateExtraButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL)
                        .SetSpacing(4.0f)
                        .SetRequestedWidth(MATCH_PARENT)
                        .SetRequestedHeight(80.0f)
                        .SetPadding(Extents(4, 4, 4, 4));

    // GIF toggle button
    StackLayout gifButton = StackLayout::New(StackOrientation::VERTICAL)
                              .SetRequestedWidth(WRAP_CONTENT)
                              .SetRequestedHeight(MATCH_PARENT)
                              .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                              .SetBackgroundColor(UiColor(0x2E7D32))
                              .Children({
                                Label::New("GIF")
                                  .SetRequestedWidth(MATCH_PARENT)
                                  .SetRequestedHeight(MATCH_PARENT)
                                  .SetFontSize(15.0f)
                                  .SetTextColor(UiColor(0xFFFFFF))
                                  .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                  .SetVerticalTextAlignment(Text::Alignment::CENTER),
                              });
    gifButton.EnsureInteractiveTrait().ClickedSignal().Connect(
      this, [this](View, InputEvent) { OnGifButtonClicked(); });
    mGifButton = gifButton;

    // Animate PixelArea button
    StackLayout animButton = StackLayout::New(StackOrientation::VERTICAL)
                               .SetRequestedWidth(WRAP_CONTENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                               .SetBackgroundColor(UiColor(0x7B1FA2))
                               .Children({
                                 Label::New("ANIMATE\nPIXEL\nAREA")
                                   .SetRequestedWidth(MATCH_PARENT)
                                   .SetRequestedHeight(MATCH_PARENT)
                                   .SetFontSize(13.0f)
                                   .SetMultiLine(true)
                                   .SetTextColor(UiColor(0xFFFFFF))
                                   .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                                   .SetVerticalTextAlignment(Text::Alignment::CENTER),
                               });
    animButton.EnsureInteractiveTrait().ClickedSignal().Connect(
      this, [this](View, InputEvent) { OnAnimateButtonClicked(); });
    mAnimateButton = animButton;

    row.Add(gifButton);
    row.Add(animButton);

    return row;
  }

  View CreateAreaButton(int index)
  {
    StackLayout button = StackLayout::New(StackOrientation::VERTICAL)
                           .SetRequestedWidth(WRAP_CONTENT)
                           .SetRequestedHeight(MATCH_PARENT)
                           .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
                           .SetBackgroundColor(index == mActiveIndex ? UiColor(0x4A90E2) : UiColor(0x333333))
                           .Children({
                             Label::New(AREAS[index].name)
                               .SetRequestedWidth(MATCH_PARENT)
                               .SetRequestedHeight(MATCH_PARENT)
                               .SetFontSize(13.0f)
                               .SetMultiLine(true)
                               .SetTextColor(UiColor(0xFFFFFF))
                               .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                               .SetVerticalTextAlignment(Text::Alignment::CENTER),
                           });

    button.EnsureInteractiveTrait().ClickedSignal().Connect(this, &ImagePixelAreaController::OnAreaButtonClicked);

    mButtons[index] = button;
    return button;
  }

  void OnAreaButtonClicked(View clickedView, InputEvent /*event*/)
  {
    for(int i = 0; i < AREA_COUNT; ++i)
    {
      if(mButtons[i] == clickedView)
      {
        SelectArea(i);
        return;
      }
    }
  }

  void OnGifButtonClicked()
  {
    StopPixelAreaAnimation();
    mUsingGif = !mUsingGif;

    mImageContainer.Remove(mImage);

    if(mUsingGif)
    {
      mImage = AnimatedImageView::New(GIF_URL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO)
        .SetPixelArea(AREAS[mActiveIndex].area);
      AnimatedImageView::DownCast(mImage).Play();
    }
    else
    {
      mImage = ImageView::New(JPG_URL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetFittingMode(Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO)
        .SetPixelArea(AREAS[mActiveIndex].area);
    }

    mImageContainer.Add(mImage);
    mGifButton.SetBackgroundColor(mUsingGif ? UiColor(0xF57F17) : UiColor(0x2E7D32));

    DALI_LOG_RELEASE_INFO("[PixelArea] Switched to %s\n", mUsingGif ? "GIF" : "JPG");
    UpdateInfoLabel();
  }

  void OnAnimateButtonClicked()
  {
    if(mAnimating)
    {
      StopPixelAreaAnimation();
    }
    else
    {
      StartPixelAreaAnimation();
    }
  }

  void StartPixelAreaAnimation()
  {
    mAnimating = true;
    mAnimateButton.SetBackgroundColor(UiColor(0xE91E63));

    // Animate pixelArea with key frames:
    //   t=0.0  : full image
    //   t=0.25 : zoom into top-left quadrant
    //   t=0.5  : zoom into center
    //   t=0.75 : zoom into bottom-right quadrant
    //   t=1.0  : back to full (loops)
    KeyFrames keyFrames = KeyFrames::New();
    keyFrames.Add(0.00f, Property::Value(Vector4(0.0f,  0.0f,  1.0f,  1.0f)));
    keyFrames.Add(0.25f, Property::Value(Vector4(0.0f,  0.0f,  0.5f,  0.5f)));
    keyFrames.Add(0.50f, Property::Value(Vector4(0.25f, 0.25f, 0.5f,  0.5f)));
    keyFrames.Add(0.75f, Property::Value(Vector4(0.5f,  0.5f,  0.5f,  0.5f)));
    keyFrames.Add(1.00f, Property::Value(Vector4(0.0f,  0.0f,  1.0f,  1.0f)));

    mPixelAreaAnimation = Animation::New(3.0f);
    mPixelAreaAnimation.SetLooping(true);

    // PIXEL_AREA is an animatable property on both ImageView and AnimatedImageView
    int pixelAreaIndex = mUsingGif
      ? static_cast<int>(AnimatedImageView::Property::PIXEL_AREA)
      : static_cast<int>(ImageView::Property::PIXEL_AREA);

    mPixelAreaAnimation.AnimateBetween(
      Property(mImage, pixelAreaIndex),
      keyFrames,
      AlphaFunction::EASE_IN_OUT);

    mPixelAreaAnimation.Play();

    DALI_LOG_RELEASE_INFO("[PixelArea] Animation started\n");
    UpdateInfoLabel();
  }

  void StopPixelAreaAnimation()
  {
    if(mPixelAreaAnimation)
    {
      mPixelAreaAnimation.Stop();
      mPixelAreaAnimation.Reset();
    }
    mAnimating = false;
    mAnimateButton.SetBackgroundColor(UiColor(0x7B1FA2));

    // Restore static pixel area
    if(mUsingGif)
    {
      AnimatedImageView::DownCast(mImage).SetPixelArea(AREAS[mActiveIndex].area);
    }
    else
    {
      ImageView::DownCast(mImage).SetPixelArea(AREAS[mActiveIndex].area);
    }

    DALI_LOG_RELEASE_INFO("[PixelArea] Animation stopped\n");
    UpdateInfoLabel();
  }

  void SelectArea(int index)
  {
    // Selecting a preset region stops any running animation
    if(mAnimating)
    {
      StopPixelAreaAnimation();
    }

    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x333333));
    mActiveIndex = index;
    mButtons[mActiveIndex].SetBackgroundColor(UiColor(0x4A90E2));

    const Vector4& area = AREAS[mActiveIndex].area;

    // SetPixelArea requires DownCast since mImage is View type
    if(mUsingGif)
    {
      AnimatedImageView::DownCast(mImage).SetPixelArea(area);
    }
    else
    {
      ImageView::DownCast(mImage).SetPixelArea(area);
    }

    DALI_LOG_RELEASE_INFO("[PixelArea] %s -> (x=%.2f y=%.2f w=%.2f h=%.2f)\n",
                          AREAS[mActiveIndex].name,
                          area.x, area.y, area.z, area.w);
    UpdateInfoLabel();
  }

  void UpdateInfoLabel()
  {
    mInfoLabel.SetText(MakeInfoText());
  }

  Dali::String MakeInfoText() const
  {
    const char* source = mUsingGif ? "GIF" : "JPG";
    if(mAnimating)
    {
      char buf[128];
      snprintf(buf, sizeof(buf), "[%s] PixelArea: animating...", source);
      return Dali::String(buf);
    }
    const Vector4& a = AREAS[mActiveIndex].area;
    char buf[128];
    snprintf(buf, sizeof(buf),
             "[%s] PixelArea: x=%.2f  y=%.2f  w=%.2f  h=%.2f",
             source, a.x, a.y, a.z, a.w);
    return Dali::String(buf);
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
  View          mImage;
  View          mImageContainer;
  Label         mInfoLabel;
  View          mButtons[AREA_COUNT];
  View          mGifButton;
  View          mAnimateButton;
  Animation     mPixelAreaAnimation;
  int           mActiveIndex;
  bool          mUsingGif;
  bool          mAnimating;
};

const char* const ImagePixelAreaController::JPG_URL = RESOURCES_DIR "gallery-large-3.jpg";
const char* const ImagePixelAreaController::GIF_URL = RESOURCES_DIR "dali-logo-anim.gif";

const ImagePixelAreaController::AreaEntry ImagePixelAreaController::AREAS[ImagePixelAreaController::AREA_COUNT] = {
  {"FULL",         Vector4(0.0f,  0.0f,  1.0f, 1.0f)},
  {"TOP-L",        Vector4(0.0f,  0.0f,  0.5f, 0.5f)},
  {"TOP-R",        Vector4(0.5f,  0.0f,  0.5f, 0.5f)},
  {"BOT-L",        Vector4(0.0f,  0.5f,  0.5f, 0.5f)},
  {"BOT-R",        Vector4(0.5f,  0.5f,  0.5f, 0.5f)},
  {"CENTER\nZOOM", Vector4(0.25f, 0.25f, 0.5f, 0.5f)},
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImagePixelAreaController controller(application);
  application.MainLoop();
  return 0;
}
