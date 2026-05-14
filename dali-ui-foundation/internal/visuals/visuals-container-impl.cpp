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
#include <dali-ui-foundation/internal/visuals/visuals-container-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraints.h>

#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h> ///< To get viewDataImpl by Internal::ViewDataImpl::Get()
#include <dali-ui-foundation/internal/views/view/visual-constraint-functions.h>
#include <dali-ui-foundation/public-api/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToPropertyValue;

namespace Dali::Ui::Internal
{
namespace
{
constexpr std::string_view VISUAL_OBJECT_PROPERTY_NAME_PREFIX("VisualBase");

constexpr uint32_t MAXIMUM_VISUAL_OBJECTS_COUNT = (Dali::Ui::DepthIndex::Ranges::CONTENT - Dali::Ui::DepthIndex::Ranges::BACKGROUND) / 2;

static constexpr uint32_t INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 10);

Dali::Constraint CreateVisualCornerConstraint(Dali::Ui::View view, Dali::Ui::Internal::VisualBaseImpl& visualObjectImpl)
{
  Dali::Constraint constraint;
  auto             visualBase                 = visualObjectImpl.GetVisual();
  auto             visualCornerRadiusProperty = visualBase.GetPropertyObject(Dali::Ui::DevelVisual::Property::CORNER_RADIUS);
  if(visualObjectImpl.GetShadowType() == Dali::Ui::VisualsContainer::ShadowType::INNER_SHADOW)
  {
    auto visualBorderlineProperty = visualBase.GetPropertyObject(Dali::Ui::DevelVisual::Property::BORDERLINE_WIDTH);

    if(DALI_LIKELY(visualCornerRadiusProperty.propertyIndex != Property::INVALID_INDEX && visualCornerRadiusProperty.object) &&
       DALI_LIKELY(visualBorderlineProperty.propertyIndex != Property::INVALID_INDEX && visualBorderlineProperty.object))
    {
      constraint = Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex, InnerShadowCornerRadiusConstraint);
      constraint.AddSource(Source(view, Dali::Ui::View::Property::CORNER_RADIUS));
      constraint.AddSource(Source(view, Dali::Ui::View::Property::CORNER_RADIUS_POLICY));
      constraint.AddSource(Source(view, Dali::Actor::Property::SIZE));
      constraint.AddSource(LocalSource(Dali::VisualRenderer::Property::EXTRA_SIZE));
      constraint.AddSource(LocalSource(Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH));
      Dali::Integration::ConstraintSetInternalTag(constraint, INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG);
    }
  }
  else if(visualObjectImpl.GetShadowType() == Dali::Ui::VisualsContainer::ShadowType::BOX_SHADOW)
  {
    constraint = Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex, Dali::EqualToConstraint());
    constraint.AddSource(Source(view, Dali::Ui::View::Property::CORNER_RADIUS));
  }
  return constraint;
}

} // namespace

VisualsContainerPtr VisualsContainer::New(Dali::Ui::View view, Dali::Ui::DevelVisual::InternalContainerRangeType rangeType)
{
  VisualsContainerPtr container(new VisualsContainer(view, rangeType));
  return container;
}

///< Public API

Dali::Ui::View VisualsContainer::GetOwner() const
{
  return mView.GetHandle();
}

Dali::Ui::DevelVisual::InternalContainerRangeType VisualsContainer::GetContainerRangeType() const
{
  return mRangeType;
}

uint32_t VisualsContainer::GetVisualBasesCount() const
{
  return static_cast<uint32_t>(mVisualBases.size());
}

Dali::Ui::VisualBase VisualsContainer::GetVisualBaseAt(uint32_t index) const
{
  DALI_ASSERT_ALWAYS(index < mVisualBases.size() && "Visual object index out of bounds");
  return mVisualBases[index];
}

bool VisualsContainer::AddVisualBase(Dali::Ui::VisualBase visualObject, Dali::Ui::VisualsContainer::ShadowType shadowType)
{
  if(visualObject)
  {
    if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
    {
      DALI_LOG_ERROR("Application is terminated, or worker thread call this API. Add VisualBase failed.\n");
      return false;
    }

    Dali::Ui::VisualsContainer self(this); // Keep reference for safety

    auto& visualObjectImpl = GetImplementation(visualObject);
    auto  oldContainer     = visualObjectImpl.GetContainer();

    // Skip below logic if visual object is already in this container.
    if(self != oldContainer)
    {
      if(mVisualBases.size() >= MAXIMUM_VISUAL_OBJECTS_COUNT)
      {
        DALI_LOG_ERROR("Visual objects container is full. Add VisualBase failed.\n");
        return false;
      }

      if(oldContainer)
      {
        visualObject.Detach();
      }

      // Add to this container.
      visualObjectImpl.AttachToContainerInternal(self);
      visualObjectImpl.SetSiblingOrderInternal(mVisualBases.size());
      visualObjectImpl.SetShadowType(shadowType);

      ReplaceVisualBase(visualObjectImpl);

      mVisualBases.push_back(visualObject);
      return true;
    }
  }
  return false;
}

void VisualsContainer::RemoveVisualBase(Dali::Ui::VisualBase visualObject)
{
  if(visualObject)
  {
    if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
    {
      return;
    }

    Dali::Ui::VisualsContainer self(this); // Keep reference for safety

    auto iter = std::find(mVisualBases.begin(), mVisualBases.end(), visualObject);
    if(iter != mVisualBases.end())
    {
      // Shift down other visual objects sibling order
      uint32_t siblingOrder = visualObject.GetSiblingOrder();
      for(auto jter = iter + 1; jter != mVisualBases.end(); ++jter)
      {
        GetImplementation(*jter).SetSiblingOrderInternal(siblingOrder++);
      }
      mVisualBases.erase(iter);

      auto& visualObjectImpl = GetImplementation(visualObject);
      UnregisterVisualBase(visualObjectImpl);
      visualObjectImpl.DetachFromContainerInternal();
    }
  }
}

