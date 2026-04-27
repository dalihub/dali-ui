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
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float WINDOW_WIDTH  = 1280.0f;
constexpr float WINDOW_HEIGHT = 768.0f;
constexpr float PI            = 3.14159265359f;

constexpr float PLAY_LEFT   = 45.0f;
constexpr float PLAY_TOP    = 51.0f;
constexpr float PLAY_RIGHT  = WINDOW_WIDTH - 45.0f;
constexpr float PLAY_BOTTOM = WINDOW_HEIGHT - 64.0f;
constexpr float PLAY_WIDTH  = PLAY_RIGHT - PLAY_LEFT;
constexpr float PLAY_HEIGHT = PLAY_BOTTOM - PLAY_TOP;
constexpr float BACKGROUND_TEXT_INSET  = 14.0f;
constexpr float BACKGROUND_TEXT_LEFT   = PLAY_LEFT + BACKGROUND_TEXT_INSET;
constexpr float BACKGROUND_TEXT_TOP    = PLAY_TOP + BACKGROUND_TEXT_INSET;
constexpr float BACKGROUND_TEXT_WIDTH  = PLAY_WIDTH - (BACKGROUND_TEXT_INSET * 2.0f);
constexpr float BACKGROUND_TEXT_HEIGHT = PLAY_HEIGHT - (BACKGROUND_TEXT_INSET * 2.0f);

constexpr float BRICK_AREA_TOP    = 140.0f;
constexpr float BRICK_GAP_X       = 6.0f;
constexpr float BRICK_GAP_Y       = 24.0f;
constexpr uint32_t BRICK_ROWS     = 3u;
constexpr uint32_t BRICK_COUNT    = 14u;
constexpr float MONO_GLYPH_WIDTH_RATIO = 0.58f;
constexpr float BACKGROUND_FONT_SIZE   = 22.0f;
constexpr float BRICK_FONT_SIZE        = 44.0f;
constexpr float BRICK_HEIGHT           = BRICK_FONT_SIZE * 1.18f;
constexpr float PADDLE_FONT_SIZE       = 26.0f;

constexpr float BALL_RADIUS       = 10.0f;
constexpr float BALL_EXCLUSION_RADIUS = 44.0f;
constexpr float BALL_SPEED        = 344.0f;
constexpr float PADDLE_WIDTH      = 14.0f * PADDLE_FONT_SIZE * MONO_GLYPH_WIDTH_RATIO;
constexpr float PADDLE_HEIGHT     = PADDLE_FONT_SIZE * 1.60f;
constexpr float PADDLE_Y          = WINDOW_HEIGHT - 100.0f;
constexpr float PADDLE_SPEED      = 624.0f;
constexpr float PADDLE_AUTO_SPEED = 416.0f;

constexpr uint32_t TIMER_INTERVAL_MS       = 16u;
constexpr uint32_t STATUS_INTERVAL         = 10u;
constexpr uint32_t BALL_EXCLUSION_BANDS    = 15u;
constexpr uint32_t BRICK_FRAGMENT_COUNT    = 8u;
constexpr uint32_t PADDLE_HIT_FRAGMENT_COUNT = 4u;
constexpr uint32_t BALL_TRAIL_SPARK_COUNT  = 1u;
constexpr uint32_t BALL_TRAIL_SPARK_INTERVAL = 4u;
constexpr float    EXCLUSION_PADDING       = 4.0f;
constexpr float    BRICK_EXCLUSION_HORIZONTAL_PADDING = 22.0f;
constexpr float    BRICK_PULSE_DURATION    = 0.42f;
constexpr float    BRICK_BUMP_DURATION     = 0.24f;
constexpr float    BRICK_BUMP_DISTANCE     = 8.0f;
constexpr float    FRAGMENT_DURATION       = 0.70f;
constexpr float    PADDLE_HIT_FRAGMENT_FONT_SIZE = 20.5f;
constexpr float    PADDLE_HIT_FRAGMENT_EXCLUSION_SCALE = 1.45f;
constexpr float    BRICK_FRAGMENT_FONT_SIZE = 20.5f;
constexpr float    BRICK_FRAGMENT_EXCLUSION_SCALE = 1.5f;
constexpr float    BALL_TRAIL_SPARK_DURATION = 0.34f;
constexpr float    BALL_TRAIL_SPARK_FONT_SIZE = 14.0f;
constexpr float    BALL_TRAIL_EXCLUSION_SCALE = 1.25f;
constexpr float    MIN_DELTA_SECONDS       = 0.001f;
constexpr float    MAX_DELTA_SECONDS       = 0.034f;

constexpr const char* SAMPLE_FONT_FAMILY   = "Ubuntu Mono";
constexpr const char* PADDLE_TEXT          = "==============\n"
                                             "==============";

enum class BrickRenderMode : uint8_t
{
  LABEL,
  TEXT_VISUALIZER,
};

const UiColor WINDOW_BG_COLOR(0x090A10);
const UiColor BACKGROUND_TEXT_COLOR = UiColor(0xC8D7E8).WithAlpha(0.38f);
const UiColor STATUS_TEXT_COLOR     = UiColor(0xF6F0DE).WithAlpha(0.80f);
const UiColor BALL_COLOR(0xF9E56E);
const UiColor BALL_TRAIL_COLOR = UiColor(0xFFB15A).WithAlpha(0.80f);
const UiColor PADDLE_COLOR(0x59D3F6);
const UiColor PLAY_BORDER_COLOR = UiColor(0x6EDCFF).WithAlpha(0.26f);

