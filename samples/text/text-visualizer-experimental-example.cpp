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

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <random>
#include <string>
#include <utility>
#include <vector>

// DALI
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/devel-api/ui-foundation-pre-initialize.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float    STACK_SPACING         = 10.0f;
constexpr float    STACK_PADDING         = 20.0f;
constexpr float    TITLE_FONT_SIZE       = 20.0f;
constexpr uint32_t TIMER_INTERVAL_MS     = 16;
constexpr float    COLOR_ANIM_DURATION   = 0.72f;
constexpr float    COLOR_FLICKER_FRAME_STEP = 3.0f;

constexpr float GLYPH_WIDTH              = 8.0f;
constexpr float GLYPH_HEIGHT             = 16.0f;

constexpr uint32_t TILE_GLYPH_COLS       = 20u;
constexpr uint32_t TILE_GLYPH_ROWS       = 10u;
constexpr float TILE_PIXEL_WIDTH         = TILE_GLYPH_COLS * GLYPH_WIDTH;
constexpr float TILE_PIXEL_HEIGHT        = TILE_GLYPH_ROWS * GLYPH_HEIGHT;

constexpr uint32_t COLOR_WHITE           = 0xFFFFFF;
constexpr uint32_t COLOR_BG              = 0x121212;
constexpr uint32_t COLOR_TEXT            = 0xEFEFEF;
constexpr uint32_t COLOR_BALL            = 0xFF6B4A;
constexpr uint32_t COLOR_INFO            = 0x404040;

constexpr float    GRID_WIDTH            = 1.0f;
constexpr uint32_t COLOR_GRID            = 0x7CFF00;
constexpr uint32_t COLOR_DIRTY_TILE      = 0xFF0000;
constexpr float    DIRTY_TILE_ALPHA      = 0.18f;

constexpr float TRAIL_DECAY              = 0.94f;
constexpr float IMPACT_DECAY             = 0.88f;
constexpr float IMPACT_STRENGTH          = 1.35f;
constexpr float IMPACT_RADIUS            = 96.0f;
constexpr float IMPACT_RING_THICKNESS    = 18.0f;
constexpr float COLLISION_IMPACT_STRENGTH = 1.95f;
constexpr float COLLISION_IMPACT_RADIUS   = 54.0f;
constexpr float COLLISION_RESTITUTION     = 1.03f;

constexpr float SPAWN_IMPACT_STRENGTH    = 1.9f;
constexpr float SPAWN_IMPACT_RADIUS      = 44.0f;

constexpr float SQUASH_DECAY             = 0.78f;
constexpr float SQUASH_STRENGTH          = 0.32f;
constexpr float METEOR_SHAPE_SPEED_MIN   = 1.2f;
constexpr float METEOR_SHAPE_SPEED_MAX   = 4.8f;
constexpr float METEOR_HEAD_LEAD         = 0.12f;
constexpr float METEOR_AXIS_STRETCH      = 0.16f;
constexpr float METEOR_AXIS_SQUEEZE      = 0.10f;
constexpr float METEOR_TAIL_BOOST        = 0.36f;

constexpr float EDGE_NOISE_STRENGTH      = 0.18f;
constexpr float EDGE_NOISE_BAND_INNER    = 0.82f;
constexpr float EDGE_NOISE_BAND_OUTER    = 1.06f;

constexpr float TAIL_STRENGTH            = 0.24f;
constexpr float TAIL_BASE_LENGTH         = 0.92f;
constexpr float TAIL_SPEED_SCALE         = 0.16f;
constexpr float TAIL_HEAD_WIDTH          = 0.34f;
constexpr float TAIL_TIP_WIDTH           = 0.04f;
constexpr float TAIL_FADE_POWER          = 1.65f;
constexpr float TAIL_BODY_BULGE          = 0.14f;
constexpr float TAIL_BODY_CENTER         = 0.30f;
constexpr float TAIL_BODY_SPREAD         = 0.24f;
constexpr float TAIL_WIDTH_POWER         = 1.55f;
constexpr float TAIL_HAZE_WIDTH_SCALE    = 1.75f;
constexpr float TAIL_HAZE_LENGTH_SCALE   = 0.82f;
constexpr float TAIL_HAZE_STRENGTH       = 0.11f;
constexpr float TAIL_HAZE_SOFTNESS       = 0.32f;

constexpr float TRAIL_SHIMMER_STRENGTH   = 0.32f;
constexpr float TAIL_SPARK_THRESHOLD     = 0.52f;
constexpr float TAIL_SPARK_BONUS         = 0.30f;

constexpr float PLASMA_ZONE_LENGTH       = 0.30f;
constexpr float PLASMA_WIDTH_SCALE       = 0.50f;
constexpr float PLASMA_STRENGTH          = 0.14f;
constexpr float PLASMA_FLICKER_SPEED     = 1.90f;

constexpr size_t INITIAL_BALL_COUNT      = 3u;
constexpr size_t MAX_ADDED_BALL_COUNT    = 7u;
constexpr size_t MAX_TOTAL_BALL_COUNT    = INITIAL_BALL_COUNT + MAX_ADDED_BALL_COUNT;

constexpr float RANDOM_BALL_MIN_SIZE     = 50.0f;
constexpr float RANDOM_BALL_MAX_SIZE     = 200.0f;
constexpr float RANDOM_SPEED_MIN         = 1.6f;
constexpr float RANDOM_SPEED_MAX         = 4.8f;
constexpr float TWO_PI                   = 6.28318530718f;

constexpr char TRAIL_CHARS_CALM[]        = "   ...---===oooOO";
constexpr char TRAIL_CHARS_RIPPLE[]      = "   ..--==+ooO0Q";
constexpr char TRAIL_CHARS_DENSE[]       = "   ..::==+*oO0@";
constexpr char TRAIL_CHARS_ELECTRIC[]    = "  .`:-=+*xoO0#@";
constexpr char TAIL_CHARS[]              = "   .,:-~=+*oO0Q@";
constexpr char OVERLAP_CHARS[]           = "  .:+xXMW#";
constexpr char IMPACT_CHARS[]            = "  .,:-~=+*#%@";

const std::array<Vector4, 10u> COLOR_ANIM_PALETTE = {{
  Vector4(1.00f, 0.26f, 0.32f, 1.0f),
  Vector4(1.00f, 0.62f, 0.08f, 1.0f),
  Vector4(1.00f, 0.98f, 0.18f, 1.0f),
  Vector4(0.72f, 1.00f, 0.24f, 1.0f),
  Vector4(0.18f, 1.00f, 0.82f, 1.0f),
  Vector4(0.14f, 0.92f, 1.00f, 1.0f),
  Vector4(0.38f, 0.44f, 1.00f, 1.0f),
  Vector4(0.84f, 0.30f, 1.00f, 1.0f),
  Vector4(1.00f, 0.28f, 0.92f, 1.0f),
  Vector4(1.00f, 1.00f, 1.00f, 1.0f),
}};

struct BallInit
{
  float   size;
  Vector2 position;
  Vector2 velocity;
};

constexpr std::array<BallInit, INITIAL_BALL_COUNT> BALL_INITS = {{
  {100.0f, Vector2(40.0f, 40.0f),   Vector2(3.4f,  2.6f)},
  {150.0f, Vector2(220.0f, 120.0f), Vector2(-2.1f, 3.8f)},
  {200.0f, Vector2(420.0f, 80.0f),  Vector2(4.2f, -1.9f)},
}};
} // namespace

class TextVisualizerExperimentalController : public ConnectionTracker
{
public:
  explicit TextVisualizerExperimentalController(Application& application)
  : mApplication(application),
    mRandomEngine(std::random_device{}())
  {
    mApplication.InitSignal().Connect(this, &TextVisualizerExperimentalController::OnInit);

    mBalls.reserve(MAX_TOTAL_BALL_COUNT);

    for(size_t i = 0u; i < BALL_INITS.size(); ++i)
    {
      BallState ball;
      ball.size     = BALL_INITS[i].size;
      ball.position = BALL_INITS[i].position;
      ball.velocity = BALL_INITS[i].velocity;
      ball.style    = DefaultStyleForIndex(i);
      ball.glyphSet = GetGlyphSetForStyle(ball.style);
      mBalls.push_back(ball);
    }
  }

private:
  enum class BallStyle : uint8_t
  {
    Calm = 0,
    Ripple,
    Dense,
    Electric
  };

  enum class NoiseQuality : uint8_t
  {
    FULL = 0,
    SIMPLIFIED
  };

  enum class RenderMode : uint8_t
  {
    SINGLE_TEXT_VISUALIZER = 0,
    PARTIAL_TEXT_VISUALIZER_TILES
  };

  struct BallState
  {
    float       size       = 0.0f;
    Vector2     position   = Vector2::ZERO;
    Vector2     velocity   = Vector2::ZERO;
    float       squashX    = 0.0f;
    float       squashY    = 0.0f;
    float       spawnBoost = 0.0f;
    BallStyle   style      = BallStyle::Calm;
    const char* glyphSet   = nullptr;
  };

  struct BallRenderData
  {
    float   centerX    = 0.0f;
    float   centerY    = 0.0f;
    float   baseRadius = 0.0f;
    float   radiusX    = 0.0f;
    float   radiusY    = 0.0f;
    float   speed      = 0.0f;
    float   dirX       = 0.0f;
    float   dirY       = 0.0f;
    float   tailLength = 0.0f;
    int32_t minCellX   = 0;
    int32_t maxCellX   = -1;
    int32_t minCellY   = 0;
    int32_t maxCellY   = -1;
  };

  struct Impact
  {
    Vector2 center   = Vector2::ZERO;
    float   radius   = 0.0f;
    float   strength = 0.0f;
    bool    active   = false;
  };

  struct Tile
  {
    TextVisualizer textVisualizer;
    uint32_t    tileX       = 0u;
    uint32_t    tileY       = 0u;
    uint32_t    glyphCols   = 0u;
    uint32_t    glyphRows   = 0u;
    uint64_t    previousHash = 0u;
    std::string previousText;
    std::string workingText;
    std::string starText;
  };

  struct ColorTagCache
  {
    const char* color = nullptr;
    std::string openTag;
  };

  struct DirtyRegion
  {
    int32_t minX = 0;
    int32_t minY = 0;
    int32_t maxX = -1;
    int32_t maxY = -1;
  };

private: // ---------- background ----------
  void BuildTileStarText(Tile& tile)
  {
    tile.starText.clear();
    tile.starText.reserve(static_cast<size_t>(tile.glyphRows) * static_cast<size_t>(tile.glyphCols + 1u));

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for(uint32_t y = 0u; y < tile.glyphRows; ++y)
    {
      for(uint32_t x = 0u; x < tile.glyphCols; ++x)
      {
        const float r = dist(mRandomEngine);

        char ch = ' ';
        if(r < 0.010f)
        {
          ch = '.';
        }
        else if(r < 0.0125f)
        {
          ch = '+';
        }
        else if(r < 0.0135f)
        {
          ch = '*';
        }

        tile.starText += ch;
      }

      if(y + 1u < tile.glyphRows)
      {
        tile.starText += '\n';
      }
    }
  }

private: // ---------- basic helpers ----------
  static float Clamp01(float value)
  {
    return std::max(0.0f, std::min(value, 1.0f));
  }

