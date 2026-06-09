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
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr float    LABEL_WIDTH               = 96.0f;
constexpr float    LABEL_HEIGHT              = 96.0f;
constexpr float    LABEL_SPACING             = 4.0f;
constexpr uint32_t DEFAULT_PHASE_TIMEOUT_MS  = 60000u;
constexpr size_t   DEFAULT_RUNS              = 5u;
constexpr float    DEFAULT_INITIAL_FONT_SIZE = 48.0f;

struct PerfCase
{
  std::vector<uint32_t> codepoints;
  std::string           status;
  std::string           signature;
  std::string           name;
};

struct PerfOptions
{
  std::string fixturePath;
  std::string fontFamily{"SamsungOneUI_400"};
  size_t      maxItems{0u};
  size_t      runs{DEFAULT_RUNS};
  uint32_t    phaseTimeoutMs{DEFAULT_PHASE_TIMEOUT_MS};
  float       initialFontSize{DEFAULT_INITIAL_FONT_SIZE};
};

struct PerfPhase
{
  const char* name;
  float       fontSize;
  size_t      textOffset;
};

const char* GetEnv(const char* name)
{
  return std::getenv(name);
}

size_t GetEnvSize(const char* name, size_t fallback)
{
  const char* value = GetEnv(name);
  return value ? static_cast<size_t>(std::strtoull(value, nullptr, 10)) : fallback;
}

float GetEnvFloat(const char* name, float fallback)
{
  const char* value = GetEnv(name);
  return value ? std::strtof(value, nullptr) : fallback;
}

std::vector<std::string> SplitTabs(const std::string& line)
{
  std::vector<std::string> result;
  std::string              item;
  std::stringstream        stream(line);
  while(std::getline(stream, item, '\t'))
  {
    result.push_back(item);
  }
  return result;
}

std::vector<uint32_t> ParseCodepoints(const std::string& text)
{
  std::vector<uint32_t> codepoints;
  std::stringstream     stream(text);
  std::string           item;
  while(stream >> item)
  {
    if(item.compare(0u, 2u, "U+") == 0u)
    {
      item = item.substr(2u);
    }
    codepoints.push_back(static_cast<uint32_t>(std::stoul(item, nullptr, 16)));
  }
  return codepoints;
}

