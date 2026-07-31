/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/property-map-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/stream-operators.h>

#include <locale>

// INTERNAL
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visuals-container.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visuals-container-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali::Ui::Internal
{
namespace
{
constexpr uint32_t MAXIMUM_VISUAL_OBJECTS_COUNT = (Dali::Ui::Integration::DepthIndex::Ranges::CONTENT - Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND) / 2;

Vector4 ToVector4(const Insets& insets)
{
  return Vector4(insets.start, insets.end, insets.top, insets.bottom);
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gVisualBaseLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VISUAL_BASE");
#endif

inline bool IsMutableVisualPropertyIndex(Dali::Property::Index index)
{
  return (index >= Dali::Ui::PropertyRanges::VISUAL_MUTABLE_PROPERTY_START_INDEX && index <= Dali::Ui::PropertyRanges::VISUAL_MUTABLE_PROPERTY_END_INDEX);
}

inline bool IsReadOnlyVisualPropertyIndex(Dali::Property::Index index)
{
  return (index >= Dali::Ui::PropertyRanges::VISUAL_READ_ONLY_PROPERTY_START_INDEX && index <= Dali::Ui::PropertyRanges::VISUAL_READ_ONLY_PROPERTY_END_INDEX);
}

inline Vector4 ConvertProportionFlagsToOffsetSizeMode(Dali::Ui::Visual::Transform::ProportionFlags flags)
{
  // fast-out for standard cases
  if(flags == Dali::Ui::Visual::Transform::ProportionFlags::ALL)
  {
    return Vector4::ZERO;
  }
  if(flags == Dali::Ui::Visual::Transform::ProportionFlags::NONE)
  {
    return Vector4::ONE;
  }

  Vector4 offsetSizeMode = Vector4::ONE;
  if((flags & Dali::Ui::Visual::Transform::ProportionFlags::X_PROPORTIONAL) == Dali::Ui::Visual::Transform::ProportionFlags::X_PROPORTIONAL)
  {
    offsetSizeMode.x = 0.0f;
  }
  if((flags & Dali::Ui::Visual::Transform::ProportionFlags::Y_PROPORTIONAL) == Dali::Ui::Visual::Transform::ProportionFlags::Y_PROPORTIONAL)
  {
    offsetSizeMode.y = 0.0f;
  }
  if((flags & Dali::Ui::Visual::Transform::ProportionFlags::WIDTH_PROPORTIONAL) == Dali::Ui::Visual::Transform::ProportionFlags::WIDTH_PROPORTIONAL)
  {
    offsetSizeMode.z = 0.0f;
  }
  if((flags & Dali::Ui::Visual::Transform::ProportionFlags::HEIGHT_PROPORTIONAL) == Dali::Ui::Visual::Transform::ProportionFlags::HEIGHT_PROPORTIONAL)
  {
    offsetSizeMode.w = 0.0f;
  }
  return offsetSizeMode;
}
inline Dali::Ui::Visual::Transform::ProportionFlags ConvertOffsetSizeModeToProportionFlags(const Vector4& offsetSizeMode)
{
  // fast-out for standard cases
  if(offsetSizeMode == Vector4::ZERO)
  {
    return Dali::Ui::Visual::Transform::ProportionFlags::ALL;
  }
  if(offsetSizeMode == Vector4::ONE)
  {
    return Dali::Ui::Visual::Transform::ProportionFlags::NONE;
  }

  Dali::Ui::Visual::Transform::ProportionFlags flags = Dali::Ui::Visual::Transform::ProportionFlags::NONE;
  if(Dali::EqualsZero(offsetSizeMode.x))
  {
    flags = flags | Dali::Ui::Visual::Transform::ProportionFlags::X_PROPORTIONAL;
  }
  if(Dali::EqualsZero(offsetSizeMode.y))
  {
    flags = flags | Dali::Ui::Visual::Transform::ProportionFlags::Y_PROPORTIONAL;
  }
  if(Dali::EqualsZero(offsetSizeMode.z))
  {
    flags = flags | Dali::Ui::Visual::Transform::ProportionFlags::WIDTH_PROPORTIONAL;
  }
  if(Dali::EqualsZero(offsetSizeMode.w))
  {
    flags = flags | Dali::Ui::Visual::Transform::ProportionFlags::HEIGHT_PROPORTIONAL;
  }
  return flags;
}
} // namespace

VisualBaseImplPtr VisualBaseImpl::New(Dali::Ui::VisualType type)
{
  VisualBaseImplPtr visualObject(new VisualBaseImpl(type));
  return visualObject;
}

///< Public API

Dali::Ui::View VisualBaseImpl::GetOwner() const
{
  Dali::Ui::View result;
  if(auto container = mContainer.GetHandle())
  {
    result = container.GetOwner();
  }
  return result;
}

Dali::Ui::Integration::Visual::InternalContainerRangeType VisualBaseImpl::GetInternalContainerRangeType() const
{
  return mRangeType;
}

void VisualBaseImpl::DetachFromContainer()
{
  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) Detached (Visual::Base[%p])\n", this, GetName().CStr(), mVisual.GetObjectPtr());
  if(auto container = mContainer.GetHandle())
  {
    container.RemoveVisualBase(Dali::Ui::VisualBase(this));
  }
  DetachFromContainerInternal();
}

void VisualBaseImpl::CreateVisual(const Dali::Property::Map& map)
{
  if(Dali::Adaptor::IsAvailable())
  {
    auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
    if(visualFactory)
    {
      if(!map.Empty())
      {
        mVisual = visualFactory.CreateVisual(map, static_cast<Dali::Ui::Integration::VisualFactory::CreationOptions>(static_cast<int>(Dali::Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY) |
                                                                                                                     static_cast<int>(Dali::Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_IGNORE_VIEW_PADDING)));
      }
      else
      {
        // Special behavior, if map is empty, unregister the visual.
        mVisual.Reset();
      }
    }

    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) Create new visual (Visual::Base[%p])\n", this, GetName().CStr(), mVisual.GetObjectPtr());

    // Replace as newly created visual now.
    // DepthIndex will be applied here.
    if(auto container = mContainer.GetHandle())
    {
      GetImplementation(container).ReplaceVisualBase(*this);
    }
  }
}