  uint32_t ClampCellX(int32_t x, uint32_t cols) const
  {
    if(cols == 0u)
    {
      return 0u;
    }
    if(x < 0)
    {
      return 0u;
    }
    if(static_cast<uint32_t>(x) >= cols)
    {
      return cols - 1u;
    }
    return static_cast<uint32_t>(x);
  }

  uint32_t ClampCellY(int32_t y, uint32_t rows) const
  {
    if(rows == 0u)
    {
      return 0u;
    }
    if(y < 0)
    {
      return 0u;
    }
    if(static_cast<uint32_t>(y) >= rows)
    {
      return rows - 1u;
    }
    return static_cast<uint32_t>(y);
  }

  size_t IndexOf(uint32_t x, uint32_t y, uint32_t cols) const
  {
    return static_cast<size_t>(y) * static_cast<size_t>(cols) + static_cast<size_t>(x);
  }

private: // ---------- style / option helpers ----------
  BallStyle DefaultStyleForIndex(size_t index) const
  {
    switch(index)
    {
      case 0u: return BallStyle::Calm;
      case 1u: return BallStyle::Ripple;
      default: return BallStyle::Dense;
    }
  }

  const char* GetGlyphSetForStyle(BallStyle style) const
  {
    switch(style)
    {
      case BallStyle::Calm:
        return TRAIL_CHARS_CALM;
      case BallStyle::Ripple:
        return TRAIL_CHARS_RIPPLE;
      case BallStyle::Dense:
        return TRAIL_CHARS_DENSE;
      case BallStyle::Electric:
      default:
        return TRAIL_CHARS_ELECTRIC;
    }
  }

  const char* GetNoiseQualityName() const
  {
    return (mNoiseQuality == NoiseQuality::FULL) ? "FULL" : "SIMPLIFIED";
  }

  const char* GetRenderModeName() const
  {
    return (mRenderMode == RenderMode::SINGLE_TEXT_VISUALIZER)
             ? "SINGLE"
             : "PARTIAL_TILES";
  }

  float GetStyleEdgeNoiseStrength(BallStyle style) const
  {
    switch(style)
    {
      case BallStyle::Calm:
        return EDGE_NOISE_STRENGTH * 0.35f;
      case BallStyle::Ripple:
        return EDGE_NOISE_STRENGTH * 0.55f;
      case BallStyle::Dense:
        return EDGE_NOISE_STRENGTH * 0.25f;
      case BallStyle::Electric:
      default:
        return EDGE_NOISE_STRENGTH * 1.10f;
    }
  }

  float GetStyleSquashStrength(BallStyle style) const
  {
    switch(style)
    {
      case BallStyle::Calm:
        return SQUASH_STRENGTH * 0.85f;
      case BallStyle::Ripple:
        return SQUASH_STRENGTH * 1.00f;
      case BallStyle::Dense:
        return SQUASH_STRENGTH * 0.65f;
      case BallStyle::Electric:
      default:
        return SQUASH_STRENGTH * 1.15f;
    }
  }

  Vector2 GetBallCenter(const BallState& ball) const
  {
    return Vector2(ball.position.x + ball.size * 0.5f, ball.position.y + ball.size * 0.5f);
  }

  UiColor GetClearTileColor() const
  {
    return UiColor(0x000000).WithAlpha(0.0f);
  }

  UiColor GetDirtyTileColor() const
  {
    return UiColor(COLOR_DIRTY_TILE).WithAlpha(DIRTY_TILE_ALPHA);
  }

  UiColor GetGridColor() const
  {
    return mBorderlineVisible ? UiColor(COLOR_GRID).WithAlpha(1.0f)
                              : UiColor(0x000000).WithAlpha(0.0f);
  }

private: // ---------- noise ----------
  float ComputeEdgeNoiseFull(float x, float y, float time, int seed) const
  {
    const float a = std::sin(x * 0.19f + y * 0.11f + time * 0.23f + seed * 1.7f);
    const float b = std::sin(x * 0.07f - y * 0.17f - time * 0.31f + seed * 2.3f);
    const float c = std::sin((x + y) * 0.05f + time * 0.41f + seed * 0.9f);
    return 0.5f + 0.5f * ((a + b + c) / 3.0f);
  }

  float ComputeEdgeNoiseSimplified(float x, float y, float time, int seed) const
  {
    const float a = std::sin(x * 0.11f + y * 0.07f + time * 0.19f + seed * 1.3f);
    const float b = std::sin((x + y) * 0.04f - time * 0.17f + seed * 0.8f);
    return 0.5f + 0.25f * (a + b);
  }

  float SampleNoise(float x, float y, float time, int seed) const
  {
    return (mNoiseQuality == NoiseQuality::FULL)
             ? ComputeEdgeNoiseFull(x, y, time, seed)
             : ComputeEdgeNoiseSimplified(x, y, time, seed);
  }

private: // ---------- ball raster helpers ----------
  float ComputeShapeDistance(BallStyle style, float nx, float ny, float noise) const
  {
    const float base = std::sqrt(nx * nx + ny * ny);

    switch(style)
    {
      case BallStyle::Calm:
        return base;
      case BallStyle::Ripple:
        return base + (noise - 0.5f) * 0.030f;
      case BallStyle::Dense:
        return base * 0.985f;
      case BallStyle::Electric:
      default:
        return base + (noise - 0.5f) * 0.055f;
    }
  }

  BallRenderData BuildBallRenderData(const BallState& ball, const Vector2& position) const
  {
    BallRenderData data;

    data.centerX    = position.x + ball.size * 0.5f;
    data.centerY    = position.y + ball.size * 0.5f;
    data.baseRadius = ball.size * 0.5f;

    const float spawnScale = 1.0f + ball.spawnBoost * 0.22f;
    data.radiusX           = data.baseRadius * (1.0f + ball.squashX) * spawnScale;
    data.radiusY           = data.baseRadius * (1.0f + ball.squashY) * spawnScale;

    data.speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);

    if(data.speed > 0.001f)
    {
      data.dirX = ball.velocity.x / data.speed;
      data.dirY = ball.velocity.y / data.speed;
    }

    if(data.speed > METEOR_SHAPE_SPEED_MIN)
    {
      const float speedT = Clamp01((data.speed - METEOR_SHAPE_SPEED_MIN) /
                                   std::max(METEOR_SHAPE_SPEED_MAX - METEOR_SHAPE_SPEED_MIN, 0.0001f));

      float styleMeteor = 1.0f;
      switch(ball.style)
      {
        case BallStyle::Calm:
          styleMeteor = 0.82f;
          break;
        case BallStyle::Ripple:
          styleMeteor = 1.00f;
          break;
        case BallStyle::Dense:
          styleMeteor = 0.92f;
          break;
        case BallStyle::Electric:
        default:
          styleMeteor = 1.14f;
          break;
      }

      const float meteorAmount = speedT * styleMeteor;
      const float headLead     = data.baseRadius * METEOR_HEAD_LEAD * meteorAmount;
      const float stretchX     = std::fabs(data.dirX) * METEOR_AXIS_STRETCH * meteorAmount;
      const float stretchY     = std::fabs(data.dirY) * METEOR_AXIS_STRETCH * meteorAmount;
      const float squeezeX     = std::fabs(data.dirY) * METEOR_AXIS_SQUEEZE * meteorAmount;
      const float squeezeY     = std::fabs(data.dirX) * METEOR_AXIS_SQUEEZE * meteorAmount;

      data.centerX += data.dirX * headLead;
      data.centerY += data.dirY * headLead;

      data.radiusX *= 1.0f + stretchX - squeezeX;
      data.radiusY *= 1.0f + stretchY - squeezeY;
    }

    const float meteorTailBoost = (data.speed > METEOR_SHAPE_SPEED_MIN)
                                    ? Clamp01((data.speed - METEOR_SHAPE_SPEED_MIN) /
                                              std::max(METEOR_SHAPE_SPEED_MAX - METEOR_SHAPE_SPEED_MIN, 0.0001f))
                                    : 0.0f;

    data.tailLength = data.baseRadius * (TAIL_BASE_LENGTH + data.speed * TAIL_SPEED_SCALE + meteorTailBoost * METEOR_TAIL_BOOST);

    const float tailMinX = (data.dirX > 0.0f) ? (-data.dirX * data.tailLength) : 0.0f;
    const float tailMaxX = (data.dirX < 0.0f) ? (-data.dirX * data.tailLength) : 0.0f;
    const float tailMinY = (data.dirY > 0.0f) ? (-data.dirY * data.tailLength) : 0.0f;
    const float tailMaxY = (data.dirY < 0.0f) ? (-data.dirY * data.tailLength) : 0.0f;

    const float plasmaMarginX = data.baseRadius * PLASMA_WIDTH_SCALE + GLYPH_WIDTH;
    const float plasmaMarginY = data.baseRadius * PLASMA_WIDTH_SCALE + GLYPH_HEIGHT;
    const float edgeMarginX   = data.radiusX * EDGE_NOISE_BAND_OUTER + plasmaMarginX;
    const float edgeMarginY   = data.radiusY * EDGE_NOISE_BAND_OUTER + plasmaMarginY;

    const float minWorldX = data.centerX - edgeMarginX + tailMinX;
    const float maxWorldX = data.centerX + edgeMarginX + tailMaxX;
    const float minWorldY = data.centerY - edgeMarginY + tailMinY;
    const float maxWorldY = data.centerY + edgeMarginY + tailMaxY;

    data.minCellX = static_cast<int32_t>(std::floor(minWorldX / GLYPH_WIDTH));
    data.maxCellX = static_cast<int32_t>(std::floor(maxWorldX / GLYPH_WIDTH));
    data.minCellY = static_cast<int32_t>(std::floor(minWorldY / GLYPH_HEIGHT));
    data.maxCellY = static_cast<int32_t>(std::floor(maxWorldY / GLYPH_HEIGHT));

