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

#include "tv-ext.h"
#include "tv-config.h"

#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/common/dali-common.h>

using namespace Dali;
using namespace Dali::Ui;

namespace TVExt
{

namespace
{

const AttachmentId FLUX_VIEW_DATA_ID = AttachmentId::Alloc();

FluxViewData& GetFluxViewData(View view)
{
  // NOTE Attachment 리스트에서 매번 lookup하는 비용을 절약하기 위해
  // 즉시 조회 가능한 reserved slot을 제공하고, 사업부 확장 전용으로만 사용하는 방법도 고려해 볼 수 있습니다.
  // return view ? view.GetAttachment<FluxViewData>(FLUX_VIEW_DATA_ID) : nullptr;

  DALI_ASSERT_ALWAYS(TVConfig::IsApplied() && "TVConfig must be applied before using TVExt APIs");
  return *view.GetAttachment<FluxViewData>(FLUX_VIEW_DATA_ID);
}

FluxLabelData& GetFluxLabelData(Label label)
{
  return static_cast<FluxLabelData&>(GetFluxViewData(label));
}

FluxImageViewData& GetFluxImageViewData(ImageView imageView)
{
  return static_cast<FluxImageViewData&>(GetFluxViewData(imageView));
}

} // unnamed namespace

void SetFluxPolicy(View view, FluxPolicy policy)
{
  GetFluxViewData(view).SetFluxPolicy(policy);
}

FluxPolicy GetFluxPolicy(View view)
{
  return GetFluxViewData(view).GetFluxPolicy();
}

FluxViewData::FluxPolicyAppliedSignalType& FluxPolicyAppliedSignal(View view)
{
  return GetFluxViewData(view).FluxPolicyAppliedSignal();
}

void SetOverflowOption(Label label, int option)
{
  GetFluxLabelData(label).SetOverflowOption(option);
}

int GetOverflowOption(Label label)
{
  return GetFluxLabelData(label).GetOverflowOption();
}

void SetAsyncImageResize(ImageView imageView, bool asyncImageResize)
{
  GetFluxImageViewData(imageView).SetAsyncImageResize(asyncImageResize);
}

bool GetAsyncImageResize(ImageView imageView)
{
  return GetFluxImageViewData(imageView).IsImageFittingResize();
}

FluxImageViewData::AsyncImageResizeFinishedSignalType& AsyncImageResizeFinishedSignal(ImageView imageView)
{
  return GetFluxImageViewData(imageView).AsyncImageResizeFinishedSignal();
}

void AttachFluxViewData(View view)
{
  // NOTE view 타입별로 적합한 data 를 생성해 넣어줍니다.
  // data 클래스간 관계가 상속으로 정의되어 있으므로 이 방법이 최선으로 보입니다.
  ImageView imageView = ImageView::DownCast(view);
  if(imageView)
  {
    Dali::UniquePtr<FluxViewData> data = Dali::MakeUnique<FluxImageViewData>(imageView);
    view.SetAttachment(FLUX_VIEW_DATA_ID, Dali::Move(data));
    return;
  }

  Label label = Label::DownCast(view);
  if(label)
  {
    Dali::UniquePtr<FluxViewData> data = Dali::MakeUnique<FluxLabelData>(label);
    view.SetAttachment(FLUX_VIEW_DATA_ID, Dali::Move(data));
    return;
  }

  view.SetAttachment(FLUX_VIEW_DATA_ID, Dali::MakeUnique<FluxViewData>(view));
}

} // namespace TVExt
