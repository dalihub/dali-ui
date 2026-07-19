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

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using Clock = std::chrono::steady_clock;

constexpr int      WINDOW_WIDTH          = 1920;
constexpr int      WINDOW_HEIGHT         = 1080;
constexpr float    HUD_HEIGHT            = 104.0f;
constexpr uint32_t GRID_COLUMNS          = 20u;
constexpr uint32_t HEARTBEAT_INTERVAL_MS = 100u;
constexpr uint32_t STAT_REFRESH_MS       = 750u;

enum class SourceMode
{
  SAME,
  TEN_REPEAT,
  ALL_DIFFERENT_FAILURE,
};

enum class SizeMode
{
  SMALL,
  MIXED,
};

struct MemorySnapshot
{
  long rssKb{0};
  long hwmKb{0};
};

bool ParseProcMetric(const char* line, const char* key, long& value)
{
  const std::size_t length = std::strlen(key);
  return std::strncmp(line, key, length) == 0 && std::sscanf(line + length, "%ld", &value) == 1;
}

MemorySnapshot ReadMemory()
{
  MemorySnapshot snapshot;
  if(FILE* file = std::fopen("/proc/self/status", "r"))
  {
    char line[128];
    while(std::fgets(line, sizeof(line), file))
    {
      ParseProcMetric(line, "VmRSS:", snapshot.rssKb);
      ParseProcMetric(line, "VmHWM:", snapshot.hwmKb);
    }
    std::fclose(file);
  }
  return snapshot;
}

double ElapsedMilliseconds(const Clock::time_point& start)
{
  return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

const char* SourceModeName(SourceMode mode)
{
  switch(mode)
  {
    case SourceMode::TEN_REPEAT: return "10-source repeat";
    case SourceMode::ALL_DIFFERENT_FAILURE: return "all-different (failure-path)";
    case SourceMode::SAME:
    default: return "same source";
  }
}

const char* SizeModeName(SizeMode mode)
{
  return mode == SizeMode::MIXED ? "mixed size" : "small image";
}

uint32_t CountActors(Actor actor)
{
  if(!actor)
  {
    return 0u;
  }
  uint32_t count = 1u;
  for(uint32_t index = 0u; index < actor.GetChildCount(); ++index)
  {
    count += CountActors(actor.GetChildAt(index));
  }
  return count;
}

void CollectRenderStats(Actor actor, uint32_t& rendererCount, std::vector<TextureSet>& textureSets)
{
  if(!actor)
  {
    return;
  }

  rendererCount += actor.GetRendererCount();
  for(uint32_t index = 0u; index < actor.GetRendererCount(); ++index)
  {
    TextureSet textures = actor.GetRendererAt(index).GetTextures();
    if(textures && std::find(textureSets.begin(), textureSets.end(), textures) == textureSets.end())
    {
      textureSets.push_back(textures);
    }
  }
  for(uint32_t index = 0u; index < actor.GetChildCount(); ++index)
  {
    CollectRenderStats(actor.GetChildAt(index), rendererCount, textureSets);
  }
}
} // unnamed namespace

class TextImageSpanPerfController : public ConnectionTracker
{
public:
  explicit TextImageSpanPerfController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextImageSpanPerfController::OnInit);
  }

