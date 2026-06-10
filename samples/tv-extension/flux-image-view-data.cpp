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

#include "flux-image-view-data.h"

namespace
{
constexpr uint32_t SIGNAL_DELAY_MS = 3000u;
}

FluxImageViewData::FluxImageViewData(Dali::Ui::ImageView targetImageView)
: FluxViewData(targetImageView)
{
}

void FluxImageViewData::SetAsyncImageResize(bool asyncImageResize)
{
  mAsyncImageResize = asyncImageResize;

  if(!mAsyncImageResizeTimer)
  {
    mAsyncImageResizeTimer = Dali::Timer::New(SIGNAL_DELAY_MS);
    mAsyncImageResizeTimer.TickSignal().Connect(this, &FluxImageViewData::OnAsyncImageResizeTimerTick);
  }

  mAsyncImageResizeTimer.Start();
}

bool FluxImageViewData::IsImageFittingResize() const
{
  return mAsyncImageResize;
}

FluxImageViewData::AsyncImageResizeFinishedSignalType& FluxImageViewData::AsyncImageResizeFinishedSignal()
{
  return mAsyncImageResizeFinishedSignal;
}

Dali::Ui::ImageView FluxImageViewData::GetTargetImageView() const
{
  return Dali::Ui::ImageView::DownCast(GetTargetView());
}

bool FluxImageViewData::OnAsyncImageResizeTimerTick()
{
  Dali::Ui::ImageView targetImageView = GetTargetImageView();
  if(targetImageView)
  {
    mAsyncImageResizeFinishedSignal.Emit(targetImageView);
  }

  return false;
}

