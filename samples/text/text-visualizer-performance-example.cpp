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
#include <dali-ui-foundation/devel-api/ui-foundation-pre-initialize.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float WINDOW_WIDTH  = 1920.0f;
constexpr float WINDOW_HEIGHT = 1080.0f;

constexpr float SIDE_MARGIN         = 286.0f;
constexpr float TITLE_TOP           = 58.0f;
constexpr float TITLE_HEIGHT        = 310.0f;
constexpr float CONTENT_TOP         = 350.0f;
constexpr float CONTENT_WIDTH       = WINDOW_WIDTH - (SIDE_MARGIN * 2.0f);
constexpr float COLUMN_GAP          = 58.0f;
constexpr float COLUMN_SPLIT_WIDTH  = COLUMN_GAP;
constexpr float STATUS_HEIGHT       = 36.0f;
constexpr float STATUS_BOTTOM       = 16.0f;
constexpr float TITLE_FONT_SIZE     = 95.0f;
constexpr float BODY_FONT_SIZE      = 16.0f;
constexpr float BODY_LINE_HEIGHT    = 2.0f;
constexpr float MIN_FONT_SIZE       = 16.0f;
constexpr float MAX_FONT_SIZE       = 32.0f;
constexpr float FONT_STEP           = 2.0f;
constexpr uint32_t MAX_ORB_COUNT    = 6u;
constexpr uint32_t MIN_ORB_COUNT    = 2u;
constexpr uint32_t DEFAULT_ORB_COUNT = 5u;
constexpr uint32_t STATUS_INTERVAL  = 10u;
constexpr uint32_t TIMER_INTERVAL_MS = 16u;
constexpr uint32_t ORB_EXCLUSION_BAND_COUNT = 11u;
constexpr float BODY_SURFACE_LEFT             = SIDE_MARGIN - 6.0f;
constexpr float BODY_SURFACE_TOP              = TITLE_TOP;
constexpr float BODY_SURFACE_WIDTH            = WINDOW_WIDTH - BODY_SURFACE_LEFT - SIDE_MARGIN;
constexpr float BODY_SURFACE_HEIGHT           = WINDOW_HEIGHT - BODY_SURFACE_TOP - STATUS_HEIGHT - STATUS_BOTTOM;
constexpr float TITLE_WIDTH                   = WINDOW_WIDTH / 2.0f - 80.0f;
constexpr float TITLE_EXCLUSION_LINE_0_WIDTH  = 880.0f;
constexpr float TITLE_EXCLUSION_LINE_2_WIDTH  = 510.0f;
constexpr float TITLE_EXCLUSION_LINE_HEIGHT   = TITLE_FONT_SIZE * 0.72f;
constexpr float TITLE_EXCLUSION_LINE_PITCH    = TITLE_FONT_SIZE * 0.88f;
constexpr float TITLE_EXCLUSION_TOP_OFFSET    = 8.0f;
constexpr float TITLE_EXCLUSION_PADDING       = 8.0f;
constexpr float DROP_CAP_TEXT_X               = TITLE_EXCLUSION_LINE_0_WIDTH + TITLE_EXCLUSION_PADDING + 45.0f;
constexpr float DROP_CAP_TEXT_Y               = 0.0f;
constexpr float DROP_CAP_TEXT_WIDTH           = 60.0f;
constexpr float DROP_CAP_TEXT_HEIGHT          = 60.0f;
constexpr float DROP_CAP_FONT_SIZE            = 80.0f;
constexpr const char* SAMPLE_FONT_FAMILY      = "DejaVu Serif";
constexpr float TEXT_VISUALIZER_LABEL_WIDTH   = 790.0f;
constexpr float TEXT_VISUALIZER_LABEL_HEIGHT  = 60.0f;
constexpr float TEXT_VISUALIZER_LABEL_TRAVEL  = 160.0f;
constexpr float TEXT_VISUALIZER_LABEL_X       = BODY_SURFACE_WIDTH - TEXT_VISUALIZER_LABEL_WIDTH - TEXT_VISUALIZER_LABEL_TRAVEL;
constexpr float TEXT_VISUALIZER_LABEL_Y       = 70.0f + (CONTENT_TOP - BODY_SURFACE_TOP);
constexpr float TIZEN_LABEL_X                 = -10.0f + (SIDE_MARGIN - BODY_SURFACE_LEFT);
constexpr float TIZEN_LABEL_Y                 = 340.0f + (CONTENT_TOP - BODY_SURFACE_TOP) + 140.0f;
constexpr float TIZEN_LABEL_WIDTH             = 520.0f;
constexpr float TIZEN_LABEL_HEIGHT            = 60.0f;
constexpr float TIZEN_LABEL_TRAVEL            = 140.0f;
constexpr float OVERLAY_TEXT_MOVE_SPEED       = 0.55f;
constexpr float OVERLAY_TEXT_OPPOSITE_PHASE   = 3.14159265f;