void VisualBaseImpl::RetrieveVisualPropertyMap(Dali::Property::Map& map) const
{
  if(mVisual)
  {
    mVisual.CreatePropertyMap(map);
  }
}

void VisualBaseImpl::DoAction(Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
#if defined(DEBUG_ENABLED)
  {
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << attributes;
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) DoAction(%d, %s) (Visual::Base[%p])\n", this, GetName().CStr(), actionId, oss.str().c_str(), mVisual.GetObjectPtr());
  }
#endif
  if(mVisual)
  {
    mVisual.DoAction(actionId, attributes);
  }
}
/// Properties

void VisualBaseImpl::SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue)
{
#if defined(DEBUG_ENABLED)
  {
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << propertyValue;
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) Status(%d) SetProperty(%d, %s) (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(mPropertyUpdatedStatus), index, oss.str().c_str(), mVisual.GetObjectPtr());
  }
#endif

  if(DALI_UNLIKELY(IsReadOnlyVisualPropertyIndex(index)))
  {
    DALI_LOG_ERROR("PropertyIndex(%d) is read-only. Ignore setter\n", index);
    return;
  }

  // TODO : Check if value is really changed. Is it overhead?
  if(mPropertyUpdatedStatus == PropertyUpdatedStatus::IDLE)
  {
    mPropertyUpdatedStatus = IsMutableVisualPropertyIndex(index) ? PropertyUpdatedStatus::MUTABLE_PROPERTY_CHANGED : PropertyUpdatedStatus::IMMUTABLE_PROPERTY_CHANGED;
  }
  switch(mPropertyUpdatedStatus)
  {
    case PropertyUpdatedStatus::MUTABLE_PROPERTY_CHANGED:
    {
      if(IsMutableVisualPropertyIndex(index))
      {
        mUpdatedMutableVisualProperties[index] = propertyValue;
        break;
      }

      DALI_FALLTHROUGH;
    }
    default:
    {
      // Some immutable property changed. Prepare to create new visual.
      mPropertyUpdatedStatus = PropertyUpdatedStatus::IMMUTABLE_PROPERTY_CHANGED;
      mCachedVisualPropertyMap.Merge(std::move(mUpdatedMutableVisualProperties));
      mUpdatedMutableVisualProperties = Dali::Property::Map();
      DALI_FALLTHROUGH;
    }
    case PropertyUpdatedStatus::IMMUTABLE_PROPERTY_CHANGED:
    {
      mCachedVisualPropertyMap[index] = propertyValue;
      break;
    }
  }

  RequestUpdateProperty();
}

