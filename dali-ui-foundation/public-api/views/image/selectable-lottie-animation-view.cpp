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
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/selectable-lottie-animation-view-impl.h>

namespace Dali
{
namespace Ui
{

SelectableLottieAnimationView SelectableLottieAnimationView::New(const SelectableLottieImage& image)
{
  auto* impl = Integration::SelectableLottieAnimationViewImpl::New(image.GetUrl(),
                                                                   image.GetSelectRange(),
                                                                   image.GetDeselectRange(),
                                                                   image.GetInnerFillKeyPath());
  return SelectableLottieAnimationView(impl);
}

SelectableLottieAnimationView SelectableLottieAnimationView::DownCast(BaseHandle handle)
{
  return SelectableLottieAnimationView(dynamic_cast<Integration::SelectableLottieAnimationViewImpl*>(handle.GetObjectPtr()));
}

SelectableLottieAnimationView::SelectableLottieAnimationView(Integration::SelectableLottieAnimationViewImpl* impl)
: SelectableImageInterface(impl)
{
}

} // namespace Ui
} // namespace Dali