const UiColor WINDOW_BG_COLOR(0x0B0B10);
const UiColor TITLE_COLOR(0xF4F1EC);
const UiColor STATUS_TEXT_COLOR = UiColor(0xFFFFFF).WithAlpha(0.45f);
const UiColor QUOTE_TEXT_COLOR(0xCFB06A);
const UiColor TIZEN_TEXT_COLOR(0x57C7F3);

const UiColor TEXT_COLORS[] = {
  UiColor(0xF1E8D2),
  UiColor(0xF2E7C6),
  UiColor(0xEDE5D4),
  UiColor(0xF0DEB5),
};

const char* TEXT_COLOR_NAMES[] = {
  "Ivory",
  "Warm",
  "Paper",
  "Amber",
};

struct OrbPreset
{
  Vector2 position;
  Vector2 size;
  Vector2 velocity;
  UiColor color;
};

const OrbPreset ORB_PRESETS[MAX_ORB_COUNT] = {
  {Vector2(168.0f, 218.0f),  Vector2(172.0f, 172.0f), Vector2(54.0f, 34.0f), UiColor(0x2D355C).WithAlpha(0.92f)},
  {Vector2(742.0f, 92.0f),   Vector2(238.0f, 238.0f), Vector2(-42.0f, 28.0f), UiColor(0x584D35).WithAlpha(0.90f)},
  {Vector2(1420.0f, 130.0f), Vector2(148.0f, 148.0f), Vector2(48.0f, 31.0f), UiColor(0x43315E).WithAlpha(0.92f)},
  {Vector2(1456.0f, 428.0f), Vector2(212.0f, 212.0f), Vector2(-30.0f, -24.0f), UiColor(0x6A3846).WithAlpha(0.92f)},
  {Vector2(108.0f, 442.0f),  Vector2(162.0f, 162.0f), Vector2(28.0f, -22.0f), UiColor(0x32524C).WithAlpha(0.90f)},
  {Vector2(914.0f, 382.0f),  Vector2(134.0f, 134.0f), Vector2(33.0f, 38.0f), UiColor(0x6B4C2F).WithAlpha(0.88f)},
};

Dali::String BuildTitleText()
{
  return Dali::String("THE FUTURE OF\nTEXT LAYOUT IS\nDYNAMIC");
}

Dali::String BuildBodyText()
{
  std::ostringstream builder;
  builder
    << "he rendering pipeline was designed for static pages, yet every modern interface asks text to respond to moving objects, live composition, and editorial pacing. "
    << "텍스트는 더 이상 사각형 안에 갇힌 데이터가 아니라 주변 요소와 함께 리듬을 만들어내는 시각 재료다. "
    << "When a system can prepare shaping once and then reflow around objects, it stops feeling like a rigid document engine and starts behaving like a layout instrument. "
    << "A browser still hides the real cost of measurement behind synchronous barriers. A paragraph looks effortless only because the engine measured, broke, aligned, and painted it before the reader noticed. "
    << "But when interactive objects move through the composition, the hidden pipeline becomes visible. The question is no longer whether text can wrap. The question is whether text can stay fluid without forcing the whole page to stall."
    << "Design systems often treat text as the last step: render a block, then stack objects around it. Editorial interfaces reverse the rule. "
    << "Objects enter first, and the text must adapt in real time without losing its tone, measure, or cadence. "
    << "That means line height, baseline placement, and the geometry passed to the renderer all matter at once. "
    << "When the gap between lines is too tight, every moving obstacle feels heavier because more lines must be processed and more fragments must squeeze into the same height. "
    << "Once line metrics become more natural, the paragraphs breathe. The visual change looks subtle, but the engine also gains room: fewer visible lines, fewer interval scans, and less layout churn under pressure."
    << "CSS taught the industry to think in rectangles, but the most interesting interfaces are rarely rectangular. Cards expand, images drift, subtitles appear, and notifications cut across the reading flow. "
    << "Developers end up guessing text height, over-measuring the DOM, or paying for expensive relayout loops because the engine exposes too little of its own structure. "
    << "A text visualizer should move in the opposite direction. It should prepare text once, expose stable layout inputs, and let the application reposition exclusion bounds as often as composition demands. "
    << "The renderer then needs to update cleanly when those placements change."
    << "This sample keeps one large text surface and uses invisible fixed bounds to split the body into three reading lanes. "
    << "The moving orb should feel like an inserted image or a conversation bubble that the paragraph naturally avoids. "
    << "It is not enough for the box height to change. The glyphs must truly reflow and find a new path through the composition as the scene shifts. "
    << "If the text remains readable while the objects drift, the pipeline stops being a demo and starts becoming a usable editorial primitive."
    << "The hard part is not drawing a glyph atlas. The hard part is keeping every later decision honest after the glyphs are prepared. "
    << "A moving title, a floating quote, or a circular object can invalidate naive assumptions about where a line begins and how many fragments a renderer must accept. "
    << "If the exclusion geometry is too coarse, the result looks mechanical; if it is too dense, the frame budget disappears. "
    << "A practical implementation has to balance editorial shape with predictable cost, then make that trade visible enough for developers to tune."
    << "The title at the top is deliberately oversized so the body text can demonstrate a second kind of avoidance: not just an obstacle box, but a typographic silhouette. "
    << "The body starts at the same origin as the title and flows through the remaining space, while the title itself stays readable as an independent visual layer. "
    << "This is the kind of composition that usually forces applications into screenshots, masks, or custom canvas drawing. "
    << "Here it remains text: shaped once, measured as text, and continuously rearranged as the scene moves.";

  const std::string text = builder.str();
  return Dali::String(text.c_str());
}