    return data;
  }

  float ComputeDirectionalTail(const BallState& ball, const BallRenderData& data, float dx, float dy) const
  {
    if(data.speed < 0.001f)
    {
      return 0.0f;
    }

    const float behind = -(dx * data.dirX + dy * data.dirY);
    if(behind <= 0.0f)
    {
      return 0.0f;
    }

    const float side = std::fabs(dx * (-data.dirY) + dy * data.dirX);

    if(behind > data.tailLength)
    {
      return 0.0f;
    }

    const float t             = behind / std::max(data.tailLength, 0.0001f);
    const float widthNearHead = data.baseRadius * TAIL_HEAD_WIDTH;
    const float widthNearTip  = data.baseRadius * TAIL_TIP_WIDTH;
    const float taperedT      = std::pow(t, TAIL_WIDTH_POWER);
    float       allowedWidth  = widthNearHead + (widthNearTip - widthNearHead) * taperedT;

    const float bulgeDelta = (t - TAIL_BODY_CENTER) / std::max(TAIL_BODY_SPREAD, 0.0001f);
    const float bulge      = std::exp(-(bulgeDelta * bulgeDelta)) * data.baseRadius * TAIL_BODY_BULGE;
    allowedWidth += bulge;
    allowedWidth = std::max(allowedWidth, widthNearTip);

    const float hazeLength       = data.tailLength * TAIL_HAZE_LENGTH_SCALE;
    const float hazeT            = behind / std::max(hazeLength, 0.0001f);
    const float hazeWidthNearTip = widthNearTip + data.baseRadius * TAIL_HAZE_SOFTNESS;
    const float hazeAllowedWidth = std::max((allowedWidth + data.baseRadius * TAIL_HAZE_SOFTNESS) * TAIL_HAZE_WIDTH_SCALE,
                                            hazeWidthNearTip);
    const bool insideHaze        = (behind <= hazeLength && side <= hazeAllowedWidth);

    if(side > allowedWidth && !insideHaze)
    {
      return 0.0f;
    }

    const float longitudinal = 1.0f - t;
    const float lateral      = 1.0f - (side / std::max(allowedWidth, 0.0001f));

    float shape = 0.0f;
    if(side <= allowedWidth)
    {
      shape = std::pow(std::max(0.0f, longitudinal), TAIL_FADE_POWER) * std::max(0.0f, lateral);
    }

    if(insideHaze)
    {
      const float hazeLongitudinal = 1.0f - Clamp01(hazeT);
      const float hazeLateral      = 1.0f - (side / std::max(hazeAllowedWidth, 0.0001f));
      const float hazeShape        = std::pow(std::max(0.0f, hazeLongitudinal), TAIL_FADE_POWER + 0.55f) *
                              std::pow(std::max(0.0f, hazeLateral), 1.35f);
      shape += hazeShape * TAIL_HAZE_STRENGTH;
    }

    const float sampleX   = data.centerX + dx;
    const float sampleY   = data.centerY + dy;
    const float tailNoise = SampleNoise(sampleX, sampleY, mTime * 1.25f, static_cast<int>(ball.style) + 7);

    shape *= (0.90f + tailNoise * 0.20f);

    const float plasmaLength = data.baseRadius * PLASMA_ZONE_LENGTH;
    const float plasmaWidth  = data.baseRadius * PLASMA_WIDTH_SCALE;

    if(behind > 0.0f && behind < plasmaLength && side < plasmaWidth)
    {
      const float plasmaT     = 1.0f - behind / std::max(plasmaLength, 0.0001f);
      const float plasmaSide  = 1.0f - side / std::max(plasmaWidth, 0.0001f);
      const float plasmaNoise = SampleNoise(sampleX * 1.7f,
                                            sampleY * 1.7f,
                                            mTime * PLASMA_FLICKER_SPEED,
                                            static_cast<int>(ball.style) + 29);

      shape += plasmaT * plasmaSide * plasmaNoise * PLASMA_STRENGTH;
    }

    switch(ball.style)
    {
      case BallStyle::Calm:
        return shape * (TAIL_STRENGTH * 0.65f);
      case BallStyle::Ripple:
        return shape * (TAIL_STRENGTH * 0.95f);
      case BallStyle::Dense:
        return shape * (TAIL_STRENGTH * 0.55f);
      case BallStyle::Electric:
      default:
        return shape * (TAIL_STRENGTH * 1.25f);
    }
  }

private: // ---------- dirty region ----------
  void MarkTileDirty(size_t tileIndex)
  {
    if(tileIndex < mDirtyTiles.size() && mDirtyTiles[tileIndex] == 0u)
    {
      mDirtyTiles[tileIndex] = 1u;
      mDirtyTileIndices.push_back(tileIndex);
    }
  }

  void MarkAllTilesDirty()
  {
    mDirtyTileIndices.clear();
    mDirtyTileIndices.reserve(mDirtyTiles.size());

    for(size_t tileIndex = 0u; tileIndex < mDirtyTiles.size(); ++tileIndex)
    {
      mDirtyTiles[tileIndex] = 1u;
      mDirtyTileIndices.push_back(tileIndex);
    }
  }

  void MarkDirtyTilesInCellRegion(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY)
  {
    const uint32_t startTileX = static_cast<uint32_t>(minX) / TILE_GLYPH_COLS;
    const uint32_t endTileX   = static_cast<uint32_t>(maxX) / TILE_GLYPH_COLS;
    const uint32_t startTileY = static_cast<uint32_t>(minY) / TILE_GLYPH_ROWS;
    const uint32_t endTileY   = static_cast<uint32_t>(maxY) / TILE_GLYPH_ROWS;

    for(uint32_t tileY = startTileY; tileY <= endTileY; ++tileY)
    {
      for(uint32_t tileX = startTileX; tileX <= endTileX; ++tileX)
      {
        const size_t tileIndex =
          static_cast<size_t>(tileY) * static_cast<size_t>(mTileCountX) + static_cast<size_t>(tileX);

        MarkTileDirty(tileIndex);
      }
    }
  }

  void ResetDirtyRegion()
  {
    mDirtyRegion.minX = static_cast<int32_t>(mTileCols);
    mDirtyRegion.minY = static_cast<int32_t>(mTileRows);
    mDirtyRegion.maxX = -1;
    mDirtyRegion.maxY = -1;
  }

  bool HasDirtyRegion() const
  {
    return (mDirtyRegion.maxX >= mDirtyRegion.minX) &&
           (mDirtyRegion.maxY >= mDirtyRegion.minY);
  }

  void ExpandDirtyRegion(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, uint32_t cols, uint32_t rows)
  {
    if(cols == 0u || rows == 0u)
    {
      return;
    }

    const int32_t clampedMinX = static_cast<int32_t>(ClampCellX(minX, cols));
    const int32_t clampedMinY = static_cast<int32_t>(ClampCellY(minY, rows));
    const int32_t clampedMaxX = static_cast<int32_t>(ClampCellX(maxX, cols));
    const int32_t clampedMaxY = static_cast<int32_t>(ClampCellY(maxY, rows));

    if(clampedMaxX < clampedMinX || clampedMaxY < clampedMinY)
    {
      return;
    }

    mDirtyRegion.minX = std::min(mDirtyRegion.minX, clampedMinX);
    mDirtyRegion.minY = std::min(mDirtyRegion.minY, clampedMinY);
    mDirtyRegion.maxX = std::max(mDirtyRegion.maxX, clampedMaxX);
    mDirtyRegion.maxY = std::max(mDirtyRegion.maxY, clampedMaxY);

    MarkDirtyTilesInCellRegion(clampedMinX, clampedMinY, clampedMaxX, clampedMaxY);
  }

  void MarkBallDirtyAt(const BallState& ball, const Vector2& position, uint32_t cols, uint32_t rows)
  {
    const BallRenderData data = BuildBallRenderData(ball, position);
    ExpandDirtyRegion(data.minCellX, data.minCellY, data.maxCellX, data.maxCellY, cols, rows);
  }

