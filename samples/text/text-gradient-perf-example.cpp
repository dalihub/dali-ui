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

#include <dali-ui-foundation/dali-ui-foundation.h>

#if defined(__GLIBC__)
#include <malloc.h>
#endif

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int      WINDOW_WIDTH         = 1920;
constexpr int      WINDOW_HEIGHT        = 1080;
constexpr uint32_t DEFAULT_LABEL_COUNT  = 300u;
constexpr uint32_t GRID_COLUMNS         = 20u;
constexpr float    HUD_HEIGHT           = 112.0f;
constexpr float    GRID_SPACING         = 3.0f;
constexpr float    SMALL_SIZE_SCALE     = 0.85f;
constexpr float    LARGE_SIZE_SCALE     = 1.15f;
constexpr float    FONT_SIZE_RATIO      = 0.36f;
constexpr float    MIN_LABEL_FONT_SIZE  = 12.0f;
constexpr float    MASK_RATIO_STEP      = 0.05f;
constexpr float    GRADIENT_OFFSET_STEP = 0.12f;
constexpr uint32_t HEARTBEAT_INTERVAL_MS = 100u;
constexpr uint32_t DEFER_OPERATION_MS    = 60u;
constexpr uint32_t POST_OPERATION_MEMORY_DELAY_MS = 1500u;

enum class PerfRenderMode
{
  TEXT_GRADIENT,
  MASK,
};

enum class PendingOperation
{
  NONE,
  CLEAR,
  RECREATE,
  LIVE_RESIZE,
};

struct GradientSpec
{
  Vector2                start{-0.5f, 0.0f};
  Vector2                end{0.5f, 0.0f};
  Gradient::Units        units{Gradient::Units::OBJECT_BOUNDING_BOX};
  Gradient::SpreadMethod spreadMethod{Gradient::SpreadMethod::PAD};
  float                  startOffset{0.0f};
};

struct MaskGradientBounds
{
  float xRatio{0.0f};
  float yRatio{0.0f};
  float widthRatio{1.0f};
  float heightRatio{1.0f};
};

struct MemorySnapshot
{
  long pssKb{0};
  long rssKb{0};

  bool allocatorAvailable{false};
  long mallocUsedKb{0};

  uint32_t    labelCount{0u};
  std::size_t labelVectorSize{0u};
  std::size_t labelVectorCapacity{0u};
};

const char* GetModeName(PerfRenderMode mode)
{
  return mode == PerfRenderMode::TEXT_GRADIENT ? "TextGradient" : "Mask";
}

UiColor GetModeColor(PerfRenderMode mode)
{
  return mode == PerfRenderMode::TEXT_GRADIENT ? UiColor(0x0F5FA8) : UiColor(0xA32655);
}

UiColor GetTimingColor(double elapsedMs, double stallMs)
{
  const double value = std::max(elapsedMs, stallMs);
  if(value >= 400.0)
  {
    return UiColor(0xB91C1C);
  }
  if(value >= 120.0)
  {
    return UiColor(0xB45309);
  }
  return UiColor(0x047857);
}

UiColor GetMemoryColor(long deltaKb)
{
  const long absDeltaKb = std::labs(deltaKb);
  if(absDeltaKb >= 16 * 1024)
  {
    return UiColor(0xB91C1C);
  }
  if(absDeltaKb >= 4 * 1024)
  {
    return UiColor(0xB45309);
  }
  return UiColor(0x155E75);
}

double KbToMb(long kb)
{
  return static_cast<double>(kb) / 1024.0;
}

std::string FormatMb(long kb)
{
  std::ostringstream text;
  text << std::fixed << std::setprecision(1) << KbToMb(kb) << "MB";
  return text.str();
}

std::string FormatDeltaMb(long kb)
{
  std::ostringstream text;
  text << std::fixed << std::setprecision(1) << (kb >= 0 ? "+" : "") << KbToMb(kb) << "MB";
  return text.str();
}

bool ParseProcMetricKb(const char* line, const char* key, long& value)
{
  const std::size_t keyLength = std::strlen(key);
  if(std::strncmp(line, key, keyLength) != 0)
  {
    return false;
  }

  long parsedValue = 0;
#if defined(_MSC_VER)
  if(sscanf_s(line + keyLength, "%ld", &parsedValue) == 1)
#else
  if(std::sscanf(line + keyLength, "%ld", &parsedValue) == 1)
#endif
  {
    value = parsedValue;
    return true;
  }
  return false;
}

