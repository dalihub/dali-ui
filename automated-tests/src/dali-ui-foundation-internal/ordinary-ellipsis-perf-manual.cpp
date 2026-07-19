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

#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using Clock = std::chrono::steady_clock;

constexpr uint32_t WARM_UP_COUNT = 3u;
constexpr uint32_t SAMPLE_COUNT  = 10u;

struct Case
{
  const char*           name;
  const char*           text;
  Size                  size;
  bool                  multiline;
  float                 characterSpacing;
  float                 renderScale;
  LayoutDirection::Type direction;
  bool                  ellipsis;
  bool                  widthSweep;
  bool                  heightSweep;
};

struct Timing
{
  double totalMicroseconds{0.0};
  double glyphMicroseconds{0.0};
};

double Microseconds(const Clock::time_point& start)
{
  return std::chrono::duration<double, std::micro>(Clock::now() - start).count();
}

double Median(std::vector<double>& samples)
{
  std::sort(samples.begin(), samples.end());
  const std::size_t middle = samples.size() / 2u;
  return samples.size() % 2u ? samples[middle] : 0.5 * (samples[middle - 1u] + samples[middle]);
}

Timing RunSync(const Case& item)
{
  const auto start = Clock::now();

  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText(item.text);
  controller->SetDefaultFontSize(18.0f, Text::Controller::PIXEL_SIZE);
  controller->SetMultiLineEnabled(item.multiline);
  controller->SetLineWrapMode(Text::LineWrapMode::WORD);
  controller->SetTextElideEnabled(item.ellipsis);
  controller->SetEllipsisPosition(Text::EllipsisPosition::END);
  controller->SetCharacterSpacing(item.characterSpacing);
  controller->SetRenderScale(item.renderScale);

  static constexpr std::array<float, 6u> WIDTHS{{80.0f, 104.0f, 128.0f, 160.0f, 200.0f, 240.0f}};
  static constexpr std::array<float, 6u> HEIGHTS{{8.0f, 16.0f, 24.0f, 36.0f, 52.0f, 72.0f}};
  if(item.widthSweep)
  {
    for(float width : WIDTHS)
    {
      controller->Relayout(Size(width, item.size.height), item.direction);
    }
  }
  else if(item.heightSweep)
  {
    for(float height : HEIGHTS)
    {
      controller->Relayout(Size(item.size.width, height), item.direction);
    }
  }
  else
  {
    controller->Relayout(item.size, item.direction);
  }
  const double total = Microseconds(start);

  const Text::Length glyphCount = controller->GetView().GetNumberOfGlyphs();
  std::vector<Text::GlyphInfo> glyphs(glyphCount);
  std::vector<Vector2>         positions(glyphCount);
  float                        minimumLineOffset = 0.0f;
  const auto                   glyphStart        = Clock::now();
  controller->GetView().GetGlyphs(glyphs.data(), positions.data(), minimumLineOffset, 0u, glyphCount);
  const double glyph = Microseconds(glyphStart);
  return {total, glyph};
}

Timing RunAsync(const Case& item)
{
  Text::AsyncTextParameters parameters;
  parameters.text               = item.text;
  parameters.fontSize           = 18.0f;
  parameters.textWidth          = item.size.width;
  parameters.textHeight         = item.size.height;
  parameters.originWidth        = item.size.width;
  parameters.originHeight       = item.size.height;
  parameters.isMultiLine        = item.multiline;
  parameters.lineWrapMode       = Text::LineWrapMode::WORD;
  parameters.ellipsis           = item.ellipsis;
  parameters.ellipsisPosition   = Text::EllipsisPosition::END;
  parameters.characterSpacing   = item.characterSpacing;
  parameters.renderScale        = item.renderScale;
  parameters.layoutDirection    = item.direction;
  parameters.maxTextureSize     = 4096;
  parameters.requestType        = Text::Async::RENDER_FIXED_SIZE;

  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  const auto            start  = Clock::now();
  static constexpr std::array<float, 6u> WIDTHS{{80.0f, 104.0f, 128.0f, 160.0f, 200.0f, 240.0f}};
  static constexpr std::array<float, 6u> HEIGHTS{{8.0f, 16.0f, 24.0f, 36.0f, 52.0f, 72.0f}};
  auto render = [&]()
  {
    bool cachedNaturalSize = false;
    Size naturalSize       = Size::ZERO;
    if(item.renderScale > 1.0f)
    {
      naturalSize = loader.SetupRenderScale(parameters, cachedNaturalSize);
    }
    loader.RenderText(parameters, cachedNaturalSize, naturalSize);
  };
  if(item.widthSweep)
  {
    for(float width : WIDTHS)
    {
      parameters.textWidth   = width;
      parameters.originWidth = width;
      render();
    }
  }
  else if(item.heightSweep)
  {
    for(float height : HEIGHTS)
    {
      parameters.textHeight   = height;
      parameters.originHeight = height;
      render();
    }
  }
  else
  {
    render();
  }
  return {Microseconds(start), 0.0};
}