private: // ---------- UI ----------
  void InvalidateAllTileText()
  {
    for(auto& tile : mTiles)
    {
      tile.previousText.clear();
      tile.previousHash = 0u;
    }

    MarkAllTilesDirty();
  }

  void SetMarkupColorEnabled(bool enabled)
  {
    (void)enabled;
    mMarkupColorEnabled = false;
    DALI_LOG_ERROR("Markup color is not supported by TextVisualizer experimental sample.\n");
    UpdateInfoText();
  }

  struct MarkupPalette
  {
    const char* low;
    const char* mid;
    const char* high;
    const char* hot;
  };

  MarkupPalette GetTrailPalette(float flicker) const
  {
    if(flicker < 0.16f)
    {
      return {"#FF341F", "#FF6E00", "#FFBE45", "#FFF0D2"};
    }
    if(flicker < 0.32f)
    {
      return {"#FF4728", "#FF7F00", "#FFCB58", "#FFF4DB"};
    }
    if(flicker < 0.50f)
    {
      return {"#FF5533", "#FF9200", "#FFD46A", "#FFF6E2"};
    }
    if(flicker < 0.68f)
    {
      return {"#FF6540", "#FFA400", "#FFDB7A", "#FFF9E8"};
    }
    if(flicker < 0.84f)
    {
      return {"#FF7454", "#FFB600", "#FFE695", "#FFFBEF"};
    }
    return {"#FF8464", "#FFC700", "#FFF0B5", "#FFFDF5"};
  }

  const char* GetStarColor(char ch, float flicker) const
  {
    if(!mStarTwinkleEnabled)
    {
      switch(ch)
      {
        case '.': return "#6FA8FF";
        case '+': return "#BFE8FF";
        case '*': return "#FFFFFF";
        default:  return nullptr;
      }
    }

    if(ch == '.')
    {
      return (flicker < 0.5f) ? "#6FA8FF" : "#BFE8FF";
    }
    if(ch == '+')
    {
      return (flicker < 0.5f) ? "#BFE8FF" : "#FFFFFF";
    }
    if(ch == '*')
    {
      return (flicker < 0.7f) ? "#FFFFFF" : "#FFF7CC";
    }
    return nullptr;
  }

  float ComputeColorFlicker(uint32_t x, uint32_t y, int owner, float time) const
  {
    const float colorTime = std::floor(time / COLOR_FLICKER_FRAME_STEP) * COLOR_FLICKER_FRAME_STEP;

    return SampleNoise(static_cast<float>(x) * 9.7f,
                      static_cast<float>(y) * 7.3f,
                      colorTime * 1.2f,
                      owner + 37);
  }

  const char* GetMarkupColor(char ch,
                             bool isStar,
                             uint32_t x,
                             uint32_t y,
                             int owner,
                             float strength) const
  {
    if(ch == ' ' || !mMarkupColorEnabled)
    {
      return nullptr;
    }

    const float flicker = ComputeColorFlicker(x, y, owner, mTime);
    const float variation = SampleNoise(static_cast<float>(x) * 5.9f,
                                        static_cast<float>(y) * 4.1f,
                                        mTime * 0.85f,
                                        owner + 71);

    if(isStar)
    {
      return GetStarColor(ch, flicker);
    }

    const MarkupPalette palette = GetTrailPalette(flicker);

    const char* color = palette.low;
    if(strength > 0.82f)
    {
      color = palette.hot;
    }
    else if(strength > 0.58f)
    {
      color = (variation > 0.78f) ? palette.hot : palette.high;
    }
    else if(strength > 0.30f)
    {
      color = (variation > 0.72f) ? palette.high : palette.mid;
    }
    else if(variation > 0.82f)
    {
      color = palette.mid;
    }

    return color;
  }

  const std::string& GetColorOpenTag(const char* color)
  {
    for(const auto& cachedTag : mColorTagCache)
    {
      if(cachedTag.color == color)
      {
        return cachedTag.openTag;
      }
    }

    ColorTagCache cachedTag;
    cachedTag.color = color;
    cachedTag.openTag.reserve(24u);
    cachedTag.openTag += "<color value='";
    cachedTag.openTag += color;
    cachedTag.openTag += "'>";

    mColorTagCache.push_back(std::move(cachedTag));
    return mColorTagCache.back().openTag;
  }

  void AppendMarkupGlyph(std::string& out, char ch, const char*& activeColor, const char* color)
  {
    if(color != activeColor)
    {
      if(activeColor)
      {
        out += "</color>";
      }

      if(color)
      {
        out += GetColorOpenTag(color);
      }

      activeColor = color;
    }

    out += ch;
  }

  void CloseMarkupColor(std::string& out, const char*& activeColor) const
  {
    if(activeColor)
    {
      out += "</color>";
      activeColor = nullptr;
    }
  }

  void UpdateInfoText()
  {
    if(!mInfoText)
    {
      return;
    }

    std::string text = "Ball Count: " + std::to_string(mBalls.size()) +
                       " / " + std::to_string(MAX_TOTAL_BALL_COUNT) +
                       " | Mode: " + GetRenderModeName() +
                       " | Noise: " + GetNoiseQualityName() +
                       " | TextVisualizer";

    mInfoText.SetText(text.c_str());
  }

  Vector4 GetAnimationColor(size_t colorIndex) const
  {
    return COLOR_ANIM_PALETTE[colorIndex % COLOR_ANIM_PALETTE.size()];
  }

  void EnsureColorAnimationKeyFrames()
  {
    if(mColorAnimationKeyFrames)
    {
      return;
    }

    constexpr std::array<float, 9u> KEY_FRAME_PROGRESS = {{
      0.00f, 0.12f, 0.24f, 0.36f, 0.48f, 0.62f, 0.78f, 0.90f, 1.00f
    }};
    constexpr std::array<size_t, 9u> KEY_FRAME_COLOR_INDEX = {{
      9u, 2u, 9u, 5u, 1u, 7u, 4u, 9u, 9u
    }};

    mColorAnimationKeyFrames = KeyFrames::New();
    for(size_t index = 0u; index < KEY_FRAME_PROGRESS.size(); ++index)
    {
      mColorAnimationKeyFrames.Add(
        KEY_FRAME_PROGRESS[index],
        Property::Value(GetAnimationColor(KEY_FRAME_COLOR_INDEX[index])));
    }
  }

  void PlayColorAnimation(Label label)
  {
    if(!label)
    {
      return;
    }

    EnsureColorAnimationKeyFrames();

    Animation animation = Animation::New(COLOR_ANIM_DURATION);
    animation.AnimateBetween(
      Property(label, Label::Property::TEXT_COLOR),
      mColorAnimationKeyFrames,
      AlphaFunction::SIN);
    animation.SetLoopCount(0);
    animation.Play();

    mColorAnimations.push_back(animation);
  }

  void ResetColorAnimationTextColors()
  {
    if(mInfoText)
    {
      mInfoText.SetTextColor(UiColor(COLOR_INFO));
    }

    for(auto& tile : mTiles)
    {
      if(tile.textVisualizer)
      {
        tile.textVisualizer.SetTextColor(UiColor(COLOR_TEXT));
      }
    }
  }

  void StopDaliColorAnimations(bool resetColor)
  {
    for(auto& animation : mColorAnimations)
    {
      if(animation)
      {
        animation.Stop();
        animation.Clear();
      }
    }

    mColorAnimations.clear();

    if(resetColor)
    {
      ResetColorAnimationTextColors();
    }
  }

  void SetTileTextVisualizerLayout(Tile& tile)
  {
    tile.textVisualizer.SetLayoutParams(
      AbsoluteLayoutParams::New().SetBounds(
        LayoutRect(static_cast<float>(tile.tileX) * TILE_PIXEL_WIDTH,
                   static_cast<float>(tile.tileY) * TILE_PIXEL_HEIGHT,
                   static_cast<float>(tile.glyphCols) * GLYPH_WIDTH,
                   static_cast<float>(tile.glyphRows) * GLYPH_HEIGHT)));
  }

  void RecreateTileTextVisualizer(Tile& tile)
  {
    if(tile.textVisualizer)
    {
      mTileRoot.Remove(tile.textVisualizer);
    }

    tile.textVisualizer = CreateTileTextVisualizer(tile.glyphCols, tile.glyphRows);
    SetTileTextVisualizerLayout(tile);
    mTileRoot.Add(tile.textVisualizer);
  }

  void RebuildTilePlainTextAndClearDirty(Tile& tile, size_t tileIndex)
  {
    RebuildDirtyTileTextPlain(tile);

    if(tileIndex < mDirtyTiles.size())
    {
      mDirtyTiles[tileIndex] = 0u;
    }
  }

  void PrepareDaliColorAnimationText()
  {
    StopDaliColorAnimations(false);
    InvalidateAllTileText();

    if(!mTileRoot)
    {
      return;
    }

    for(size_t tileIndex = 0u; tileIndex < mTiles.size(); ++tileIndex)
    {
      auto& tile = mTiles[tileIndex];
      RecreateTileTextVisualizer(tile);
      RebuildTilePlainTextAndClearDirty(tile, tileIndex);
    }
  }

  void AttachDaliColorAnimations()
  {
    DALI_LOG_ERROR("DALI color animation is not supported by TextVisualizer experimental sample.\n");
  }

  void RequestDaliColorAnimationStart()
  {
    DALI_LOG_ERROR("DALI color animation is not supported by TextVisualizer experimental sample.\n");
  }

  void SetDaliColorAnimationEnabled(bool enabled)
  {
    (void)enabled;
    mDaliColorAnimationEnabled       = false;
    StopDaliColorAnimations(true);
    DALI_LOG_ERROR("DALI color animation is not supported by TextVisualizer experimental sample.\n");
    UpdateInfoText();
  }

  TextVisualizer CreateTileTextVisualizer(uint32_t glyphCols, uint32_t glyphRows)
  {
    TextVisualizer textVisualizer = TextVisualizer::New();
    textVisualizer.SetRequestedWidth(static_cast<float>(glyphCols) * GLYPH_WIDTH);
    textVisualizer.SetRequestedHeight(static_cast<float>(glyphRows) * GLYPH_HEIGHT);
    textVisualizer.SetFontSize(16.0f);
    textVisualizer.SetTextColor(UiColor(COLOR_TEXT));
    textVisualizer.SetBackgroundColor(GetClearTileColor());
    textVisualizer.SetFontFamily("Ubuntu Mono");
    textVisualizer.SetLineHeight(1.0f);
    textVisualizer.SetBorderlineWidth(GRID_WIDTH);
    textVisualizer.SetBorderlineColor(GetGridColor());

    return textVisualizer;
  }

  View CreateBallView(float size)
  {
    return View::New()
      .SetRequestedWidth(size)
      .SetRequestedHeight(size)
      .SetBackgroundColor(UiColor(COLOR_BALL))
      .SetCornerRadius(size * 0.5f)
      .SetPositionX(0.0f)
      .SetPositionY(0.0f)
      .SetOpacity(0.0f);
  }

  View CreateContents()
  {
    mPlayArea = AbsoluteLayout::New();
    mPlayArea
      .SetRequestedWidth(MATCH_PARENT)
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
      .SetBackgroundColor(UiColor(COLOR_BG));

    mTileRoot = AbsoluteLayout::New();
    mTileRoot
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT);

    mSingleTextVisualizer = TextVisualizer::New();
    mSingleTextVisualizer.SetRequestedWidth(MATCH_PARENT);
    mSingleTextVisualizer.SetRequestedHeight(MATCH_PARENT);
    mSingleTextVisualizer.SetFontSize(16.0f);
    mSingleTextVisualizer.SetTextColor(UiColor(COLOR_TEXT));
    mSingleTextVisualizer.SetBackgroundColor(GetClearTileColor());
    mSingleTextVisualizer.SetFontFamily("Ubuntu Mono");
    mSingleTextVisualizer.SetLineHeight(1.0f);

    mInfoText = TextVisualizer::New();
    mInfoText.SetRequestedWidth(MATCH_PARENT);
    mInfoText.SetRequestedHeight(20.0f);
    mInfoText.SetFontSize(12.0f);
    mInfoText.SetTextColor(UiColor(COLOR_INFO));
    mInfoText.SetBackgroundColor(GetClearTileColor());
    mInfoText.SetFontFamily("Ubuntu Mono");
    mInfoText.SetLineHeight(1.0f);

    UpdateInfoText();

    mPlayArea.Add(mSingleTextVisualizer);
    mPlayArea.Add(mTileRoot);
    ApplyRenderModeVisibility();

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("TextVisualizer Experimental Example")
          .SetFontSize(TITLE_FONT_SIZE)
          .SetTextColor(UiColor(0x000000)),
        mInfoText,
        mPlayArea
      });
  }

  void RebuildTiles(uint32_t cols, uint32_t rows)
  {
    if(!mTileRoot)
    {
      return;
    }

    for(auto& tile : mTiles)
    {
      if(tile.textVisualizer)
      {
        mTileRoot.Remove(tile.textVisualizer);
      }
    }

    mTiles.clear();

    mTileCols   = cols;
    mTileRows   = rows;
    mTileCountX = (cols + TILE_GLYPH_COLS - 1u) / TILE_GLYPH_COLS;
    mTileCountY = (rows + TILE_GLYPH_ROWS - 1u) / TILE_GLYPH_ROWS;

    mTiles.reserve(static_cast<size_t>(mTileCountX) * static_cast<size_t>(mTileCountY));

    for(uint32_t tileY = 0u; tileY < mTileCountY; ++tileY)
    {
      for(uint32_t tileX = 0u; tileX < mTileCountX; ++tileX)
      {
        Tile tile;
        tile.tileX     = tileX;
        tile.tileY     = tileY;
        tile.glyphCols = std::min(TILE_GLYPH_COLS, cols - tileX * TILE_GLYPH_COLS);
        tile.glyphRows = std::min(TILE_GLYPH_ROWS, rows - tileY * TILE_GLYPH_ROWS);
        tile.textVisualizer = CreateTileTextVisualizer(tile.glyphCols, tile.glyphRows);

        SetTileTextVisualizerLayout(tile);

        tile.workingText.reserve(static_cast<size_t>(tile.glyphRows) * static_cast<size_t>(tile.glyphCols + 1u));
        tile.starText.reserve(static_cast<size_t>(tile.glyphRows) * static_cast<size_t>(tile.glyphCols + 1u));
        BuildTileStarText(tile);

        tile.previousText = tile.starText;
        tile.textVisualizer.SetText(tile.previousText.c_str());

        mTileRoot.Add(tile.textVisualizer);
        mTiles.push_back(tile);
      }
    }

    mDirtyTiles.assign(mTiles.size(), 0u);
    MarkAllTilesDirty();
    mDirtyRegion.minX = 0;
    mDirtyRegion.minY = 0;
    mDirtyRegion.maxX = static_cast<int32_t>(cols) - 1;
    mDirtyRegion.maxY = static_cast<int32_t>(rows) - 1;
  }

  void EnsureTiles(uint32_t cols, uint32_t rows)
  {
    if(cols != mTileCols || rows != mTileRows || mTiles.empty())
    {
      RebuildTiles(cols, rows);
    }
  }

  void EnsureBuffers(uint32_t cols, uint32_t rows)
  {
    const size_t count = static_cast<size_t>(cols) * static_cast<size_t>(rows);

    if(mTrailBuffer.size() != count)
    {
      mTrailBuffer.assign(count, 0.0f);
      mImpactBuffer.assign(count, 0.0f);
      mOwnerBuffer.assign(count, -1);
      mHitCountBuffer.assign(count, 0);
      mGlyphBuffer.assign(count, ' ');
    }
  }

  void BuildSingleStarText(uint32_t cols, uint32_t rows)
  {
    mSingleStarText.clear();
    mSingleStarText.reserve(static_cast<size_t>(rows) * static_cast<size_t>(cols + 1u));

    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for(uint32_t y = 0u; y < rows; ++y)
    {
      for(uint32_t x = 0u; x < cols; ++x)
      {
        const float r = dist(mRandomEngine);

        char ch = ' ';
        if(r < 0.010f)
        {
          ch = '.';
        }
        else if(r < 0.0125f)
        {
          ch = '+';
        }
        else if(r < 0.0135f)
        {
          ch = '*';
        }

        mSingleStarText += ch;
      }

      if(y + 1u < rows)
      {
        mSingleStarText += '\n';
      }
    }

    mSingleStarCols = cols;
    mSingleStarRows = rows;
  }

  void EnsureSingleTextVisualizerLayout(uint32_t cols, uint32_t rows, float textWidth, float textHeight)
  {
    if(!mSingleTextVisualizer)
    {
      return;
    }

    mSingleTextVisualizer.SetRequestedWidth(textWidth);
    mSingleTextVisualizer.SetRequestedHeight(textHeight);
    mSingleTextVisualizer.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, textWidth, textHeight)));

    if(mSingleStarCols != cols || mSingleStarRows != rows || mSingleStarText.empty())
    {
      BuildSingleStarText(cols, rows);
      mSinglePreviousText.clear();
      mSinglePreviousHash = 0u;
    }
  }

  void ApplyRenderModeVisibility()
  {
    if(mSingleTextVisualizer)
    {
      mSingleTextVisualizer.SetProperty(Actor::Property::VISIBLE, mRenderMode == RenderMode::SINGLE_TEXT_VISUALIZER);
    }

    if(mTileRoot)
    {
      mTileRoot.SetProperty(Actor::Property::VISIBLE, mRenderMode == RenderMode::PARTIAL_TEXT_VISUALIZER_TILES);
    }
  }

  void ToggleRenderMode()
  {
    mRenderMode = (mRenderMode == RenderMode::SINGLE_TEXT_VISUALIZER)
                    ? RenderMode::PARTIAL_TEXT_VISUALIZER_TILES
                    : RenderMode::SINGLE_TEXT_VISUALIZER;

    if(mRenderMode == RenderMode::SINGLE_TEXT_VISUALIZER)
    {
      mSinglePreviousText.clear();
      mSinglePreviousHash = 0u;
    }
    else
    {
      InvalidateAllTileText();
    }

    ApplyRenderModeVisibility();
    UpdateInfoText();
  }

  void SetTilesAsyncRendering(bool enabled)
  {
    (void)enabled;
    DALI_LOG_ERROR("Async rendering is not supported by TextVisualizer experimental sample.\n");
    UpdateInfoText();
  }

  void ToggleNoiseQuality()
  {
    mNoiseQuality = (mNoiseQuality == NoiseQuality::FULL)
                      ? NoiseQuality::SIMPLIFIED
                      : NoiseQuality::FULL;

    UpdateInfoText();
  }

  void SetDirtyTileDebugVisible(bool visible)
  {
    mDirtyTileDebugVisible = visible;

    if(!mDirtyTileDebugVisible)
    {
      const UiColor clearColor = GetClearTileColor();
      for(auto& tile : mTiles)
      {
        if(tile.textVisualizer)
        {
          tile.textVisualizer.SetBackgroundColor(clearColor);
        }
      }
    }
  }

  void SetGridVisible(bool visible)
  {
    mBorderlineVisible = visible;

    const UiColor color = GetGridColor();
    for(auto& tile : mTiles)
    {
      if(tile.textVisualizer)
      {
        tile.textVisualizer.SetBorderlineWidth(GRID_WIDTH);
        tile.textVisualizer.SetBorderlineColor(color);
      }
    }
  }