MemorySnapshot ReadProcessMemorySnapshot()
{
  MemorySnapshot snapshot;
  FILE*          statusFile = nullptr;
#if defined(_MSC_VER)
  fopen_s(&statusFile, "/proc/self/status", "r");
#else
  statusFile = std::fopen("/proc/self/status", "r");
#endif
  if(FILE* file = statusFile)
  {
    char line[128];
    while(std::fgets(line, sizeof(line), file))
    {
      ParseProcMetricKb(line, "VmRSS:", snapshot.rssKb);
    }
    std::fclose(file);
  }

  FILE* smapsFile = nullptr;
#if defined(_MSC_VER)
  fopen_s(&smapsFile, "/proc/self/smaps_rollup", "r");
#else
  smapsFile = std::fopen("/proc/self/smaps_rollup", "r");
#endif
  if(FILE* file = smapsFile)
  {
    char line[128];
    while(std::fgets(line, sizeof(line), file))
    {
      ParseProcMetricKb(line, "Pss:", snapshot.pssKb);
      ParseProcMetricKb(line, "Rss:", snapshot.rssKb);
    }
    std::fclose(file);
  }

  if(snapshot.pssKb == 0)
  {
    snapshot.pssKb = snapshot.rssKb;
  }

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 33)
  const struct mallinfo2 mallocInfo = mallinfo2();
  snapshot.allocatorAvailable       = true;
  snapshot.mallocUsedKb             = static_cast<long>(mallocInfo.uordblks / 1024u);
#else
  const struct mallinfo mallocInfo = mallinfo();
  snapshot.allocatorAvailable      = true;
  snapshot.mallocUsedKb            = mallocInfo.uordblks / 1024;
#endif
#elif defined(__GLIBC__)
  const struct mallinfo mallocInfo = mallinfo();
  snapshot.allocatorAvailable      = true;
  snapshot.mallocUsedKb            = mallocInfo.uordblks / 1024;
#endif

  return snapshot;
}

constexpr std::array<const char*, 3u> TEXT_PRESETS{{
  "Gradient",
  "TextGradient",
  "DALi UI",
}};

using Clock = std::chrono::steady_clock;
} // namespace

class TextGradientPerfExample : public ConnectionTracker
{
public:
  explicit TextGradientPerfExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextGradientPerfExample::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    auto   positionSize = window.GetPositionSize();
    window.SetPositionSize(PositionSize(positionSize.x, positionSize.y, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0x10131A));