private:
  std::string Resource(const char* file) const
  {
    return std::string(RESOURCES_DIR) + file;
  }

  std::string SourceFor(uint32_t labelIndex, uint32_t imageIndex) const
  {
    static constexpr std::array<const char*, 10u> SOURCES{{
      "flag_kr.png",
      "flag_us.png",
      "flag_ae.png",
      "flag_kr_alt.png",
      "flag_us_alt.png",
      "flag_ae_alt.png",
      "cursor_handle.png",
      "cursor_handle_pressed.png",
      "selection_handle_left.png",
      "selection_handle_right.png",
    }};

    const uint32_t occurrence = labelIndex * mImagesPerLabel + imageIndex;
    switch(mSourceMode)
    {
      case SourceMode::TEN_REPEAT:
        return Resource(SOURCES[occurrence % SOURCES.size()]);
      case SourceMode::ALL_DIFFERENT_FAILURE:
        // The repository does not ship thousands of distinct decoded assets. This mode intentionally uses unique
        // missing descriptors to measure descriptor/Visual/failure callback cost, not decoded texture memory.
        return std::string(RESOURCES_DIR) + "missing-image-span-perf-" + std::to_string(occurrence) + ".png";
      case SourceMode::SAME:
      default:
        return Resource(SOURCES[0u]);
    }
  }

  Text::StyledText BuildText(uint32_t labelIndex) const
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
    static constexpr std::array<Vector2, 9u> MIXED_SIZES{{
      Vector2(8.0f, 8.0f),
      Vector2(12.0f, 12.0f),
      Vector2(16.0f, 16.0f),
      Vector2(24.0f, 24.0f),
      Vector2(32.0f, 20.0f),
      Vector2(40.0f, 40.0f),
      Vector2(64.0f, 32.0f),
      Vector2(80.0f, 48.0f),
      Vector2(120.0f, 60.0f),
    }};
    for(uint32_t imageIndex = 0u; imageIndex < mImagesPerLabel; ++imageIndex)
    {
      const uint32_t begin = builder.GetUtf32Length();
      builder.AppendText("\uFFFC");
      const Vector2 size = mSizeMode == SizeMode::MIXED
                             ? MIXED_SIZES[(labelIndex + imageIndex) % MIXED_SIZES.size()]
                             : Vector2(mImagesPerLabel == 1u ? 18.0f : 8.0f,
                                       mImagesPerLabel == 1u ? 18.0f : 8.0f);
      Text::ImageAttributes attributes(SourceFor(labelIndex, imageIndex).c_str(), size);
      attributes.SetAlternativeText("");
      DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), begin, begin + 1u) &&
                         "Performance ImageSpan range must be valid");
      builder.AppendText("x");
    }
    return builder.Build();
  }

  LayoutRect LabelBounds(uint32_t index) const
  {
    const uint32_t rows = (mLabelCount + GRID_COLUMNS - 1u) / GRID_COLUMNS;
    const float cellWidth = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(GRID_COLUMNS);
    const float cellHeight = (static_cast<float>(WINDOW_HEIGHT) - HUD_HEIGHT) / static_cast<float>(rows);
    const uint32_t column = index % GRID_COLUMNS;
    const uint32_t row    = index / GRID_COLUMNS;
    return LayoutRect(column * cellWidth + 1.0f,
                      row * cellHeight + 1.0f,
                      std::max(1.0f, cellWidth - 2.0f),
                      std::max(1.0f, cellHeight - 2.0f));
  }

  void ClearGrid()
  {
    if(mGrid && mGridAttached)
    {
      mRoot.Remove(mGrid);
    }
    mLabels.clear();
    mGrid.Reset();
    mGridAttached = false;
  }

  void CreateGrid(const char* operation)
  {
    ClearGrid();
    mReadyCallbacks = 0u;
    mMaxHeartbeatStallMs = 0.0;

    const auto creationStart = Clock::now();
    mGrid = AbsoluteLayout::New();
    mGrid.SetRequestedWidth(static_cast<float>(WINDOW_WIDTH));
    mGrid.SetRequestedHeight(static_cast<float>(WINDOW_HEIGHT) - HUD_HEIGHT);
    mGrid.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
      LayoutRect(0.0f, HUD_HEIGHT, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT) - HUD_HEIGHT)));
    mLabels.reserve(mLabelCount);

    for(uint32_t index = 0u; index < mLabelCount; ++index)
    {
      Label label = Label::New();
      label.SetAsyncRendering(mAsyncRendering);
      label.SetFontSize(mImagesPerLabel == 1u ? 13.0f : 8.0f);
      label.SetMultiLine(false);
      label.SetTextOverflowMode(mEllipsis ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
      label.SetTextColor(UiColor(0xE5E7EB));
      label.SetBackgroundColor(index % 2u ? UiColor(0x1F2937) : UiColor(0x111827));
      label.SetPadding(Extents(0, 0, 0, 0));
      label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      label.SetVerticalTextAlignment(Text::Alignment::CENTER);
      const LayoutRect bounds = LabelBounds(index);
      label.SetRequestedWidth(bounds.width);
      label.SetRequestedHeight(bounds.height);
      label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(bounds));
      label.SetStyledText(BuildText(index));
      label.ResourceReadySignal().Connect(this, [this](View)
      {
        ++mReadyCallbacks;
      });
      mGrid.Add(label);
      mLabels.push_back(label);
    }

    mRoot.Add(mGrid);
    mGridAttached = true;
    mCreationMs   = ElapsedMilliseconds(creationStart);

    const auto measureStart = Clock::now();
    for(Label label : mLabels)
    {
      label.GetNaturalSize();
    }
    mMeasureMs     = ElapsedMilliseconds(measureStart);
    mLastOperation = operation;
    RefreshStats();
  }

  void ResizeLabels()
  {
    const auto start = Clock::now();
    mWide = !mWide;
    for(uint32_t index = 0u; index < mLabels.size(); ++index)
    {
      const LayoutRect base = LabelBounds(index);
      const float width = mWide ? base.width : std::max(1.0f, base.width * 0.72f);
      mLabels[index].SetRequestedWidth(width);
      mLabels[index].SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
        LayoutRect(base.x, base.y, width, base.height)));
    }
    mRelayoutMutationMs = ElapsedMilliseconds(start);
    mLastOperation      = mWide ? "live resize wide" : "live resize narrow";
    RefreshStats();
  }

  void ToggleAttached()
  {
    if(mGridAttached)
    {
      mRoot.Remove(mGrid);
      mGridAttached = false;
      mLastOperation = "detached (entries off-scene)";
    }
    else
    {
      mRoot.Add(mGrid);
      mGridAttached = true;
      mLastOperation = "reattached";
    }
    RefreshStats();
  }

  void CycleSourceMode()
  {
    mSourceMode = static_cast<SourceMode>((static_cast<uint32_t>(mSourceMode) + 1u) % 3u);
    CreateGrid("source mode recreate");
  }

  void RefreshStats()
  {
    mMemory = ReadMemory();
    mActorCount = CountActors(mRoot);
    mRendererCount = 0u;
    std::vector<TextureSet> textureSets;
    CollectRenderStats(mRoot, mRendererCount, textureSets);
    mTextureSetCount = static_cast<uint32_t>(textureSets.size());
    UpdateHud();
  }

  void UpdateHud()
  {
    if(!mHud)
    {
      return;
    }

    std::ostringstream text;
    text << "ImageSpan PERF (public API, real visible grid) | " << mLabelCount << " Label x "
         << mImagesPerLabel << " image | " << SourceModeName(mSourceMode)
         << " | " << SizeModeName(mSizeMode) << " | END ellipsis " << (mEllipsis ? "on" : "off")
         << " | " << (mAsyncRendering ? "async" : "sync")
         << " | " << (mGridAttached ? "visible/attached" : "detached") << "\n"
         << std::fixed << std::setprecision(2)
         << "op=" << mLastOperation << " create=" << mCreationMs << "ms natural-query=" << mMeasureMs
         << "ms resize-mutation=" << mRelayoutMutationMs << "ms heartbeat-max-stall=" << mMaxHeartbeatStallMs
         << "ms | actors=" << mActorCount << " renderers/draw-upper-bound=" << mRendererCount
         << " unique-TextureSet=" << mTextureSetCount << " authored-ImageVisual="
         << mLabelCount * mImagesPerLabel << " ready-label-callback=" << mReadyCallbacks
         << " | VmRSS=" << (mMemory.rssKb / 1024.0) << "MB VmHWM=" << (mMemory.hwmKb / 1024.0) << "MB\n"
         << "Keys: N=100/300  I=1/10 images  S=source  M=size  E=ellipsis  W=warm recreate  D=detach  R=resize  A=sync/async  Esc=quit. "
            "Cold-cache numbers require a fresh process; all-different mode measures unique failure descriptors, not decoded texture memory.";
    mHud.SetText(text.str().c_str());
  }

  bool OnHeartbeat()
  {
    const auto now = Clock::now();
    if(mLastHeartbeat.time_since_epoch().count() != 0)
    {
      const double interval = std::chrono::duration<double, std::milli>(now - mLastHeartbeat).count();
      mMaxHeartbeatStallMs  = std::max(mMaxHeartbeatStallMs,
                                     std::max(0.0, interval - static_cast<double>(HEARTBEAT_INTERVAL_MS)));
    }
    mLastHeartbeat = now;
    return true;
  }

  bool OnStatRefresh()
  {
    RefreshStats();
    return true;
  }

  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    const auto key = event.GetKeyName();
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(key == "n" || key == "N")
    {
      mLabelCount = mLabelCount == 100u ? 300u : 100u;
      CreateGrid("label-count recreate");
    }
    else if(key == "i" || key == "I")
    {
      mImagesPerLabel = mImagesPerLabel == 1u ? 10u : 1u;
      CreateGrid("image-count recreate");
    }
    else if(key == "s" || key == "S")
    {
      CycleSourceMode();
    }
    else if(key == "m" || key == "M")
    {
      mSizeMode = mSizeMode == SizeMode::SMALL ? SizeMode::MIXED : SizeMode::SMALL;
      CreateGrid("size-mode recreate");
    }
    else if(key == "e" || key == "E")
    {
      mEllipsis = !mEllipsis;
      CreateGrid("ellipsis-mode recreate");
    }
    else if(key == "w" || key == "W")
    {
      CreateGrid("warm-cache recreate");
    }
    else if(key == "d" || key == "D")
    {
      ToggleAttached();
    }
    else if(key == "r" || key == "R")
    {
      ResizeLabels();
    }
    else if(key == "a" || key == "A")
    {
      mAsyncRendering = !mAsyncRendering;
      CreateGrid("render-mode recreate");
    }
  }

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0x030712));

    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);

    mHud = Label::New();
    mHud.SetFontSize(14.0f);
    mHud.SetMultiLine(true);
    mHud.SetTextColor(UiColor(0xF8FAFC));
    mHud.SetBackgroundColor(UiColor(0x0F3D5E));
    mHud.SetPadding(Extents(10, 10, 4, 4));
    mHud.SetHorizontalTextAlignment(Text::Alignment::START);
    mHud.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mHud.SetRequestedWidth(static_cast<float>(WINDOW_WIDTH));
    mHud.SetRequestedHeight(HUD_HEIGHT);
    mHud.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
      LayoutRect(0.0f, 0.0f, static_cast<float>(WINDOW_WIDTH), HUD_HEIGHT)));
    mRoot.Add(mHud);

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &TextImageSpanPerfController::OnKey);

    mHeartbeat = Timer::New(HEARTBEAT_INTERVAL_MS);
    mHeartbeat.TickSignal().Connect(this, &TextImageSpanPerfController::OnHeartbeat);
    mHeartbeat.Start();
    mStatRefresh = Timer::New(STAT_REFRESH_MS);
    mStatRefresh.TickSignal().Connect(this, &TextImageSpanPerfController::OnStatRefresh);
    mStatRefresh.Start();

    CreateGrid("fresh-process create");
  }

private:
  Application&       mApplication;
  AbsoluteLayout     mRoot;
  AbsoluteLayout     mGrid;
  Label              mHud;
  std::vector<Label> mLabels;
  Timer              mHeartbeat;
  Timer              mStatRefresh;
  Clock::time_point  mLastHeartbeat;
  MemorySnapshot     mMemory;
  std::string        mLastOperation{"startup"};
  SourceMode         mSourceMode{SourceMode::SAME};
  SizeMode           mSizeMode{SizeMode::SMALL};
  uint32_t           mLabelCount{100u};
  uint32_t           mImagesPerLabel{1u};
  uint32_t           mReadyCallbacks{0u};
  uint32_t           mActorCount{0u};
  uint32_t           mRendererCount{0u};
  uint32_t           mTextureSetCount{0u};
  double             mCreationMs{0.0};
  double             mMeasureMs{0.0};
  double             mRelayoutMutationMs{0.0};
  double             mMaxHeartbeatStallMs{0.0};
  bool               mAsyncRendering{false};
  bool               mGridAttached{false};
  bool               mWide{true};
  bool               mEllipsis{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextImageSpanPerfController controller(application);
  application.MainLoop();
  return 0;
}
