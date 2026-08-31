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
#include <algorithm>
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
// Per-frame color callback plumbing.
//
// MakeCallback wraps a capture-less free function, so per-instance colours are reached
// through the DynamicPropertyInfo.id via a small process-wide registry. The callback runs
// on a worker thread and must read ONLY pre-resolved Vector4 colours (no DALi API calls).
//
// The registry/mutex are leaked singletons (never destroyed) so a worker-thread callback
// firing during process shutdown cannot touch destroyed statics.
// ---------------------------------------------------------------------------
struct ColorBindingState
{
  Vector4                                   deselectedColor;
  Vector4                                   selectedColor;
  SelectableLottieColorBinding::ColorPolicy colorPolicy;
  int32_t                                   selectedColorStart;
  int32_t                                   selectedColorEnd;
  bool                                      logicalSelected;
};

std::mutex& ColorBindingMutex()
{
  static std::mutex* m = new std::mutex(); // leaked: must outlive worker callbacks at shutdown
  return *m;
}
std::map<int32_t, ColorBindingState>& ColorBindingRegistry()
{
  static auto* r = new std::map<int32_t, ColorBindingState>(); // leaked (see above)
  return *r;
}

int32_t AllocateDynamicPropertyId()
{
  static std::atomic<int32_t> sNext{1};
  return sNext++;
}

void ValidateColorBindings(const SelectableLottieColorBindings& colorBindings)
{
  for(uint32_t bindingIndex = 0u; bindingIndex < colorBindings.Count(); ++bindingIndex)
  {
    const Dali::String                    keyPath  = colorBindings[bindingIndex].GetKeyPath();
    const LottieAnimation::VectorProperty property = colorBindings[bindingIndex].GetProperty();
    for(uint32_t previousIndex = 0u; previousIndex < bindingIndex; ++previousIndex)
    {
      DALI_ASSERT_ALWAYS(!(colorBindings[previousIndex].GetKeyPath() == keyPath &&
                           colorBindings[previousIndex].GetProperty() == property) &&
                         "Selectable Lottie color bindings must have unique key-path and property pairs");
    }
  }
}

Dali::Property::Value OnBindingColor(int32_t id,
                                     Dali::VectorAnimationRenderer::VectorProperty /*property*/,
                                     uint32_t frameNumber)
{
  // Worker thread: copy plain pre-resolved state under lock, then make no DALi API calls.
  ColorBindingState state{Vector4(1.f, 1.f, 1.f, 1.f),
                          Vector4(1.f, 1.f, 1.f, 1.f),
                          SelectableLottieColorBinding::ColorPolicy::ALWAYS_DESELECTED,
                          0,
                          0,
                          false};
  {
    std::lock_guard<std::mutex> lock(ColorBindingMutex());
    auto                        it = ColorBindingRegistry().find(id);
    if(it != ColorBindingRegistry().end())
    {
      state = it->second;
    }
  }

  bool useSelectedColor = false;
  switch(state.colorPolicy)
  {
    case SelectableLottieColorBinding::ColorPolicy::BY_SELECTION_STATE:
      useSelectedColor = state.logicalSelected;
      break;
    case SelectableLottieColorBinding::ColorPolicy::ALWAYS_DESELECTED:
      break;
    case SelectableLottieColorBinding::ColorPolicy::ALWAYS_SELECTED:
      useSelectedColor = true;
      break;
    case SelectableLottieColorBinding::ColorPolicy::SELECTED_IN_FRAME_RANGE:
    {
      const int64_t signedFrame = static_cast<int64_t>(frameNumber);
      useSelectedColor          = (signedFrame >= static_cast<int64_t>(state.selectedColorStart) &&
                          signedFrame <= static_cast<int64_t>(state.selectedColorEnd));
      break;
    }
  }
  return Dali::Property::Value(useSelectedColor ? state.selectedColor : state.deselectedColor);
}

} // namespace

SelectableLottieAnimationViewImpl* SelectableLottieAnimationViewImpl::New(const Dali::String& url,
                                                                          const FrameRange&   selectRange,
                                                                          const FrameRange&   deselectRange,
                                                                          const Dali::String& keyPath)
{
  SelectableLottieColorBindings colorBindings;
  colorBindings.PushBack(SelectableLottieColorBinding(
    keyPath.Empty() ? Dali::String(DEFAULT_INNER_FILL_KEY_PATH) : keyPath,
    LottieAnimation::VectorProperty::FILL_COLOR,
    SelectableLottieColorBinding::ColorPolicy::BY_SELECTION_STATE));
  return New(url, selectRange, deselectRange, colorBindings);
}