Dali::Property::Value VisualBaseImpl::GetProperty(Dali::Property::Index index) const
{
  // 1. Query to mutalbe properties first.
  if(mPropertyUpdatedStatus == PropertyUpdatedStatus::MUTABLE_PROPERTY_CHANGED)
  {
    const auto* valuePtr = mUpdatedMutableVisualProperties.Find(index);
    if(valuePtr)
    {
      DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) GetProperty(%d) from mutable (Visual::Base[%p])\n", this, GetName().CStr(), index, mVisual.GetObjectPtr());
      return *valuePtr;
    }
  }

  // 2. Query to cached properties.
  {
    const auto* valuePtr = mCachedVisualPropertyMap.Find(index);
    if(valuePtr)
    {
      DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) GetProperty(%d) from cache (Visual::Base[%p])\n", this, GetName().CStr(), index, mVisual.GetObjectPtr());
      return *valuePtr;
    }
  }

  // 3. Query to visual directy.
  if(mVisual)
  {
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) Retrieve properties at GetProperty(%d) (Visual::Base[%p])\n", this, GetName().CStr(), index, mVisual.GetObjectPtr());
    RetrieveVisualPropertyMap(mCachedVisualPropertyMap);

    const auto* valuePtr = mCachedVisualPropertyMap.Find(index);
    if(valuePtr)
    {
      DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) GetProperty(%d) from cache (Visual::Base[%p])\n", this, GetName().CStr(), index, mVisual.GetObjectPtr());
      return *valuePtr;
    }
  }

  // 4. No result. Return default properties now.
  // Some properties has non-empty default value.
  switch(index)
  {
    case Ui::VisualBasePropertyIndex::MIX_COLOR:
    {
      return mCachedVisualPropertyMap[index] = Color::WHITE;
    }
    case Ui::Integration::Visual::Property::CORNER_RADIUS_POLICY:
    {
      return mCachedVisualPropertyMap[index] = CornerRadiusPolicy::ABSOLUTE;
    }
    case Ui::Integration::Visual::Property::BORDERLINE_COLOR:
    {
      return mCachedVisualPropertyMap[index] = Color::BLACK;
    }
  }

  // Store empty property, so we can fast-out next getter
  return mCachedVisualPropertyMap[index] = Property::Value();
}

const Dali::String& VisualBaseImpl::GetName() const
{
  return mName;
}

void VisualBaseImpl::SetName(const Dali::String& name)
{
  mName = name;
  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) SetName (Visual::Base[%p])\n", this, GetName().CStr(), mVisual.GetObjectPtr());
}

UiColor VisualBaseImpl::GetColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Dali::BaseHandle(const_cast<VisualBaseImpl*>(this)), "VisualMixColor", outColor))
  {
    return outColor;
  }
  return GetProperty(Dali::Ui::VisualBasePropertyIndex::MIX_COLOR).Get<Vector4>();
}

void VisualBaseImpl::SetColor(const UiColor& color)
{
  SetColorBinding("VisualMixColor", color, this, &VisualBaseImpl::SetColorInternal);
}

// Transform relative properties
float VisualBaseImpl::GetOffsetX() const
{
  return mTransform ? mTransform->mOffset.x : 0.0f;
}

void VisualBaseImpl::SetOffsetX(float x)
{
  if(mTransform || !Dali::Equals(x, 0.0f))
  {
    if(!mTransform || !Dali::Equals(mTransform->mOffset.x, x))
    {
      GetOrCreateTransform().mOffset.x = x;
      mTransformChanged                = true;

      RequestUpdateProperty();
    }
  }
}

float VisualBaseImpl::GetOffsetY() const
{
  return mTransform ? mTransform->mOffset.y : 0.0f;
}

void VisualBaseImpl::SetOffsetY(float y)
{
  if(mTransform || !Dali::Equals(y, 0.0f))
  {
    if(!mTransform || !Dali::Equals(mTransform->mOffset.y, y))
    {
      GetOrCreateTransform().mOffset.y = y;
      mTransformChanged                = true;

      RequestUpdateProperty();
    }
  }
}

float VisualBaseImpl::GetWidth() const
{
  return mTransform ? mTransform->mSize.width : 1.0f;
}

