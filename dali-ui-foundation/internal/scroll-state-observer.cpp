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
#include <dali-ui-foundation/internal/scroll-state-observer.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

ScrollStateObserver& ScrollStateObserver::Get()
{
  static ScrollStateObserver instance;
  return instance;
}

void ScrollStateObserver::NotifyGestureDisambiguationBegan()
{
  ++mDisambiguatingDepth;
  mDisambiguationBeganSignal.Emit();
}

void ScrollStateObserver::NotifyGestureDisambiguationEnded()
{
  if(DALI_LIKELY(mDisambiguatingDepth > 0))
  {
    --mDisambiguatingDepth;
  }
  mDisambiguationEndedSignal.Emit();
}

void ScrollStateObserver::NotifyDragStarted()
{
  ++mDraggingDepth;
  mDragStartedSignal.Emit();
}

void ScrollStateObserver::NotifyDragFinished()
{
  if(DALI_LIKELY(mDraggingDepth > 0))
  {
    --mDraggingDepth;
  }
  mDragFinishedSignal.Emit();
}

void ScrollStateObserver::NotifyScrollStarted()
{
  ++mScrollingDepth;
  mScrollStartedSignal.Emit();
}

void ScrollStateObserver::NotifyScrollFinished()
{
  if(DALI_LIKELY(mScrollingDepth > 0))
  {
    --mScrollingDepth;
  }
  mScrollFinishedSignal.Emit();
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