    mWindowWidth  = static_cast<float>(WINDOW_WIDTH);
    mWindowHeight = static_cast<float>(WINDOW_HEIGHT);

    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);

    CreateHud();

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &TextGradientPerfExample::OnKeyEvent);

    const auto start = Clock::now();
    CreateGrid();
    mLastOperationMs = LogElapsed("create", start);
    mLastOperation   = "create";
    SetMemoryBaseline("startup");
    UpdateHud();
    StartHeartbeat();
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    auto keyName = event.GetKeyName();
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }
    if(keyName == "1")
    {
      SwitchMode(PerfRenderMode::TEXT_GRADIENT);
    }
    else if(keyName == "2")
    {
      SwitchMode(PerfRenderMode::MASK);
    }
    else if(keyName == "r" || keyName == "R")
    {
      ResizeGridByRecreate();
    }
    else if(keyName == "s" || keyName == "S")
    {
      ResizeGridLive();
    }
    else if(keyName == "d" || keyName == "D")
    {
      ScheduleClear("clear");
    }
    else if(keyName == "n" || keyName == "N")
    {
      ScheduleRecreate("new");
    }
    else if(keyName == "t" || keyName == "T")
    {
      UpdateAllText();
    }
    else if(keyName == "c" || keyName == "C")
    {
      ScheduleRecreate("recreate");
    }
    else if(keyName == "space" || keyName == "Space" || keyName == " ")
    {
      ScheduleRecreate("stress");
    }
    else if(keyName == "a" || keyName == "A")
    {
      ToggleAsyncRendering();
    }
    else if(keyName == "g" || keyName == "G")
    {
      UpdateGradientSpec();
    }
    else if(keyName == "h" || keyName == "H")
    {
      mHelpVisible = !mHelpVisible;
      mLastOperation = mHelpVisible ? "help_show" : "help_hide";
      mRunningOperation = false;
      UpdateHud();
    }
    else if(keyName == "bracketleft")
    {
      AdjustMaskWidth(-MASK_RATIO_STEP);
    }
    else if(keyName == "bracketright")
    {
      AdjustMaskWidth(MASK_RATIO_STEP);
    }
    else if(keyName == "semicolon")
    {
      AdjustMaskHeight(-MASK_RATIO_STEP);
    }
    else if(keyName == "apostrophe")
    {
      AdjustMaskHeight(MASK_RATIO_STEP);
    }
  }

  void SwitchMode(PerfRenderMode mode)
  {
    mMode = mode;
    ScheduleRecreate(mode == PerfRenderMode::TEXT_GRADIENT ? "switch_text_gradient" : "switch_mask");
  }

  void ToggleAsyncRendering()
  {
    mAsyncRendering = !mAsyncRendering;
    ScheduleRecreate(mAsyncRendering ? "async_on" : "sync_on");
  }

  void UpdateGradientSpec()
  {
    mGradientSpec.startOffset += GRADIENT_OFFSET_STEP;
    if(mGradientSpec.startOffset > 1.0f)
    {
      mGradientSpec.startOffset = 0.0f;
    }
    ScheduleRecreate("gradient_offset");
  }

  void ResizeGridByRecreate()
  {
    mLargeSize = !mLargeSize;
    ScheduleRecreate(mLargeSize ? "resize_recreate_large" : "resize_recreate_small");
  }

  void ResizeGridLive()
  {
    mLargeSize = !mLargeSize;
    ScheduleOperation(PendingOperation::LIVE_RESIZE, mLargeSize ? "resize_live_large" : "resize_live_small");
  }

  void UpdateAllText()
  {
    CancelPendingMemoryRefresh();

    const auto start = Clock::now();

    mTextIndex = (mTextIndex + 1u) % TEXT_PRESETS.size();
    for(auto& label : mLabels)
    {
      label.SetText(TEXT_PRESETS[mTextIndex]);
    }

    mLastOperationMs = LogElapsed("set_text", start);
    mLastOperation = "set_text";
    mRunningOperation = false;
    UpdateMemoryAfterOperation();
    UpdateHud();
  }

  void AdjustMaskWidth(float delta)
  {
    mMaskBounds.widthRatio = std::clamp(mMaskBounds.widthRatio + delta, 0.20f, 1.50f);
    ScheduleRecreate("mask_width_ratio");
  }

  void AdjustMaskHeight(float delta)
  {
    mMaskBounds.heightRatio = std::clamp(mMaskBounds.heightRatio + delta, 0.20f, 1.50f);
    ScheduleRecreate("mask_height_ratio");
  }

  void ScheduleClear(const char* operation)
  {
    ScheduleOperation(PendingOperation::CLEAR, operation);
  }

  void ScheduleRecreate(const char* operation)
  {
    ScheduleOperation(PendingOperation::RECREATE, operation);
  }

  void ScheduleOperation(PendingOperation operation, const char* operationName)
  {
    CancelPendingMemoryRefresh();

    if(mDeferredOperationTimer)
    {
      mDeferredOperationTimer.Stop();
      mDeferredOperationTimer.Reset();
    }

    mPendingOperation     = operation;
    mPendingOperationName = operationName;
    mRunningOperation     = true;
    mLastOperation        = operationName;
    mLastOperationMs      = 0.0;
    mLastStallMs          = 0.0;
    UpdateHud();

    mDeferredOperationTimer = Timer::New(DEFER_OPERATION_MS);
    mDeferredOperationTimer.TickSignal().Connect(this, &TextGradientPerfExample::OnDeferredOperationTimerTick);
    mDeferredOperationTimer.Start();
  }

  bool OnDeferredOperationTimerTick()
  {
    if(mDeferredOperationTimer)
    {
      mDeferredOperationTimer.Stop();
      mDeferredOperationTimer.Reset();
    }

    const PendingOperation operation     = mPendingOperation;
    const std::string      operationName = mPendingOperationName;
    mPendingOperation                    = PendingOperation::NONE;

    if(operation == PendingOperation::CLEAR)
    {
      ClearGrid(operationName.c_str());
    }
    else if(operation == PendingOperation::RECREATE)
    {
      RecreateGrid(operationName.c_str());
    }
    else if(operation == PendingOperation::LIVE_RESIZE)
    {
      ResizeGridInPlace(operationName.c_str());
    }

    return false;
  }

  void RecreateGrid(const char* operation)
  {
    const auto start = Clock::now();
    ClearGrid(nullptr);
    CreateGrid();
    mLastOperationMs = LogElapsed(operation, start);
    mLastOperation = operation;
    mRunningOperation = false;
    UpdateMemoryAfterOperation();
    UpdateHud();
  }

  void ClearGrid(const char* operation)
  {
    const auto start = operation ? Clock::now() : Clock::time_point{};

    if(mGridRoot)
    {
      mGridRoot.Unparent();
      mGridRoot.Reset();
    }
    mLabels.clear();
    mMaskViews.clear();

    if(operation)
    {
      mLastOperationMs = LogElapsed(operation, start);
      mLastOperation = operation;
      mRunningOperation = false;
      UpdateMemoryAfterOperation();
      UpdateHud();
    }
  }

  void CreateGrid()
  {
    if(mGridRoot)
    {
      ClearGrid(nullptr);
    }

    mGridRoot = AbsoluteLayout::New();
    mGridRoot.SetRequestedWidth(mWindowWidth);
    mGridRoot.SetRequestedHeight(mWindowHeight - HUD_HEIGHT);
    mGridRoot.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, HUD_HEIGHT, mWindowWidth, mWindowHeight - HUD_HEIGHT)));
    mRoot.Add(mGridRoot);

    mLabels.reserve(DEFAULT_LABEL_COUNT);
    mMaskViews.reserve(mMode == PerfRenderMode::MASK ? DEFAULT_LABEL_COUNT : 0u);
    mLabelSize = CalculateLabelSize();

    for(uint32_t index = 0u; index < DEFAULT_LABEL_COUNT; ++index)
    {
      Label label = CreateLabel(mLabelSize.x, mLabelSize.y);
      label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(CalculateLabelBounds(index, mLabelSize)));

      if(mMode == PerfRenderMode::TEXT_GRADIENT)
      {
        label.SetTextGradient(CreateTextGradient());
      }
      else
      {
        View maskView = CreateMaskGradientView(mLabelSize.x, mLabelSize.y);
        label.SetMaskEffect(maskView);
        mMaskViews.push_back(maskView);
      }

      mGridRoot.Add(label);
      mLabels.push_back(label);
    }
  }

  void ResizeGridInPlace(const char* operation)
  {
    const auto start = Clock::now();

    mLabelSize = CalculateLabelSize();
    const uint32_t labelCount = static_cast<uint32_t>(mLabels.size());
    for(uint32_t index = 0u; index < labelCount; ++index)
    {
      Label& label = mLabels[index];
      label.SetRequestedWidth(mLabelSize.x);
      label.SetRequestedHeight(mLabelSize.y);
      label.SetFontSize(CalculateLabelFontSize(mLabelSize.y));
      label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(CalculateLabelBounds(index, mLabelSize)));

      if(mMode == PerfRenderMode::MASK)
      {
        if(index < mMaskViews.size())
        {
          ResizeMaskGradientView(mMaskViews[index], mLabelSize.x, mLabelSize.y);
        }
      }
    }

    mLastOperationMs = LogElapsed(operation, start);
    mLastOperation = operation;
    mRunningOperation = false;
    UpdateMemoryAfterOperation();
    UpdateHud();
  }

  Vector2 CalculateLabelSize() const
  {
    const uint32_t rows            = (DEFAULT_LABEL_COUNT + GRID_COLUMNS - 1u) / GRID_COLUMNS;
    const float    availableWidth  = mWindowWidth;
    const float    availableHeight = mWindowHeight - HUD_HEIGHT;
    const float    cellWidth       = availableWidth / static_cast<float>(GRID_COLUMNS);
    const float    cellHeight      = availableHeight / static_cast<float>(rows);
    const float    scale           = mLargeSize ? LARGE_SIZE_SCALE : SMALL_SIZE_SCALE;
    const float    labelWidth      = std::max(1.0f, cellWidth * scale - GRID_SPACING);
    const float    labelHeight     = std::max(1.0f, cellHeight * scale - GRID_SPACING);

    return Vector2(labelWidth, labelHeight);
  }

  LayoutRect CalculateLabelBounds(uint32_t index, const Vector2& labelSize) const
  {
    const uint32_t rows       = (DEFAULT_LABEL_COUNT + GRID_COLUMNS - 1u) / GRID_COLUMNS;
    const float    cellWidth  = mWindowWidth / static_cast<float>(GRID_COLUMNS);
    const float    cellHeight = (mWindowHeight - HUD_HEIGHT) / static_cast<float>(rows);
    const uint32_t column     = index % GRID_COLUMNS;
    const uint32_t row        = index / GRID_COLUMNS;
    const float    x          = column * cellWidth + (cellWidth - labelSize.x) * 0.5f;
    const float    y          = row * cellHeight + (cellHeight - labelSize.y) * 0.5f;

    return LayoutRect(x, y, labelSize.x, labelSize.y);
  }

  float CalculateLabelFontSize(float labelHeight) const
  {
    return std::max(MIN_LABEL_FONT_SIZE, labelHeight * FONT_SIZE_RATIO);
  }

  Label CreateLabel(float width, float height) const
  {
    Label label = Label::New(TEXT_PRESETS[mTextIndex]);
    label.SetAsyncRendering(mAsyncRendering);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetFontFamily("SamsungOneUI_700");
    label.SetFontSize(CalculateLabelFontSize(height));
    label.SetMultiLine(false);
    label.SetRequestedWidth(width);
    label.SetRequestedHeight(height);
    label.SetPadding(Insets(0.0f, 0.0f, 0.0f, 0.0f));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  Gradient::Linear CreateTextGradient() const
  {
    Gradient::Linear gradient(mGradientSpec.start, mGradientSpec.end);
    gradient.SetUnits(mGradientSpec.units);
    gradient.SetSpreadMethod(mGradientSpec.spreadMethod);
    gradient.SetStartOffset(mGradientSpec.startOffset);
    SetCommonGradientStops(gradient);
    return gradient;
  }

  GradientVisual CreateGradientVisual() const
  {
    GradientVisual visual = GradientVisual::New();
    visual.SetLinearGradient(mGradientSpec.start, mGradientSpec.end);
    visual.SetUnits(mGradientSpec.units);
    visual.SetSpreadMethod(mGradientSpec.spreadMethod);
    visual.SetStartOffset(mGradientSpec.startOffset);
    SetCommonGradientStops(visual);
    return visual;
  }

  View CreateMaskGradientView(float labelWidth, float labelHeight) const
  {
    View gradientView = View::New();
    gradientView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    gradientView.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    gradientView.AddVisual(CreateGradientVisual(), Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
    ResizeMaskGradientView(gradientView, labelWidth, labelHeight);
    return gradientView;
  }

  void ResizeMaskGradientView(View maskView, float labelWidth, float labelHeight) const
  {
    if(!maskView)
    {
      return;
    }

    const float contentX      = labelWidth * mMaskBounds.xRatio;
    const float contentY      = labelHeight * mMaskBounds.yRatio;
    const float contentWidth  = labelWidth * mMaskBounds.widthRatio;
    const float contentHeight = labelHeight * mMaskBounds.heightRatio;

    maskView.SetRequestedWidth(contentWidth);
    maskView.SetRequestedHeight(contentHeight);
    maskView.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(contentX, contentY, contentWidth, contentHeight)));
  }

  template<typename GradientType>
  void SetCommonGradientStops(GradientType& gradient) const
  {
    gradient.SetStopNodes({
      Gradient::StopNode(0.0f, UiColor(1.0f, 0.12f, 0.20f, 1.0f)),
      Gradient::StopNode(0.48f, UiColor(0.08f, 0.74f, 0.42f, 1.0f)),
      Gradient::StopNode(1.0f, UiColor(0.12f, 0.38f, 1.0f, 1.0f)),
    });
  }

  Label CreateHudBadge(float x, float y, float width, float height, float fontSize)
  {
    Label label = Label::New("");
    label.SetAsyncRendering(false);
    label.SetTextColor(UiColor(0xF8FAFC));
    label.SetFontSize(fontSize);
    label.SetMultiLine(false);
    label.SetPadding(Insets(10.0f, 10.0f, 0.0f, 0.0f));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetBackgroundColor(UiColor(0x1F2937));
    label.SetCornerRadius(7.0f);
    label.SetBorderlineWidth(1.0f);
    label.SetBorderlineOffset(-1.0f);
    label.SetBorderlineColor(UiColor(0x334155));
    label.SetRequestedWidth(width);
    label.SetRequestedHeight(height);
    label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, width, height)));
    return label;
  }

  void CreateHud()
  {
    mHudRoot = AbsoluteLayout::New();
    mHudRoot.SetRequestedWidth(mWindowWidth);
    mHudRoot.SetRequestedHeight(HUD_HEIGHT);
    mHudRoot.SetBackgroundColor(UiColor(0x111827));
    mHudRoot.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, mWindowWidth, HUD_HEIGHT)));

    mModeBadge      = CreateHudBadge(12.0f, 10.0f, 218.0f, 32.0f, 15.0f);
    mCountBadge     = CreateHudBadge(240.0f, 10.0f, 132.0f, 32.0f, 15.0f);
    mAsyncBadge     = CreateHudBadge(382.0f, 10.0f, 124.0f, 32.0f, 15.0f);
    mOperationBadge = CreateHudBadge(516.0f, 10.0f, 302.0f, 32.0f, 15.0f);
    mTimingBadge    = CreateHudBadge(828.0f, 10.0f, 332.0f, 32.0f, 15.0f);
    mHeartbeatBadge = CreateHudBadge(1170.0f, 10.0f, 176.0f, 32.0f, 15.0f);
    mHeapBadge      = CreateHudBadge(1356.0f, 10.0f, 238.0f, 32.0f, 15.0f);
    mMemoryBadge    = CreateHudBadge(1604.0f, 10.0f, 304.0f, 32.0f, 15.0f);

    mHeapBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetMemoryBaseline("heap-baseline");
      UpdateHud();
    });
    mMemoryBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      TrimAllocatorMemory();
      UpdateHud();
    });

    mDetailLabel = CreateHudBadge(12.0f, 52.0f, 1888.0f, 26.0f, 14.0f);
    mDetailLabel.SetHorizontalTextAlignment(Text::Alignment::START);
    mDetailLabel.SetBackgroundColor(UiColor(0x0F172A));

    mHelpLabel = CreateHudBadge(12.0f, 82.0f, 1888.0f, 22.0f, 12.0f);
    mHelpLabel.SetHorizontalTextAlignment(Text::Alignment::START);
    mHelpLabel.SetBackgroundColor(UiColor(0x111827));
    mHelpLabel.SetBorderlineColor(UiColor(0x111827));
    mHelpLabel.SetTextColor(UiColor(0xCBD5E1));

    mHudRoot.Add(mModeBadge);
    mHudRoot.Add(mCountBadge);
    mHudRoot.Add(mAsyncBadge);
    mHudRoot.Add(mOperationBadge);
    mHudRoot.Add(mTimingBadge);
    mHudRoot.Add(mHeartbeatBadge);
    mHudRoot.Add(mHeapBadge);
    mHudRoot.Add(mMemoryBadge);
    mHudRoot.Add(mDetailLabel);
    mHudRoot.Add(mHelpLabel);
    mRoot.Add(mHudRoot);
  }

  void SetHudBadge(Label label, const std::string& text, const UiColor& backgroundColor, const UiColor& borderlineColor, const UiColor& textColor = UiColor(0xF8FAFC))
  {
    if(!label)
    {
      return;
    }

    label.SetText(text.c_str());
    label.SetTextColor(textColor);
    label.SetBackgroundColor(backgroundColor);
    label.SetBorderlineColor(borderlineColor);
  }

  void StartHeartbeat()
  {
    mLastHeartbeatAt = Clock::now();
    mHeartbeatTimer = Timer::New(HEARTBEAT_INTERVAL_MS);
    mHeartbeatTimer.TickSignal().Connect(this, &TextGradientPerfExample::OnHeartbeatTick);
    mHeartbeatTimer.Start();
  }

  bool OnHeartbeatTick()
  {
    const auto now = Clock::now();
    const double gapMs = std::chrono::duration_cast<std::chrono::microseconds>(now - mLastHeartbeatAt).count() / 1000.0;
    const bool ignoreStall = mIgnoreNextHeartbeatStall;
    mIgnoreNextHeartbeatStall = false;

    if(!ignoreStall && gapMs > static_cast<double>(HEARTBEAT_INTERVAL_MS) + 45.0)
    {
      mLastStallMs = std::max(0.0, gapMs - static_cast<double>(HEARTBEAT_INTERVAL_MS));
    }

    mLastHeartbeatAt = now;
    mHeartbeatIndex = (mHeartbeatIndex + 1u) % 8u;
    UpdateTimingBadge();
    UpdateHeartbeatBadge();
    return true;
  }

  MemorySnapshot CaptureMemorySnapshot() const
  {
    MemorySnapshot snapshot = ReadProcessMemorySnapshot();
    snapshot.labelCount           = static_cast<uint32_t>(mLabels.size());
    snapshot.labelVectorSize      = mLabels.size();
    snapshot.labelVectorCapacity  = mLabels.capacity();
    return snapshot;
  }

  void UpdateObservedMemory(const MemorySnapshot& snapshot)
  {
    mLastMemorySnapshot = snapshot;
    mPeakMemoryKb       = std::max(mPeakMemoryKb, snapshot.pssKb);
  }

  void SetMemoryBaseline(const char* tag)
  {
    mIgnoreNextHeartbeatStall = true;

    const MemorySnapshot snapshot = CaptureMemorySnapshot();
    mMemoryBaseline              = snapshot;
    mHasMemoryBaseline           = true;
    mPeakMemoryKb                = snapshot.pssKb;
    UpdateObservedMemory(snapshot);
    PrintMemorySnapshot(tag, snapshot);
  }

  void AppendMemorySnapshot(const char* tag)
  {
    mIgnoreNextHeartbeatStall = true;

    const MemorySnapshot snapshot = CaptureMemorySnapshot();
    UpdateObservedMemory(snapshot);
    PrintMemorySnapshot(tag, snapshot);
  }

  void TrimAllocatorMemory()
  {
    AppendMemorySnapshot("before-trim");
#if defined(__GLIBC__)
    malloc_trim(0);
    AppendMemorySnapshot("after-trim");
#else
    std::printf("[MEM][trim-unavailable] malloc_trim is unavailable on this platform\n");
#endif
  }

  void CancelPendingMemoryRefresh()
  {
    if(mMemoryUpdateTimer)
    {
      mMemoryUpdateTimer.Stop();
      mMemoryUpdateTimer.Reset();
    }
    mMemoryRefreshPending = false;
  }

  void SchedulePostOperationMemoryRefresh(const std::string& tag)
  {
    CancelPendingMemoryRefresh();

    mPendingMemoryUpdateTag = tag;
    mMemoryRefreshPending   = true;
    mMemoryUpdateTimer      = Timer::New(POST_OPERATION_MEMORY_DELAY_MS);
    mMemoryUpdateTimer.TickSignal().Connect(this, &TextGradientPerfExample::OnMemoryUpdateTimerTick);
    mMemoryUpdateTimer.Start();
  }

  bool OnMemoryUpdateTimerTick()
  {
    if(mMemoryUpdateTimer)
    {
      mMemoryUpdateTimer.Stop();
      mMemoryUpdateTimer.Reset();
    }

    mMemoryRefreshPending = false;
    mIgnoreNextHeartbeatStall = true;

    AppendMemorySnapshot((mPendingMemoryUpdateTag + "-before-trim").c_str());
#if defined(__GLIBC__)
    malloc_trim(0);
    AppendMemorySnapshot((mPendingMemoryUpdateTag + "-after-trim").c_str());
#else
    std::printf("[MEM][trim-unavailable] malloc_trim is unavailable on this platform\n");
    AppendMemorySnapshot((mPendingMemoryUpdateTag + "-after-delay").c_str());
#endif
    UpdateHud();
    return false;
  }

  void UpdateMemoryAfterOperation()
  {
    SchedulePostOperationMemoryRefresh(mLastOperation);
  }

  void PrintMemorySnapshot(const char* tag, const MemorySnapshot& snapshot) const
  {
    const bool hasAllocatorDelta = mHasMemoryBaseline && snapshot.allocatorAvailable && mMemoryBaseline.allocatorAvailable;
    std::printf(
      "[MEM][%s] HEAP=%s dHEAP=%s PSS=%s dPSS=%s Labels=%u Vec=%zu/%zu\n",
      tag,
      snapshot.allocatorAvailable ? FormatMb(snapshot.mallocUsedKb).c_str() : "N/A",
      hasAllocatorDelta ? FormatDeltaMb(snapshot.mallocUsedKb - mMemoryBaseline.mallocUsedKb).c_str() : "N/A",
      FormatMb(snapshot.pssKb).c_str(),
      mHasMemoryBaseline ? FormatDeltaMb(snapshot.pssKb - mMemoryBaseline.pssKb).c_str() : "N/A",
      snapshot.labelCount,
      snapshot.labelVectorSize,
      snapshot.labelVectorCapacity);
  }

  void UpdateTimingBadge()
  {
    const UiColor timingColor = GetTimingColor(mLastOperationMs, mLastStallMs);
    std::ostringstream timingText;
    timingText << std::fixed << std::setprecision(1)
               << "WORK " << mLastOperationMs << "ms"
               << "  UI FREEZE " << mLastStallMs << "ms";
    SetHudBadge(mTimingBadge, timingText.str(), timingColor, UiColor(0xFDE68A));
  }

  void UpdateHeartbeatBadge()
  {
    static constexpr std::array<const char*, 8u> HEARTBEAT_TEXT{{
      "LIVE  ⠋",
      "LIVE  ⠙",
      "LIVE  ⠹",
      "LIVE  ⠸",
      "LIVE  ⠼",
      "LIVE  ⠴",
      "LIVE  ⠦",
      "LIVE  ⠧",
    }};
    SetHudBadge(
      mHeartbeatBadge,
      mRunningOperation ? "LIVE  blocked soon" : HEARTBEAT_TEXT[mHeartbeatIndex],
      mRunningOperation ? UiColor(0x7C2D12) : UiColor(0x164E63),
      mRunningOperation ? UiColor(0xFDBA74) : UiColor(0x22D3EE));
  }

  void UpdateMemoryBadge()
  {
    const long memoryDeltaKb = mHasMemoryBaseline ? mLastMemorySnapshot.pssKb - mMemoryBaseline.pssKb : 0;

    std::ostringstream memoryText;
    memoryText << "PSS " << FormatMb(mLastMemorySnapshot.pssKb)
               << "  peak " << FormatMb(mPeakMemoryKb);
    SetHudBadge(mMemoryBadge, memoryText.str(), GetMemoryColor(memoryDeltaKb), UiColor(0x67E8F9));
  }

  void UpdateHeapBadge()
  {
    const bool hasAllocatorDelta = mHasMemoryBaseline && mLastMemorySnapshot.allocatorAvailable && mMemoryBaseline.allocatorAvailable;
    const long heapDeltaKb       = hasAllocatorDelta ? mLastMemorySnapshot.mallocUsedKb - mMemoryBaseline.mallocUsedKb : 0;

    std::ostringstream heapText;
    heapText << "HEAP ";
    if(mLastMemorySnapshot.allocatorAvailable)
    {
      heapText << FormatMb(mLastMemorySnapshot.mallocUsedKb);
      if(hasAllocatorDelta)
      {
        heapText << " " << FormatDeltaMb(heapDeltaKb);
      }
    }
    else
    {
      heapText << "N/A";
    }

    SetHudBadge(
      mHeapBadge,
      heapText.str(),
      mLastMemorySnapshot.allocatorAvailable ? GetMemoryColor(heapDeltaKb) : UiColor(0x334155),
      mLastMemorySnapshot.allocatorAvailable ? UiColor(0x86EFAC) : UiColor(0x64748B));
  }

  void UpdateHud()
  {
    std::ostringstream modeText;
    modeText << "MODE  " << GetModeName(mMode);
    SetHudBadge(mModeBadge, modeText.str(), GetModeColor(mMode), UiColor(0x93C5FD));

    std::ostringstream countText;
    countText << "COUNT  " << mLabels.size();
    SetHudBadge(mCountBadge, countText.str(), UiColor(0x1E293B), UiColor(0x475569));

    SetHudBadge(
      mAsyncBadge,
      mAsyncRendering ? "ASYNC  ON" : "SYNC  ON",
      mAsyncRendering ? UiColor(0x065F46) : UiColor(0x57534E),
      mAsyncRendering ? UiColor(0x34D399) : UiColor(0xA8A29E));

    std::ostringstream operationText;
    operationText << (mRunningOperation ? "RUNNING  " : "LAST  ") << mLastOperation;
    SetHudBadge(
      mOperationBadge,
      operationText.str(),
      mRunningOperation ? UiColor(0x9A3412) : UiColor(0x312E81),
      mRunningOperation ? UiColor(0xFDBA74) : UiColor(0x818CF8));

    UpdateTimingBadge();
    UpdateHeartbeatBadge();
    UpdateHeapBadge();
    UpdateMemoryBadge();

    std::ostringstream detailText;
    detailText << "Label " << static_cast<int>(mLabelSize.x) << "x" << static_cast<int>(mLabelSize.y)
               << "   Mask ratio " << mMaskBounds.xRatio << "/" << mMaskBounds.yRatio << "/" << mMaskBounds.widthRatio << "/" << mMaskBounds.heightRatio
               << "   Offset " << mGradientSpec.startOffset
               << "   Text " << TEXT_PRESETS[mTextIndex]
               << "   Resize R=recreate S=live"
               << "   Mem " << (mMemoryRefreshPending ? "settling" : "settled")
               << "   HEAP click: baseline"
               << "   PSS click: trim";
    SetHudBadge(mDetailLabel, detailText.str(), UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1));

    if(mHelpVisible)
    {
      mHelpLabel.SetText("Keys: 1 TextGradient, 2 Mask, R Resize+Recreate, S Resize Live, D Delete, N New, T Text, C Recreate, Space Stress, A Async/Sync, G Offset, [/] Mask W, ;/' Mask H, HEAP Baseline, PSS Trim, H Help, Esc Quit");
    }
    else
    {
      mHelpLabel.SetText("Keys: 1 TextGradient, 2 Mask, A Async/Sync, R Resize+Recreate, S Resize Live, D Delete, N New, T Text, C Recreate, HEAP/PSS Memory, Esc Quit, H More");
    }
  }

  double LogElapsed(const char* operation, Clock::time_point start) const
  {
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start).count();
    const double elapsedMs = static_cast<double>(elapsed) / 1000.0;
    std::printf(
      "TEXT_GRADIENT_PERF_EXAMPLE operation=%s mode=%s count=%zu async=%s elapsedMs=%.3f\n",
      operation,
      GetModeName(mMode),
      mLabels.size(),
      mAsyncRendering ? "on" : "off",
      elapsedMs);
    return elapsedMs;
  }

