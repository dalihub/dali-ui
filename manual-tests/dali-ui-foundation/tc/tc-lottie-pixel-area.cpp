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

#include "manual-test-case.h"

#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/key-frames.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* const LOTTIE_WALKER = TEST_RESOURCE_DIR "/jolly_walker.json";

constexpr float    PREVIEW_W     = 240.0f;
constexpr float    PREVIEW_H     = 240.0f;
constexpr float    BTN_H         = 48.0f;
constexpr float    STATUS_H      = 28.0f;
constexpr uint32_t C_BTN_BG      = 0x555555;
constexpr uint32_t C_BTN_ACTIVE  = 0x1565C0;
constexpr uint32_t C_BTN_ANIM    = 0x7B1FA2;
constexpr uint32_t C_BTN_TEXT    = 0xEEEEEE;
constexpr uint32_t C_STATUS_BG   = 0x222222;
constexpr uint32_t C_STATUS_TEXT = 0xCCCCCC;
constexpr uint32_t C_BG          = 0x1A1A1A;
constexpr uint32_t C_FRAME_BG    = 0x2A2A2A;

struct AreaEntry
{
  const char* name;
  Vector4     area;
};

const AreaEntry AREAS[] = {
  {"FULL",   Vector4(0.0f,  0.0f,  1.0f,  1.0f)},
  {"TOP-L",  Vector4(0.0f,  0.0f,  0.5f,  0.5f)},
  {"TOP-R",  Vector4(0.5f,  0.0f,  0.5f,  0.5f)},
  {"BOT-L",  Vector4(0.0f,  0.5f,  0.5f,  0.5f)},
  {"BOT-R",  Vector4(0.5f,  0.5f,  0.5f,  0.5f)},
  {"CENTER", Vector4(0.25f, 0.25f, 0.5f,  0.5f)},
};
const int AREA_COUNT = static_cast<int>(sizeof(AREAS) / sizeof(AREAS[0]));
} // namespace

/**
 * @brief Verifies LottieAnimationView PixelArea:
 *
 * [PixelArea verification]:
 *   - FULL / TOP-L / TOP-R / BOT-L / BOT-R / CENTER buttons set a preset sub-region
 *   - ANIMATE button runs a looping AnimateBetween across the pixel area (pan + zoom)
 *   - Status label shows the active area coordinates
 *
 * Expected result:
 *   Each preset button shows only the corresponding quadrant of the animation.
 *   ANIMATE smoothly pans and zooms across the animation content.
 *   GetPixelArea() returns the last-set value.
 */
class TcLottiePixelArea : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Lottie: PixelArea";
  }

  Dali::String GetDescription() const override
  {
    return "Verify SetPixelArea() preset regions and AnimateBetween animation";
  }

  void OnEnter(View contentArea) override
  {
    mActiveIndex = 0;
    mAnimating   = false;

    mView = LottieAnimationView::New(LOTTIE_WALKER);
    mView.SetRequestedWidth(PREVIEW_W);
    mView.SetRequestedHeight(PREVIEW_H);
    mView.SetLoopCount(-1);
    mView.Play();

    mStatusLabel = MakeStatusLabel(MakeStatusText());

    // The label reads GetPixelArea() and the PIXEL_AREA animation changes it
    // per frame, so a click-driven label would freeze mid-animation. Poll like
    // the frame-range screen does.
    mPollTimer = Timer::New(100);
    mPollTimer.TickSignal().Connect(this, &TcLottiePixelArea::OnPollTick);
    mPollTimer.Start();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetBackgroundColor(UiColor(C_BG));
    content.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));

    // Preview frame
    StackLayout frame = StackLayout::New(StackOrientation::VERTICAL);
    frame.SetRequestedWidth(MATCH_PARENT);
    frame.SetRequestedHeight(PREVIEW_H + 16);
    frame.SetBackgroundColor(UiColor(C_FRAME_BG));
    frame.SetPadding(Insets(0.0f, 0.0f, 8.0f, 8.0f));

    StackLayout centreRow = StackLayout::New(StackOrientation::HORIZONTAL);
    centreRow.SetRequestedWidth(MATCH_PARENT);
    centreRow.SetRequestedHeight(PREVIEW_H);
    centreRow.Add(ManualTest::MakeWeightedSpacer());
    centreRow.Add(mView);
    centreRow.Add(ManualTest::MakeWeightedSpacer());

    frame.Add(centreRow);
    content.Add(frame);
    content.Add(mStatusLabel);

    // Lottie playback control: stopped stills are what let the six presets be
    // compared to each other as pictures.
    content.Add(MakeButtonRow({
      MakeButton("Play", C_BTN_BG, [this] { mView.Play(); }),
      MakeButton("Stop", C_BTN_BG, [this] { mView.Stop(); }),
    }));

    // Area preset buttons
    StackLayout areaRow = StackLayout::New(StackOrientation::HORIZONTAL);
    areaRow.SetRequestedWidth(MATCH_PARENT);
    areaRow.SetRequestedHeight(BTN_H);
    areaRow.SetPadding(Insets(0.0f, 0.0f, 2.0f, 2.0f));
    for(int i = 0; i < AREA_COUNT; ++i)
    {
      areaRow.Add(MakeAreaButton(i));
    }
    content.Add(areaRow);

    // Animate button
    mAnimateButton = MakeButton("ANIMATE\nPIXEL AREA", C_BTN_ANIM, [this] { OnAnimateClicked(); });
    content.Add(MakeButtonRow({mAnimateButton}));

    contentArea.Add(content);

    UpdateAreaButtonColors();
  }

  void OnExit() override
  {
    StopAnimation();
    if(mPollTimer)
    {
      mPollTimer.Stop();
      mPollTimer.Reset();
    }
  }

