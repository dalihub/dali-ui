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
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-actions-integ.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>

#include <memory>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
namespace AnimatedVectorImageVisual
{
namespace
{
/**
 * @brief Forwards a renderer callback to a dali-ui one, converting the property argument.
 *
 * CallbackFunctorReturn3 copies the functor it is given, so the dali-ui callback is held
 * through a shared pointer: it is move-only and must outlive every copy of this functor.
 */
struct DynamicPropertyForwarder
{
  Dali::Property::Value operator()(int32_t id, Dali::VectorAnimationRenderer::VectorProperty property, uint32_t frameNumber)
  {
    // The renderer knows properties dali-ui does not expose; nothing sensible can be returned
    // for those, so they resolve to an empty value.
    const auto index = static_cast<uint32_t>(property);
    if(!mCallback || index > static_cast<uint32_t>(Ui::LottieAnimation::ContentProperty::TRANSFORM_OPACITY))
    {
      return Dali::Property::Value();
    }
    return mCallback->Invoke(id, static_cast<Ui::LottieAnimation::ContentProperty>(index), frameNumber);
  }

  std::shared_ptr<Ui::LottieAnimation::DynamicPropertyCallback> mCallback;
};
} // namespace

CallbackBase* WrapDynamicPropertyCallback(Ui::LottieAnimation::DynamicPropertyCallback callback)
{
  if(!callback)
  {
    return nullptr;
  }

  DynamicPropertyForwarder forwarder{std::make_shared<Ui::LottieAnimation::DynamicPropertyCallback>(std::move(callback))};

  return new CallbackFunctorReturn3<DynamicPropertyForwarder,
                                    int32_t,
                                    Dali::VectorAnimationRenderer::VectorProperty,
                                    uint32_t,
                                    Dali::Property::Value>(forwarder);
}

} // namespace AnimatedVectorImageVisual

} // namespace Integration
} // namespace Ui

} //namespace DALI_NAMESPACE