private: // ---------- simulation ----------
  void EmitImpact(const Vector2& center, float strength = IMPACT_STRENGTH, float radius = IMPACT_RADIUS)
  {
    for(auto& impact : mImpacts)
    {
      if(!impact.active)
      {
        impact.center   = center;
        impact.radius   = radius;
        impact.strength = strength;
        impact.active   = true;
        return;
      }
    }

    mImpacts[0].center   = center;
    mImpacts[0].radius   = radius;
    mImpacts[0].strength = strength;
    mImpacts[0].active   = true;
  }

  void EmitCollisionSquash(BallState& ball, bool hitX, bool hitY)
  {
    const float strength = GetStyleSquashStrength(ball.style);

    if(hitX)
    {
      ball.squashX = -strength;
      ball.squashY =  strength * 0.75f;
    }

    if(hitY)
    {
      ball.squashY = -strength;
      ball.squashX =  strength * 0.75f;
    }
  }

  void EmitDirectionalCollisionSquash(BallState& ball, const Vector2& normal, float amount)
  {
    const float strength = GetStyleSquashStrength(ball.style) * amount;
    const float nx       = std::fabs(normal.x);
    const float ny       = std::fabs(normal.y);

    ball.squashX = std::min(ball.squashX, -(strength * nx) + strength * 0.20f * ny);
    ball.squashY = std::min(ball.squashY, -(strength * ny) + strength * 0.20f * nx);
    ball.squashX = std::max(ball.squashX, -strength);
    ball.squashY = std::max(ball.squashY, -strength);

    if(nx > ny)
    {
      ball.squashY = std::max(ball.squashY, strength * (0.55f + ny * 0.35f));
    }
    else
    {
      ball.squashX = std::max(ball.squashX, strength * (0.55f + nx * 0.35f));
    }
  }

  void AddTrailFlash(const Vector2& center, float radius, float innerValue, float outerValue, int owner)
  {
    if(mTrailBuffer.empty())
    {
      return;
    }

    const float areaWidth  = mPlayArea.GetSize().GetWidth();
    const float areaHeight = mPlayArea.GetSize().GetHeight();
    const uint32_t cols    = static_cast<uint32_t>(areaWidth / GLYPH_WIDTH);
    const uint32_t rows    = static_cast<uint32_t>(areaHeight / GLYPH_HEIGHT);

    if(cols == 0u || rows == 0u)
    {
      return;
    }

    const int32_t minCellX = static_cast<int32_t>(std::floor((center.x - radius) / GLYPH_WIDTH));
    const int32_t maxCellX = static_cast<int32_t>(std::floor((center.x + radius) / GLYPH_WIDTH));
    const int32_t minCellY = static_cast<int32_t>(std::floor((center.y - radius) / GLYPH_HEIGHT));
    const int32_t maxCellY = static_cast<int32_t>(std::floor((center.y + radius) / GLYPH_HEIGHT));

    ExpandDirtyRegion(minCellX, minCellY, maxCellX, maxCellY, cols, rows);

    const uint32_t startX = ClampCellX(minCellX, cols);
    const uint32_t endX   = ClampCellX(maxCellX, cols);
    const uint32_t startY = ClampCellY(minCellY, rows);
    const uint32_t endY   = ClampCellY(maxCellY, rows);

    float cellCenterY = static_cast<float>(startY) * GLYPH_HEIGHT + GLYPH_HEIGHT * 0.5f;

    for(uint32_t y = startY; y <= endY; ++y, cellCenterY += GLYPH_HEIGHT)
    {
      float cellCenterX = static_cast<float>(startX) * GLYPH_WIDTH + GLYPH_WIDTH * 0.5f;

      for(uint32_t x = startX; x <= endX; ++x, cellCenterX += GLYPH_WIDTH)
      {
        const float dx       = cellCenterX - center.x;
        const float dy       = cellCenterY - center.y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if(distance <= radius)
        {
          const float  t     = 1.0f - distance / std::max(radius, 1.0f);
          const float  value = outerValue + t * (innerValue - outerValue);
          const size_t index = IndexOf(x, y, cols);

          mTrailBuffer[index]    = std::max(mTrailBuffer[index], value);
          mOwnerBuffer[index]    = owner;
          mHitCountBuffer[index] = std::max(mHitCountBuffer[index], 2);
        }
      }
    }
  }

  void EmitBallCollisionImpact(const Vector2& center, float collisionEnergy, int owner)
  {
    const float energy = Clamp01(collisionEnergy);

    EmitImpact(center,
               COLLISION_IMPACT_STRENGTH * (0.85f + energy * 0.55f),
               COLLISION_IMPACT_RADIUS * (0.90f + energy * 0.50f));
    EmitImpact(center,
               COLLISION_IMPACT_STRENGTH * (0.45f + energy * 0.35f),
               COLLISION_IMPACT_RADIUS * (1.45f + energy * 0.40f));

    AddTrailFlash(center,
                  COLLISION_IMPACT_RADIUS * (0.42f + energy * 0.22f),
                  1.0f,
                  0.60f + energy * 0.15f,
                  owner);
  }

  void UpdateBall(BallState& ball, float textWidth, float textHeight, uint32_t cols, uint32_t rows)
  {
    const Vector2 oldPosition = ball.position;
    MarkBallDirtyAt(ball, oldPosition, cols, rows);

    ball.position += ball.velocity;

    const float maxX = textWidth - ball.size;
    const float maxY = textHeight - ball.size;

    bool hitX = false;
    bool hitY = false;

    if(ball.position.x < 0.0f)
    {
      ball.position.x = 0.0f;
      ball.velocity.x = -ball.velocity.x;
      hitX            = true;
      EmitImpact(Vector2(0.0f, ball.position.y + ball.size * 0.5f));
    }
    else if(ball.position.x > maxX)
    {
      ball.position.x = maxX;
      ball.velocity.x = -ball.velocity.x;
      hitX            = true;
      EmitImpact(Vector2(textWidth, ball.position.y + ball.size * 0.5f));
    }

    if(ball.position.y < 0.0f)
    {
      ball.position.y = 0.0f;
      ball.velocity.y = -ball.velocity.y;
      hitY            = true;
      EmitImpact(Vector2(ball.position.x + ball.size * 0.5f, 0.0f));
    }
    else if(ball.position.y > maxY)
    {
      ball.position.y = maxY;
      ball.velocity.y = -ball.velocity.y;
      hitY            = true;
      EmitImpact(Vector2(ball.position.x + ball.size * 0.5f, textHeight));
    }

    if(hitX || hitY)
    {
      EmitCollisionSquash(ball, hitX, hitY);
    }

    MarkBallDirtyAt(ball, ball.position, cols, rows);
  }

  void SimulateBalls(float textWidth, float textHeight, uint32_t cols, uint32_t rows)
  {
    for(auto& ball : mBalls)
    {
      if(textWidth <= ball.size || textHeight <= ball.size)
      {
        continue;
      }

      UpdateBall(ball, textWidth, textHeight, cols, rows);
    }

    for(size_t i = 0u; i < mBalls.size(); ++i)
    {
      for(size_t j = i + 1u; j < mBalls.size(); ++j)
      {
        BallState& ballA = mBalls[i];
        BallState& ballB = mBalls[j];

        const Vector2 centerA = GetBallCenter(ballA);
        const Vector2 centerB = GetBallCenter(ballB);
        Vector2       delta   = centerB - centerA;

        const float radiusA = ballA.size * 0.5f;
        const float radiusB = ballB.size * 0.5f;
        const float minDist = radiusA + radiusB;
        const float distSq  = delta.x * delta.x + delta.y * delta.y;

        if(distSq >= minDist * minDist)
        {
          continue;
        }

        MarkBallDirtyAt(ballA, ballA.position, cols, rows);
        MarkBallDirtyAt(ballB, ballB.position, cols, rows);

        float distance = std::sqrt(std::max(distSq, 0.0001f));
        if(distance < 0.001f)
        {
          delta    = Vector2(1.0f, 0.0f);
          distance = 1.0f;
        }

        const Vector2 normal      = delta / distance;
        const float   penetration = minDist - distance;
        const float   separation  = penetration * 0.5f + 0.5f;

        ballA.position -= normal * separation;
        ballB.position += normal * separation;

        const Vector2 relativeVelocity = ballB.velocity - ballA.velocity;
        const float   normalSpeed      = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

        if(normalSpeed < 0.0f)
        {
          const float impulse = -(1.0f + COLLISION_RESTITUTION) * normalSpeed * 0.5f;
          ballA.velocity -= normal * impulse;
          ballB.velocity += normal * impulse;
        }

        const float collisionEnergy = Clamp01((std::fabs(normalSpeed) + penetration * 0.18f) / 7.0f);
        EmitDirectionalCollisionSquash(ballA, Vector2(-normal.x, -normal.y), 0.80f + collisionEnergy * 0.55f);
        EmitDirectionalCollisionSquash(ballB, normal, 0.80f + collisionEnergy * 0.55f);

        const Vector2 contactCenter = centerA + normal * std::max(radiusA - penetration * 0.5f, 0.0f);
        const float speedSqA = ballA.velocity.x * ballA.velocity.x + ballA.velocity.y * ballA.velocity.y;
        const float speedSqB = ballB.velocity.x * ballB.velocity.x + ballB.velocity.y * ballB.velocity.y;
        const int fasterOwner = (speedSqA >= speedSqB)
                                  ? static_cast<int>(i)
                                  : static_cast<int>(j);
        EmitBallCollisionImpact(contactCenter, collisionEnergy, fasterOwner);

        MarkBallDirtyAt(ballA, ballA.position, cols, rows);
        MarkBallDirtyAt(ballB, ballB.position, cols, rows);
      }
    }
  }

  BallStyle GetRandomStyle()
  {
    std::uniform_int_distribution<int> styleDist(0, 3);
    return static_cast<BallStyle>(styleDist(mRandomEngine));
  }

  void AddSpawnFlashToTrail(const BallState& ball)
  {
    if(mTrailBuffer.empty())
    {
      return;
    }

    const float areaWidth  = mPlayArea.GetSize().GetWidth();
    const float areaHeight = mPlayArea.GetSize().GetHeight();
    const uint32_t cols    = static_cast<uint32_t>(areaWidth / GLYPH_WIDTH);
    const uint32_t rows    = static_cast<uint32_t>(areaHeight / GLYPH_HEIGHT);

    if(cols == 0u || rows == 0u)
    {
      return;
    }

    const float centerX = ball.position.x + ball.size * 0.5f;
    const float centerY = ball.position.y + ball.size * 0.5f;
    const float radius  = ball.size * 0.38f;

    const int32_t minCellX = static_cast<int32_t>(std::floor((centerX - radius) / GLYPH_WIDTH));
    const int32_t maxCellX = static_cast<int32_t>(std::floor((centerX + radius) / GLYPH_WIDTH));
    const int32_t minCellY = static_cast<int32_t>(std::floor((centerY - radius) / GLYPH_HEIGHT));
    const int32_t maxCellY = static_cast<int32_t>(std::floor((centerY + radius) / GLYPH_HEIGHT));

    ExpandDirtyRegion(minCellX, minCellY, maxCellX, maxCellY, cols, rows);

    const uint32_t startX = ClampCellX(minCellX, cols);
    const uint32_t endX   = ClampCellX(maxCellX, cols);
    const uint32_t startY = ClampCellY(minCellY, rows);
    const uint32_t endY   = ClampCellY(maxCellY, rows);

    float cellCenterY = static_cast<float>(startY) * GLYPH_HEIGHT + GLYPH_HEIGHT * 0.5f;

    for(uint32_t y = startY; y <= endY; ++y, cellCenterY += GLYPH_HEIGHT)
    {
      float cellCenterX = static_cast<float>(startX) * GLYPH_WIDTH + GLYPH_WIDTH * 0.5f;

      for(uint32_t x = startX; x <= endX; ++x, cellCenterX += GLYPH_WIDTH)
      {
        const float dx       = cellCenterX - centerX;
        const float dy       = cellCenterY - centerY;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if(distance <= radius)
        {
          const float  t     = 1.0f - distance / std::max(radius, 1.0f);
          const size_t index = IndexOf(x, y, cols);

          mTrailBuffer[index]    = std::max(mTrailBuffer[index], 0.55f + t * 0.45f);
          mOwnerBuffer[index]    = static_cast<int>(mBalls.size() - 1u);
          mHitCountBuffer[index] = std::max(mHitCountBuffer[index], 1);
        }
      }
    }
  }

  void AddRandomBall()
  {
    if(mBalls.size() >= MAX_TOTAL_BALL_COUNT)
    {
      DALI_LOG_ERROR("Maximum ball count reached: %zu\n", mBalls.size());
      return;
    }

    if(!mPlayArea)
    {
      return;
    }

    const float areaWidth  = mPlayArea.GetSize().GetWidth();
    const float areaHeight = mPlayArea.GetSize().GetHeight();
    const uint32_t cols    = static_cast<uint32_t>(areaWidth / GLYPH_WIDTH);
    const uint32_t rows    = static_cast<uint32_t>(areaHeight / GLYPH_HEIGHT);

    if(cols == 0u || rows == 0u)
    {
      return;
    }

    const float textWidth  = static_cast<float>(cols) * GLYPH_WIDTH;
    const float textHeight = static_cast<float>(rows) * GLYPH_HEIGHT;

    std::uniform_real_distribution<float> sizeDist(RANDOM_BALL_MIN_SIZE, RANDOM_BALL_MAX_SIZE);
    const float size = sizeDist(mRandomEngine);

    if(textWidth <= size || textHeight <= size)
    {
      return;
    }

    std::uniform_real_distribution<float> posXDist(0.0f, textWidth - size);
    std::uniform_real_distribution<float> posYDist(0.0f, textHeight - size);
    std::uniform_real_distribution<float> angleDist(0.0f, TWO_PI);
    std::uniform_real_distribution<float> speedDist(RANDOM_SPEED_MIN, RANDOM_SPEED_MAX);

    BallState ball;
    ball.size     = size;
    ball.position = Vector2(posXDist(mRandomEngine), posYDist(mRandomEngine));

    const float angle = angleDist(mRandomEngine);
    const float speed = speedDist(mRandomEngine);

    ball.velocity   = Vector2(std::cos(angle) * speed, std::sin(angle) * speed);
    ball.style      = GetRandomStyle();
    ball.glyphSet   = GetGlyphSetForStyle(ball.style);
    ball.spawnBoost = 1.0f;

    mBalls.push_back(ball);

    const Vector2 center(ball.position.x + ball.size * 0.5f, ball.position.y + ball.size * 0.5f);

    EmitImpact(center, SPAWN_IMPACT_STRENGTH, SPAWN_IMPACT_RADIUS);
    EmitImpact(center, SPAWN_IMPACT_STRENGTH * 0.8f, SPAWN_IMPACT_RADIUS * 1.65f);
    EmitImpact(center, SPAWN_IMPACT_STRENGTH * 0.55f, SPAWN_IMPACT_RADIUS * 2.15f);

    AddSpawnFlashToTrail(mBalls.back());
    UpdateInfoText();

    DALI_LOG_ERROR("AddRandomBall size=%.2f pos=(%.2f, %.2f) vel=(%.2f, %.2f) style=%d total=%zu\n",
                   ball.size,
                   ball.position.x,
                   ball.position.y,
                   ball.velocity.x,
                   ball.velocity.y,
                   static_cast<int>(ball.style),
                   mBalls.size());
  }