void VisualBaseImpl::SetWidth(float width)
{
  if(mTransform || !Dali::Equals(width, 1.0f))
  {
    if(!mTransform || !Dali::Equals(mTransform->mSize.width, width))
    {
      GetOrCreateTransform().mSize.width = width;
      mTransformChanged                  = true;

      RequestUpdateProperty();
    }
  }
}

float VisualBaseImpl::GetHeight() const
{
  return mTransform ? mTransform->mSize.height : 1.0f;
}

void VisualBaseImpl::SetHeight(float height)
{
  if(mTransform || !Dali::Equals(height, 1.0f))
  {
    if(!mTransform || !Dali::Equals(mTransform->mSize.height, height))
    {
      GetOrCreateTransform().mSize.height = height;
      mTransformChanged                   = true;

      RequestUpdateProperty();
    }
  }
}

Dali::Ui::Visual::Transform::ProportionFlags VisualBaseImpl::GetProportionFlags() const
{
  return ConvertOffsetSizeModeToProportionFlags(mTransform ? mTransform->mOffsetSizeMode : Vector4::ZERO);
}

void VisualBaseImpl::SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags flags)
{
  if(mTransform || flags != Dali::Ui::Visual::Transform::ProportionFlags::ALL)
  {
    Vector4 offsetSizeMode = ConvertProportionFlagsToOffsetSizeMode(flags);
    if(!mTransform || mTransform->mOffsetSizeMode != offsetSizeMode)
    {
      GetOrCreateTransform().mOffsetSizeMode = offsetSizeMode;
      mTransformChanged                      = true;

      RequestUpdateProperty();
    }
  }
}

float VisualBaseImpl::GetExtraWidth() const
{
  return mTransform ? mTransform->mExtraSize.width : 0.0f;
}

void VisualBaseImpl::SetExtraWidth(float extraWidth)
{
  if(mTransform || !Dali::Equals(extraWidth, 0.0f))
  {
    if(!mTransform || !Dali::Equals(mTransform->mExtraSize.width, extraWidth))
    {
      GetOrCreateTransform().mExtraSize.width = extraWidth;
      mTransformChanged                       = true;

      RequestUpdateProperty();
    }
  }
}

float VisualBaseImpl::GetExtraHeight() const
{
  return mTransform ? mTransform->mExtraSize.height : 0.0f;
}

void VisualBaseImpl::SetExtraHeight(float extraHeight)
{
  if(mTransform || !Dali::Equals(extraHeight, 0.0f))
  {
    if(!mTransform || !Dali::Equals(mTransform->mExtraSize.height, extraHeight))
    {
      GetOrCreateTransform().mExtraSize.height = extraHeight;
      mTransformChanged                        = true;

      RequestUpdateProperty();
    }
  }
}

Align::Type VisualBaseImpl::GetOrigin() const
{
  return mTransform ? mTransform->mOrigin : Align::TOP_BEGIN;
}

void VisualBaseImpl::SetOrigin(Align::Type origin)
{
  if(mTransform || origin != Align::TOP_BEGIN)
  {
    if(!mTransform || mTransform->mOrigin != origin)
    {
      GetOrCreateTransform().mOrigin = origin;
      mTransformChanged              = true;

      RequestUpdateProperty();
    }
  }
}

Align::Type VisualBaseImpl::GetPivot() const
{
  return mTransform ? mTransform->mPivot : Align::TOP_BEGIN;
}

void VisualBaseImpl::SetPivot(Align::Type pivot)
{
  if(mTransform || pivot != Align::TOP_BEGIN)
  {
    if(!mTransform || mTransform->mPivot != pivot)
    {
      GetOrCreateTransform().mPivot = pivot;
      mTransformChanged             = true;

      RequestUpdateProperty();
    }
  }
}

// Decorated properties (CornerRadius / Borderline)

UiColor VisualBaseImpl::GetBorderlineColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Dali::BaseHandle(const_cast<VisualBaseImpl*>(this)), "VisualBorderlineColor", outColor))
  {
    return outColor;
  }
  return GetProperty(Dali::Ui::Integration::Visual::Property::BORDERLINE_COLOR).Get<Vector4>();
}

void VisualBaseImpl::SetBorderlineColor(const UiColor& color)
{
  SetColorBinding("VisualBorderlineColor", color, this, &VisualBaseImpl::SetBorderlineColorInternal);
}