private:
  bool OnPollTick()
  {
    UpdateStatusLabel();
    return true;
  }

  void OnAreaClicked(int index)
  {
    StopAnimation();
    mActiveIndex = index;
    mView.SetPixelArea(AREAS[index].area);
    UpdateAreaButtonColors();
    UpdateStatusLabel();
  }

  void OnAnimateClicked()
  {
    if(mAnimating)
    {
      StopAnimation();
    }
    else
    {
      StartAnimation();
    }
  }

  void StartAnimation()
  {
    mAnimating = true;
    mAnimateButton.SetBackgroundColor(UiColor(0xE91E63));

    KeyFrames kf = KeyFrames::New();
    // Without an explicit 0.0 keyframe the animation interpolates from a
    // zero Vector4 — measured: the first ~0.5 s showed w=0 h=0, an empty
    // pixel area the documented FULL→TOP-L→…→FULL path never contains.
    kf.Add(0.00f, Property::Value(Vector4(0.0f,  0.0f,  1.0f,  1.0f)));
    kf.Add(0.25f, Property::Value(Vector4(0.0f,  0.0f,  0.5f,  0.5f)));
    kf.Add(0.50f, Property::Value(Vector4(0.25f, 0.25f, 0.5f,  0.5f)));
    kf.Add(0.75f, Property::Value(Vector4(0.5f,  0.5f,  0.5f,  0.5f)));
    kf.Add(1.00f, Property::Value(Vector4(0.0f,  0.0f,  1.0f,  1.0f)));

    mAnimation = Animation::New(3.0f);
    mAnimation.AnimateBetween(
      Property(mView, LottieAnimationView::Property::PIXEL_AREA),
      kf,
      AlphaFunction::EASE_IN_OUT);
    // Without this the one-shot animation ended but the [anim] tag stayed
    // forever (measured on emulator), so the label claimed an animation the
    // screen had already left.
    mAnimation.FinishedSignal().Connect(this, [this](Dali::Animation) { OnAnimationFinished(); });
    mAnimation.Play();

    UpdateStatusLabel();
  }

  void OnAnimationFinished()
  {
    if(!mAnimating)
    {
      return;
    }
    mAnimating = false;
    mAnimateButton.SetBackgroundColor(UiColor(C_BTN_ANIM));
    // The keyframe path ends at FULL, but the impl-side value GetPixelArea()
    // reports is still the last preset — sync it (and the active-button
    // bookkeeping) to what is actually on screen, so the label stays honest.
    mView.SetPixelArea(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    mActiveIndex = 0;
    UpdateAreaButtonColors();
    UpdateStatusLabel();
  }

  void StopAnimation()
  {
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Reset();
    }
    if(mAnimating)
    {
      mAnimating = false;
      mAnimateButton.SetBackgroundColor(UiColor(C_BTN_ANIM));
      mView.SetPixelArea(AREAS[mActiveIndex].area);
      UpdateStatusLabel();
    }
  }

  void UpdateAreaButtonColors()
  {
    for(int i = 0; i < AREA_COUNT; ++i)
    {
      mAreaButtons[i].SetBackgroundColor(UiColor(i == mActiveIndex ? C_BTN_ACTIVE : C_BTN_BG));
    }
  }

  void UpdateStatusLabel()
  {
    mStatusLabel.SetText(MakeStatusText());
  }

  Dali::String MakeStatusText() const
  {
    // GetPixelArea()'s return value, ALWAYS — the old label formatted the
    // AREAS[] constant the tapped button indexed, which stayed correct with
    // SetPixelArea deleted, and printed only "animating..." during the one
    // stretch where the value actually moves.
    // While the keyframe animation runs, GetPixelArea() keeps reporting the
    // impl-side configured value (measured: frozen at the preset for the
    // whole 3 s), so read the live animated property instead — that is what
    // makes the FULL→TOP-L→CENTER→BOT-R→FULL path observable at all.
    Vector4 a = mAnimating
                  ? mView.GetCurrentProperty(LottieAnimationView::Property::PIXEL_AREA).Get<Vector4>()
                  : mView.GetPixelArea();
    char    buf[128];
    snprintf(buf, sizeof(buf),
             "PixelArea: x=%.2f y=%.2f w=%.2f h=%.2f  [%s]",
             a.x, a.y, a.z, a.w,
             mAnimating ? "anim" : AREAS[mActiveIndex].name);
    return Dali::String(buf);
  }

  // ── Helpers ──────────────────────────────────────────────────────────────

  Label MakeStatusLabel(const Dali::String& text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(STATUS_H);
    label.SetFontSize(11.0f);
    label.SetTextColor(UiColor(C_STATUS_TEXT));
    label.SetBackgroundColor(UiColor(C_STATUS_BG));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  View MakeAreaButton(int index)
  {
    StackLayout btn = StackLayout::New(StackOrientation::VERTICAL);
    btn.SetRequestedHeight(BTN_H);
    btn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    btn.SetBackgroundColor(UiColor(C_BTN_BG));
    Label buttonLabel = Label::New(AREAS[index].name);

    buttonLabel.SetRequestedWidth(MATCH_PARENT);

    buttonLabel.SetRequestedHeight(MATCH_PARENT);

    buttonLabel.SetFontSize(11.0f);

    buttonLabel.SetTextColor(UiColor(C_BTN_TEXT));

    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    btn.Add(buttonLabel);
    btn.SetFocusable(true);

    InteractiveTrait interactive = btn.AsInteractive();

    interactive.ClickedSignal().Connect(this, [this, index](View, const InputEvent&) -> bool {

      OnAreaClicked(index);

      return true;

    });
    mAreaButtons[index] = btn;
    return btn;
  }

  View MakeButton(const Dali::String& label, uint32_t bgColor, std::function<void()> onClick)
  {
    StackLayout btn = StackLayout::New(StackOrientation::VERTICAL);
    btn.SetRequestedHeight(BTN_H);
    btn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    btn.SetBackgroundColor(UiColor(bgColor));
    Label buttonLabel = Label::New(label);

    buttonLabel.SetRequestedWidth(MATCH_PARENT);

    buttonLabel.SetRequestedHeight(MATCH_PARENT);

    buttonLabel.SetFontSize(11.0f);

    buttonLabel.SetMultiLine(true);

    buttonLabel.SetTextColor(UiColor(C_BTN_TEXT));

    buttonLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    buttonLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    btn.Add(buttonLabel);
    btn.SetFocusable(true);

    InteractiveTrait interactive = btn.AsInteractive();

    interactive.ClickedSignal().Connect(this, [onClick = std::move(onClick)](View, const InputEvent&) -> bool {

      onClick();

      return true;

    });
    return btn;
  }

  StackLayout MakeButtonRow(std::initializer_list<View> buttons)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(BTN_H);
    row.SetPadding(Insets(0.0f, 0.0f, 2.0f, 2.0f));
    for(auto& b : buttons)
    {
      row.Add(b);
    }
    return row;
  }

  LottieAnimationView mView;
  Label               mStatusLabel;
  View                mAreaButtons[AREA_COUNT];
  View                mAnimateButton;
  Animation           mAnimation;
  Timer               mPollTimer;
  int                 mActiveIndex;
  bool                mAnimating;
};

REGISTER_MANUAL_TEST(TcLottiePixelArea)