Property::Map CreateOrbShadowMap(const UiColor& color)
{
  Property::Map transform;
  transform.Add(Ui::Visual::Transform::Property::OFFSET, Vector2(14.0f, 16.0f));
  transform.Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
                Vector2(static_cast<float>(Ui::Visual::Transform::Policy::ABSOLUTE),
                        static_cast<float>(Ui::Visual::Transform::Policy::ABSOLUTE)));
  transform.Add(Ui::Visual::Transform::Property::SIZE, Vector2(1.08f, 1.08f));
  transform.Add(Ui::Visual::Transform::Property::SIZE_POLICY,
                Vector2(static_cast<float>(Ui::Visual::Transform::Policy::RELATIVE),
                        static_cast<float>(Ui::Visual::Transform::Policy::RELATIVE)));

  Property::Map shadow;
  shadow.Add(Ui::Visual::Property::TYPE, Ui::Visual::COLOR);
  shadow.Add(Ui::Visual::Property::MIX_COLOR, color.Resolve());
  shadow.Add(Ui::Visual::Property::TRANSFORM, transform);
  return shadow;
}

Vector2 GetLegacyContentOffset()
{
  return Vector2(SIDE_MARGIN - BODY_SURFACE_LEFT, CONTENT_TOP - BODY_SURFACE_TOP);
}

Vector2 GetBodySurfaceOrigin()
{
  return Vector2(BODY_SURFACE_LEFT, BODY_SURFACE_TOP);
}

void AppendTitleLineExclusion(Dali::Vector<Rect<float>>& regions, float lineWidth, uint32_t lineIndex)
{
  const float lineY     = TITLE_EXCLUSION_TOP_OFFSET + (static_cast<float>(lineIndex) * TITLE_EXCLUSION_LINE_PITCH);

  regions.PushBack(Rect<float>(-TITLE_EXCLUSION_PADDING,
                               lineY - TITLE_EXCLUSION_PADDING,
                               lineWidth + (TITLE_EXCLUSION_PADDING * 2.0f),
                               TITLE_EXCLUSION_LINE_HEIGHT + (TITLE_EXCLUSION_PADDING * 2.0f)));
}

void AppendTitleLineRangeExclusion(Dali::Vector<Rect<float>>& regions, float lineWidth, uint32_t firstLineIndex, uint32_t lineCount)
{
  if(lineCount == 0u)
  {
    return;
  }

  const float lineY       = TITLE_EXCLUSION_TOP_OFFSET + (static_cast<float>(firstLineIndex) * TITLE_EXCLUSION_LINE_PITCH);
  const float rangeHeight = TITLE_EXCLUSION_LINE_HEIGHT + (static_cast<float>(lineCount - 1u) * TITLE_EXCLUSION_LINE_PITCH);

  regions.PushBack(Rect<float>(-TITLE_EXCLUSION_PADDING,
                               lineY - TITLE_EXCLUSION_PADDING,
                               lineWidth + (TITLE_EXCLUSION_PADDING * 2.0f),
                               rangeHeight + (TITLE_EXCLUSION_PADDING * 2.0f)));
}

struct MovingOrb
{
  View     view;
  Vector2  position{Vector2::ZERO};
  Vector2  size{Vector2::ZERO};
  Vector2  velocity{Vector2::ZERO};
  UiColor  color;
};

struct OverlayTextBlock
{
  TextVisualizer text;
  Rect<float>    baseBounds;
  Rect<float>    bounds;
  float          travelX{0.0f};
  float          phase{0.0f};
};
} // namespace

class TextVisualizerPerformanceController : public ConnectionTracker
{
public:
  explicit TextVisualizerPerformanceController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextVisualizerPerformanceController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(WINDOW_BG_COLOR);
    window.SetSize(Dali::Window::WindowSize(static_cast<uint32_t>(WINDOW_WIDTH), static_cast<uint32_t>(WINDOW_HEIGHT)));
    window.KeyEventSignal().Connect(this, &TextVisualizerPerformanceController::OnKeyEvent);

    CreateScene(window);
    CreateTextSurface();
    RebuildStaticExclusionRegions();
    CreateOverlayTextBlocks();
    CreateMovingOrbs();
    ApplyExclusionRegions();

    mStartTime    = std::chrono::steady_clock::now();
    mLastTickTime = mStartTime;
    UpdateStatusText(true);

