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

#include "markdown-samples.h"

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/timer.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float PANEL_WIDTH     = 360.0f;
constexpr float PADDING         = 18.0f;
constexpr char  RESOURCE_NAME[] = "markdown_flag_kr.png";
constexpr int   MIN_STREAM_INTERVAL_MS  = 1;
constexpr int   STREAM_INTERVAL_STEP_MS = 10;

std::string gMarkdownSampleImagePath;

double ElapsedMs(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end)
{
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
}

bool FileExists(const std::string& path)
{
  std::ifstream file(path.c_str(), std::ios::binary);
  return file.good();
}

int NextFasterInterval(int currentIntervalMs)
{
  if(currentIntervalMs <= STREAM_INTERVAL_STEP_MS)
  {
    return MIN_STREAM_INTERVAL_MS;
  }
  return ((currentIntervalMs - 1) / STREAM_INTERVAL_STEP_MS) * STREAM_INTERVAL_STEP_MS;
}

int NextSlowerInterval(int currentIntervalMs)
{
  if(currentIntervalMs < STREAM_INTERVAL_STEP_MS)
  {
    return STREAM_INTERVAL_STEP_MS;
  }
  return ((currentIntervalMs / STREAM_INTERVAL_STEP_MS) + 1) * STREAM_INTERVAL_STEP_MS;
}

std::string DirectoryName(const char* path)
{
  if(!path)
  {
    return std::string();
  }

  const std::string value(path);
  const auto        slash = value.find_last_of('/');
  if(slash == std::string::npos)
  {
    return std::string();
  }
  if(slash == 0u)
  {
    return "/";
  }
  return value.substr(0u, slash);
}

std::string JoinPath(const std::string& directory, const std::string& path)
{
  if(directory.empty())
  {
    return path;
  }
  if(directory.back() == '/')
  {
    return directory + path;
  }
  return directory + "/" + path;
}

std::string ResolveMarkdownSampleImagePath(int argc, char** argv)
{
  std::vector<std::string> candidates;
  const std::string        executableDirectory = argc > 0 ? DirectoryName(argv[0]) : std::string();
  if(!executableDirectory.empty())
  {
    candidates.push_back(JoinPath(executableDirectory, std::string("../res/") + RESOURCE_NAME));
    candidates.push_back(JoinPath(executableDirectory, std::string("res/") + RESOURCE_NAME));
  }
  candidates.push_back(std::string("res/") + RESOURCE_NAME);
  candidates.push_back(std::string("../res/") + RESOURCE_NAME);

  for(const auto& candidate : candidates)
  {
    if(FileExists(candidate))
    {
      return candidate;
    }
  }

  return candidates.empty() ? std::string(RESOURCE_NAME) : candidates.front();
}

const std::vector<MarkdownSampleCase>& Cases()
{
  const std::string imagePath = gMarkdownSampleImagePath.empty() ? std::string("res/") + RESOURCE_NAME : gMarkdownSampleImagePath;
  return GetMarkdownSampleCases(imagePath);
}

std::size_t NextUtf8CharacterEnd(const std::string& text, std::size_t offset)
{
  if(offset >= text.size())
  {
    return text.size();
  }

  std::size_t next = offset + 1u;
  while(next < text.size() && (static_cast<unsigned char>(text[next]) & 0xC0u) == 0x80u)
  {
    ++next;
  }
  return next;
}

Label NewPanelLabel(float height, float fontSize)
{
  Label label = Label::New();
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  label.SetFontFamily("SamsungOneUI_400");
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0x111827));
  label.SetMultiLine(true);
  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
  return label;
}

} // namespace

class MarkdownViewExample : public ConnectionTracker
{
public:
  explicit MarkdownViewExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &MarkdownViewExample::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));
    window.KeyEventSignal().Connect(this, &MarkdownViewExample::OnKeyEvent);

    StackLayout root = StackLayout::New(StackOrientation::HORIZONTAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    window.Add(root);

    BuildPanel(root);
    BuildMarkdownArea(root);
    ShowCase(0u);
  }