const std::array<UiColor, BRICK_COUNT> BRICK_COLORS = {{
  UiColor(0xFF5C8A),
  UiColor(0xFF9366),
  UiColor(0xF5CA63),
  UiColor(0xDDF06A),
  UiColor(0x7DE28F),
  UiColor(0x64E3D1),
  UiColor(0x63C4F5),
  UiColor(0x74A0FF),
  UiColor(0xA783FF),
  UiColor(0xDD7AF2),
  UiColor(0xFF80C8),
  UiColor(0xFFC06D),
  UiColor(0xD8F178),
  UiColor(0x7EE5B4),
}};

const std::array<const char*, BRICK_COUNT> BRICK_WORDS = {{
  "glyph",
  "atlas",
  "breaker",
  "layout",
  "visualizer",
  "cache",
  "metrics",
  "shaping",
  "renderer",
  "exclusion",
  "font",
  "paragraph",
  "collision",
  "texture",
}};

const std::array<uint32_t, BRICK_ROWS> BRICK_ROW_COUNTS = {{
  5u,
  4u,
  5u,
}};

const std::array<const char*, 4u> PADDLE_HIT_EFFECT_TEXTS = {{
  "==",
  "..",
  "==",
  "..",
}};

const std::array<const char*, 8u> BALL_TRAIL_EFFECT_TEXTS = {{
  "*",
  ".",
  "'",
  "^",
  "`",
  ":",
  "*",
  ".",
}};

Dali::String BuildBackgroundText()
{
  std::ostringstream builder;
  const char* lineA = "text breaker keeps prepared glyphs in motion while the ball opens a circular path through dense paragraphs. ";
  const char* lineB = "the paddle is text and every collision reshapes the field without turning the page into a bitmap. ";
  const char* lineC = "layout flows around moving exclusions and exposes the rhythm of a small arcade scene. ";

  for(uint32_t repeat = 0u; repeat < 8u; ++repeat)
  {
    builder << lineA << lineB << lineC;
  }

  return Dali::String(builder.str().c_str());
}

float GetMonospaceTextWidth(const char* text, float fontSize)
{
  return static_cast<float>(std::strlen(text)) * fontSize * MONO_GLYPH_WIDTH_RATIO;
}

float GetBrickTextWidth(const char* word)
{
  return GetMonospaceTextWidth(word, BRICK_FONT_SIZE);
}

float GetBrickExclusionWidth(const char* word)
{
  return GetBrickTextWidth(word) + (BRICK_EXCLUSION_HORIZONTAL_PADDING * 2.0f);
}

uint32_t GetBrickEffectFragmentCount(const char* text)
{
  if(!text)
  {
    return 0u;
  }

  const uint32_t textLength = static_cast<uint32_t>(std::strlen(text));
  return std::max(3u, std::min(textLength, BRICK_FRAGMENT_COUNT));
}

std::string BuildLetterEffectText(const char* text, uint32_t index)
{
  const uint32_t textLength = text ? static_cast<uint32_t>(std::strlen(text)) : 0u;
  if(textLength == 0u)
  {
    return std::string();
  }

  return std::string(1u, text[index % textLength]);
}

std::string BuildPaddleHitEffectText(uint32_t index)
{
  return PADDLE_HIT_EFFECT_TEXTS[index % PADDLE_HIT_EFFECT_TEXTS.size()];
}

std::string BuildSparkEffectText(uint32_t index)
{
  return BALL_TRAIL_EFFECT_TEXTS[index % BALL_TRAIL_EFFECT_TEXTS.size()];
}

float GetBrickRowWidth(uint32_t firstBrickIndex, uint32_t count)
{
  float width = 0.0f;
  for(uint32_t offset = 0u; offset < count; ++offset)
  {
    width += GetBrickTextWidth(BRICK_WORDS[firstBrickIndex + offset]);
  }

  if(count > 1u)
  {
    width += static_cast<float>(count - 1u) * BRICK_GAP_X;
  }
  return width;
}

Dali::String BuildBrickText(const char* word)
{
  return Dali::String(word);
}

Text::Outline BuildBrickOutline(const UiColor& color)
{
  return Text::Outline()
    .SetColor(color.WithAlpha(0.48f))
    .SetWidth(1.5f)
    .SetBlurRadius(1.6f);
}

const char* GetBrickRenderModeName(BrickRenderMode mode)
{
  return (mode == BrickRenderMode::LABEL) ? "LABEL" : "TEXT_VISUALIZER";
}

bool RectsOverlap(const Rect<float>& a, const Rect<float>& b)
{
  return a.x < b.x + b.width &&
         a.x + a.width > b.x &&
         a.y < b.y + b.height &&
         a.y + a.height > b.y;
}

float ClampFloat(float value, float minimum, float maximum)
{
  return std::max(minimum, std::min(value, maximum));
}

Rect<float> InflateRect(const Rect<float>& rect, float amount)
{
  return Rect<float>(rect.x - amount,
                     rect.y - amount,
                     rect.width + (amount * 2.0f),
                     rect.height + (amount * 2.0f));
}

Rect<float> BallBounds(const Vector2& center)
{
  return Rect<float>(center.x - BALL_RADIUS,
                     center.y - BALL_RADIUS,
                     BALL_RADIUS * 2.0f,
                     BALL_RADIUS * 2.0f);
}

} // namespace

