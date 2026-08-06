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

#include <array>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float    PANEL_WIDTH             = 360.0f;
constexpr float    PANEL_PADDING           = 14.0f;
constexpr float    CONTENT_PADDING         = 18.0f;
constexpr float    MENU_BUTTON_HEIGHT      = 38.0f;
constexpr float    MENU_ROW_SPACING        = 6.0f;
constexpr int      MIN_STREAM_INTERVAL_MS  = 1;
constexpr int      STREAM_INTERVAL_STEP_MS = 10;
constexpr uint32_t AUTO_TEST_DELAY_MS      = 1000u;

constexpr std::array<float, 5u> UI_SCALES{{0.8f, 1.0f, 1.2f, 1.5f, 2.0f}};

double ElapsedMs(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end)
{
  const auto elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  return static_cast<double>(elapsedMicroseconds) / 1000.0;
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

const std::vector<MarkdownSampleCase>& Cases()
{
  return GetMarkdownSampleCases(RESOURCES_DIR "markdown_flag_kr.png");
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

Label NewPanelLabel(const char* text, float height, float fontSize)
{
  Label label = Label::New(text);
  label.SetAsyncRendering(false);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  label.SetFontFamily("SamsungOneUI_500");
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0xCBD5E1));
  label.SetMultiLine(true);
  label.SetTextOverflowMode(Text::OverflowMode::CLIP);
  return label;
}

Label NewMenuButton(const char* text)
{
  Label button = NewPanelLabel(text, MENU_BUTTON_HEIGHT, 14.0f);
  button.SetMultiLine(false);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(UiColor(0x1F2937));
  button.SetCornerRadius(7.0f);
  button.SetBorderlineWidth(1.0f);
  button.SetBorderlineOffset(-1.0f);
  button.SetBorderlineColor(UiColor(0x475569));
  button.SetPadding(Extents(8, 8, 0, 0));
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  return button;
}

StackLayout NewMenuRow()
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(MENU_BUTTON_HEIGHT);
  row.SetSpacing(MENU_ROW_SPACING);
  row.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
  return row;
}

void SetMenuButton(Label              button,
                   const std::string& text,
                   const UiColor&     backgroundColor = UiColor(0x1F2937),
                   const UiColor&     borderlineColor = UiColor(0x475569),
                   const UiColor&     textColor       = UiColor(0xCBD5E1))
{
  button.SetText(text.c_str());
  button.SetTextColor(textColor);
  button.SetBackgroundColor(backgroundColor);
  button.SetBorderlineColor(borderlineColor);
}

void SetMenuToggleButton(Label button, const char* text, bool selected)
{
  SetMenuButton(button,
                text,
                selected ? UiColor(0x1D4ED8) : UiColor(0x1F2937),
                selected ? UiColor(0x93C5FD) : UiColor(0x475569),
                selected ? UiColor(0xF8FAFC) : UiColor(0xCBD5E1));
}

void SetMenuActionButton(Label button, const char* text)
{
  SetMenuButton(button, text, UiColor(0x0F5FA8), UiColor(0x60A5FA), UiColor(0xF8FAFC));
}