    mTimer = Timer::New(TIMER_INTERVAL_MS);
    mTimer.TickSignal().Connect(this, &TextVisualizerPerformanceController::OnTimerTick);
    mTimer.Start();
  }

  void CreateScene(Window& window)
  {
    mRoot = View::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(WINDOW_BG_COLOR);

    mTitleText = TextVisualizer::New();
    mTitleText.SetPositionX(BODY_SURFACE_LEFT);
    mTitleText.SetPositionY(BODY_SURFACE_TOP);
    mTitleText.SetRequestedWidth(TITLE_WIDTH);
    mTitleText.SetRequestedHeight(TITLE_HEIGHT);
    mTitleText.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mTitleText.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
    mTitleText.SetText(BuildTitleText());
    mTitleText.SetFontFamily(SAMPLE_FONT_FAMILY);
    mTitleText.SetFontSize(TITLE_FONT_SIZE);
    mTitleText.SetLineHeight(0.88f);
    mTitleText.SetTextColor(TITLE_COLOR);

    mContentArea = View::New();
    mContentArea.SetLayoutMode(LayoutMode::STANDALONE);
    mContentArea.SetPositionX(BODY_SURFACE_LEFT);
    mContentArea.SetPositionY(BODY_SURFACE_TOP);
    mContentArea.SetRequestedWidth(BODY_SURFACE_WIDTH);
    mContentArea.SetRequestedHeight(BODY_SURFACE_HEIGHT);
    mContentArea.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mContentArea.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

    mStatusText = TextVisualizer::New();
    mStatusText.SetPositionX(SIDE_MARGIN);
    mStatusText.SetPositionY(WINDOW_HEIGHT - STATUS_HEIGHT - STATUS_BOTTOM);
    mStatusText.SetRequestedWidth(CONTENT_WIDTH);
    mStatusText.SetRequestedHeight(STATUS_HEIGHT);
    mStatusText.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mStatusText.SetFontFamily(SAMPLE_FONT_FAMILY);
    mStatusText.SetFontSize(15.0f);
    mStatusText.SetLineHeight(1.2f);
    mStatusText.SetTextColor(STATUS_TEXT_COLOR);

    mRoot.Add(mContentArea);
    mRoot.Add(mTitleText);
    mRoot.Add(mStatusText);
    window.Add(mRoot);
  }

  void CreateTextSurface()
  {
    mTextVisualizer = TextVisualizer::New();
    mTextVisualizer.SetPositionX(0.0f);
    mTextVisualizer.SetPositionY(0.0f);
    mTextVisualizer.SetRequestedWidth(BODY_SURFACE_WIDTH);
    mTextVisualizer.SetRequestedHeight(BODY_SURFACE_HEIGHT);
    mTextVisualizer.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mTextVisualizer.SetText(BuildBodyText());
    mTextVisualizer.SetFontFamily(SAMPLE_FONT_FAMILY);
    mTextVisualizer.SetFontSize(mBodyFontSize);
    mTextVisualizer.SetLineHeight(BODY_LINE_HEIGHT);
    mTextVisualizer.SetTextColor(TEXT_COLORS[mCurrentTextColorIndex]);

    const float bodyColumnWidth = (BODY_SURFACE_WIDTH - (COLUMN_GAP * 2.0f)) / 3.0f;
    mFixedColumnBounds.Clear();
    mFixedColumnBounds.PushBack(Rect<float>(bodyColumnWidth, 0.0f, COLUMN_SPLIT_WIDTH, BODY_SURFACE_HEIGHT));
    mFixedColumnBounds.PushBack(Rect<float>((bodyColumnWidth * 2.0f) + COLUMN_GAP, 0.0f, COLUMN_SPLIT_WIDTH, BODY_SURFACE_HEIGHT));

    mContentArea.Add(mTextVisualizer);

    mDropCapText = TextVisualizer::New();
    mDropCapText.SetPositionX(BODY_SURFACE_LEFT + DROP_CAP_TEXT_X);
    mDropCapText.SetPositionY(BODY_SURFACE_TOP + DROP_CAP_TEXT_Y);
    mDropCapText.SetRequestedWidth(DROP_CAP_TEXT_WIDTH);
    mDropCapText.SetRequestedHeight(DROP_CAP_TEXT_HEIGHT);
    mDropCapText.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mDropCapText.SetText("T");
    mDropCapText.SetFontFamily(SAMPLE_FONT_FAMILY);
    mDropCapText.SetFontSize(DROP_CAP_FONT_SIZE);
    mDropCapText.SetLineHeight(1.0f);
    mDropCapText.SetTextColor(QUOTE_TEXT_COLOR);

    mRoot.Add(mDropCapText);
  }

  void CreateOverlayTextBlocks()
  {
    mOverlayTextBlocks.clear();
    mOverlayTextBlocks.reserve(2u);

    auto addTextBlock = [&](const Dali::String& text, const Rect<float>& bounds, float travelX, float phase, float fontSize, const UiColor& textColor)
    {
      OverlayTextBlock block;
      block.baseBounds = bounds;
      block.bounds     = bounds;
      block.travelX    = travelX;
      block.phase      = phase;

      block.text = TextVisualizer::New();
      block.text.SetPositionX(BODY_SURFACE_LEFT + bounds.x);
      block.text.SetPositionY(BODY_SURFACE_TOP + bounds.y);
      block.text.SetRequestedWidth(bounds.width);
      block.text.SetRequestedHeight(bounds.height);
      block.text.SetBackgroundColor(UiColor(Color::TRANSPARENT));
      block.text.SetText(text);
      block.text.SetFontFamily(SAMPLE_FONT_FAMILY);
      block.text.SetFontSize(fontSize);
      block.text.SetLineHeight(1.0f);
      block.text.SetTextColor(textColor);

      mRoot.Add(block.text);
      mOverlayTextBlocks.push_back(block);
    };

    addTextBlock("TIZEN DALI UI",
                 Rect<float>(TIZEN_LABEL_X, TIZEN_LABEL_Y, TIZEN_LABEL_WIDTH, TIZEN_LABEL_HEIGHT),
                 TIZEN_LABEL_TRAVEL,
                 OVERLAY_TEXT_OPPOSITE_PHASE,
                 65.0f,
                 TIZEN_TEXT_COLOR);

    addTextBlock("TEXT VISUALIZER",
                 Rect<float>(TEXT_VISUALIZER_LABEL_X, TEXT_VISUALIZER_LABEL_Y, TEXT_VISUALIZER_LABEL_WIDTH, TEXT_VISUALIZER_LABEL_HEIGHT),
                 TEXT_VISUALIZER_LABEL_TRAVEL,
                 0.0f,
                 80.0f,
                 QUOTE_TEXT_COLOR);
  }

  void CreateMovingOrbs()
  {
    mOrbs.clear();
    mOrbs.reserve(MAX_ORB_COUNT);

    for(uint32_t index = 0u; index < MAX_ORB_COUNT; ++index)
    {
      MovingOrb orb;
      orb.position    = ClampOrbPosition(ORB_PRESETS[index].position + GetLegacyContentOffset() + GetBodySurfaceOrigin(), ORB_PRESETS[index].size);
      orb.size        = ORB_PRESETS[index].size;
      orb.velocity    = ORB_PRESETS[index].velocity;
      orb.color       = ORB_PRESETS[index].color;

      orb.view = View::New();
      orb.view.SetLayoutMode(LayoutMode::STANDALONE);
      orb.view.SetBackgroundColor(orb.color);
      orb.view.SetCornerRadiusPolicyRelative();
      orb.view.SetCornerRadius(0.5f);
      orb.view.SetProperty(View::Property::SHADOW, CreateOrbShadowMap(orb.color.WithAlpha(0.28f)));
      orb.view.SetProperty(Actor::Property::VISIBLE, index < mActiveOrbCount);
      orb.view.SetPositionX(orb.position.x);
      orb.view.SetPositionY(orb.position.y);
      orb.view.SetRequestedWidth(orb.size.x);
      orb.view.SetRequestedHeight(orb.size.y);
      orb.view.TouchedSignal().Connect(this, &TextVisualizerPerformanceController::OnOrbTouched);

      mRoot.Add(orb.view);
      mOrbs.push_back(orb);
    }

    RaiseForegroundTextToTop();
  }

  void RaiseForegroundTextToTop()
  {
    mTitleText.RaiseToTop(LayoutOrderPolicy::PRESERVE);
    mDropCapText.RaiseToTop(LayoutOrderPolicy::PRESERVE);
    for(OverlayTextBlock& block : mOverlayTextBlocks)
    {
      block.text.RaiseToTop(LayoutOrderPolicy::PRESERVE);
    }
  }

  int32_t FindOrbIndex(const Actor& actor) const
  {
    for(uint32_t index = 0u; index < mOrbs.size(); ++index)
    {
      if(mOrbs[index].view == actor)
      {
        return static_cast<int32_t>(index);
      }
    }
    return -1;
  }

  Vector2 ClampOrbPosition(const Vector2& position, const Vector2& size) const
  {
    const float maxX = std::max(0.0f, WINDOW_WIDTH - size.x);
    const float maxY = std::max(0.0f, WINDOW_HEIGHT - size.y);
    return Vector2(std::clamp(position.x, 0.0f, maxX),
                   std::clamp(position.y, 0.0f, maxY));
  }

  bool GetWindowLocalPosition(const TouchEvent& touch, Vector2& localPosition) const
  {
    if(touch.GetPointCount() < 1u)
    {
      return false;
    }

    const Vector2 screenPosition = touch.GetScreenPosition(0u);
    float         localX         = 0.0f;
    float         localY         = 0.0f;
    if(!mRoot.ScreenToLocal(localX, localY, screenPosition.x, screenPosition.y))
    {
      return false;
    }

    localPosition = Vector2(localX, localY);
    return true;
  }

  void UpdateDraggedOrb(const Vector2& windowLocalPosition)
  {
    if(mDraggedOrbIndex < 0 || static_cast<uint32_t>(mDraggedOrbIndex) >= mOrbs.size())
    {
      return;
    }

    MovingOrb& orb = mOrbs[static_cast<uint32_t>(mDraggedOrbIndex)];
    orb.position    = ClampOrbPosition(windowLocalPosition - mDragGrabOffset, orb.size);
    UpdateOrbPositionsOnScreen();
    ApplyExclusionRegions();
    UpdateStatusText(true);
  }

  bool OnOrbTouched(Actor actor, const TouchEvent& touch)
  {
    const int32_t orbIndex = FindOrbIndex(actor);
    if(orbIndex < 0 || static_cast<uint32_t>(orbIndex) >= mActiveOrbCount || touch.GetPointCount() < 1u)
    {
      return false;
    }

    Vector2 windowLocalPosition;
    if(!GetWindowLocalPosition(touch, windowLocalPosition))
    {
      return false;
    }

    const PointState::Type state = touch.GetState(0u);
    if(state == PointState::DOWN)
    {
      mDraggedOrbIndex = orbIndex;
      mDragGrabOffset  = windowLocalPosition - mOrbs[static_cast<uint32_t>(orbIndex)].position;
      UpdateDraggedOrb(windowLocalPosition);
      return true;
    }

    if(mDraggedOrbIndex != orbIndex)
    {
      return false;
    }

    if(state == PointState::MOTION)
    {
      UpdateDraggedOrb(windowLocalPosition);
      return true;
    }

    if(state == PointState::UP || state == PointState::INTERRUPTED || state == PointState::LEAVE)
    {
      UpdateDraggedOrb(windowLocalPosition);
      mDraggedOrbIndex = -1;
      UpdateStatusText(true);
      return true;
    }

    return true;
  }

  void ApplyTextStyle()
  {
    mTextVisualizer.SetFontSize(mBodyFontSize);
    mTextVisualizer.SetLineHeight(BODY_LINE_HEIGHT);
    mTextVisualizer.SetTextColor(TEXT_COLORS[mCurrentTextColorIndex]);
  }

  void UpdateOrbVisibility()
  {
    for(uint32_t index = 0u; index < mOrbs.size(); ++index)
    {
      mOrbs[index].view.SetProperty(Actor::Property::VISIBLE, index < mActiveOrbCount);
    }
  }

  void UpdateOrbPositionsOnScreen()
  {
    for(uint32_t index = 0u; index < std::min<uint32_t>(mActiveOrbCount, static_cast<uint32_t>(mOrbs.size())); ++index)
    {
      mOrbs[index].view.SetPositionX(mOrbs[index].position.x);
      mOrbs[index].view.SetPositionY(mOrbs[index].position.y);
      ++mOrbVisualUpdateCount;
    }
  }

  Rect<float> GetOverlayTextExclusionRect(const OverlayTextBlock& block) const
  {
    return block.bounds;
  }

  Rect<float> GetDropCapExclusionRect() const
  {
    return Rect<float>(DROP_CAP_TEXT_X, DROP_CAP_TEXT_Y, DROP_CAP_TEXT_WIDTH, DROP_CAP_TEXT_HEIGHT);
  }

  void AppendOrbExclusionRegions(Dali::Vector<Rect<float>>& regions, const MovingOrb& orb) const
  {
    constexpr float ORB_BOUND_PADDING = 8.0f;
    const Vector2   bodyLocalPosition = orb.position - GetBodySurfaceOrigin();
    const float     radiusX    = orb.size.x * 0.5f;
    const float     radiusY    = orb.size.y * 0.5f;
    const float     centerX    = bodyLocalPosition.x + radiusX;
    const float     centerY    = bodyLocalPosition.y + radiusY;
    const float     bandHeight = orb.size.y / static_cast<float>(ORB_EXCLUSION_BAND_COUNT);

    for(uint32_t bandIndex = 0u; bandIndex < ORB_EXCLUSION_BAND_COUNT; ++bandIndex)
    {
      const float bandTop     = bodyLocalPosition.y + (static_cast<float>(bandIndex) * bandHeight);
      const float bandCenterY = bandTop + (bandHeight * 0.5f);
      const float normalizedY = (radiusY > 0.0f) ? ((bandCenterY - centerY) / radiusY) : 0.0f;
      const float xScale      = std::sqrt(std::max(0.0f, 1.0f - (normalizedY * normalizedY)));
      const float bandWidth   = std::max(0.0f, orb.size.x * xScale);
      const float bandLeft    = centerX - (bandWidth * 0.5f);

      regions.PushBack(Rect<float>(bandLeft - ORB_BOUND_PADDING,
                                   bandTop - ORB_BOUND_PADDING,
                                   bandWidth + (ORB_BOUND_PADDING * 2.0f),
                                   bandHeight + (ORB_BOUND_PADDING * 2.0f)));
    }
  }

  void AppendTitleExclusionRegions(Dali::Vector<Rect<float>>& regions) const
  {
    AppendTitleLineRangeExclusion(regions, TITLE_EXCLUSION_LINE_0_WIDTH, 0u, 2u);
    AppendTitleLineExclusion(regions, TITLE_EXCLUSION_LINE_2_WIDTH, 2u);
  }

  void RebuildStaticExclusionRegions()
  {
    mStaticExclusionRegions.Clear();
    mStaticExclusionRegions.Reserve(5u);

    AppendTitleExclusionRegions(mStaticExclusionRegions);
    mStaticExclusionRegions.PushBack(GetDropCapExclusionRect());

    for(uint32_t index = 0u; index < mFixedColumnBounds.Count(); ++index)
    {
      mStaticExclusionRegions.PushBack(mFixedColumnBounds[index]);
    }
  }

  void RebuildDynamicExclusionRegions()
  {
    const uint32_t activeOrbCount = std::min<uint32_t>(mActiveOrbCount, static_cast<uint32_t>(mOrbs.size()));

    mDynamicExclusionRegions.Clear();
    mDynamicExclusionRegions.Reserve((activeOrbCount * ORB_EXCLUSION_BAND_COUNT) + static_cast<uint32_t>(mOverlayTextBlocks.size()));

    for(uint32_t orbIndex = 0u; orbIndex < activeOrbCount; ++orbIndex)
    {
      AppendOrbExclusionRegions(mDynamicExclusionRegions, mOrbs[orbIndex]);
    }

    for(uint32_t blockIndex = 0u; blockIndex < mOverlayTextBlocks.size(); ++blockIndex)
    {
      mDynamicExclusionRegions.PushBack(GetOverlayTextExclusionRect(mOverlayTextBlocks[blockIndex]));
    }
  }

  void RebuildCombinedExclusionRegions()
  {
    mCombinedExclusionRegions.Clear();
    mCombinedExclusionRegions.Reserve(mStaticExclusionRegions.Count() + mDynamicExclusionRegions.Count());

    for(uint32_t index = 0u; index < mStaticExclusionRegions.Count(); ++index)
    {
      mCombinedExclusionRegions.PushBack(mStaticExclusionRegions[index]);
    }

    for(uint32_t index = 0u; index < mDynamicExclusionRegions.Count(); ++index)
    {
      mCombinedExclusionRegions.PushBack(mDynamicExclusionRegions[index]);
    }
  }

  void ApplyExclusionRegions()
  {
    ++mApplyExclusionCallCount;

    if(!mExclusionEnabled)
    {
      if(mHasAppliedExclusionRegions)
      {
        mTextVisualizer.ClearExclusionRegions();
        mHasAppliedExclusionRegions = false;
        ++mAppliedExclusionUpdateCount;
      }
      return;
    }

    RebuildDynamicExclusionRegions();
    RebuildCombinedExclusionRegions();

    mTextVisualizer.SetExclusionRegions(mCombinedExclusionRegions);
    mHasAppliedExclusionRegions  = true;
    ++mAppliedExclusionUpdateCount;
    ++mSetExclusionRegionsCallCount;
  }

  void UpdateOrbPositions(float deltaSeconds)
  {
    for(uint32_t index = 0u; index < std::min<uint32_t>(mActiveOrbCount, static_cast<uint32_t>(mOrbs.size())); ++index)
    {
      if(mDraggedOrbIndex == static_cast<int32_t>(index))
      {
        continue;
      }

      MovingOrb& orb = mOrbs[index];
      orb.position += orb.velocity * deltaSeconds;
      const Vector2 clampedPosition = ClampOrbPosition(orb.position, orb.size);

      if(clampedPosition.x != orb.position.x)
      {
        orb.velocity.x = (clampedPosition.x <= 0.0f) ? std::abs(orb.velocity.x) : -std::abs(orb.velocity.x);
      }

      if(clampedPosition.y != orb.position.y)
      {
        orb.velocity.y = (clampedPosition.y <= 0.0f) ? std::abs(orb.velocity.y) : -std::abs(orb.velocity.y);
      }

      orb.position = clampedPosition;
      ++mOrbPositionUpdateCount;
    }
  }

  void UpdateOverlayTextBlocks(float elapsedSeconds)
  {
    for(OverlayTextBlock& block : mOverlayTextBlocks)
    {
      const float offset = ((std::sin((elapsedSeconds * OVERLAY_TEXT_MOVE_SPEED) + block.phase) + 1.0f) * 0.5f) * block.travelX;
      block.bounds      = block.baseBounds;
      block.bounds.x += offset;
      block.text.SetPositionX(BODY_SURFACE_LEFT + block.bounds.x);
    }
  }

  bool OnTimerTick()
  {
    const auto now        = std::chrono::steady_clock::now();
    const float deltaSecs = std::chrono::duration<float>(now - mLastTickTime).count();
    const float elapsedSecs = std::chrono::duration<float>(now - mStartTime).count();
    mLastTickTime         = now;
    ++mFrameCount;

    UpdateOrbPositions(std::max(deltaSecs, 0.001f));
    UpdateOverlayTextBlocks(std::max(elapsedSecs, 0.0f));
    UpdateOrbPositionsOnScreen();
    ApplyExclusionRegions();

    if((mFrameCount % STATUS_INTERVAL) == 0u)
    {
      UpdateStatusText(false);
    }

    return true;
  }

  void UpdateStatusText(bool force)
  {
    if(!force && (mFrameCount % STATUS_INTERVAL) != 0u)
    {
      return;
    }

    const auto  now        = std::chrono::steady_clock::now();
    const float elapsedSec = std::max(std::chrono::duration<float>(now - mStartTime).count(), 0.001f);
    const float fps        = static_cast<float>(mFrameCount) / elapsedSec;

    ++mStatusTextUpdateCount;

    std::ostringstream builder;
    builder.setf(std::ios::fixed);
    builder.precision(1);
    builder << "FPS " << fps;
    if(!mDetailedStatusEnabled)
    {
      mStatusText.SetText(builder.str().c_str());
      return;
    }

    builder << "   FRAME " << mFrameCount
            << "   ORBS " << mActiveOrbCount
            << "   APPLIED " << mAppliedExclusionUpdateCount
            << "   APPLY " << mApplyExclusionCallCount
            << "   SET " << mSetExclusionRegionsCallCount
            << "   ORBPOS " << mOrbPositionUpdateCount
            << "   ORBVIEW " << mOrbVisualUpdateCount
            << "   STATUS " << mStatusTextUpdateCount
            << "   FONT " << mBodyFontSize
            << "   LINE " << BODY_LINE_HEIGHT
            << "   COLOR " << TEXT_COLOR_NAMES[mCurrentTextColorIndex]
            << "   EXCLUSION " << (mExclusionEnabled ? "ON" : "OFF")
            << "   0 debug · 1/2 orbs · 3/4 font · 5 exclusion · 6 color";

    mStatusText.SetText(builder.str().c_str());
  }

  void ToggleStatusDetails()
  {
    mDetailedStatusEnabled = !mDetailedStatusEnabled;
    UpdateStatusText(true);
  }

  void SetOrbCount(uint32_t count)
  {
    mActiveOrbCount = std::min(std::max(count, MIN_ORB_COUNT), MAX_ORB_COUNT);
    UpdateOrbVisibility();
    ApplyExclusionRegions();
    UpdateStatusText(true);
  }

  void AdjustFontSize(float delta)
  {
    mBodyFontSize = std::min(std::max(mBodyFontSize + delta, MIN_FONT_SIZE), MAX_FONT_SIZE);
    ApplyTextStyle();
    ApplyExclusionRegions();
    UpdateStatusText(true);
  }

  void ToggleExclusion()
  {
    mExclusionEnabled = !mExclusionEnabled;
    ApplyExclusionRegions();
    UpdateStatusText(true);
  }

  void CycleTextColor()
  {
    mCurrentTextColorIndex = (mCurrentTextColorIndex + 1u) % static_cast<uint32_t>(sizeof(TEXT_COLORS) / sizeof(TEXT_COLORS[0]));
    ApplyTextStyle();
    UpdateStatusText(true);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const std::string keyName = event.GetKeyName().CStr();
    if(keyName == "0")
    {
      ToggleStatusDetails();
    }
    else if(keyName == "1")
    {
      SetOrbCount(mActiveOrbCount - 1u);
    }
    else if(keyName == "2")
    {
      SetOrbCount(mActiveOrbCount + 1u);
    }
    else if(keyName == "3")
    {
      AdjustFontSize(-FONT_STEP);
    }
    else if(keyName == "4")
    {
      AdjustFontSize(FONT_STEP);
    }
    else if(keyName == "5")
    {
      ToggleExclusion();
    }
    else if(keyName == "6")
    {
      CycleTextColor();
    }
  }