private: // ---------- raster ----------
  void DecayDirtyRegionBuffers(uint32_t cols)
  {
    if(!HasDirtyRegion())
    {
      return;
    }

    const uint32_t startX = static_cast<uint32_t>(mDirtyRegion.minX);
    const uint32_t endX   = static_cast<uint32_t>(mDirtyRegion.maxX);
    const uint32_t startY = static_cast<uint32_t>(mDirtyRegion.minY);
    const uint32_t endY   = static_cast<uint32_t>(mDirtyRegion.maxY);

    for(uint32_t y = startY; y <= endY; ++y)
    {
      for(uint32_t x = startX; x <= endX; ++x)
      {
        const size_t index = IndexOf(x, y, cols);

        mTrailBuffer[index] *= TRAIL_DECAY;
        if(mTrailBuffer[index] < 0.01f)
        {
          mTrailBuffer[index] = 0.0f;
        }

        mImpactBuffer[index] *= IMPACT_DECAY;
        if(mImpactBuffer[index] < 0.01f)
        {
          mImpactBuffer[index] = 0.0f;
        }

        mOwnerBuffer[index]    = -1;
        mHitCountBuffer[index] = 0;
      }
    }

    for(auto& ball : mBalls)
    {
      ball.squashX *= SQUASH_DECAY;
      ball.squashY *= SQUASH_DECAY;
      ball.spawnBoost *= 0.86f;

      if(std::fabs(ball.squashX) < 0.001f)
      {
        ball.squashX = 0.0f;
      }
      if(std::fabs(ball.squashY) < 0.001f)
      {
        ball.squashY = 0.0f;
      }
      if(ball.spawnBoost < 0.01f)
      {
        ball.spawnBoost = 0.0f;
      }
    }
  }

  void RasterizeBallsToTrail(uint32_t cols, uint32_t rows)
  {
    if(cols == 0u || rows == 0u)
    {
      return;
    }

    for(size_t i = 0u; i < mBalls.size(); ++i)
    {
      const auto&          ball = mBalls[i];
      const BallRenderData data = BuildBallRenderData(ball, ball.position);

      ExpandDirtyRegion(data.minCellX, data.minCellY, data.maxCellX, data.maxCellY, cols, rows);

      const uint32_t startX = ClampCellX(data.minCellX, cols);
      const uint32_t endX   = ClampCellX(data.maxCellX, cols);
      const uint32_t startY = ClampCellY(data.minCellY, rows);
      const uint32_t endY   = ClampCellY(data.maxCellY, rows);

      float cellCenterY = static_cast<float>(startY) * GLYPH_HEIGHT + GLYPH_HEIGHT * 0.5f;

      for(uint32_t y = startY; y <= endY; ++y, cellCenterY += GLYPH_HEIGHT)
      {
        float cellCenterX = static_cast<float>(startX) * GLYPH_WIDTH + GLYPH_WIDTH * 0.5f;

        for(uint32_t x = startX; x <= endX; ++x, cellCenterX += GLYPH_WIDTH)
        {
          const float dx = cellCenterX - data.centerX;
          const float dy = cellCenterY - data.centerY;

          const float nx = dx / std::max(data.radiusX, 1.0f);
          const float ny = dy / std::max(data.radiusY, 1.0f);

          const float noise    = SampleNoise(cellCenterX, cellCenterY, mTime, static_cast<int>(i));
          const float shapeLen = ComputeShapeDistance(ball.style, nx, ny, noise);

          const bool insideBody     = (shapeLen <= 1.0f);
          const bool insideEdgeBand = (shapeLen >= EDGE_NOISE_BAND_INNER && shapeLen <= EDGE_NOISE_BAND_OUTER);

          if(!insideBody && !insideEdgeBand)
          {
            const float tailValue = ComputeDirectionalTail(ball, data, dx, dy);
            if(tailValue <= 0.0f)
            {
              continue;
            }
          }

          float value = 0.0f;

          if(insideBody)
          {
            const float t = 1.0f - std::max(0.0f, shapeLen);
            value = 0.42f + t * 0.95f + ball.spawnBoost * 0.35f;

            const size_t index = IndexOf(x, y, cols);
            ++mHitCountBuffer[index];
          }

          if(insideEdgeBand)
          {
            const float bandCenter    = 0.95f;
            const float bandHalfWidth = std::max((EDGE_NOISE_BAND_OUTER - EDGE_NOISE_BAND_INNER) * 0.5f, 0.0001f);
            const float bandT         = 1.0f - std::fabs(shapeLen - bandCenter) / bandHalfWidth;
            const float spark         = (noise > 0.62f) ? ((noise - 0.62f) / 0.38f) : -(0.62f - noise) * 0.35f;

            value += spark * GetStyleEdgeNoiseStrength(ball.style) * std::max(0.0f, bandT);
            value += ball.spawnBoost * 0.18f * std::max(0.0f, bandT);
          }

          value += ComputeDirectionalTail(ball, data, dx, dy);
          value = Clamp01(value);

          if(value > 0.0f)
          {
            const size_t index = IndexOf(x, y, cols);
            if(value > mTrailBuffer[index])
            {
              mTrailBuffer[index] = value;
              mOwnerBuffer[index] = static_cast<int>(i);
            }
          }
        }
      }
    }
  }

  void RasterizeImpacts(uint32_t cols, uint32_t rows)
  {
    if(cols == 0u || rows == 0u)
    {
      return;
    }

    for(auto& impact : mImpacts)
    {
      if(!impact.active)
      {
        continue;
      }

      const float outerRadius = impact.radius + IMPACT_RING_THICKNESS;

      const int32_t minCellX = static_cast<int32_t>(std::floor((impact.center.x - outerRadius) / GLYPH_WIDTH));
      const int32_t maxCellX = static_cast<int32_t>(std::floor((impact.center.x + outerRadius) / GLYPH_WIDTH));
      const int32_t minCellY = static_cast<int32_t>(std::floor((impact.center.y - outerRadius) / GLYPH_HEIGHT));
      const int32_t maxCellY = static_cast<int32_t>(std::floor((impact.center.y + outerRadius) / GLYPH_HEIGHT));

      ExpandDirtyRegion(minCellX, minCellY, maxCellX, maxCellY, cols, rows);

      const uint32_t startX = ClampCellX(minCellX, cols);
      const uint32_t endX   = ClampCellX(maxCellX, cols);
      const uint32_t startY = ClampCellY(minCellY, rows);
      const uint32_t endY   = ClampCellY(maxCellY, rows);

      float cellCenterY = static_cast<float>(startY) * GLYPH_HEIGHT + GLYPH_HEIGHT * 0.5f;

      for(uint32_t y = startY; y <= endY; ++y, cellCenterY += GLYPH_HEIGHT)
      {
        float cellCenterX = static_cast<float>(startX) * GLYPH_WIDTH + GLYPH_WIDTH * 0.5f;

        for(uint32_t x = startX; x <= endX; ++x, cellCenterX += GLYPH_WIDTH)
        {
          const float dx       = cellCenterX - impact.center.x;
          const float dy       = cellCenterY - impact.center.y;
          const float distance = std::sqrt(dx * dx + dy * dy);
          const float band     = std::fabs(distance - impact.radius);

          if(band <= IMPACT_RING_THICKNESS)
          {
            const float  t     = 1.0f - (band / IMPACT_RING_THICKNESS);
            const float  noise = 0.82f + 0.18f * std::sin(cellCenterX * 0.08f + cellCenterY * 0.11f);
            const float  value = impact.strength * t * noise;
            const size_t index = IndexOf(x, y, cols);

            mImpactBuffer[index] = std::max(mImpactBuffer[index], value);
          }
        }
      }

      impact.strength *= IMPACT_DECAY;
      impact.radius += 6.5f;

      if(impact.strength < 0.08f)
      {
        impact.active = false;
      }
    }
  }

  char PickFromSet(const char* set, float value) const
  {
    const size_t count = std::strlen(set);
    if(count == 0u)
    {
      return ' ';
    }

    const float  clamped = Clamp01(value);
    const size_t index   = static_cast<size_t>(clamped * static_cast<float>(count - 1u));
    return set[index];
  }

  char PickTrailChar(float value, int owner) const
  {
    if(owner < 0 || owner >= static_cast<int>(mBalls.size()))
    {
      return ' ';
    }
    return PickFromSet(mBalls[owner].glyphSet, value);
  }

  char PickOverlapChar(float value) const
  {
    return PickFromSet(OVERLAP_CHARS, value);
  }

  char PickImpactChar(float value) const
  {
    return PickFromSet(IMPACT_CHARS, value);
  }

  void RasterizeGlyphsInDirtyRegion(uint32_t cols)
  {
    if(!HasDirtyRegion())
    {
      return;
    }

    const uint32_t startX = static_cast<uint32_t>(mDirtyRegion.minX);
    const uint32_t endX   = static_cast<uint32_t>(mDirtyRegion.maxX);
    const uint32_t startY = static_cast<uint32_t>(mDirtyRegion.minY);
    const uint32_t endY   = static_cast<uint32_t>(mDirtyRegion.maxY);

    for(uint32_t y = startY; y <= endY; ++y)
    {
      for(uint32_t x = startX; x <= endX; ++x)
      {
        const size_t index = IndexOf(x, y, cols);

        const float baseTrailValue = Clamp01(mTrailBuffer[index]);
        const float impactValue    = Clamp01(mImpactBuffer[index]);
        const int   owner          = mOwnerBuffer[index];
        const int   hits           = mHitCountBuffer[index];

        const float shimmerNoise = SampleNoise(static_cast<float>(x) * 3.7f,
                                               static_cast<float>(y) * 5.1f,
                                               mTime * 0.55f,
                                               owner + 11);

        const float tailFactor = Clamp01((0.75f - baseTrailValue) / 0.75f);

        float trailValue = baseTrailValue;
        trailValue += (shimmerNoise - 0.5f) * TRAIL_SHIMMER_STRENGTH * baseTrailValue;
        trailValue += std::max(0.0f, shimmerNoise - TAIL_SPARK_THRESHOLD) * TAIL_SPARK_BONUS * tailFactor;
        trailValue = Clamp01(trailValue);

        char ch = ' ';

        if(impactValue > 0.28f)
        {
          ch = PickImpactChar(impactValue);
        }
        else if(hits >= 2 && trailValue > 0.04f)
        {
          ch = PickOverlapChar(trailValue);
        }
        else if(trailValue > 0.02f)
        {
          const bool isTailLike = (baseTrailValue < 0.42f);
          ch = isTailLike ? PickFromSet(TAIL_CHARS, trailValue)
                          : PickTrailChar(trailValue, owner);
        }

        mGlyphBuffer[index] = ch;
      }
    }
  }