void RunBenchmarks()
{
  TextAbstraction::FontClient::Get().InitDefaultFontDescription();

  static constexpr const char* LONG_TEXT =
    "Ordinary text with enough words to exercise END ellipsis repeatedly across layout thresholds.";
  static constexpr std::array<Case, 11u> CASES{{
    {"plain_no_ellipsis", LONG_TEXT, Size(160.0f, 36.0f), false, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, false, false, false},
    {"single_end", LONG_TEXT, Size(160.0f, 36.0f), false, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"multiline_end", LONG_TEXT, Size(132.0f, 54.0f), true, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"rtl_end", "\xD7\x90\xD7\x91 \xD7\x92\xD7\x93 \xD7\x94\xD7\x95 ordinary tail", Size(150.0f, 36.0f), false, 0.0f, 1.0f, LayoutDirection::RIGHT_TO_LEFT, true, false, false},
    {"mixed_bidi_end", "ABC \xD7\x90\xD7\x91\xD7\x92 DEF \xD7\x93\xD7\x94\xD7\x95 ordinary tail", Size(150.0f, 36.0f), false, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"vertical_fallback", LONG_TEXT, Size(160.0f, 8.0f), false, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"spacing_positive", LONG_TEXT, Size(160.0f, 36.0f), false, 2.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"spacing_negative", LONG_TEXT, Size(160.0f, 36.0f), false, -1.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"render_scale_2", LONG_TEXT, Size(160.0f, 36.0f), false, 0.0f, 2.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, false},
    {"width_sweep", LONG_TEXT, Size(160.0f, 54.0f), true, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, true, false},
    {"height_sweep", LONG_TEXT, Size(132.0f, 54.0f), true, 0.0f, 1.0f, LayoutDirection::LEFT_TO_RIGHT, true, false, true},
  }};

  for(const Case& item : CASES)
  {
    for(uint32_t warmUp = 0u; warmUp < WARM_UP_COUNT; ++warmUp)
    {
      RunSync(item);
      RunAsync(item);
    }

    std::vector<double> syncTotal;
    std::vector<double> syncGlyph;
    std::vector<double> asyncTotal;
    syncTotal.reserve(SAMPLE_COUNT);
    syncGlyph.reserve(SAMPLE_COUNT);
    asyncTotal.reserve(SAMPLE_COUNT);
    for(uint32_t sample = 0u; sample < SAMPLE_COUNT; ++sample)
    {
      const Timing sync = RunSync(item);
      const Timing async = RunAsync(item);
      syncTotal.push_back(sync.totalMicroseconds);
      syncGlyph.push_back(sync.glyphMicroseconds);
      asyncTotal.push_back(async.totalMicroseconds);
    }

    std::cout << "ORDINARY_ELLIPSIS_PERF case=" << item.name
              << " sync_total_median_us=" << Median(syncTotal)
              << " sync_get_glyphs_median_us=" << Median(syncGlyph)
              << " async_total_median_us=" << Median(asyncTotal)
              << " samples=" << SAMPLE_COUNT << '\n';
  }
}

class BenchmarkRunner : public ConnectionTracker
{
public:
  explicit BenchmarkRunner(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &BenchmarkRunner::OnInit);
  }

private:
  void OnInit(Application)
  {
    RunBenchmarks();
    mQuitTimer = Timer::New(1u);
    mQuitTimer.TickSignal().Connect(this, &BenchmarkRunner::OnQuit);
    mQuitTimer.Start();
  }

  bool OnQuit()
  {
    mApplication.Quit();
    return false;
  }

private:
  Application& mApplication;
  Timer        mQuitTimer;
};
} // unnamed namespace

int main(int argc, char** argv)
{
  Application     application = Application::New(&argc, &argv);
  BenchmarkRunner runner(application);
  application.MainLoop();
  return 0;
}
