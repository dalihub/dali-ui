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
#include "animated-image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/animated-image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
BaseHandle Create()
{
  AnimatedImageViewImplPtr impl = AnimatedImageViewImpl::New();
  Ui::View                 view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(AnimatedImageViewImpl, ViewImpl, Create)

DALI_PROPERTY_REGISTRATION(Ui::Integration, AnimatedImageViewImpl, "image", STRING, IMAGE)

DALI_TYPE_REGISTRATION_END()
// clang-format on
} // namespace

AnimatedImageViewImpl::AnimatedImageViewImpl()
: ViewImpl(),
  mUrl(),
  mLoopCount(-1),
  mImageColor(Color::WHITE)
{
}

AnimatedImageViewImpl::~AnimatedImageViewImpl()
{
}

Dali::String AnimatedImageViewImpl::GetResourceUrl() const
{
  return mUrl;
}

AnimatedImageViewImplPtr AnimatedImageViewImpl::New()
{
  return new AnimatedImageViewImpl();
}

void AnimatedImageViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
    }
  }
}

Dali::Property::Value AnimatedImageViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    AnimatedImageViewImpl& impl = static_cast<AnimatedImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case AnimatedImageViewImpl::Property::IMAGE:
        value = impl.GetResourceUrl();
        break;
    }
  }
  return value;
}

void AnimatedImageViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
}

MeasuredSize AnimatedImageViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // TODO: return natural size from animated image visual
  return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
}

void AnimatedImageViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    UpdateVisual();
  }
}

void AnimatedImageViewImpl::Play()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::PLAY, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Pause()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::PAUSE, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::Stop()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(AnimatedImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(AnimatedImageViewImpl::Property::IMAGE, Ui::DevelAnimatedImageVisual::Action::STOP, Dali::Property::Map());
  }
}

void AnimatedImageViewImpl::SetLoopCount(int count)
{
  if(mLoopCount != count)
  {
    mLoopCount = count;
    UpdateVisual();
  }
}

int AnimatedImageViewImpl::GetLoopCount() const
{
  return mLoopCount;
}

void AnimatedImageViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.Resolve() != color.Resolve())
  {
    mImageColor = color;
    // TODO: Update MIX_COLOR directly on the existing visual without full rebuild,
    // as UpdateVisual() restarts animation from the beginning.
    UpdateVisual();
  }
}

UiColor AnimatedImageViewImpl::GetImageColor() const
{
  return mImageColor;
}

Ui::Visual::ResourceStatus AnimatedImageViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(AnimatedImageViewImpl::Property::IMAGE);
}

void AnimatedImageViewImpl::UpdateVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(AnimatedImageViewImpl::Property::IMAGE);
  if(mUrl.Empty())
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE);
  map.Insert(Ui::ImageVisual::Property::URL, mUrl);

  if(mLoopCount != -1)
  {
    map.Insert(Ui::DevelImageVisual::Property::LOOP_COUNT, mLoopCount);
  }

  if(mImageColor.Resolve() != Color::WHITE)
  {
    map.Insert(Visual::Property::MIX_COLOR, mImageColor.Resolve());
  }

  auto visualFactory = Ui::VisualFactory::Get();
  if(visualFactory)
  {
    auto visual = visualFactory.CreateVisual(map);
    if(visual)
    {
      viewData.RegisterVisual(AnimatedImageViewImpl::Property::IMAGE, visual);
    }
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