private: // ---------- tile presentation ----------
  static void HashByte(uint64_t& hash, uint8_t value)
  {
    hash ^= static_cast<uint64_t>(value);
    hash *= 1099511628211ull;
  }

  static void HashColor(uint64_t& hash, const char* color)
  {
    if(!color)
    {
      HashByte(hash, 0u);
      return;
    }

    while(*color != '\0')
    {
      HashByte(hash, static_cast<uint8_t>(*color));
      ++color;
    }
    HashByte(hash, 0u);
  }

  uint64_t ComputeTilePlainHash(const Tile& tile) const
  {
    uint64_t hash = 1469598103934665603ull;

    const uint32_t startX = tile.tileX * TILE_GLYPH_COLS;
    const uint32_t startY = tile.tileY * TILE_GLYPH_ROWS;

    for(uint32_t localY = 0u; localY < tile.glyphRows; ++localY)
    {
      const uint32_t globalY = startY + localY;

      for(uint32_t localX = 0u; localX < tile.glyphCols; ++localX)
      {
        const uint32_t globalX = startX + localX;
        const char glyph = mGlyphBuffer[IndexOf(globalX, globalY, mTileCols)];

        const size_t starIndex =
          static_cast<size_t>(localY) * static_cast<size_t>(tile.glyphCols + 1u) + static_cast<size_t>(localX);

        HashByte(hash, static_cast<uint8_t>((glyph != ' ') ? glyph : tile.starText[starIndex]));
      }

      HashByte(hash, '\n');
    }

    return hash;
  }

  uint64_t ComputeSinglePlainHash() const
  {
    uint64_t hash = 1469598103934665603ull;

    for(uint32_t y = 0u; y < mTileRows; ++y)
    {
      for(uint32_t x = 0u; x < mTileCols; ++x)
      {
        const char glyph = mGlyphBuffer[IndexOf(x, y, mTileCols)];

        const size_t starIndex =
          static_cast<size_t>(y) * static_cast<size_t>(mTileCols + 1u) + static_cast<size_t>(x);

        HashByte(hash, static_cast<uint8_t>((glyph != ' ') ? glyph : mSingleStarText[starIndex]));
      }

      HashByte(hash, '\n');
    }

    return hash;
  }

  uint64_t ComputeTileMarkupHash(const Tile& tile) const
  {
    uint64_t hash = 1469598103934665603ull;

    const uint32_t startX = tile.tileX * TILE_GLYPH_COLS;
    const uint32_t startY = tile.tileY * TILE_GLYPH_ROWS;

    for(uint32_t localY = 0u; localY < tile.glyphRows; ++localY)
    {
      const uint32_t globalY = startY + localY;

      for(uint32_t localX = 0u; localX < tile.glyphCols; ++localX)
      {
        const uint32_t globalX = startX + localX;
        const size_t   index   = IndexOf(globalX, globalY, mTileCols);

        const char  glyph    = mGlyphBuffer[index];
        const int   owner    = mOwnerBuffer[index];
        const float strength = std::max(Clamp01(mTrailBuffer[index]), Clamp01(mImpactBuffer[index]));

        const size_t starIndex =
          static_cast<size_t>(localY) * static_cast<size_t>(tile.glyphCols + 1u) + static_cast<size_t>(localX);

        const bool  useStar = (glyph == ' ');
        const char  finalCh = useStar ? tile.starText[starIndex] : glyph;
        const char* color   = GetMarkupColor(finalCh, useStar, globalX, globalY, owner, strength);

        HashByte(hash, static_cast<uint8_t>(finalCh));
        HashColor(hash, color);
      }

      HashByte(hash, '\n');
    }

    return hash;
  }

  bool RebuildDirtyTileTextPlain(Tile& tile)
  {
    const uint64_t hash = ComputeTilePlainHash(tile);
    if(hash == tile.previousHash)
    {
      return false;
    }

    tile.workingText.clear();
    tile.workingText.reserve(static_cast<size_t>(tile.glyphRows) *
                            static_cast<size_t>(tile.glyphCols + 1u));

    const uint32_t startX = tile.tileX * TILE_GLYPH_COLS;
    const uint32_t startY = tile.tileY * TILE_GLYPH_ROWS;

    for(uint32_t localY = 0u; localY < tile.glyphRows; ++localY)
    {
      const uint32_t globalY = startY + localY;

      for(uint32_t localX = 0u; localX < tile.glyphCols; ++localX)
      {
        const uint32_t globalX = startX + localX;
        const char glyph = mGlyphBuffer[IndexOf(globalX, globalY, mTileCols)];

        const size_t starIndex =
          static_cast<size_t>(localY) * static_cast<size_t>(tile.glyphCols + 1u) + static_cast<size_t>(localX);

        tile.workingText += (glyph != ' ') ? glyph : tile.starText[starIndex];
      }

      if(localY + 1u < tile.glyphRows)
      {
        tile.workingText += '\n';
      }
    }

    if(tile.workingText != tile.previousText)
    {
      tile.textVisualizer.SetText(tile.workingText.c_str());
      tile.previousHash = hash;
      tile.previousText.swap(tile.workingText);
      return true;
    }

    tile.previousHash = hash;
    return false;
  }

  bool RebuildSingleText()
  {
    if(!mSingleTextVisualizer || mTileCols == 0u || mTileRows == 0u || mSingleStarText.empty())
    {
      return false;
    }

    const uint64_t hash = ComputeSinglePlainHash();
    if(hash == mSinglePreviousHash)
    {
      return false;
    }

    mSingleWorkingText.clear();
    mSingleWorkingText.reserve(static_cast<size_t>(mTileRows) * static_cast<size_t>(mTileCols + 1u));

    for(uint32_t y = 0u; y < mTileRows; ++y)
    {
      for(uint32_t x = 0u; x < mTileCols; ++x)
      {
        const char glyph = mGlyphBuffer[IndexOf(x, y, mTileCols)];

        const size_t starIndex =
          static_cast<size_t>(y) * static_cast<size_t>(mTileCols + 1u) + static_cast<size_t>(x);

        mSingleWorkingText += (glyph != ' ') ? glyph : mSingleStarText[starIndex];
      }

      if(y + 1u < mTileRows)
      {
        mSingleWorkingText += '\n';
      }
    }

    if(mSingleWorkingText != mSinglePreviousText)
    {
      mSingleTextVisualizer.SetText(mSingleWorkingText.c_str());
      mSinglePreviousHash = hash;
      mSinglePreviousText.swap(mSingleWorkingText);
      return true;
    }

    mSinglePreviousHash = hash;
    return false;
  }

  bool RebuildDirtyTileTextMarkup(Tile& tile)
  {
    const uint64_t hash = ComputeTileMarkupHash(tile);
    if(hash == tile.previousHash)
    {
      return false;
    }

    tile.workingText.clear();
    tile.workingText.reserve(static_cast<size_t>(tile.glyphRows) *
                            static_cast<size_t>(tile.glyphCols * 24u + 1u));

    const uint32_t startX = tile.tileX * TILE_GLYPH_COLS;
    const uint32_t startY = tile.tileY * TILE_GLYPH_ROWS;
    const char*    activeColor = nullptr;

    for(uint32_t localY = 0u; localY < tile.glyphRows; ++localY)
    {
      const uint32_t globalY = startY + localY;

      for(uint32_t localX = 0u; localX < tile.glyphCols; ++localX)
      {
        const uint32_t globalX = startX + localX;
        const size_t   index   = IndexOf(globalX, globalY, mTileCols);

        const char  glyph    = mGlyphBuffer[index];
        const int   owner    = mOwnerBuffer[index];
        const float strength = std::max(Clamp01(mTrailBuffer[index]), Clamp01(mImpactBuffer[index]));

        const size_t starIndex =
          static_cast<size_t>(localY) * static_cast<size_t>(tile.glyphCols + 1u) + static_cast<size_t>(localX);

        const bool useStar = (glyph == ' ');
        const char finalCh = useStar ? tile.starText[starIndex] : glyph;
        const char* color = GetMarkupColor(finalCh,
                                           useStar,
                                           globalX,
                                           globalY,
                                           owner,
                                           strength);

        AppendMarkupGlyph(tile.workingText, finalCh, activeColor, color);
      }

      if(localY + 1u < tile.glyphRows)
      {
        CloseMarkupColor(tile.workingText, activeColor);
        tile.workingText += '\n';
      }
    }

    CloseMarkupColor(tile.workingText, activeColor);

    if(tile.workingText != tile.previousText)
    {
      tile.textVisualizer.SetText(tile.workingText.c_str());
      tile.previousHash = hash;
      tile.previousText.swap(tile.workingText);
      return true;
    }

    tile.previousHash = hash;
    return false;
  }

  void ApplyTileDebugBackground(Tile& tile, bool changed)
  {
    if(mDirtyTileDebugVisible)
    {
      tile.textVisualizer.SetBackgroundColor(changed ? GetDirtyTileColor() : GetClearTileColor());
    }
  }

  void UpdateDirtyTiles()
  {
    for(size_t tileIndex : mDirtyTileIndices)
    {
      if(tileIndex >= mTiles.size() || mDirtyTiles[tileIndex] == 0u)
      {
        continue;
      }

      auto& tile = mTiles[tileIndex];
      const bool changed = mMarkupColorEnabled
                             ? RebuildDirtyTileTextMarkup(tile)
                             : RebuildDirtyTileTextPlain(tile);

      mDirtyTiles[tileIndex] = 0u;

      ApplyTileDebugBackground(tile, changed);
    }

    mDirtyTileIndices.clear();
  }