MarkdownViewStyle CreateMarkdownViewExampleStyle()
{
  return MarkdownViewStyle::Builder()
    .SetTextFontFamily("SamsungOneUI_400")
    .SetHeadingFontFamily("SamsungOneUI_700")
    .SetCodeFontFamily("SamsungOneUI_300")
    .SetTextFontSize(20.0f)
    .SetHeading1FontSize(28.0f)
    .SetHeading2FontSize(24.0f)
    .SetHeading3FontSize(20.0f)
    .SetHeading4FontSize(16.0f)
    .SetHeading5FontSize(12.0f)
    .SetHeading6FontSize(10.0f)
    .SetCodeBlockFontSize(20.0f)
    .SetCodeBlockTitleFontSize(16.0f)
    .SetTextColor(UiColor(0xEFEFEF))
    .SetHeadingTextColor(UiColor(0xEFEFEF))
    .SetQuoteTextColor(UiColor(0xDFDFDF))
    .SetCodeTextColor(UiColor(0xEFEFEF))
    .SetCodeBlockTitleTextColor(UiColor(0xE1E1E1))
    .SetInlineCodeBackgroundColor(UiColor(0x000000u, 0.0f))
    .SetCodeBlockBackgroundColor(UiColor(0x030303))
    .SetCodeBlockTitleBackgroundColor(UiColor(0x333333))
    .SetQuoteBarColor(UiColor(0xDFDFDF))
    .SetThematicBreakColor(UiColor(0xDFDFDF))
    .SetTableRuleColor(UiColor(0xFFFFFF))
    .Build();
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
    window.SetBackgroundColor(UiColor(0x0F161E));
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
    ScrollView panelScroll = ScrollView::New();
    panelScroll.SetScrollDirection(ScrollDirection::Vertical);
    panelScroll.SetBackgroundColor(UiColor(0x111827));
    panelScroll.SetRequestedWidth(PANEL_WIDTH);
    panelScroll.SetRequestedHeight(MATCH_PARENT);
    panelScroll.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetBackgroundColor(UiColor(0x111827));
    panel.SetRequestedWidth(MATCH_PARENT);
    panel.SetRequestedHeight(WRAP_CONTENT);
    panel.SetPadding(Extents(static_cast<int16_t>(PANEL_PADDING),
                             static_cast<int16_t>(PANEL_PADDING),
                             static_cast<int16_t>(PANEL_PADDING),
                             static_cast<int16_t>(PANEL_PADDING)));
    panel.SetSpacing(8.0f);
    panel.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    panelScroll.SetContent(panel);
    root.Add(panelScroll);

    Label title = NewPanelLabel("MarkdownView", 42.0f, 24.0f);
    title.SetTextColor(UiColor(0xF8FAFC));
    panel.Add(title);

    StackLayout caseRow = NewMenuRow();
    mPreviousCaseButton = NewMenuButton("Previous");
    mCaseButton         = NewMenuButton("");
    mNextCaseButton     = NewMenuButton("Next");
    caseRow.Add(mPreviousCaseButton);
    caseRow.Add(mCaseButton);
    caseRow.Add(mNextCaseButton);
    panel.Add(caseRow);

    panel.Add(NewPanelLabel("STREAM", 22.0f, 12.0f));
    StackLayout streamRow = NewMenuRow();
    mStreamButton         = NewMenuButton("Start");
    mStepButton           = NewMenuButton("Step");
    mResetButton          = NewMenuButton("Reset");
    streamRow.Add(mStreamButton);
    streamRow.Add(mStepButton);
    streamRow.Add(mResetButton);
    panel.Add(streamRow);

    panel.Add(NewPanelLabel("CHUNK MODE", 22.0f, 12.0f));
    StackLayout chunkRow  = NewMenuRow();
    mCharacterChunkButton = NewMenuButton("Char");
    mWordChunkButton      = NewMenuButton("Word");
    mRandomChunkButton    = NewMenuButton("Random");
    chunkRow.Add(mCharacterChunkButton);
    chunkRow.Add(mWordChunkButton);
    chunkRow.Add(mRandomChunkButton);
    panel.Add(chunkRow);

    panel.Add(NewPanelLabel("INTERVAL", 22.0f, 12.0f));
    StackLayout intervalRow = NewMenuRow();
    mFasterButton           = NewMenuButton("Faster");
    mIntervalButton         = NewMenuButton("");
    mSlowerButton           = NewMenuButton("Slower");
    intervalRow.Add(mFasterButton);
    intervalRow.Add(mIntervalButton);
    intervalRow.Add(mSlowerButton);
    panel.Add(intervalRow);

    panel.Add(NewPanelLabel("DIRECTION", 22.0f, 12.0f));
    StackLayout directionRow = NewMenuRow();
    mLtrButton               = NewMenuButton("LTR");
    mRtlButton               = NewMenuButton("RTL");
    directionRow.Add(mLtrButton);
    directionRow.Add(mRtlButton);
    panel.Add(directionRow);

    panel.Add(NewPanelLabel("UI SCALE", 22.0f, 12.0f));
    StackLayout scaleRow = NewMenuRow();
    for(std::size_t index = 0u; index < UI_SCALES.size(); ++index)
    {
      mScaleButtons[index] = NewMenuButton("");
      scaleRow.Add(mScaleButtons[index]);
    }
    panel.Add(scaleRow);

    panel.Add(NewPanelLabel("AUTO TEST", 22.0f, 12.0f));
    mAutoTestButton = NewMenuButton("Start Auto Test");
    panel.Add(mAutoTestButton);

    mMetrics = NewPanelLabel("", 116.0f, 13.0f);
    mMetrics.SetBackgroundColor(UiColor(0x0F172A));
    mMetrics.SetCornerRadius(7.0f);
    mMetrics.SetBorderlineWidth(1.0f);
    mMetrics.SetBorderlineOffset(-1.0f);
    mMetrics.SetBorderlineColor(UiColor(0x334155));
    mMetrics.SetPadding(Extents(10, 10, 7, 7));
    panel.Add(mMetrics);

    Label help = NewPanelLabel(
      "Keys: N/P case  3/4 stream case  S stream  Space step\n"
      "C chunk  +/- speed  L direction  Q/W/E scale  0/9 auto  R/X reset  Esc quit",
      52.0f,
      11.0f);
    panel.Add(help);

    ConnectPanelActions();
  }

  void ConnectPanelActions()
  {
    mPreviousCaseButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ShowCase(mCaseIndex + Cases().size() - 1u);
    });
    mNextCaseButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ShowCase(mCaseIndex + 1u);
    });
    mStreamButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleStreaming();
    });
    mStepButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      StepOnce();
    });
    mResetButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ResetStream();
    });
    mCharacterChunkButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetChunkMode(ChunkMode::CHARACTER);
    });
    mWordChunkButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetChunkMode(ChunkMode::WORD);
    });
    mRandomChunkButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetChunkMode(ChunkMode::RANDOM);
    });
    mFasterButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetStreamInterval(NextFasterInterval(mIntervalMs));
    });
    mIntervalButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetStreamInterval(MIN_STREAM_INTERVAL_MS);
    });
    mSlowerButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetStreamInterval(NextSlowerInterval(mIntervalMs));
    });
    mLtrButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetLayoutDirection(false);
    });
    mRtlButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetLayoutDirection(true);
    });
    for(std::size_t index = 0u; index < mScaleButtons.size(); ++index)
    {
      mScaleButtons[index].AsInteractive().ClickedSignal().Connect(this, [this, index](View, InputEvent)
      {
        SetUiScale(index);
      });
    }
    mAutoTestButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleAutoTest();
    });
  }

  void BuildMarkdownArea(StackLayout& root)
  {
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedHeight(MATCH_PARENT);
    scrollView.SetLayoutParams(StackLayoutParams::New()
                                 .SetWeight(1.0f)
                                 .SetAlignment(LayoutAlignment::FILL));
    mContent = View::New();
    mContent.SetRequestedWidth(MATCH_PARENT);
    mContent.SetRequestedHeight(WRAP_CONTENT);
    mContent.SetBackgroundColor(UiColor(0x0F161E));
    mContent.SetPadding(Extents(static_cast<int16_t>(CONTENT_PADDING),
                                static_cast<int16_t>(CONTENT_PADDING),
                                static_cast<int16_t>(CONTENT_PADDING),
                                static_cast<int16_t>(CONTENT_PADDING)));

    const MarkdownViewStyle markdownStyle = CreateMarkdownViewExampleStyle();
    mMarkdownView                         = MarkdownView::New(markdownStyle);
    mMarkdownView.SetRequestedWidth(MATCH_PARENT);
    mMarkdownView.SetRequestedHeight(WRAP_CONTENT);
    mContent.Add(mMarkdownView);

    scrollView.SetContent(mContent);
    root.Add(scrollView);
  }

  void ShowCase(std::size_t index, bool startStreaming = false)
  {
    DisableAutoTest();
    StopStreaming();
    mCaseIndex = index % Cases().size();
    ResetStreamingState();
    ClearStreamResult();
    if(startStreaming)
    {
      mMarkdownView.Clear();
      StartStreaming();
      RefreshPanel();
      return;
    }
    SetMarkdown(Cases()[mCaseIndex].markdown);
    RefreshPanel();
  }

  double SetMarkdown(const std::string& source)
  {
    const auto start = std::chrono::steady_clock::now();
    mMarkdownView.SetMarkdown(Dali::String(source.c_str()));
    const auto end     = std::chrono::steady_clock::now();
    mLastSetMarkdownMs = ElapsedMs(start, end);
    return mLastSetMarkdownMs;
  }

  void ResetStreamingState()
  {
    mCurrentSource.clear();
    mCurrentSource.reserve(CurrentMarkdown().size());
    mStreamByteOffset           = 0u;
    mStreamStepCount            = 0u;
    mStreamElapsedMs            = 0.0;
    mStreamSetMarkdownElapsedMs = 0.0;
    mStreamTimingActive         = false;
  }

  void ClearStreamResult()
  {
    mHasStreamResult                = false;
    mLastStreamElapsedMs            = 0.0;
    mLastStreamSetMarkdownElapsedMs = 0.0;
    mLastStreamStepCount            = 0u;
  }

  void EnsureStreamTimingStarted()
  {
    if(!mStreamTimingActive)
    {
      mStreamStartedAt    = std::chrono::steady_clock::now();
      mStreamTimingActive = true;
      mStreamElapsedMs    = 0.0;
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

    mStreamElapsedMs                = ElapsedMs(mStreamStartedAt, std::chrono::steady_clock::now());
    mStreamTimingActive             = false;
    mHasStreamResult                = true;
    mLastStreamElapsedMs            = mStreamElapsedMs;
    mLastStreamSetMarkdownElapsedMs = mStreamSetMarkdownElapsedMs;
    mLastStreamStepCount            = mStreamStepCount;
    std::fprintf(stderr,
                 "MarkdownView stream: case=%zu/%zu, steps=%u, wall=%.3f ms, set-markdown=%.3f ms, source=%zu bytes, interval=%d ms, chunk=%s\n",
                 mCaseIndex + 1u,
                 Cases().size(),
                 mStreamStepCount,
                 mStreamElapsedMs,
                 mStreamSetMarkdownElapsedMs,
                 CurrentMarkdown().size(),
                 mIntervalMs,
                 ChunkModeText());
    LogPlainTextSummary();
    CompleteAutoTestRun();
    RefreshPanel();
  }

  void LogPlainTextSummary()
  {
    const std::string& markdown      = CurrentMarkdown();
    const Dali::String plainText     = MarkdownView::ToPlainText(Dali::String(markdown.c_str()));
    const std::size_t  plainTextSize = plainText.Size();

    std::fprintf(stderr,
                 "MarkdownView plain text: case=%zu/%zu, source=%zu bytes, plain=%zu bytes\n",
                 mCaseIndex + 1u,
                 Cases().size(),
                 markdown.size(),
                 plainTextSize);
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
    ClearStreamResult();
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
    mStreamSetMarkdownElapsedMs += SetMarkdown(mCurrentSource);
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

    const uint32_t chars = 1u + static_cast<uint32_t>(mRandom() % 8u);
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

  void RefreshPanel()
  {
    if(mStreamTimingActive)
    {
      return;
    }

    SetMenuActionButton(mPreviousCaseButton, "Previous");
    SetMenuButton(mCaseButton,
                  "Case " + std::to_string(mCaseIndex + 1u) + " / " + std::to_string(Cases().size()),
                  UiColor(0x334155),
                  UiColor(0x64748B),
                  UiColor(0xF8FAFC));
    SetMenuActionButton(mNextCaseButton, "Next");

    const bool streamRunning = mTimer && mTimer.IsRunning();
    SetMenuButton(mStreamButton,
                  streamRunning ? "Pause" : "Start",
                  streamRunning ? UiColor(0xB45309) : UiColor(0x047857),
                  streamRunning ? UiColor(0xFDE68A) : UiColor(0x6EE7B7),
                  UiColor(0xF8FAFC));
    SetMenuActionButton(mStepButton, "Step");
    SetMenuButton(mResetButton, "Reset", UiColor(0x7C2D12), UiColor(0xFDBA74), UiColor(0xF8FAFC));

    SetMenuToggleButton(mCharacterChunkButton, "Char", mChunkMode == ChunkMode::CHARACTER);
    SetMenuToggleButton(mWordChunkButton, "Word", mChunkMode == ChunkMode::WORD);
    SetMenuToggleButton(mRandomChunkButton, "Random", mChunkMode == ChunkMode::RANDOM);

    SetMenuActionButton(mFasterButton, "Faster");
    SetMenuButton(mIntervalButton,
                  std::to_string(mIntervalMs) + " ms",
                  UiColor(0x334155),
                  UiColor(0x64748B),
                  UiColor(0xF8FAFC));
    SetMenuActionButton(mSlowerButton, "Slower");

    SetMenuToggleButton(mLtrButton, "LTR", !mIsRtl);
    SetMenuToggleButton(mRtlButton, "RTL", mIsRtl);

    for(std::size_t index = 0u; index < UI_SCALES.size(); ++index)
    {
      std::ostringstream scaleText;
      scaleText << std::fixed << std::setprecision(1) << UI_SCALES[index];
      SetMenuButton(mScaleButtons[index],
                    scaleText.str(),
                    index == mUiScaleIndex ? UiColor(0x1D4ED8) : UiColor(0x1F2937),
                    index == mUiScaleIndex ? UiColor(0x93C5FD) : UiColor(0x475569),
                    index == mUiScaleIndex ? UiColor(0xF8FAFC) : UiColor(0xCBD5E1));
    }

    SetMenuButton(mAutoTestButton,
                  mAutoTestRunning ? "Stop Auto Test" : "Start Auto Test",
                  mAutoTestRunning ? UiColor(0x7C2D12) : UiColor(0x047857),
                  mAutoTestRunning ? UiColor(0xFDBA74) : UiColor(0x6EE7B7),
                  UiColor(0xF8FAFC));

    std::ostringstream metrics;
    metrics << std::fixed << std::setprecision(2)
            << "Last SetMarkdown: " << mLastSetMarkdownMs << " ms\n";
    if(mHasStreamResult)
    {
      metrics << "Stream wall: " << mLastStreamElapsedMs << " ms\n"
              << "SetMarkdown total: " << mLastStreamSetMarkdownElapsedMs << " ms\n"
              << "Steps: " << mLastStreamStepCount;
    }
    else
    {
      metrics << "Stream wall: --\nSetMarkdown total: --\nSteps: --";
    }
    metrics << "\nAuto: " << (mAutoTestRunning ? "Running" : "Stopped") << " / " << mAutoTestRunCount << " completed";
    mMetrics.SetText(metrics.str().c_str());
  }

  void DisableAutoTest()
  {
    mAutoTestRunning = false;
    if(mAutoTestTimer)
    {
      mAutoTestTimer.Stop();
    }
  }

  void ToggleStreaming()
  {
    DisableAutoTest();
    if(mStreamByteOffset == 0u || mStreamByteOffset >= CurrentMarkdown().size())
    {
      ResetStreamingState();
      ClearStreamResult();
      mMarkdownView.Clear();
      StartStreaming();
      RefreshPanel();
    }
    else if(mTimer && mTimer.IsRunning())
    {
      StopStreaming();
      RefreshPanel();
    }
    else
    {
      StartStreaming();
      RefreshPanel();
    }
  }

  void StepOnce()
  {
    DisableAutoTest();
    StopStreaming();
    if(mStreamByteOffset >= CurrentMarkdown().size())
    {
      ResetStreamingState();
      ClearStreamResult();
    }
    if(mStreamByteOffset == 0u)
    {
      mMarkdownView.Clear();
    }
    StepStreaming();
    CompleteStreamingIfNeeded();
    RefreshPanel();
  }

  void ResetStream()
  {
    DisableAutoTest();
    StopStreaming();
    ResetStreamingState();
    ClearStreamResult();
    mMarkdownView.Clear();
    RefreshPanel();
  }

  void SetChunkMode(ChunkMode mode)
  {
    if(mChunkMode == mode)
    {
      return;
    }
    DisableAutoTest();
    CancelStreamingForConfigurationChange();
    mChunkMode = mode;
    RefreshPanel();
  }

  void SetStreamInterval(int intervalMs)
  {
    if(mIntervalMs == intervalMs)
    {
      return;
    }
    DisableAutoTest();
    CancelStreamingForConfigurationChange();
    mIntervalMs = intervalMs;
    RecreateTimer();
    RefreshPanel();
  }

  void SetLayoutDirection(bool rtl)
  {
    if(mIsRtl == rtl)
    {
      return;
    }
    DisableAutoTest();
    mIsRtl = rtl;
    mContent.SetLayoutDirection(rtl ? Dali::LayoutDirection::RIGHT_TO_LEFT : Dali::LayoutDirection::LEFT_TO_RIGHT);
    RefreshPanel();
  }

  void SetUiScale(std::size_t index)
  {
    if(index >= UI_SCALES.size() || mUiScaleIndex == index)
    {
      return;
    }
    DisableAutoTest();
    mUiScaleIndex = index;
    UiScaleManager::Get().SetScale(UI_SCALES[index]);
    RefreshPanel();
  }

  void ToggleAutoTest()
  {
    if(mAutoTestRunning)
    {
      StopAutoTest();
    }
    else
    {
      StartAutoTest();
    }
  }

  void StartAutoTest()
  {
    StopStreaming();
    if(mAutoTestTimer)
    {
      mAutoTestTimer.Stop();
    }

    mAutoTestRunning       = true;
    mAutoTestRunCount      = 0u;
    mAutoTestNextCaseIndex = mCaseIndex;
    ClearStreamResult();

    std::fprintf(stderr,
                 "MarkdownView auto test started: cases=%zu, interval=%d ms, chunk=%s\n",
                 Cases().size(),
                 mIntervalMs,
                 ChunkModeText());
    StartNextAutoTestCase();
  }

  void StopAutoTest()
  {
    if(!mAutoTestRunning)
    {
      return;
    }

    mAutoTestRunning = false;
    if(mAutoTestTimer)
    {
      mAutoTestTimer.Stop();
    }
    StopStreaming();
    if(mStreamTimingActive)
    {
      mStreamTimingActive = false;
    }
    std::fprintf(stderr, "MarkdownView auto test stopped: completed=%llu\n", static_cast<unsigned long long>(mAutoTestRunCount));
    RefreshPanel();
  }

  void StartNextAutoTestCase()
  {
    if(!mAutoTestRunning)
    {
      return;
    }

    StopStreaming();
    mCaseIndex             = mAutoTestNextCaseIndex % Cases().size();
    mAutoTestNextCaseIndex = (mCaseIndex + 1u) % Cases().size();
    ResetStreamingState();
    mMarkdownView.Clear();
    StartStreaming();
    RefreshPanel();
  }

  void CompleteAutoTestRun()
  {
    if(!mAutoTestRunning)
    {
      return;
    }

    ++mAutoTestRunCount;
    if(!mAutoTestTimer)
    {
      mAutoTestTimer = Timer::New(AUTO_TEST_DELAY_MS);
      mAutoTestTimer.TickSignal().Connect(this, &MarkdownViewExample::OnAutoTestTick);
    }
    mAutoTestTimer.Start();
  }

  bool OnAutoTestTick()
  {
    StartNextAutoTestCase();
    return false;
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
      ToggleStreaming();
    }
    else if(key == "space" || key == "Space")
    {
      StepOnce();
    }
    else if(key == "r" || key == "R" || key == "x" || key == "X")
    {
      ResetStream();
    }
    else if(key == "c" || key == "C")
    {
      SetChunkMode(static_cast<ChunkMode>((static_cast<int>(mChunkMode) + 1) % 3));
    }
    else if(key == "i" || key == "I")
    {
      RefreshPanel();
    }
    else if(key == "plus" || key == "KP_Add" || key == "+")
    {
      SetStreamInterval(NextFasterInterval(mIntervalMs));
    }
    else if(key == "minus" || key == "KP_Subtract" || key == "-")
    {
      SetStreamInterval(NextSlowerInterval(mIntervalMs));
    }
    else if(key == "l" || key == "L")
    {
      SetLayoutDirection(!mIsRtl);
    }
    else if(key == "q" || key == "Q")
    {
      SetUiScale(1u);
    }
    else if(key == "w" || key == "W")
    {
      SetUiScale(3u);
    }
    else if(key == "e" || key == "E")
    {
      SetUiScale(4u);
    }
    else if(key == "0" || key == "KP_0")
    {
      if(!mAutoTestRunning)
      {
        StartAutoTest();
      }
    }
    else if(key == "9" || key == "KP_9")
    {
      StopAutoTest();
    }
  }

private:
  Application&          mApplication;
  View                  mContent;
  MarkdownView          mMarkdownView;
  Label                 mPreviousCaseButton;
  Label                 mCaseButton;
  Label                 mNextCaseButton;
  Label                 mStreamButton;
  Label                 mStepButton;
  Label                 mResetButton;
  Label                 mCharacterChunkButton;
  Label                 mWordChunkButton;
  Label                 mRandomChunkButton;
  Label                 mFasterButton;
  Label                 mIntervalButton;
  Label                 mSlowerButton;
  Label                 mLtrButton;
  Label                 mRtlButton;
  std::array<Label, 5u> mScaleButtons;
  Label                 mAutoTestButton;
  Label                 mMetrics;
  Timer                 mTimer;
  Timer                 mAutoTestTimer;

  std::size_t                           mCaseIndex{0u};
  std::size_t                           mStreamByteOffset{0u};
  std::size_t                           mUiScaleIndex{1u};
  std::size_t                           mAutoTestNextCaseIndex{0u};
  std::string                           mCurrentSource;
  ChunkMode                             mChunkMode{ChunkMode::CHARACTER};
  int                                   mIntervalMs{1};
  bool                                  mIsRtl{false};
  bool                                  mStreamTimingActive{false};
  bool                                  mHasStreamResult{false};
  bool                                  mAutoTestRunning{false};
  double                                mLastSetMarkdownMs{0.0};
  double                                mStreamElapsedMs{0.0};
  double                                mStreamSetMarkdownElapsedMs{0.0};
  double                                mLastStreamElapsedMs{0.0};
  double                                mLastStreamSetMarkdownElapsedMs{0.0};
  uint32_t                              mStreamStepCount{0u};
  uint32_t                              mLastStreamStepCount{0u};
  uint64_t                              mAutoTestRunCount{0u};
  std::chrono::steady_clock::time_point mStreamStartedAt;
  std::minstd_rand                      mRandom{17u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.SetLabelAsyncRendering(true);
  config.Apply();
  MarkdownViewExample controller(application);
  application.MainLoop();
  return 0;
}