private:
  Application&       mApplication;
  AbsoluteLayout     mRoot;
  AbsoluteLayout     mGridRoot;
  AbsoluteLayout     mHudRoot;
  Label              mModeBadge;
  Label              mCountBadge;
  Label              mAsyncBadge;
  Label              mOperationBadge;
  Label              mTimingBadge;
  Label              mHeartbeatBadge;
  Label              mHeapBadge;
  Label              mMemoryBadge;
  Label              mDetailLabel;
  Label              mHelpLabel;
  std::vector<Label> mLabels;
  std::vector<View>  mMaskViews;
  Timer              mDeferredOperationTimer;
  Timer              mHeartbeatTimer;
  Timer              mMemoryUpdateTimer;
  PerfRenderMode     mMode{PerfRenderMode::TEXT_GRADIENT};
  PendingOperation   mPendingOperation{PendingOperation::NONE};
  GradientSpec       mGradientSpec;
  MaskGradientBounds mMaskBounds;
  Vector2            mLabelSize{0.0f, 0.0f};
  float              mWindowWidth{static_cast<float>(WINDOW_WIDTH)};
  float              mWindowHeight{static_cast<float>(WINDOW_HEIGHT)};
  std::size_t        mTextIndex{0u};
  std::string        mLastOperation{"init"};
  std::string        mPendingOperationName;
  std::string        mPendingMemoryUpdateTag;
  Clock::time_point  mLastHeartbeatAt;
  MemorySnapshot     mMemoryBaseline;
  MemorySnapshot     mLastMemorySnapshot;
  double             mLastOperationMs{0.0};
  double             mLastStallMs{0.0};
  long               mPeakMemoryKb{0};
  uint32_t           mHeartbeatIndex{0u};
  bool               mAsyncRendering{true};
  bool               mLargeSize{false};
  bool               mHelpVisible{false};
  bool               mRunningOperation{false};
  bool               mHasMemoryBaseline{false};
  bool               mMemoryRefreshPending{false};
  bool               mIgnoreNextHeartbeatStall{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  UiConfig config = UiConfig::New();
  config.SetLabelAsyncRendering(true);
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();

  TextGradientPerfExample controller(application);
  application.MainLoop();

  return 0;
}