/// Sibling Order Change API

uint32_t VisualBaseImpl::GetSiblingOrder() const
{
  // If we are not in container, return 0.
  if(!mContainer.GetHandle())
  {
    return 0u;
  }
  return mSiblingOrder;
}

void VisualBaseImpl::SetSiblingOrder(uint32_t siblingOrder)
{
  auto container = mContainer.GetHandle();
  if(container && mSiblingOrder != siblingOrder)
  {
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) SetSiblingOrder(%u -> %u). Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), mSiblingOrder, siblingOrder, static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
    GetImplementation(container).ChangeSiblingOrder(mSiblingOrder, siblingOrder);
  }
}

void VisualBaseImpl::Raise()
{
  auto container = mContainer.GetHandle();
  if(container)
  {
    if(mSiblingOrder + 1u < container.GetVisualBasesCount())
    {
      SetSiblingOrder(mSiblingOrder + 1u);
    }
  }
}

void VisualBaseImpl::Lower()
{
  auto container = mContainer.GetHandle();
  if(container)
  {
    if(mSiblingOrder > 0u)
    {
      SetSiblingOrder(mSiblingOrder - 1u);
    }
  }
}

void VisualBaseImpl::RaiseToTop()
{
  auto container = mContainer.GetHandle();
  if(container && container.GetVisualBasesCount() > 0u)
  {
    SetSiblingOrder(container.GetVisualBasesCount() - 1u);
  }
}

void VisualBaseImpl::LowerToBottom()
{
  auto container = mContainer.GetHandle();
  if(container)
  {
    SetSiblingOrder(0u);
  }
}

void VisualBaseImpl::RaiseAbove(Dali::Ui::Internal::VisualBaseImpl& target)
{
  auto container = mContainer.GetHandle();
  // Check whether target and this is in the same container.
  if(container && container == target.GetContainer())
  {
    uint32_t fromIndex = mSiblingOrder;
    uint32_t toIndex   = target.GetSiblingOrder();

    // Change sibling order only if fromIndex is less than toIndex.
    // Note : target's sibling order will be updated automatically.
    if(fromIndex < toIndex)
    {
      SetSiblingOrder(toIndex);
    }
  }
}

void VisualBaseImpl::LowerBelow(Dali::Ui::Internal::VisualBaseImpl& target)
{
  auto container = mContainer.GetHandle();
  // Check whether target and this is in the same container.
  if(container && container == target.GetContainer())
  {
    uint32_t fromIndex = mSiblingOrder;
    uint32_t toIndex   = target.GetSiblingOrder();

    // Change sibling order only if fromIndex is greater than toIndex.
    // Note : target's sibling order will be updated automatically.
    if(fromIndex > toIndex)
    {
      SetSiblingOrder(toIndex);
    }
  }
}

// Private and internal

Dali::Property VisualBaseImpl::GetPropertyObject(Dali::Property::Key visualPropertyKey)
{
  if(mVisual)
  {
    return mVisual.GetPropertyObject(std::move(visualPropertyKey));
  }

  Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void VisualBaseImpl::RemoveCache(Dali::Property::Index index) const
{
  mCachedVisualPropertyMap.Remove(index);
}

void VisualBaseImpl::UpdateProperty()
{
  UpdatePropertyInternal();
}

void VisualBaseImpl::DoActionExtension(Dali::Property::Index actionId, const Dali::Any& attributes)
{
  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) DoActionExtension(%d) (Visual::Base[%p])\n", this, GetName().CStr(), actionId, mVisual.GetObjectPtr());
  if(mVisual)
  {
    mVisual.DoActionExtension(actionId, attributes);
  }
}

Dali::Ui::Integration::VisualsContainer VisualBaseImpl::GetContainer() const
{
  return mContainer.GetHandle();
}

void VisualBaseImpl::AttachToContainerInternal(Dali::Ui::Integration::VisualsContainer container)
{
  mContainer = WeakHandle(container);
  mRangeType = container.GetContainerRangeType();

  // Request to create visuals if we never create visuals before.
  if(DALI_UNLIKELY(!mVisual))
  {
    RequestUpdateProperty();
  }
  else
  {
    RequestApplyFittingMode();
  }
}