private:
  Application&                          mApplication;
  View                                  mRoot;
  TextVisualizer                        mTitleText;
  TextVisualizer                        mStatusText;
  View                                  mContentArea;
  TextVisualizer                        mTextVisualizer;
  TextVisualizer                        mDropCapText;
  Dali::Vector<Rect<float>>             mFixedColumnBounds;
  Dali::Vector<Rect<float>>             mStaticExclusionRegions;
  Dali::Vector<Rect<float>>             mDynamicExclusionRegions;
  Dali::Vector<Rect<float>>             mCombinedExclusionRegions;
  std::vector<OverlayTextBlock>         mOverlayTextBlocks;
  std::vector<MovingOrb>                mOrbs;
  Timer                                 mTimer;
  bool                                  mExclusionEnabled{true};
  bool                                  mDetailedStatusEnabled{false};
  bool                                  mHasAppliedExclusionRegions{false};
  int32_t                               mDraggedOrbIndex{-1};
  Vector2                               mDragGrabOffset{Vector2::ZERO};
  uint32_t                              mActiveOrbCount{DEFAULT_ORB_COUNT};
  uint32_t                              mCurrentTextColorIndex{0u};
  float                                 mBodyFontSize{BODY_FONT_SIZE};
  uint64_t                              mFrameCount{0u};
  uint64_t                              mAppliedExclusionUpdateCount{0u};
  uint64_t                              mApplyExclusionCallCount{0u};
  uint64_t                              mSetExclusionRegionsCallCount{0u};
  uint64_t                              mOrbPositionUpdateCount{0u};
  uint64_t                              mOrbVisualUpdateCount{0u};
  uint64_t                              mStatusTextUpdateCount{0u};
  std::chrono::steady_clock::time_point mStartTime;
  std::chrono::steady_clock::time_point mLastTickTime;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextVisualizerPerformanceController controller(application);
  application.MainLoop();

  return 0;
}