class TextBreakerController : public ConnectionTracker
{
public:
  explicit TextBreakerController(Application& application)
  : mApplication(application),
    mRandomEngine(std::random_device{}())
  {
    mApplication.InitSignal().Connect(this, &TextBreakerController::OnInit);
  }

private:
  struct Brick
  {
    View        text;
    Rect<float> textBounds;
    Rect<float> exclusionBounds;
    std::string word;
    bool        alive{true};
    UiColor     color;
  };

  struct TextFragment
  {
    TextVisualizer text;
    Vector2        position{Vector2::ZERO};
    Vector2        velocity{Vector2::ZERO};
    float          width{0.0f};
    float          height{24.0f};
    float          age{0.0f};
    float          life{FRAGMENT_DURATION};
    bool           affectsExclusion{false};
    float          exclusionScale{0.0f};
  };

  struct Pulse
  {
    Rect<float> bounds;
    float       age{0.0f};
    float       life{BRICK_PULSE_DURATION};
  };

  Rect<float> GetBrickExclusionBounds(const Brick& brick) const
  {
    return brick.exclusionBounds;
  }

  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(WINDOW_BG_COLOR);
    window.SetSize(Dali::Window::WindowSize(static_cast<uint32_t>(WINDOW_WIDTH), static_cast<uint32_t>(WINDOW_HEIGHT)));
    window.KeyEventSignal().Connect(this, &TextBreakerController::OnKeyEvent);

    CreateScene(window);
    ResetGame();

