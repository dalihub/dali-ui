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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/selectable-lottie-animation-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/public-api/signals/callback.h>
#include <atomic>
#include <map>
#include <mutex>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace
{
// Generic fallback inner-fill key path used only when the caller supplies no explicit path.
// Concrete assets pass their own key path through SelectableLottieImage::GetInnerFillKeyPath().
const char* const DEFAULT_INNER_FILL_KEY_PATH = "checked-fill.fill-group.fill-color";

// ---------------------------------------------------------------------------
// Per-frame recolour callback plumbing.
//
// MakeCallback wraps a capture-less free function, so per-instance colours are reached
// through the DynamicPropertyInfo.id via a small process-wide registry. The callback runs
// on a worker thread and must read ONLY pre-resolved Vector4 colours (no DALi API calls).
//
// The registry/mutex are leaked singletons (never destroyed) so a worker-thread callback
// firing during process shutdown cannot touch destroyed statics.
// ---------------------------------------------------------------------------
struct InnerFillState
{
  Vector4 deselected;
  Vector4 selected;
  bool    lastSelected{false}; ///< current logical state: selected -> selected colour
};

std::mutex& RecolorMutex()
{
  static std::mutex* m = new std::mutex(); // leaked: must outlive worker callbacks at shutdown
  return *m;
}
std::map<int32_t, InnerFillState>& RecolorRegistry()
{
  static auto* r = new std::map<int32_t, InnerFillState>(); // leaked (see above)
  return *r;
}

int32_t AllocateDynamicPropertyId()
{
  static std::atomic<int32_t> sNext{1};
  return sNext++;
}

Dali::Property::Value OnInnerFillColor(int32_t id,
                                       Dali::VectorAnimationRenderer::VectorProperty /*property*/,
                                       uint32_t /*frameNumber*/)
{
  // Worker thread — registry read only, no DALi API calls.
  InnerFillState state{Vector4(1.f, 1.f, 1.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f), false};
  {
    std::lock_guard<std::mutex> lock(RecolorMutex());
    auto                        it = RecolorRegistry().find(id);
    if(it != RecolorRegistry().end())
    {
      state = it->second;
    }
  }
  // The inner fill carries the current logical state's colour for the whole segment: the
  // selected colour while selected, the deselected colour otherwise. (frameNumber is unused.)
  return Dali::Property::Value(state.lastSelected ? state.selected : state.deselected);
}

} // namespace

SelectableLottieAnimationViewImpl* SelectableLottieAnimationViewImpl::New(const Dali::String& url,
                                                                          const FrameRange&   selectRange,
                                                                          const FrameRange&   deselectRange,
                                                                          const Dali::String& keyPath)
{
  auto* impl = new SelectableLottieAnimationViewImpl();
  impl->SetFrameRanges(selectRange, deselectRange);
  if(!keyPath.Empty())
  {
    impl->mInnerFillKeyPath = keyPath; // otherwise keep the generic DEFAULT_INNER_FILL_KEY_PATH
  }
  if(!url.Empty())
  {
    impl->mLottie.SetResourceUrl(url);
  }
  return impl;
}

Ui::View SelectableLottieAnimationViewImpl::GetView() const
{
  return mLottie;
}

void SelectableLottieAnimationViewImpl::SetFrameRanges(const FrameRange& selectRange, const FrameRange& deselectRange)
{
  mSelectStart   = selectRange.startFrame;
  mSelectEnd     = selectRange.endFrame;
  mDeselectStart = deselectRange.startFrame;
  mDeselectEnd   = deselectRange.endFrame;
}

void SelectableLottieAnimationViewImpl::SetStateColors(const Vector4& deselected, const Vector4& selected)
{
  mDeselectedColor = deselected;
  mSelectedColor   = selected;

  // Re-seat the recolour if a visual already exists; otherwise the authoritative seat happens
  // the next time OnSelectedChanged() runs (SetDynamicProperty no-ops without a visual).
  if(!mLottie.GetResourceUrl().Empty())
  {
    RegisterInnerFillRecolor();
  }
}

bool SelectableLottieAnimationViewImpl::IsTransitioning() const
{
  return mLottie.GetPlayState() == AnimatedImage::PlayState::PLAYING;
}

ISelectableImage::TransitionFinishedSignalType& SelectableLottieAnimationViewImpl::TransitionFinishedSignal()
{
  return mLottie.AnimationFinishedSignal();
}

void SelectableLottieAnimationViewImpl::OnSelectedChanged(bool selected, bool animated)
{
  // Record the logical state first: the inner-fill recolour is driven by this, not by frame.
  mLastSelected = selected;

  int start = selected ? mSelectStart : mDeselectStart;
  int end   = selected ? mSelectEnd : mDeselectEnd;
  if(start > end)
  {
    std::swap(start, end); // SetMinMaxFrame requires min <= max; play forward within the segment
  }

  // SetMinMaxFrame()+JumpToFrame() rebuild the Lottie visual (mVisualDirty), which BOTH
  // drops any registered dynamic property AND constrains JumpToFrame to the play range.
  // So: set the segment range, jump to the target frame, then RE-SEAT the inner-fill
  // recolour on the rebuilt visual (SetDynamicProperty applies without re-dirtying) before
  // playing. Without the re-seat the themed recolour is lost on the first animated toggle.
  mLottie.SetMinMaxFrame(start, end);
  mLottie.JumpToFrame(animated ? start : end);
  // Re-seat only when a visual exists; SetDynamicProperty no-ops without one.
  if(!mLottie.GetResourceUrl().Empty())
  {
    RegisterInnerFillRecolor();
  }
  if(animated)
  {
    mLottie.Play();
  }
}

void SelectableLottieAnimationViewImpl::RegisterInnerFillRecolor()
{
  {
    std::lock_guard<std::mutex> lock(RecolorMutex());
    RecolorRegistry()[mDynamicPropertyId] = InnerFillState{mDeselectedColor,
                                                           mSelectedColor,
                                                           mLastSelected};
  }
  Ui::LottieAnimation::DynamicPropertyInfo info;
  info.id       = mDynamicPropertyId;
  info.keyPath  = mInnerFillKeyPath;
  info.property = Ui::LottieAnimation::VectorProperty::FILL_COLOR;
  info.callback = MakeCallback(&OnInnerFillColor);
  mLottie.SetDynamicProperty(info);
}

SelectableLottieAnimationViewImpl::SelectableLottieAnimationViewImpl()
: mLottie(LottieAnimationView::New()),
  mInnerFillKeyPath(DEFAULT_INNER_FILL_KEY_PATH),
  mDeselectedColor(1.f, 1.f, 1.f, 1.f),
  mSelectedColor(1.f, 1.f, 1.f, 1.f),
  mSelectStart(0),
  mSelectEnd(0),
  mDeselectStart(0),
  mDeselectEnd(0),
  mDynamicPropertyId(AllocateDynamicPropertyId())
{
  mLottie.SetLoopCount(1); // a segment plays once
}

SelectableLottieAnimationViewImpl::~SelectableLottieAnimationViewImpl()
{
  std::lock_guard<std::mutex> lock(RecolorMutex());
  RecolorRegistry().erase(mDynamicPropertyId);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