void VisualBaseImpl::DetachFromContainerInternal()
{
  mContainer.Reset();
  mRangeType        = Dali::Ui::Integration::Visual::InternalContainerRangeType::INVALID;
  mSiblingOrder     = 0u;
  mVisualPropertyId = INVALID_VISUAL_PROPERTY_ID;
}

void VisualBaseImpl::SetSiblingOrderInternal(uint32_t siblingOrder)
{
  mSiblingOrder = siblingOrder;
  if(mVisual)
  {
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) SiblingOder(%u), SetDepthIndex(%u). Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), mSiblingOrder, GetDepthIndex(), static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
    mVisual.SetDepthIndex(GetDepthIndex());
  }
}

int32_t VisualBaseImpl::GetDepthIndex() const
{
  int32_t baseDepthIndex = 0;
  switch(mRangeType)
  {
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::UNDER_BACKGROUND_EFFECT:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::MINIMUM_DEPTH_INDEX;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT + static_cast<int32_t>(MAXIMUM_VISUAL_OBJECTS_COUNT);
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT:
    default:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND + static_cast<int32_t>(MAXIMUM_VISUAL_OBJECTS_COUNT);
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::BETWEEN_CONTENT_AND_DECORATION:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::CONTENT + MAXIMUM_VISUAL_OBJECTS_COUNT;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::BETWEEN_DECORATION_AND_FOREGROUND_EFFECT:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::DECORATION + MAXIMUM_VISUAL_OBJECTS_COUNT;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::OVER_FOREGROUND_EFFECT:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT + MAXIMUM_VISUAL_OBJECTS_COUNT;
      break;
    }

    // Internal container cases
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::BACKGROUND_EFFECT:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::BACKGROUND:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::CONTENT:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::CONTENT;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::DECORATION:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::DECORATION;
      break;
    }
    case Dali::Ui::Integration::Visual::InternalContainerRangeType::FOREGROUND_EFFECT:
    {
      baseDepthIndex = Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT;
      break;
    }
  }
  return baseDepthIndex + static_cast<int32_t>(mSiblingOrder);
}

void VisualBaseImpl::SetColorInternal(const Vector4& color)
{
  SetProperty(Dali::Ui::VisualBasePropertyIndex::MIX_COLOR, color);
}

void VisualBaseImpl::SetBorderlineColorInternal(const Vector4& borderlineColor)
{
  SetProperty(Dali::Ui::Integration::Visual::Property::BORDERLINE_COLOR, borderlineColor);
}

void VisualBaseImpl::ApplyTransfromToPropertyMap()
{
  if(mTransformChanged && DALI_LIKELY(mTransform))
  {
    mTransformChanged = false;

    Property::Map transformProperties;
    mTransform->GetPropertyMap(transformProperties);

    SetProperty(Dali::Ui::VisualBasePropertyIndex::TRANSFORM, transformProperties);
  }
}

void VisualBaseImpl::Process(bool postProcessor)
{
  if(!postProcessor)
  {
    // Processor for UpdateProperty
    mUpdatePropertyRegistered = false;

    UpdatePropertyInternal();
  }
  else
  {
    // Processor for ApplyFittingMode
    mApplyFittingModeRegistered = false;

    Dali::Ui::View owner = GetOwner();
    if(owner)
    {
      // TODO : Can we believe this values?
      // TODO : Need to consider RTL case.
      Vector2 viewSize(owner.GetSize());
      auto&   viewImpl       = GetImpl(owner);
      Insets  viewPadding    = viewImpl.GetPadding();
      float   effectiveScale = viewImpl.GetEffectiveScale();
      ApplyFittingModeInternal(viewSize, viewPadding, effectiveScale);
    }
  }
}

void VisualBaseImpl::RequestUpdateProperty()
{
  if(!mUpdatePropertyRegistered)
  {
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) RequestUpdateProperty. Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
    Adaptor::Get().RegisterProcessorOnce(*this, false);
    mUpdatePropertyRegistered = true;
  }
}
void VisualBaseImpl::RequestApplyFittingMode()
{
  if(mVisual && GetImplementation(mVisual).IsFittingModeRequired())
  {
    if(!mApplyFittingModeRegistered)
    {
      DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) RequestApplyFittingMode. Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
      Adaptor::Get().RegisterProcessorOnce(*this, true);
      mApplyFittingModeRegistered = true;
    }
  }
}