    mTimer = Timer::New(TIMER_INTERVAL_MS);
    mTimer.TickSignal().Connect(this, &TextBreakerController::OnTimerTick);
    mTimer.Start();
  }

  void CreateScene(Window& window)
  {
    mRoot = View::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(WINDOW_BG_COLOR);

    mBackgroundText = TextVisualizer::New();
    mBackgroundText.SetPositionX(BACKGROUND_TEXT_LEFT);
    mBackgroundText.SetPositionY(BACKGROUND_TEXT_TOP);
    mBackgroundText.SetRequestedWidth(BACKGROUND_TEXT_WIDTH);
    mBackgroundText.SetRequestedHeight(BACKGROUND_TEXT_HEIGHT);
    mBackgroundText.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mBackgroundText.SetText(BuildBackgroundText());
    mBackgroundText.SetFontFamily(SAMPLE_FONT_FAMILY);
    mBackgroundText.SetFontSize(BACKGROUND_FONT_SIZE);
    mBackgroundText.SetLineHeight(1.24f);
    mBackgroundText.SetTextColor(BACKGROUND_TEXT_COLOR);

    mPlayFrame = View::New();
    mPlayFrame.SetLayoutMode(LayoutMode::STANDALONE);
    mPlayFrame.SetPositionX(PLAY_LEFT);
    mPlayFrame.SetPositionY(PLAY_TOP);
    mPlayFrame.SetRequestedWidth(PLAY_WIDTH);
    mPlayFrame.SetRequestedHeight(PLAY_HEIGHT);
    mPlayFrame.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mPlayFrame.SetBorderlineWidth(1.0f);
    mPlayFrame.SetBorderlineColor(PLAY_BORDER_COLOR);

    mRoot.Add(mBackgroundText);
    mRoot.Add(mPlayFrame);

    CreateBricks();
    CreateBallAndPaddle();
    CreateStatusText();

    window.Add(mRoot);
  }

  void CreateBricks()
  {
    ClearBricks();
    mBricks.reserve(BRICK_COUNT);

    uint32_t brickIndex = 0u;

    for(uint32_t row = 0u; row < BRICK_ROWS; ++row)
    {
      const uint32_t rowBrickCount = BRICK_ROW_COUNTS[row];
      const float    rowWidth      = GetBrickRowWidth(brickIndex, rowBrickCount);
      float          brickX        = (WINDOW_WIDTH - rowWidth) * 0.5f;
      const float    brickY        = BRICK_AREA_TOP + (static_cast<float>(row) * (BRICK_HEIGHT + BRICK_GAP_Y));

      for(uint32_t column = 0u; column < rowBrickCount; ++column)
      {
        const char* word           = BRICK_WORDS[brickIndex];
        const float textWidth      = GetBrickTextWidth(word);
        const float exclusionWidth = GetBrickExclusionWidth(word);

        const Rect<float> textBounds(brickX,
                                     brickY,
                                     textWidth,
                                     BRICK_HEIGHT);
        const Rect<float> exclusionBounds(textBounds.x + ((textBounds.width - exclusionWidth) * 0.5f),
                                          brickY,
                                          exclusionWidth,
                                          BRICK_HEIGHT);

        Brick brick;
        brick.textBounds      = textBounds;
        brick.exclusionBounds = exclusionBounds;
        brick.word            = word;
        brick.color           = BRICK_COLORS[brickIndex % BRICK_COLORS.size()].WithAlpha(0.95f);

        if(mBrickRenderMode == BrickRenderMode::LABEL)
        {
          Label label = Label::New();
          ConfigureBrickLabel(label, word, textBounds, brick.color);
          brick.text = label;
          mRoot.Add(label);
        }
        else
        {
          TextVisualizer textVisualizer = TextVisualizer::New();
          ConfigureBrickTextVisualizer(textVisualizer, word, textBounds, brick.color);
          brick.text = textVisualizer;
          mRoot.Add(textVisualizer);
        }

        mBricks.push_back(brick);

        brickX += textWidth + BRICK_GAP_X;
        ++brickIndex;
      }
    }
  }

  void ClearBricks()
  {
    for(Brick& brick : mBricks)
    {
      if(brick.text)
      {
        brick.text.Unparent();
      }
    }
    mBricks.clear();
  }

  void ConfigureBrickCommon(View actor, const Rect<float>& textBounds)
  {
    actor.SetLayoutMode(LayoutMode::STANDALONE);
    actor.SetPositionX(textBounds.x);
    actor.SetPositionY(textBounds.y);
    actor.SetRequestedWidth(textBounds.width);
    actor.SetRequestedHeight(textBounds.height);
    actor.SetBackgroundColor(UiColor(Color::TRANSPARENT));
  }

  void ConfigureBrickLabel(Label& label, const char* word, const Rect<float>& textBounds, const UiColor& color)
  {
    ConfigureBrickCommon(label, textBounds);
    label.SetText(BuildBrickText(word));
    label.SetFontFamily(SAMPLE_FONT_FAMILY);
    label.SetFontSize(BRICK_FONT_SIZE);
    label.SetLineHeight(1.0f);
    label.SetFontWeight(Text::FontWeight::BOLD);
    label.SetTextColor(color);
    label.SetOutline(BuildBrickOutline(color));
    label.SetAsyncRendering(true);
  }

  void ConfigureBrickTextVisualizer(TextVisualizer& textVisualizer, const char* word, const Rect<float>& textBounds, const UiColor& color)
  {
    ConfigureBrickCommon(textVisualizer, textBounds);
    textVisualizer.SetText(BuildBrickText(word));
    textVisualizer.SetFontFamily(SAMPLE_FONT_FAMILY);
    textVisualizer.SetFontSize(BRICK_FONT_SIZE);
    textVisualizer.SetLineHeight(1.0f);
    textVisualizer.SetTextColor(color);
  }

  void CreateBallAndPaddle()
  {
    mBallView = View::New();
    mBallView.SetLayoutMode(LayoutMode::STANDALONE);
    mBallView.SetRequestedWidth(BALL_RADIUS * 2.0f);
    mBallView.SetRequestedHeight(BALL_RADIUS * 2.0f);
    mBallView.SetBackgroundColor(BALL_COLOR);
    mBallView.SetCornerRadiusPolicyRelative();
    mBallView.SetCornerRadius(0.5f);
    mBallView.SetBorderlineWidth(1.5f);
    mBallView.SetBorderlineColor(UiColor(0xFFFFFF).WithAlpha(0.65f));

    mPaddleText = TextVisualizer::New();
    mPaddleText.SetLayoutMode(LayoutMode::STANDALONE);
    mPaddleText.SetRequestedWidth(PADDLE_WIDTH);
    mPaddleText.SetRequestedHeight(PADDLE_HEIGHT);
    mPaddleText.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mPaddleText.SetText(PADDLE_TEXT);
    mPaddleText.SetFontFamily(SAMPLE_FONT_FAMILY);
    mPaddleText.SetFontSize(PADDLE_FONT_SIZE);
    mPaddleText.SetLineHeight(0.65f);
    mPaddleText.SetTextColor(PADDLE_COLOR);

    mRoot.Add(mBallView);
    mRoot.Add(mPaddleText);
  }

  void CreateStatusText()
  {
    mStatusText = TextVisualizer::New();
    mStatusText.SetLayoutMode(LayoutMode::STANDALONE);
    mStatusText.SetPositionX(PLAY_LEFT);
    mStatusText.SetPositionY(WINDOW_HEIGHT - 44.0f);
    mStatusText.SetRequestedWidth(PLAY_WIDTH);
    mStatusText.SetRequestedHeight(32.0f);
    mStatusText.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    mStatusText.SetFontFamily(SAMPLE_FONT_FAMILY);
    mStatusText.SetFontSize(14.0f);
    mStatusText.SetLineHeight(1.0f);
    mStatusText.SetTextColor(STATUS_TEXT_COLOR);
    mRoot.Add(mStatusText);
  }

  void ResetGame()
  {
    mStartTime    = std::chrono::steady_clock::now();
    mLastTickTime = mStartTime;
    mScore        = 0u;
    mFrameCount   = 0u;
    mBallCenter   = Vector2(WINDOW_WIDTH * 0.50f, WINDOW_HEIGHT * 0.64f);
    mBallVelocity = Normalize(Vector2(0.76f, -1.0f)) * BALL_SPEED;
    mPaddleX      = (WINDOW_WIDTH - PADDLE_WIDTH) * 0.5f;

    for(Brick& brick : mBricks)
    {
      brick.alive = true;
      brick.text.SetProperty(Actor::Property::VISIBLE, true);
      brick.text.SetOpacity(1.0f);
      brick.text.SetPositionX(brick.textBounds.x);
      brick.text.SetPositionY(brick.textBounds.y);
    }

    mBrickBumpActive = false;
    mBrickBumpAge    = 0.0f;
    ClearEffects();
    UpdateBallAndPaddleActors();
    ApplyExclusionRegions();
    UpdateStatusText(true);
  }

  void ClearEffects()
  {
    for(TextFragment& fragment : mFragments)
    {
      if(fragment.text)
      {
        fragment.text.Unparent();
      }
    }
    mFragments.clear();
    mPulses.clear();
  }

  Vector2 Normalize(const Vector2& value) const
  {
    const float length = std::sqrt((value.x * value.x) + (value.y * value.y));
    if(length <= 0.0001f)
    {
      return Vector2(1.0f, -1.0f);
    }
    return Vector2(value.x / length, value.y / length);
  }

  bool OnTimerTick()
  {
    const auto  now          = std::chrono::steady_clock::now();
    const float deltaSeconds = ClampFloat(std::chrono::duration<float>(now - mLastTickTime).count(), MIN_DELTA_SECONDS, MAX_DELTA_SECONDS);
    mLastTickTime            = now;
    ++mFrameCount;

    const Vector2 previousBallCenter = mBallCenter;
    UpdatePaddle(deltaSeconds);
    UpdateBall(deltaSeconds);
    UpdateEffects(deltaSeconds);
    SpawnBallTrailSparks(previousBallCenter);
    UpdateBallAndPaddleActors();
    ApplyExclusionRegions();

    if((mFrameCount % STATUS_INTERVAL) == 0u)
    {
      UpdateStatusText(false);
    }

    return true;
  }

  void UpdatePaddle(float deltaSeconds)
  {
    const float manualDirection = (mMoveRight ? 1.0f : 0.0f) - (mMoveLeft ? 1.0f : 0.0f);

    if(std::abs(manualDirection) > 0.0f)
    {
      mPaddleX += manualDirection * PADDLE_SPEED * deltaSeconds;
    }
    else if(mAutoplayEnabled)
    {
      const float targetX = mBallCenter.x - (PADDLE_WIDTH * 0.5f);
      const float deltaX  = targetX - mPaddleX;
      const float step    = PADDLE_AUTO_SPEED * deltaSeconds;
      mPaddleX += ClampFloat(deltaX, -step, step);
    }

    mPaddleX = ClampFloat(mPaddleX, PLAY_LEFT, PLAY_RIGHT - PADDLE_WIDTH);
  }

  void UpdateBall(float deltaSeconds)
  {
    mBallCenter += mBallVelocity * deltaSeconds;

    if(mBallCenter.x - BALL_RADIUS <= PLAY_LEFT)
    {
      mBallCenter.x     = PLAY_LEFT + BALL_RADIUS;
      mBallVelocity.x   = std::abs(mBallVelocity.x);
    }
    else if(mBallCenter.x + BALL_RADIUS >= PLAY_RIGHT)
    {
      mBallCenter.x     = PLAY_RIGHT - BALL_RADIUS;
      mBallVelocity.x   = -std::abs(mBallVelocity.x);
    }

    if(mBallCenter.y - BALL_RADIUS <= PLAY_TOP)
    {
      mBallCenter.y   = PLAY_TOP + BALL_RADIUS;
      mBallVelocity.y = std::abs(mBallVelocity.y);
    }

    HandlePaddleCollision();
    HandleBrickCollision();

    if(mBallCenter.y - BALL_RADIUS > WINDOW_HEIGHT)
    {
      mBallCenter   = Vector2(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.65f);
      mBallVelocity = Normalize(Vector2(0.65f, -1.0f)) * BALL_SPEED;
    }
  }

  void HandlePaddleCollision()
  {
    const Rect<float> ballBounds = BallBounds(mBallCenter);
    const Rect<float> paddleBounds = GetPaddleBounds();

    if(mBallVelocity.y > 0.0f && RectsOverlap(ballBounds, paddleBounds))
    {
      const float paddleCenterX = paddleBounds.x + (paddleBounds.width * 0.5f);
      const float hit           = ClampFloat((mBallCenter.x - paddleCenterX) / (paddleBounds.width * 0.5f), -1.0f, 1.0f);
      mBallCenter.y             = paddleBounds.y - BALL_RADIUS - 1.0f;
      mBallVelocity             = Normalize(Vector2(hit * 0.85f, -1.0f)) * BALL_SPEED;
      SpawnPaddleHitEffect(Vector2(mBallCenter.x, paddleBounds.y + paddleBounds.height));
    }
  }

  void SpawnPaddleHitEffect(const Vector2& source)
  {
    if(!mEffectsEnabled)
    {
      return;
    }

    std::uniform_real_distribution<float> xOffsetDistribution(-42.0f, 42.0f);
    std::uniform_real_distribution<float> xVelocityDistribution(-54.0f, 54.0f);
    std::uniform_real_distribution<float> yVelocityDistribution(-188.0f, -108.0f);

    for(uint32_t index = 0u; index < PADDLE_HIT_FRAGMENT_COUNT; ++index)
    {
      const Vector2 origin(source.x + xOffsetDistribution(mRandomEngine), source.y + 2.0f);
      AddTextFragmentCentered(BuildPaddleHitEffectText(index),
                              PADDLE_COLOR.WithAlpha(0.95f),
                              origin,
                              Vector2(xVelocityDistribution(mRandomEngine), yVelocityDistribution(mRandomEngine)),
                              PADDLE_HIT_FRAGMENT_FONT_SIZE,
                              FRAGMENT_DURATION,
                              PADDLE_HIT_FRAGMENT_EXCLUSION_SCALE,
                              26.0f,
                              24.0f);
    }
  }

  void HandleBrickCollision()
  {
    const Rect<float> ballBounds = BallBounds(mBallCenter);

    for(Brick& brick : mBricks)
    {
      if(!brick.alive || !RectsOverlap(ballBounds, brick.exclusionBounds))
      {
        continue;
      }

      DestroyBrick(brick);
      ReflectBallFromBrick(brick.exclusionBounds);
      return;
    }
  }

  void ReflectBallFromBrick(const Rect<float>& brickBounds)
  {
    const float brickCenterX = brickBounds.x + (brickBounds.width * 0.5f);
    const float brickCenterY = brickBounds.y + (brickBounds.height * 0.5f);
    const float dx           = (mBallCenter.x - brickCenterX) / (brickBounds.width * 0.5f);
    const float dy           = (mBallCenter.y - brickCenterY) / (brickBounds.height * 0.5f);

    if(std::abs(dx) > std::abs(dy))
    {
      mBallVelocity.x = (dx > 0.0f) ? std::abs(mBallVelocity.x) : -std::abs(mBallVelocity.x);
    }
    else
    {
      mBallVelocity.y = (dy > 0.0f) ? std::abs(mBallVelocity.y) : -std::abs(mBallVelocity.y);
    }
  }

  void DestroyBrick(Brick& brick)
  {
    brick.alive = false;
    brick.text.SetProperty(Actor::Property::VISIBLE, false);
    ++mScore;

    SpawnBrickDestroyEffect(brick);
    mPulses.push_back(Pulse{InflateRect(brick.exclusionBounds, 2.0f), 0.0f, BRICK_PULSE_DURATION});
    StartBrickBump();

    if(GetAliveBrickCount() == 0u)
    {
      ResetGame();
    }
  }

  void SpawnBrickDestroyEffect(const Brick& brick)
  {
    if(!mEffectsEnabled)
    {
      return;
    }

    std::uniform_real_distribution<float> angleDistribution(0.0f, PI * 2.0f);
    std::uniform_real_distribution<float> speedDistribution(95.0f, 210.0f);

    const uint32_t fragmentCount = GetBrickEffectFragmentCount(brick.word.c_str());
    const Vector2  sourceCenter(brick.textBounds.x + (brick.textBounds.width * 0.5f),
                                brick.textBounds.y + (brick.textBounds.height * 0.5f));

    for(uint32_t index = 0u; index < fragmentCount; ++index)
    {
      const float angle = angleDistribution(mRandomEngine);
      const float speed = speedDistribution(mRandomEngine);
      AddTextFragmentCentered(BuildLetterEffectText(brick.word.c_str(), index),
                              brick.color,
                              sourceCenter,
                              Vector2(std::cos(angle) * speed, std::sin(angle) * speed),
                              BRICK_FRAGMENT_FONT_SIZE,
                              FRAGMENT_DURATION,
                              BRICK_FRAGMENT_EXCLUSION_SCALE,
                              26.0f,
                              24.0f);
    }
  }

  void AddTextFragmentCentered(const std::string& text, const UiColor& color, const Vector2& center, const Vector2& velocity, float fontSize, float life, float exclusionScale, float minimumWidth, float minimumHeight)
  {
    if(text.empty())
    {
      return;
    }

    const float textWidth  = std::max(minimumWidth, GetMonospaceTextWidth(text.c_str(), fontSize) + 8.0f);
    const float textHeight = std::max(minimumHeight, fontSize * 1.35f);

    TextFragment fragment;
    fragment.position = Vector2(center.x - (textWidth * 0.5f),
                                center.y - (textHeight * 0.5f));
    fragment.velocity         = velocity;
    fragment.width            = textWidth;
    fragment.height           = textHeight;
    fragment.life             = life;
    fragment.affectsExclusion = exclusionScale > 0.0f;
    fragment.exclusionScale   = exclusionScale;

    fragment.text = TextVisualizer::New();
    fragment.text.SetLayoutMode(LayoutMode::STANDALONE);
    fragment.text.SetPositionX(fragment.position.x);
    fragment.text.SetPositionY(fragment.position.y);
    fragment.text.SetRequestedWidth(fragment.width);
    fragment.text.SetRequestedHeight(fragment.height);
    fragment.text.SetBackgroundColor(UiColor(Color::TRANSPARENT));
    fragment.text.SetText(text.c_str());
    fragment.text.SetFontFamily(SAMPLE_FONT_FAMILY);
    fragment.text.SetFontSize(fontSize);
    fragment.text.SetLineHeight(1.0f);
    fragment.text.SetTextColor(color);

    mRoot.Add(fragment.text);
    mFragments.push_back(fragment);
  }

  void UpdateEffects(float deltaSeconds)
  {
    UpdateBrickBump(deltaSeconds);

    for(TextFragment& fragment : mFragments)
    {
      fragment.age += deltaSeconds;
      fragment.position += fragment.velocity * deltaSeconds;
      fragment.velocity.y += 180.0f * deltaSeconds;

      const float normalized = ClampFloat(fragment.age / fragment.life, 0.0f, 1.0f);
      fragment.text.SetPositionX(fragment.position.x);
      fragment.text.SetPositionY(fragment.position.y);
      fragment.text.SetOpacity(1.0f - normalized);
    }

    mFragments.erase(
      std::remove_if(mFragments.begin(),
                     mFragments.end(),
                     [](TextFragment& fragment)
                     {
                       if(fragment.age >= fragment.life)
                       {
                         if(fragment.text)
                         {
                           fragment.text.Unparent();
                         }
                         return true;
                       }
                       return false;
                     }),
      mFragments.end());

    for(Pulse& pulse : mPulses)
    {
      pulse.age += deltaSeconds;
    }

    mPulses.erase(
      std::remove_if(mPulses.begin(),
                     mPulses.end(),
                     [](const Pulse& pulse)
                     {
                       return pulse.age >= pulse.life;
                     }),
      mPulses.end());
  }

  void StartBrickBump()
  {
    mBrickBumpAge    = 0.0f;
    mBrickBumpActive = true;
  }

  void UpdateBrickBump(float deltaSeconds)
  {
    if(!mBrickBumpActive)
    {
      return;
    }

    mBrickBumpAge += deltaSeconds;
    const float normalized = ClampFloat(mBrickBumpAge / BRICK_BUMP_DURATION, 0.0f, 1.0f);
    const float offsetY    = -std::sin(normalized * PI) * BRICK_BUMP_DISTANCE;

    for(Brick& brick : mBricks)
    {
      if(brick.text)
      {
        brick.text.SetPositionY(brick.textBounds.y + offsetY);
      }
    }

    if(normalized >= 1.0f)
    {
      mBrickBumpActive = false;
      for(Brick& brick : mBricks)
      {
        if(brick.text)
        {
          brick.text.SetPositionY(brick.textBounds.y);
        }
      }
    }
  }

  void SpawnBallTrailSparks(const Vector2& previousBallCenter)
  {
    if(!mEffectsEnabled || (mFrameCount % BALL_TRAIL_SPARK_INTERVAL) != 0u)
    {
      return;
    }

    std::uniform_real_distribution<float> offsetDistribution(-8.0f, 8.0f);
    std::uniform_real_distribution<float> driftDistribution(-42.0f, 42.0f);
    std::uniform_real_distribution<float> speedDistribution(32.0f, 92.0f);

    const Vector2 backward      = Normalize(Vector2(-mBallVelocity.x, -mBallVelocity.y));
    const Vector2 perpendicular = Vector2(-backward.y, backward.x);

    for(uint32_t index = 0u; index < BALL_TRAIL_SPARK_COUNT; ++index)
    {
      const Vector2 origin = previousBallCenter +
                             (backward * speedDistribution(mRandomEngine) * 0.11f) +
                             (perpendicular * offsetDistribution(mRandomEngine));
      AddTextFragmentCentered(BuildSparkEffectText(mFrameCount + index),
                              BALL_TRAIL_COLOR,
                              origin,
                              (backward * speedDistribution(mRandomEngine)) + (perpendicular * driftDistribution(mRandomEngine)),
                              BALL_TRAIL_SPARK_FONT_SIZE,
                              BALL_TRAIL_SPARK_DURATION,
                              BALL_TRAIL_EXCLUSION_SCALE,
                              18.0f,
                              18.0f);
    }
  }

  void UpdateBallAndPaddleActors()
  {
    mBallView.SetPositionX(mBallCenter.x - BALL_RADIUS);
    mBallView.SetPositionY(mBallCenter.y - BALL_RADIUS);
    mPaddleText.SetPositionX(mPaddleX);
    mPaddleText.SetPositionY(PADDLE_Y);
  }

  Rect<float> GetPaddleBounds() const
  {
    return Rect<float>(mPaddleX, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
  }

  Rect<float> ToBackgroundLocalRect(const Rect<float>& windowRect) const
  {
    return Rect<float>(windowRect.x - BACKGROUND_TEXT_LEFT,
                       windowRect.y - BACKGROUND_TEXT_TOP,
                       windowRect.width,
                       windowRect.height);
  }

  void ApplyExclusionRegions()
  {
    if(!mExclusionEnabled)
    {
      mBackgroundText.ClearExclusionRegions();
      return;
    }

    mExclusionRegions.Clear();
    mExclusionRegions.Reserve(BRICK_COUNT +
                               BALL_EXCLUSION_BANDS +
                               static_cast<uint32_t>(mFragments.size()) +
                               4u +
                               static_cast<uint32_t>(mPulses.size()));

    for(const Brick& brick : mBricks)
    {
      if(brick.alive)
      {
        mExclusionRegions.PushBack(ToBackgroundLocalRect(GetBrickExclusionBounds(brick)));
      }
    }

    AppendCircularExclusionRegions(mExclusionRegions, mBallCenter, BALL_EXCLUSION_RADIUS);
    AppendFragmentExclusionRegions(mExclusionRegions);
    mExclusionRegions.PushBack(ToBackgroundLocalRect(GetPaddleBounds()));

    for(const Pulse& pulse : mPulses)
    {
      const float normalized = ClampFloat(pulse.age / pulse.life, 0.0f, 1.0f);
      mExclusionRegions.PushBack(ToBackgroundLocalRect(InflateRect(pulse.bounds, normalized * 24.0f)));
    }

    mBackgroundText.SetExclusionRegions(mExclusionRegions);
  }

  void AppendFragmentExclusionRegions(Dali::Vector<Rect<float>>& regions) const
  {
    for(const TextFragment& fragment : mFragments)
    {
      if(!fragment.affectsExclusion)
      {
        continue;
      }

      const Vector2 center(fragment.position.x + (fragment.width * 0.5f),
                           fragment.position.y + (fragment.height * 0.5f));
      const float width  = fragment.width * fragment.exclusionScale;
      const float height = fragment.height * fragment.exclusionScale;
      regions.PushBack(ToBackgroundLocalRect(Rect<float>(center.x - (width * 0.5f),
                                                         center.y - (height * 0.5f),
                                                         width,
                                                         height)));
    }
  }

  void AppendCircularExclusionRegions(Dali::Vector<Rect<float>>& regions, const Vector2& center, float radius) const
  {
    const Rect<float> bounds(center.x - radius,
                             center.y - radius,
                             radius * 2.0f,
                             radius * 2.0f);
    const float       bandHeight = bounds.height / static_cast<float>(BALL_EXCLUSION_BANDS);
    const float       radiusX    = bounds.width * 0.5f;
    const float       radiusY    = bounds.height * 0.5f;
    const float       centerX    = bounds.x + radiusX;
    const float       centerY    = bounds.y + radiusY;

    for(uint32_t bandIndex = 0u; bandIndex < BALL_EXCLUSION_BANDS; ++bandIndex)
    {
      const float bandTop     = bounds.y + (static_cast<float>(bandIndex) * bandHeight);
      const float bandCenterY = bandTop + (bandHeight * 0.5f);
      const float normalizedY = (radiusY > 0.0f) ? ((bandCenterY - centerY) / radiusY) : 0.0f;
      const float xScale      = std::sqrt(std::max(0.0f, 1.0f - (normalizedY * normalizedY)));
      const float bandWidth   = bounds.width * xScale;
      const float bandLeft    = centerX - (bandWidth * 0.5f);

      regions.PushBack(ToBackgroundLocalRect(Rect<float>(bandLeft - EXCLUSION_PADDING,
                                                         bandTop - EXCLUSION_PADDING,
                                                         bandWidth + (EXCLUSION_PADDING * 2.0f),
                                                         bandHeight + (EXCLUSION_PADDING * 2.0f))));
    }
  }

  uint32_t GetAliveBrickCount() const
  {
    uint32_t count = 0u;
    for(const Brick& brick : mBricks)
    {
      if(brick.alive)
      {
        ++count;
      }
    }
    return count;
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

    std::ostringstream builder;
    builder.setf(std::ios::fixed);
    builder.precision(1);
    builder << "TEXT BREAKER   FPS " << fps
            << "   SCORE " << mScore
            << "   BRICKS " << GetAliveBrickCount()
            << "   BRICK_RENDER " << GetBrickRenderModeName(mBrickRenderMode)
            << "   EXCLUSION " << (mExclusionEnabled ? "ON" : "OFF")
            << "   EFFECTS " << (mEffectsEnabled ? "ON" : "OFF")
            << "   1 reset  2 exclusion  3 effects  4 brick  0 autoplay  arrows/A/D paddle";

    mStatusText.SetText(builder.str().c_str());
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      if(event.GetState() == KeyEvent::UP)
      {
        mApplication.Quit();
      }
      return;
    }

    const std::string keyName = event.GetKeyName().CStr();
    const bool        pressed = event.GetState() != KeyEvent::UP;

    if(keyName == "Left" || keyName == "Left Arrow" || keyName == "a" || keyName == "A")
    {
      mMoveLeft = pressed;
      return;
    }

    if(keyName == "Right" || keyName == "Right Arrow" || keyName == "d" || keyName == "D")
    {
      mMoveRight = pressed;
      return;
    }

    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(keyName == "1" || keyName == "r" || keyName == "R")
    {
      ResetGame();
    }
    else if(keyName == "2")
    {
      mExclusionEnabled = !mExclusionEnabled;
      ApplyExclusionRegions();
      UpdateStatusText(true);
    }
    else if(keyName == "3")
    {
      mEffectsEnabled = !mEffectsEnabled;
      UpdateStatusText(true);
    }
    else if(keyName == "4")
    {
      ToggleBrickRenderMode();
    }
    else if(keyName == "0")
    {
      mAutoplayEnabled = !mAutoplayEnabled;
      UpdateStatusText(true);
    }
  }

  void ToggleBrickRenderMode()
  {
    mBrickRenderMode = (mBrickRenderMode == BrickRenderMode::LABEL) ? BrickRenderMode::TEXT_VISUALIZER : BrickRenderMode::LABEL;
    CreateBricks();
    ResetGame();
  }