private:
  enum class ChunkMode
  {
    CHARACTER,
    WORD,
    RANDOM
  };

  void BuildPanel(StackLayout& root)
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetBackgroundColor(UiColor(0xF8FAFC));
    panel.SetRequestedWidth(PANEL_WIDTH);
    panel.SetRequestedHeight(MATCH_PARENT);
    panel.SetPadding(Extents(static_cast<int16_t>(PADDING),
                             static_cast<int16_t>(PADDING),
                             static_cast<int16_t>(PADDING),
                             static_cast<int16_t>(PADDING)));
    panel.SetSpacing(12.0f);
    panel.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    root.Add(panel);

    mTitle = NewPanelLabel(46.0f, 24.0f);
    mTitle.SetText("MarkdownView");
    panel.Add(mTitle);

    mStatus = NewPanelLabel(430.0f, 16.0f);
    panel.Add(mStatus);

    mHelp = NewPanelLabel(WRAP_CONTENT, 15.0f);
    mHelp.SetText(
      "Keys\n"
      "N/P: next/prev case\n"
      "3/4: prev/next stream\n"
      "S: start or pause stream\n"
      "Space: step\n"
      "R: reset stream\n"
      "C: chunk mode\n"
      "I: refresh info\n"
      "+/-: speed\n"
      "L: LTR/RTL\n"
      "X: clear\n"
      "Esc/Back: quit");
    panel.Add(mHelp);
  }

  void BuildMarkdownArea(StackLayout& root)
  {
    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetRequestedHeight(MATCH_PARENT);
    mScrollView.SetLayoutParams(StackLayoutParams::New()
                                  .SetWeight(1.0f)
                                  .SetAlignment(LayoutAlignment::FILL));
    mContent = View::New();
    mContent.SetRequestedWidth(MATCH_PARENT);
    mContent.SetRequestedHeight(WRAP_CONTENT);
    mContent.SetPadding(Extents(PADDING, PADDING, PADDING, PADDING));

    mMarkdownView = MarkdownView::New();
    mMarkdownView.SetRequestedWidth(MATCH_PARENT);
    mMarkdownView.SetRequestedHeight(WRAP_CONTENT);
    mContent.Add(mMarkdownView);

    mScrollView.SetContent(mContent);
    root.Add(mScrollView);
  }

  void ShowCase(std::size_t index, bool startStreaming = false)
  {
    StopStreaming();
    mCaseIndex = index % Cases().size();
    ResetStreamingState();
    if(startStreaming)
    {
      RefreshStaticStatus();
      mMarkdownView.Clear();
      StartStreaming();
      return;
    }
    SetMarkdown(Cases()[mCaseIndex].markdown);
    RefreshStaticStatus();
  }

  void SetMarkdown(const std::string& source)
  {
    const auto start = std::chrono::steady_clock::now();
    mMarkdownView.SetMarkdown(Dali::String(source.c_str()));
    const auto end = std::chrono::steady_clock::now();
    mLastUpdateMs  = ElapsedMs(start, end);
  }

  void ResetStreamingState()
  {
    mCurrentSource.clear();
    mCurrentSource.reserve(CurrentMarkdown().size());
    mStreamByteOffset = 0u;
    mStreamStepCount  = 0u;
    mStreamElapsedMs  = 0.0;
    mStreamTimingActive = false;
  }

  void EnsureStreamTimingStarted()
  {
    if(!mStreamTimingActive)
    {
      mStreamStartedAt     = std::chrono::steady_clock::now();
      mStreamTimingActive  = true;
      mStreamElapsedMs     = 0.0;
    }
  }

  void RecordStreamStep()
  {
    mStreamStepCount++;
  }

  void FinishStreamTiming()
  {
    if(!mStreamTimingActive)
    {
      return;
    }

    mStreamElapsedMs    = ElapsedMs(mStreamStartedAt, std::chrono::steady_clock::now());
    mStreamTimingActive = false;
    std::fprintf(stderr,
                 "MarkdownView stream elapsed: case=\"%s\", steps=%u, wall=%.3f ms, source=%zu bytes, interval=%d ms, chunk=%s\n",
                 Cases()[mCaseIndex].title,
                 mStreamStepCount,
                 mStreamElapsedMs,
                 CurrentMarkdown().size(),
                 mIntervalMs,
                 ChunkModeText());
    RefreshStaticStatus();
    PrintStreamPlainText();
  }

  void PrintStreamPlainText()
  {
    const std::string& markdown      = CurrentMarkdown();
    const Dali::String plainText     = MarkdownView::ToPlainText(Dali::String(markdown.c_str()));
    const std::size_t  plainTextSize = plainText.Size();

    std::fprintf(stderr,
                 "MarkdownView stream plain text: case=\"%s\", plain=%zu bytes\n",
                 Cases()[mCaseIndex].title,
                 plainTextSize);
    std::fwrite(plainText.CStr(), 1u, plainTextSize, stderr);
    std::fprintf(stderr, "\n");
  }

  bool CompleteStreamingIfNeeded()
  {
    if(mStreamByteOffset < CurrentMarkdown().size())
    {
      return false;
    }

    StopStreaming();
    FinishStreamTiming();
    return true;
  }

  void StartStreaming()
  {
    if(!mTimer)
    {
      mTimer = Timer::New(static_cast<uint32_t>(mIntervalMs));
      mTimer.TickSignal().Connect(this, &MarkdownViewExample::OnTick);
    }
    if(!mTimer.IsRunning())
    {
      mTimer.Start();
    }
  }

  void StopStreaming()
  {
    if(mTimer)
    {
      mTimer.Stop();
    }
  }

  void CancelStreamingForConfigurationChange()
  {
    StopStreaming();
    ResetStreamingState();
    mMarkdownView.Clear();
  }

  bool OnTick()
  {
    StepStreaming();
    return !CompleteStreamingIfNeeded();
  }

  void StepStreaming()
  {
    const std::string& full = CurrentMarkdown();
    if(mStreamByteOffset >= full.size())
    {
      CompleteStreamingIfNeeded();
      return;
    }

    const std::size_t next = NextChunkOffset(full);
    mStreamByteOffset      = next;
    mCurrentSource.assign(full.data(), mStreamByteOffset);
    EnsureStreamTimingStarted();
    RecordStreamStep();
    SetMarkdown(mCurrentSource);
  }

  std::size_t NextChunkOffset(const std::string& text)
  {
    if(mChunkMode == ChunkMode::CHARACTER)
    {
      return NextUtf8CharacterEnd(text, mStreamByteOffset);
    }

    if(mChunkMode == ChunkMode::WORD)
    {
      std::size_t next = mStreamByteOffset;
      do
      {
        next = NextUtf8CharacterEnd(text, next);
      } while(next < text.size() && !std::isspace(static_cast<unsigned char>(text[next])));
      while(next < text.size() && std::isspace(static_cast<unsigned char>(text[next])))
      {
        next = NextUtf8CharacterEnd(text, next);
      }
      return next;
    }

    const uint32_t chars = 1u + (mRandom() % 8u);
    std::size_t    next  = mStreamByteOffset;
    for(uint32_t i = 0u; i < chars && next < text.size(); ++i)
    {
      next = NextUtf8CharacterEnd(text, next);
    }
    return next;
  }

  const std::string& CurrentMarkdown() const
  {
    return Cases()[mCaseIndex].markdown;
  }

  const char* ChunkModeText() const
  {
    switch(mChunkMode)
    {
      case ChunkMode::CHARACTER:
        return "character";
      case ChunkMode::WORD:
        return "word";
      case ChunkMode::RANDOM:
        return "random";
    }
    return "unknown";
  }

  void RefreshStaticStatus()
  {
    std::ostringstream status;
    status << "Case: " << Cases()[mCaseIndex].title << "\n"
           << "Chunk: " << ChunkModeText() << "\n"
           << "Interval: " << mIntervalMs << " ms\n"
           << "Direction: " << (mIsRtl ? "RTL" : "LTR") << "\n"
           << "Source: " << CurrentMarkdown().size() << " bytes";
    if(!mStreamTimingActive && mStreamStepCount > 0u)
    {
      status << "\nResult: " << mStreamElapsedMs << " ms";
    }
    mStatus.SetText(Dali::String(status.str().c_str()));
  }

  void RecreateTimer()
  {
    const bool wasRunning = mTimer && mTimer.IsRunning();
    if(mTimer)
    {
      mTimer.Stop();
    }
    mTimer = Timer::New(static_cast<uint32_t>(mIntervalMs));
    mTimer.TickSignal().Connect(this, &MarkdownViewExample::OnTick);
    if(wasRunning)
    {
      mTimer.Start();
    }
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const std::string key = event.GetKeyName().CStr();
    if(key == "n" || key == "N")
    {
      ShowCase(mCaseIndex + 1u);
    }
    else if(key == "p" || key == "P")
    {
      ShowCase(mCaseIndex + Cases().size() - 1u);
    }
    else if(key == "3" || key == "KP_3")
    {
      ShowCase(mCaseIndex + Cases().size() - 1u, true);
    }
    else if(key == "4" || key == "KP_4")
    {
      ShowCase(mCaseIndex + 1u, true);
    }
    else if(key == "s" || key == "S")
    {
      if(mStreamByteOffset == 0u || mStreamByteOffset >= CurrentMarkdown().size())
      {
        ResetStreamingState();
        mMarkdownView.Clear();
        RefreshStaticStatus();
        StartStreaming();
      }
      else if(mTimer && mTimer.IsRunning())
      {
        mTimer.Stop();
      }
      else
      {
        StartStreaming();
      }
    }
    else if(key == "space" || key == "Space")
    {
      if(mStreamTimingActive)
      {
        StepStreaming();
        CompleteStreamingIfNeeded();
      }
    }
    else if(key == "r" || key == "R")
    {
      StopStreaming();
      ResetStreamingState();
      mMarkdownView.Clear();
    }
    else if(key == "x" || key == "X")
    {
      StopStreaming();
      ResetStreamingState();
      mMarkdownView.Clear();
    }
    else if(key == "c" || key == "C")
    {
      CancelStreamingForConfigurationChange();
      mChunkMode = static_cast<ChunkMode>((static_cast<int>(mChunkMode) + 1) % 3);
      RefreshStaticStatus();
    }
    else if(key == "i" || key == "I")
    {
      if(!mStreamTimingActive)
      {
        RefreshStaticStatus();
      }
    }
    else if(key == "plus" || key == "KP_Add" || key == "+")
    {
      CancelStreamingForConfigurationChange();
      mIntervalMs = NextFasterInterval(mIntervalMs);
      RecreateTimer();
      RefreshStaticStatus();
    }
    else if(key == "minus" || key == "KP_Subtract" || key == "-")
    {
      CancelStreamingForConfigurationChange();
      mIntervalMs = NextSlowerInterval(mIntervalMs);
      RecreateTimer();
      RefreshStaticStatus();
    }
    else if(key == "l" || key == "L")
    {
      CancelStreamingForConfigurationChange();
      mIsRtl = !mIsRtl;
      mContent.SetLayoutDirection(mIsRtl ? Dali::LayoutDirection::RIGHT_TO_LEFT : Dali::LayoutDirection::LEFT_TO_RIGHT);
      RefreshStaticStatus();
    }
    else if(event.GetKeyName() == "q")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "w")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(event.GetKeyName() == "e")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
  }

private:
  Application& mApplication;
  ScrollView   mScrollView;
  View         mContent;
  MarkdownView mMarkdownView;
  Label        mTitle;
  Label        mStatus;
  Label        mHelp;
  Timer        mTimer;

  std::size_t      mCaseIndex{0u};
  std::size_t      mStreamByteOffset{0u};
  std::string      mCurrentSource;
  ChunkMode        mChunkMode{ChunkMode::CHARACTER};
  int              mIntervalMs{1};
  bool             mIsRtl{false};
  double           mLastUpdateMs{0.0};
  double           mStreamElapsedMs{0.0};
  bool             mStreamTimingActive{false};
  uint32_t         mStreamStepCount{0u};
  std::chrono::steady_clock::time_point mStreamStartedAt;
  std::minstd_rand mRandom{17u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application  = Application::New(&argc, &argv);
  gMarkdownSampleImagePath = ResolveMarkdownSampleImagePath(argc, argv);
  UiConfig config          = UiConfig::New();
  config.SetLabelAsyncRendering(true);
  config.Apply();
  MarkdownViewExample controller(application);
  application.MainLoop();
  return 0;
}