void VisualBaseImpl::UpdatePropertyInternal()
{
  ApplyTransfromToPropertyMap();

  auto previousUpdatedStatus = mPropertyUpdatedStatus;
  mPropertyUpdatedStatus     = PropertyUpdatedStatus::IDLE;

  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) UpdatePropertyInternal() Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(previousUpdatedStatus), mVisual.GetObjectPtr());

  switch(previousUpdatedStatus)
  {
    case PropertyUpdatedStatus::IDLE:
    default:
    {
      break;
    }
    case PropertyUpdatedStatus::MUTABLE_PROPERTY_CHANGED:
    {
      const bool visualCreateRequired = DALI_UNLIKELY(!mVisual);

      if(DALI_LIKELY(mVisual))
      {
        DoAction(Dali::Ui::Integration::Visual::Action::UPDATE_PROPERTY, mUpdatedMutableVisualProperties);
      }

      mCachedVisualPropertyMap.Merge(std::move(mUpdatedMutableVisualProperties));
      mUpdatedMutableVisualProperties = Dali::Property::Map();

      if(DALI_LIKELY(!visualCreateRequired))
      {
        break;
      }

      // (Rare case) Create new visual if we remove visual forcibly previous.
      DALI_FALLTHROUGH;
    }
    case PropertyUpdatedStatus::IMMUTABLE_PROPERTY_CHANGED:
    {
      CreateVisual(mCachedVisualPropertyMap);

      RetrieveVisualPropertyMap(mCachedVisualPropertyMap);

      RequestApplyFittingMode();
      break;
    }
  }
  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) UpdatePropertyInternal() Status(%d) done (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(previousUpdatedStatus), mVisual.GetObjectPtr());
}

void VisualBaseImpl::ApplyFittingModeInternal(const Vector2& controlSize, const Insets& viewPadding, float effectiveScale)
{
#if defined(DEBUG_ENABLED)
  {
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << controlSize << ", " << ToVector4(viewPadding) << ", " << effectiveScale;
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) ApplyFittingModeInternal(%s) Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), oss.str().c_str(), static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
  }
#endif

  if(mVisual)
  {
    auto& visualImpl = GetImplementation(mVisual);
    if(visualImpl.IsFittingModeRequired())
    {
      // Make to use mTransform->GetVisualSize(controlSize) instead in future.
      // For now, mTransform is same with visual's transform. So we cannot use it.
      // We need to separate this variables.
      visualImpl.ApplyFittingMode(controlSize, viewPadding, effectiveScale);
    }
  }

  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) ApplyFittingModeInternal() Status(%d) done (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
}

VisualBaseImpl::VisualBaseImpl(Dali::Ui::VisualType type)
: mContainer(),
  mVisualType(type),
  mName(),
  mCachedVisualPropertyMap(Dali::CreatePropertyMap({{Dali::Ui::VisualBasePropertyIndex::TYPE, mVisualType}, {Dali::Ui::VisualBasePropertyIndex::MIX_COLOR, Color::WHITE}})),
  mUpdatedMutableVisualProperties(),
  mTransform(nullptr),
  mVisual(),
  mSiblingOrder(0u),
  mVisualPropertyId(VisualBaseImpl::INVALID_VISUAL_PROPERTY_ID),
  mPropertyUpdatedStatus(PropertyUpdatedStatus::IMMUTABLE_PROPERTY_CHANGED), ///< To make ensure to create visual at least once.
  mTransformChanged(false),
  mUpdatePropertyRegistered(false),
  mApplyFittingModeRegistered(false)
{
}

VisualBaseImpl::~VisualBaseImpl()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General, "VisualBaseImpl[%p](%s) Status(%d) (Visual::Base[%p])\n", this, GetName().CStr(), static_cast<int>(mPropertyUpdatedStatus), mVisual.GetObjectPtr());
    if(mUpdatePropertyRegistered)
    {
      // Unregister the processor from the adaptor
      Adaptor::Get().UnregisterProcessorOnce(*this, false);
    }
    if(mApplyFittingModeRegistered)
    {
      // Unregister the processor from the adaptor
      Adaptor::Get().UnregisterProcessorOnce(*this, true);
    }

    DetachFromContainerInternal();

    if(mVisual)
    {
      auto visualFactory = Dali::Ui::Integration::VisualFactory::Get();
      if(visualFactory)
      {
        visualFactory.DiscardVisual(std::move(mVisual));
      }
    }
  }
}

} // namespace Dali::Ui::Internal