private:
  Application&                          mApplication;
  View                                  mRoot;
  View                                  mPlayFrame;
  TextVisualizer                        mBackgroundText;
  TextVisualizer                        mStatusText;
  View                                  mBallView;
  TextVisualizer                        mPaddleText;
  std::vector<Brick>                    mBricks;
  std::vector<TextFragment>             mFragments;
  std::vector<Pulse>                    mPulses;
  Dali::Vector<Rect<float>>             mExclusionRegions;
  Timer                                 mTimer;
  std::mt19937                          mRandomEngine;
  Vector2                               mBallCenter{Vector2::ZERO};
  Vector2                               mBallVelocity{Vector2::ZERO};
  float                                 mPaddleX{0.0f};
  bool                                  mMoveLeft{false};
  bool                                  mMoveRight{false};
  bool                                  mAutoplayEnabled{true};
  bool                                  mExclusionEnabled{true};
  bool                                  mEffectsEnabled{true};
  bool                                  mBrickBumpActive{false};
  float                                 mBrickBumpAge{0.0f};
  BrickRenderMode                       mBrickRenderMode{BrickRenderMode::LABEL};
  uint32_t                              mScore{0u};
  uint64_t                              mFrameCount{0u};
  std::chrono::steady_clock::time_point mStartTime;
  std::chrono::steady_clock::time_point mLastTickTime;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextBreakerController controller(application);
  application.MainLoop();

  return 0;
}