void VisualsContainer::ChangeSiblingOrder(uint32_t fromIndex, uint32_t toIndex)
{
  DALI_ASSERT_ALWAYS(fromIndex < mVisualBases.size() && "fromIndex is out of bounds");
  DALI_ASSERT_ALWAYS(toIndex < mVisualBases.size() && "toIndex is out of bounds");
  if(fromIndex != toIndex)
  {
    // Keep reference of visual object.
    Dali::Ui::VisualBase visualObject = mVisualBases[fromIndex];

    if(fromIndex < toIndex)
    {
      for(uint32_t i = fromIndex; i != toIndex; ++i)
      {
        GetImplementation(mVisualBases[i + 1]).SetSiblingOrderInternal(i);
        mVisualBases[i] = mVisualBases[i + 1];
      }
    }
    else if(fromIndex > toIndex)
    {
      for(uint32_t i = fromIndex; i != toIndex; --i)
      {
        GetImplementation(mVisualBases[i - 1]).SetSiblingOrderInternal(i);
        mVisualBases[i] = mVisualBases[i - 1];
      }
    }
    GetImplementation(visualObject).SetSiblingOrderInternal(toIndex);
    mVisualBases[toIndex] = visualObject;
  }
}

// Private and internal

void VisualsContainer::ReplaceVisualBase(Dali::Ui::Internal::VisualBaseImpl& visualObjectImpl)
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    Dali::Ui::View view = mView.GetHandle();
    if(view)
    {
      VisualBaseImpl::VisualPropertyId propertyId = visualObjectImpl.GetVisualPropertyId();

      Property::Index index = Property::INVALID_INDEX;

      if(propertyId == VisualBaseImpl::INVALID_VISUAL_PROPERTY_ID)
      {
        // Add dummy index to converter, and get unique id.
        propertyId = mVisualIndexConverter.Add(static_cast<uint32_t>(Property::INVALID_INDEX));

        // Register new property to control using propertyId
        {
          std::ostringstream oss;
          oss << VISUAL_OBJECT_PROPERTY_NAME_PREFIX << "_" << static_cast<int>(mRangeType) << "_" << propertyId;
          index = view.RegisterProperty(ToDaliString(oss.str()), ToPropertyValue(oss.str()), Property::AccessMode::READ_WRITE);
        }

        // Change as valid index now.
        mVisualIndexConverter[propertyId] = static_cast<uint32_t>(index);

        visualObjectImpl.SetVisualPropertyId(propertyId);
      }
      else
      {
        index = mVisualIndexConverter[propertyId];
      }

      if(index != Property::INVALID_INDEX)
      {
        auto& viewData = Internal::ViewDataImpl::Get(GetImpl(view));

        // Replace visual.
        auto visualBase = visualObjectImpl.GetVisual();
        if(visualBase)
        {
          // Register the visual to the view.
          viewData.RegisterVisual(index, visualBase, static_cast<int>(visualObjectImpl.GetDepthIndex()));

          if(visualObjectImpl.GetShadowType() != Dali::Ui::VisualsContainer::ShadowType::NONE)
          {
            Dali::Constraint constraint = CreateVisualCornerConstraint(view, visualObjectImpl);
            // Apply Once
            // constraint.Apply();
            viewData.EnableCornerPropertiesOverridden(visualBase, true, constraint);
          }
        }
        else
        {
          // Unregister the visual from the control.
          viewData.UnregisterVisual(index);
        }
      }
    }
  }
}

void VisualsContainer::UnregisterVisualBase(Dali::Ui::Internal::VisualBaseImpl& visualObjectImpl)
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    Dali::Ui::View view = mView.GetHandle();
    if(view)
    {
      VisualBaseImpl::VisualPropertyId propertyId = visualObjectImpl.GetVisualPropertyId();

      DALI_ASSERT_ALWAYS(propertyId != VisualBaseImpl::INVALID_VISUAL_PROPERTY_ID && "VisualBase is not registered before!");

      Property::Index index = static_cast<Property::Index>(mVisualIndexConverter[propertyId]);
      if(index != Property::INVALID_INDEX)
      {
        auto& viewData = Internal::ViewDataImpl::Get(GetImpl(view));
        viewData.UnregisterVisual(index);
      }
      mVisualIndexConverter.Remove(propertyId);

      visualObjectImpl.SetVisualPropertyId(VisualBaseImpl::INVALID_VISUAL_PROPERTY_ID);
    }
  }
}

VisualsContainer::VisualsContainer(Dali::Ui::View view, Dali::Ui::DevelVisual::InternalContainerRangeType rangeType)
: BaseObject(),
  mVisualBases(),
  mView(view),
  mRangeType(rangeType),
  mVisualIndexConverter()
{
  DALI_ASSERT_ALWAYS(0 <= static_cast<int>(mRangeType) && static_cast<int>(mRangeType) < static_cast<int>(Dali::Ui::DevelVisual::InternalContainerRangeType::MAX_COUNT) && "Invalid container range inputed!");
}

VisualsContainer::~VisualsContainer()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    // Detach visual objects without touch the containers.
    for(auto&& visualObject : mVisualBases)
    {
      auto& visualObjectImpl = GetImplementation(visualObject);

      UnregisterVisualBase(visualObjectImpl);
      visualObjectImpl.DetachFromContainerInternal();
    }
    mVisualBases.clear();
  }
}

} //namespace Dali::Ui::Internal