void AppendUtf8(std::string& output, uint32_t codepoint)
{
  if(codepoint <= 0x7Fu)
  {
    output.push_back(static_cast<char>(codepoint));
  }
  else if(codepoint <= 0x7FFu)
  {
    output.push_back(static_cast<char>(0xC0u | (codepoint >> 6u)));
    output.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
  else if(codepoint <= 0xFFFFu)
  {
    output.push_back(static_cast<char>(0xE0u | (codepoint >> 12u)));
    output.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
  else
  {
    output.push_back(static_cast<char>(0xF0u | (codepoint >> 18u)));
    output.push_back(static_cast<char>(0x80u | ((codepoint >> 12u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
    output.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
  }
}

std::string CodepointsToUtf8(const std::vector<uint32_t>& codepoints)
{
  std::string text;
  for(uint32_t codepoint : codepoints)
  {
    AppendUtf8(text, codepoint);
  }
  return text;
}

std::string FindFixturePath()
{
  const char* envPath = GetEnv("DALI_EMOJI_PERF_FIXTURE");
  if(envPath && envPath[0] != '\0')
  {
    return envPath;
  }

  const std::vector<std::string> candidates{
    "automated-tests/emoji/res/emoji-test-latest.tsv",
    "../emoji/res/emoji-test-latest.tsv",
    "../../emoji/res/emoji-test-latest.tsv",
    "../src/dali-ui-foundation-internal/resources/emoji/emoji-test-latest.tsv"};

  for(const std::string& candidate : candidates)
  {
    std::ifstream input(candidate.c_str());
    if(input.good())
    {
      return candidate;
    }
  }

  return "automated-tests/emoji/res/emoji-test-latest.tsv";
}

PerfOptions LoadOptions()
{
  PerfOptions options;
  options.fixturePath    = FindFixturePath();
  options.maxItems       = GetEnvSize("DALI_EMOJI_PERF_MAX_ITEMS", 0u);
  options.runs           = std::max<size_t>(1u, GetEnvSize("DALI_EMOJI_PERF_RUNS", DEFAULT_RUNS));
  options.phaseTimeoutMs = static_cast<uint32_t>(GetEnvSize("DALI_EMOJI_PERF_TIMEOUT_MS", DEFAULT_PHASE_TIMEOUT_MS));
  options.initialFontSize = GetEnvFloat("DALI_EMOJI_PERF_FONT_SIZE", DEFAULT_INITIAL_FONT_SIZE);

  const char* fontFamily = GetEnv("DALI_EMOJI_PERF_FONT_FAMILY");
  if(fontFamily && fontFamily[0] != '\0')
  {
    options.fontFamily = fontFamily;
  }

  return options;
}

std::vector<PerfCase> LoadCases(const PerfOptions& options)
{
  std::ifstream input(options.fixturePath.c_str());
  if(!input)
  {
    throw std::runtime_error("failed to open emoji perf fixture: " + options.fixturePath);
  }

  std::vector<PerfCase> items;
  std::string           line;
  while(std::getline(input, line))
  {
    if(line.empty() || line[0] == '#')
    {
      continue;
    }

    const std::vector<std::string> fields = SplitTabs(line);
    if(fields.size() < 4u)
    {
      continue;
    }

    PerfCase item;
    item.codepoints = ParseCodepoints(fields[0]);
    item.status     = fields[1];
    item.signature  = fields[2];
    item.name       = fields[3];
    items.push_back(item);

    if(options.maxItems > 0u && items.size() >= options.maxItems)
    {
      break;
    }
  }

  return items;
}

uint64_t ElapsedMilliseconds(const std::chrono::steady_clock::time_point& start)
{
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count());
}

class EmojiLabelPerfController : public ConnectionTracker
{
public:
  EmojiLabelPerfController(Application& application, const PerfOptions& options, std::vector<PerfCase>&& items)
  : mApplication(application),
    mOptions(options),
    mItems(std::move(items))
  {
    mApplication.InitSignal().Connect(this, &EmojiLabelPerfController::OnInit);
  }

  int ExitStatus() const
  {
    return mExitStatus;
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);

    if(mItems.empty())
    {
      std::cerr << "emoji label perf failed: no items selected\n";
      mExitStatus = 2;
      mApplication.Quit();
      return;
    }

    BuildTextItems();
    BuildLabels();
    BuildPhases();

    std::cout << "LABEL_PERF_BEGIN"
              << " fixture=" << mOptions.fixturePath
              << " count=" << mLabels.size()
              << " runs=" << mOptions.runs
              << " fontFamily=" << mOptions.fontFamily
              << " async=1\n";

    ScheduleNextPhase();
  }

  void BuildTextItems()
  {
    mTexts.reserve(mItems.size());
    for(const PerfCase& item : mItems)
    {
      mTexts.push_back(CodepointsToUtf8(item.codepoints));
    }
  }

  void BuildLabels()
  {
    const size_t columns = 8u;
    const size_t rows    = (mItems.size() + columns - 1u) / columns;

    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(columns * (LABEL_WIDTH + LABEL_SPACING));
    mRoot.SetRequestedHeight(rows * (LABEL_HEIGHT + LABEL_SPACING));
    mRoot.SetBackgroundColor(Color::WHITE);
    mWindow.Add(mRoot);

    mLabels.reserve(mItems.size());
    for(size_t index = 0u; index < mItems.size(); ++index)
    {
      const size_t row    = index / columns;
      const size_t column = index % columns;

      Label label = Label::New();
      label.SetRequestedWidth(LABEL_WIDTH);
      label.SetRequestedHeight(LABEL_HEIGHT);
      label.SetFontFamily(mOptions.fontFamily.c_str());
      label.SetFontSize(mOptions.initialFontSize);
      label.SetTextColor(Color::BLACK);
      label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      label.SetVerticalTextAlignment(Text::Alignment::CENTER);
      label.SetMultiLine(false);
      label.SetAsyncRendering(true);
      label.SetLayoutParams(AbsoluteLayoutParams::New()
                              .SetX(column * (LABEL_WIDTH + LABEL_SPACING))
                              .SetY(row * (LABEL_HEIGHT + LABEL_SPACING))
                              .SetWidth(LABEL_WIDTH)
                              .SetHeight(LABEL_HEIGHT));
      label.AsyncRenderFinishedSignal().Connect(this, &EmojiLabelPerfController::OnAsyncRenderFinished);
      mRoot.Add(label);
      mLabels.push_back(label);
    }
  }

  void BuildPhases()
  {
    mPhases.push_back({"initial_48", mOptions.initialFontSize, 0u});
    mPhases.push_back({"same_text_48", mOptions.initialFontSize, 1u});
    mPhases.push_back({"scale_32", 32.0f, 2u});
    mPhases.push_back({"scale_64", 64.0f, 3u});
  }

  void ScheduleNextPhase()
  {
    if(mNextPhaseTimer)
    {
      mNextPhaseTimer.Stop();
      mNextPhaseTimer.Reset();
    }

    mNextPhaseTimer = Timer::New(1u);
    mNextPhaseTimer.TickSignal().Connect(this, &EmojiLabelPerfController::OnNextPhaseTimer);
    mNextPhaseTimer.Start();
  }

  bool OnNextPhaseTimer()
  {
    if(mNextPhaseTimer)
    {
      mNextPhaseTimer.Stop();
      mNextPhaseTimer.Reset();
    }

    StartCurrentPhase();
    return false;
  }

  void StartCurrentPhase()
  {
    if(mCurrentRun >= mOptions.runs)
    {
      std::cout << "LABEL_PERF_DONE status=OK\n";
      mApplication.Quit();
      return;
    }

    if(mCurrentPhase >= mPhases.size())
    {
      ++mCurrentRun;
      mCurrentPhase = 0u;
      StartCurrentPhase();
      return;
    }

    const PerfPhase& phase = mPhases[mCurrentPhase];
    mCompleted.assign(mLabels.size(), false);
    mCompletedCount  = 0u;
    mPhaseActive     = true;
    mPhaseStart      = std::chrono::steady_clock::now();

    const std::chrono::steady_clock::time_point updateStart = std::chrono::steady_clock::now();
    for(size_t index = 0u; index < mLabels.size(); ++index)
    {
      const size_t textIndex = (index + phase.textOffset + mCurrentRun) % mTexts.size();
      mLabels[index].SetFontSize(phase.fontSize);
      mLabels[index].SetText(mTexts[textIndex].c_str());
    }
    mUpdateCallMs = ElapsedMilliseconds(updateStart);

    StartTimeoutTimer();
  }

  void StartTimeoutTimer()
  {
    if(mTimeoutTimer)
    {
      mTimeoutTimer.Stop();
      mTimeoutTimer.Reset();
    }

    mTimeoutTimer = Timer::New(mOptions.phaseTimeoutMs);
    mTimeoutTimer.TickSignal().Connect(this, &EmojiLabelPerfController::OnTimeout);
    mTimeoutTimer.Start();
  }

  bool OnTimeout()
  {
    const PerfPhase& phase = mPhases[mCurrentPhase];
    std::cout << "LABEL_PERF_PHASE"
              << " run=" << (mCurrentRun + 1u)
              << " phase=" << phase.name
              << " fontSize=" << phase.fontSize
              << " count=" << mLabels.size()
              << " completed=" << mCompletedCount
              << " updateCallMs=" << mUpdateCallMs
              << " asyncAllMs=" << ElapsedMilliseconds(mPhaseStart)
              << " status=TIMEOUT\n";
    mPhaseActive = false;
    mExitStatus = 2;
    mApplication.Quit();
    return false;
  }

  void OnAsyncRenderFinished(View view, float /*width*/, float /*height*/)
  {
    Label label = Label::DownCast(view);
    if(!mPhaseActive || !label || mCompleted.empty() || mCurrentRun >= mOptions.runs || mCurrentPhase >= mPhases.size())
    {
      return;
    }

    for(size_t index = 0u; index < mLabels.size(); ++index)
    {
      if(!mCompleted[index] && mLabels[index] == label)
      {
        mCompleted[index] = true;
        ++mCompletedCount;
        break;
      }
    }

    if(mCompletedCount == mLabels.size())
    {
      FinishCurrentPhase();
    }
  }

  void FinishCurrentPhase()
  {
    if(mTimeoutTimer)
    {
      mTimeoutTimer.Stop();
      mTimeoutTimer.Reset();
    }

    const PerfPhase& phase = mPhases[mCurrentPhase];
    mPhaseActive = false;
    std::cout << "LABEL_PERF_PHASE"
              << " run=" << (mCurrentRun + 1u)
              << " phase=" << phase.name
              << " fontSize=" << phase.fontSize
              << " count=" << mLabels.size()
              << " completed=" << mCompletedCount
              << " updateCallMs=" << mUpdateCallMs
              << " asyncAllMs=" << ElapsedMilliseconds(mPhaseStart)
              << " status=OK\n";

    ++mCurrentPhase;
    ScheduleNextPhase();
  }

private:
  Application&                                     mApplication;
  PerfOptions                                     mOptions;
  std::vector<PerfCase>                           mItems;
  std::vector<std::string>                        mTexts;
  Window                                          mWindow;
  AbsoluteLayout                                  mRoot;
  std::vector<Label>                              mLabels;
  std::vector<PerfPhase>                          mPhases;
  std::vector<bool>                               mCompleted;
  Timer                                           mNextPhaseTimer;
  Timer                                           mTimeoutTimer;
  std::chrono::steady_clock::time_point           mPhaseStart;
  size_t                                          mCurrentRun{0u};
  size_t                                          mCurrentPhase{0u};
  size_t                                          mCompletedCount{0u};
  uint64_t                                        mUpdateCallMs{0u};
  int                                             mExitStatus{0};
  bool                                            mPhaseActive{false};
};

} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  try
  {
    PerfOptions options = LoadOptions();
    std::vector<PerfCase> items = LoadCases(options);

    Application application = Application::New(&argc, &argv);
    UiConfig config = UiConfig::New();
    config.SetLabelAsyncRendering(true);
    config.Apply();
    EmojiLabelPerfController controller(application, options, std::move(items));
    application.MainLoop();
    return controller.ExitStatus();
  }
  catch(const std::exception& exception)
  {
    std::cerr << "emoji label perf failed: " << exception.what() << '\n';
    return 2;
  }
}