private: // ---------- frame pipeline ----------
  bool ValidateFrameObjects() const
  {
    if(!mPlayArea || !mTileRoot)
    {
      return false;
    }
    return true;
  }

  bool ComputeGridSize(uint32_t& cols, uint32_t& rows, float& textWidth, float& textHeight) const
  {
    const float areaWidth  = mPlayArea.GetSize().GetWidth();
    const float areaHeight = mPlayArea.GetSize().GetHeight();

    cols = static_cast<uint32_t>(areaWidth / GLYPH_WIDTH);
    rows = static_cast<uint32_t>(areaHeight / GLYPH_HEIGHT);

    if(cols == 0u || rows == 0u)
    {
      return false;
    }

    textWidth  = static_cast<float>(cols) * GLYPH_WIDTH;
    textHeight = static_cast<float>(rows) * GLYPH_HEIGHT;
    return true;
  }

  void PrepareFrame(uint32_t cols, uint32_t rows, float textWidth, float textHeight)
  {
    EnsureBuffers(cols, rows);
    EnsureTiles(cols, rows);
    EnsureSingleTextVisualizerLayout(cols, rows, textWidth, textHeight);
    ResetDirtyRegion();
  }

  void RasterizeFrame(uint32_t cols, uint32_t rows)
  {
    DecayDirtyRegionBuffers(cols);

    RasterizeBallsToTrail(cols, rows);
    RasterizeImpacts(cols, rows);

    RasterizeGlyphsInDirtyRegion(cols);
  }

  void PresentFrame()
  {
    if(mRenderMode == RenderMode::SINGLE_TEXT_VISUALIZER)
    {
      RebuildSingleText();
    }
    else
    {
      UpdateDirtyTiles();
    }
  }

private: // ---------- app lifecycle ----------
  void OnInit(Application& application)
  {
    DALI_LOG_ERROR("Application OnInit\n");

    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_WHITE));
    window.Add(CreateContents());

    mBallTimer = Timer::New(TIMER_INTERVAL_MS);
    mBallTimer.TickSignal().Connect(this, &TextVisualizerExperimentalController::OnBallTick);
    mBallTimer.Start();

    window.KeyEventSignal().Connect(this, &TextVisualizerExperimentalController::OnKeyEvent);
  }

  bool OnBallTick()
  {
    if(!ValidateFrameObjects())
    {
      return true;
    }

    mTime += 1.0f;

    uint32_t cols       = 0u;
    uint32_t rows       = 0u;
    float    textWidth  = 0.0f;
    float    textHeight = 0.0f;

    if(!ComputeGridSize(cols, rows, textWidth, textHeight))
    {
      return true;
    }

    PrepareFrame(cols, rows, textWidth, textHeight);
    SimulateBalls(textWidth, textHeight, cols, rows);
    RasterizeFrame(cols, rows);
    PresentFrame();

    return true;
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

    if(event.GetKeyName() == "1")
    {
      AddRandomBall();
    }
    else if(event.GetKeyName() == "2")
    {
      SetTilesAsyncRendering(true);
      DALI_LOG_ERROR("AsyncRendering\n");
    }
    else if(event.GetKeyName() == "3")
    {
      SetTilesAsyncRendering(false);
      DALI_LOG_ERROR("SyncRendering\n");
    }
    else if(event.GetKeyName() == "4")
    {
      ToggleNoiseQuality();
      DALI_LOG_ERROR("NoiseQuality: %s\n", GetNoiseQualityName());
    }
    else if(event.GetKeyName() == "5")
    {
      ToggleRenderMode();
      DALI_LOG_ERROR("RenderMode: %s\n", GetRenderModeName());
    }
    else if(event.GetKeyName() == "6")
    {
      SetDaliColorAnimationEnabled(!mDaliColorAnimationEnabled);
    }
    else if(event.GetKeyName() == "7")
    {
      mStarTwinkleEnabled = !mStarTwinkleEnabled;
      DALI_LOG_ERROR("StarTwinkle %s\n", mStarTwinkleEnabled ? "ON" : "OFF");
    }
    else if(event.GetKeyName() == "8")
    {
      SetMarkupColorEnabled(!mMarkupColorEnabled);
      DALI_LOG_ERROR("MarkupColor %s\n", mMarkupColorEnabled ? "ON" : "OFF");
    }
    else if(event.GetKeyName() == "9")
    {
      SetDirtyTileDebugVisible(!mDirtyTileDebugVisible);
      DALI_LOG_ERROR("DirtyTileDebug %s\n", mDirtyTileDebugVisible ? "ON" : "OFF");
    }
    else if(event.GetKeyName() == "0")
    {
      SetGridVisible(!mBorderlineVisible);
      DALI_LOG_ERROR("Grid %s\n", mBorderlineVisible ? "ON" : "OFF");
    }
  }

private:
  Application&            mApplication;
  AbsoluteLayout          mPlayArea;
  AbsoluteLayout          mTileRoot;
  TextVisualizer          mInfoText;
  TextVisualizer          mSingleTextVisualizer;
  Timer                   mBallTimer;
  KeyFrames               mColorAnimationKeyFrames;
  std::vector<Animation>  mColorAnimations;
  std::vector<ColorTagCache> mColorTagCache;

  std::vector<BallState>  mBalls;
  std::array<Impact, 24u> mImpacts;

  std::vector<float>      mTrailBuffer;
  std::vector<float>      mImpactBuffer;
  std::vector<int>        mOwnerBuffer;
  std::vector<int>        mHitCountBuffer;
  std::vector<char>       mGlyphBuffer;

  std::vector<Tile>       mTiles;
  std::vector<uint8_t>    mDirtyTiles;
  std::vector<size_t>     mDirtyTileIndices;
  std::string             mSingleStarText;
  std::string             mSingleWorkingText;
  std::string             mSinglePreviousText;

  uint32_t                mTileCols   = 0u;
  uint32_t                mTileRows   = 0u;
  uint32_t                mTileCountX = 0u;
  uint32_t                mTileCountY = 0u;
  uint32_t                mSingleStarCols = 0u;
  uint32_t                mSingleStarRows = 0u;
  uint64_t                mSinglePreviousHash = 0u;

  DirtyRegion             mDirtyRegion;

  NoiseQuality            mNoiseQuality = NoiseQuality::FULL;
  RenderMode              mRenderMode   = RenderMode::SINGLE_TEXT_VISUALIZER;
  std::mt19937            mRandomEngine;

  float                   mTime                  = 0.0f;
  bool                    mBorderlineVisible     = false;
  bool                    mDirtyTileDebugVisible = false;
  bool                    mStarTwinkleEnabled    = false;
  bool                    mMarkupColorEnabled    = false;
  bool                    mDaliColorAnimationEnabled = false;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize START\n");
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize END\n");

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextVisualizerExperimentalController controller(application);
  application.MainLoop();

  return 0;
}