SelectableLottieAnimationViewImpl* SelectableLottieAnimationViewImpl::New(
  const Dali::String&                  url,
  const FrameRange&                    selectRange,
  const FrameRange&                    deselectRange,
  const SelectableLottieColorBindings& colorBindings)
{
  ValidateColorBindings(colorBindings);
  auto* impl = new SelectableLottieAnimationViewImpl();
  impl->SetFrameRanges(selectRange, deselectRange);
  impl->SetColorBindings(colorBindings);
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

  // A configured resource URL lets LottieAnimationView materialize its visual and transfer
  // callback ownership. An empty URL has no visual to bind.
  if(!mLottie.GetResourceUrl().Empty())
  {
    ApplyColorBindings();
  }
}

bool SelectableLottieAnimationViewImpl::IsTransitioning() const
{
  return mLottie.GetPlayState() == AnimatedImage::PlayState::PLAYING;
}

SelectableImageInterface::TransitionFinishedSignalType& SelectableLottieAnimationViewImpl::TransitionFinishedSignal()
{
  return mLottie.AnimationFinishedSignal();
}

void SelectableLottieAnimationViewImpl::OnSelectedChanged(bool selected, bool animated)
{
  // Record the logical state before rebuilding and re-registering the dynamic properties.
  mLastSelected = selected;

  int start = selected ? mSelectStart : mDeselectStart;
  int end   = selected ? mSelectEnd : mDeselectEnd;
  NormalizeFrameRange(start, end);

  // A snap is a terminal state request. Cancel any in-flight segment before moving to the
  // target frame so IsTransitioning() and the completion signal cannot lag behind the visual.
  if(!animated && IsTransitioning())
  {
    mLottie.Stop();
  }

  // Apply the segment before the jump so both the Lottie view and its vector task clamp against
  // the same range. Re-apply bindings afterwards because a pending visual rebuild can replace
  // the visual that owned the previous callbacks.
  mLottie.SetMinMaxFrame(start, end);
  mLottie.JumpToFrame(animated ? start : end);
  if(!mLottie.GetResourceUrl().Empty())
  {
    ApplyColorBindings();
  }
  if(animated)
  {
    mLottie.Play();
  }
}

void SelectableLottieAnimationViewImpl::NormalizeFrameRange(int& start, int& end) const
{
  if(start > end)
  {
    std::swap(start, end);
  }

  start = std::max(start, 0);
  end   = std::max(end, 0);

  const int totalFrame = mLottie.GetTotalFrame();
  if(totalFrame > 0)
  {
    const int lastFrame = totalFrame - 1;
    start               = std::min(start, lastFrame);
    end                 = std::min(end, lastFrame);
  }
}

void SelectableLottieAnimationViewImpl::SetColorBindings(const SelectableLottieColorBindings& colorBindings)
{
  for(const auto& binding : colorBindings)
  {
    const FrameRange selectedColorRange = binding.GetSelectedColorRange();
    mColorBindings.push_back(ColorBindingData{binding.GetKeyPath(),
                                              binding.GetProperty(),
                                              binding.GetColorPolicy(),
                                              selectedColorRange.startFrame,
                                              selectedColorRange.endFrame,
                                              AllocateDynamicPropertyId()});
  }
}

void SelectableLottieAnimationViewImpl::ApplyColorBindings()
{
  {
    std::lock_guard<std::mutex> lock(ColorBindingMutex());
    for(const auto& binding : mColorBindings)
    {
      ColorBindingRegistry()[binding.dynamicPropertyId] = ColorBindingState{mDeselectedColor,
                                                                            mSelectedColor,
                                                                            binding.colorPolicy,
                                                                            binding.selectedColorStart,
                                                                            binding.selectedColorEnd,
                                                                            mLastSelected};
    }
  }

  for(const auto& binding : mColorBindings)
  {
    Ui::LottieAnimation::DynamicPropertyInfo info;
    info.id       = binding.dynamicPropertyId;
    info.keyPath  = binding.keyPath;
    info.property = binding.property;
    info.callback = MakeCallback(&OnBindingColor);
    mLottie.SetDynamicProperty(info);
  }
}

SelectableLottieAnimationViewImpl::SelectableLottieAnimationViewImpl()
: mLottie(LottieAnimationView::New()),
  mColorBindings(),
  mDeselectedColor(1.f, 1.f, 1.f, 1.f),
  mSelectedColor(1.f, 1.f, 1.f, 1.f),
  mSelectStart(0),
  mSelectEnd(0),
  mDeselectStart(0),
  mDeselectEnd(0)
{
  mLottie.SetLoopCount(1); // a segment plays once
}

SelectableLottieAnimationViewImpl::~SelectableLottieAnimationViewImpl()
{
  std::lock_guard<std::mutex> lock(ColorBindingMutex());
  for(const auto& binding : mColorBindings)
  {
    ColorBindingRegistry().erase(binding.dynamicPropertyId);
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
