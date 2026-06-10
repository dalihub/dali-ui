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

#pragma once

#include "flux-image-view-data.h"
#include "flux-label-data.h"
#include "flux-view-data.h"

#include <dali-ui-foundation/public-api/attachment-id.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/label.h>
#include <dali-ui-foundation/public-api/view.h>

namespace TVExt
{

void AttachFluxViewData(Dali::Ui::View view);

void SetFluxPolicy(Dali::Ui::View view, FluxPolicy policy);
FluxPolicy GetFluxPolicy(Dali::Ui::View view);
FluxViewData::FluxPolicyAppliedSignalType& FluxPolicyAppliedSignal(Dali::Ui::View view);

void SetOverflowOption(Dali::Ui::Label label, int option);
int GetOverflowOption(Dali::Ui::Label label);

void SetAsyncImageResize(Dali::Ui::ImageView imageView, bool asyncImageResize);
bool GetAsyncImageResize(Dali::Ui::ImageView imageView);
FluxImageViewData::AsyncImageResizeFinishedSignalType& AsyncImageResizeFinishedSignal(Dali::Ui::ImageView imageView);

} // namespace TVExt
