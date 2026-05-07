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
#include "visual-base-impl.h"

// EXTERNAL HEADER
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/integration-api/stream-operators.h>
#include <algorithm>

// INTERNAL HEARDER
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/helpers/property-helper.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/primitive-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gVisualBaseLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VISUAL_BASE");
#endif
} // namespace

namespace Dali
{
namespace Ui
{
namespace Internal
{
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

namespace
{
/**
 * @brief Check whether this visual type can use corner radius feature or not.
 * @param type VisualType that want to checkup
 * @return true if type can use corner radius feature.
 */
static bool IsTypeAvailableForCornerRadius(Ui::Visual::Type type)
{
  switch(static_cast<Ui::DevelVisual::Type>(type))
  {
    case Ui::Visual::Type::COLOR:
    case Ui::Visual::Type::GRADIENT:
    case Ui::Visual::Type::IMAGE:
    case Ui::Visual::Type::SVG:
    case Ui::Visual::Type::ANIMATED_IMAGE:
    case Ui::DevelVisual::Type::ANIMATED_VECTOR_IMAGE:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}

/**
 * @brief Check whether this visual type can use borderline feature or not.
 * @param type VisualType that want to checkup
 * @return true if type can use borderline feature.
 */
static bool IsTypeAvailableForBorderline(Ui::Visual::Type type)
{
  switch(static_cast<Ui::DevelVisual::Type>(type))
  {
    case Ui::Visual::Type::COLOR:
    case Ui::Visual::Type::GRADIENT:
    case Ui::Visual::Type::IMAGE:
    case Ui::Visual::Type::SVG:
    case Ui::Visual::Type::ANIMATED_IMAGE:
    case Ui::DevelVisual::Type::ANIMATED_VECTOR_IMAGE:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}

struct StringProperty
{
  const char* const name;
  Property::Index   index;
};
StringProperty PROPERTY_NAME_INDEX_TABLE[] = {
  {CUSTOM_SHADER, Ui::Visual::Property::SHADER},
  {TRANSFORM, Ui::Visual::Property::TRANSFORM},
  {MIX_COLOR, Ui::Visual::Property::MIX_COLOR},
  {OPACITY, Ui::Visual::Property::OPACITY},
  {BORDERLINE_WIDTH, Ui::DevelVisual::Property::BORDERLINE_WIDTH},
  {BORDERLINE_COLOR, Ui::DevelVisual::Property::BORDERLINE_COLOR},
  {BORDERLINE_OFFSET, Ui::DevelVisual::Property::BORDERLINE_OFFSET},
  {CORNER_RADIUS, Ui::DevelVisual::Property::CORNER_RADIUS},
  {CORNER_RADIUS_POLICY, Ui::DevelVisual::Property::CORNER_RADIUS_POLICY},
  {CORNER_SQUARENESS, Ui::DevelVisual::Property::CORNER_SQUARENESS},
};
const uint16_t PROPERTY_NAME_INDEX_TABLE_COUNT =
  sizeof(PROPERTY_NAME_INDEX_TABLE) / sizeof(PROPERTY_NAME_INDEX_TABLE[0]);

Property::Index GetVisualPropertyIndex(Property::Key key)
{
  if(key.type == Property::Key::STRING)
  {
    for(auto tableId = 0u; tableId < PROPERTY_NAME_INDEX_TABLE_COUNT; ++tableId)
    {
      if(key == PROPERTY_NAME_INDEX_TABLE[tableId].name)
      {
        return PROPERTY_NAME_INDEX_TABLE[tableId].index;
        break;
      }
    }
  }
  return key.indexKey;
}

} // namespace

Visual::Base::Base(VisualFactoryCache& factoryCache, Ui::Visual::Type type)
: mImpl(new Impl(type)),
  mFactoryCache(factoryCache)
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("Visual::Base[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }
}

Visual::Base::~Base()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("Visual::~Base[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }
  delete mImpl;
}

void Visual::Base::Initialize()
{
  // The Renderer should be created inside derived class here.
  OnInitialize();

  if(mImpl->mRenderer)
  {
    RegisterMixColor();
    RegisterDecoration();

    if(IsBorderlineRequired())
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);
    }
    else if(IsRoundedCornerRequired())
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
    }
  }
}

void Visual::Base::SetCustomShader(const Property::Map& shaderMap)
{
  if(IsUsingCustomShader())
  {
    mImpl->ClearCustomShader();
  }
  mImpl->AddCustomShader(shaderMap);

  // Let derived class know
  UpdateShader();
}

void Visual::Base::SetCustomShader(const Property::Array& shaderArray)
{
  if(IsUsingCustomShader())
  {
    mImpl->ClearCustomShader();
  }

  uint32_t arraySize = shaderArray.Count();
  for(uint32_t i = 0; i < arraySize; ++i)
  {
    const Dali::Property::Map* shaderMap = shaderArray.GetElementAt(i).GetMap();
    if(shaderMap)
    {
      mImpl->AddCustomShader(*shaderMap);
    }
  }

  // Let derived class know
  UpdateShader();
}

bool Visual::Base::IsUsingCustomShader() const
{
  return !mImpl->mCustomShaders.empty();
}

void Visual::Base::SetProperties(const Property::Map& propertyMap)
{
  bool needUpdateShader = false;
  for(size_t i = 0; i < propertyMap.Count(); ++i)
  {
    const KeyValuePair&    pair  = propertyMap.GetKeyValue(i);
    const Property::Key&   key   = pair.first;
    const Property::Value& value = pair.second;

    switch(GetVisualPropertyIndex(key))
    {
      case Ui::Visual::Property::SHADER:
      {
        if(value.GetType() == Property::MAP)
        {
          const Dali::Property::Map* map = value.GetMap();
          if(map)
          {
            SetCustomShader(*map);
          }
        }
        else if(value.GetType() == Property::ARRAY)
        {
          const Dali::Property::Array* array = value.GetArray();
          if(array)
          {
            SetCustomShader(*array);
          }
        }
        break;
      }

      case Ui::Visual::Property::TRANSFORM:
      {
        Property::Map map;
        if(value.Get(map))
        {
          if(!map.Empty() && (!mImpl->mTransformMapUsingDefault ||
                              map.GetHash() != Transform::GetDefaultTransformMap().GetHash() ||
                              DALI_UNLIKELY(map != Transform::GetDefaultTransformMap())))
          {
            if(DALI_UNLIKELY(mImpl->mRenderer))
            {
              // Unusual case. SetProperty called after OnInitialize().
              // Assume that DoAction call UPDATE_PROPERTY.
              mImpl->mTransformMapChanged = true;
              if(mImpl->mTransformMapUsingDefault)
              {
                mImpl->mTransformMapUsingDefault = false;
              }

              if(!mImpl->mTransformMapUsingDefault)
              {
                mImpl->GetOrCreateTransform().UpdatePropertyMap(map);

                // Set Renderer uniforms, and change logics for subclasses.
                OnSetTransform();
              }

              // Reset flag here.
              mImpl->mTransformMapChanged = false;
            }
            else
            {
              mImpl->mTransformMapUsingDefault = false;
              mImpl->GetOrCreateTransform().SetPropertyMap(map);
            }
          }
        }
        break;
      }

      case Ui::Visual::Property::MIX_COLOR:
      {
        Vector4 mixColor;
        if(value.Get(mixColor))
        {
          if(value.GetType() == Property::VECTOR4)
          {
            SetMixColor(mixColor);
          }
          else
          {
            Vector3 mixColor3(mixColor);
            SetMixColor(mixColor3);
          }
        }
        break;
      }
      case Ui::Visual::Property::OPACITY:
      {
        float opacity;
        if(value.Get(opacity))
        {
          mImpl->mMixColor.a = opacity;
          SetMixColor(mImpl->mMixColor);
        }
        break;
      }
      case Ui::DevelVisual::Property::BORDERLINE_WIDTH:
      {
        float width;
        if(value.Get(width) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(width)))
        {
          mImpl->SetBorderlineWidth(width);
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForBorderline(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH,
                                       mImpl->GetBorderlineWidth());

          // Check whether we must update shader.
          if(!mImpl->mAlwaysUsingBorderline && IsBorderlineRequired())
          {
            // Required to change shader mean, we didn't setup BORDERLINE_COLOR and BORDERLINE_OFFSET into mRenderer
            // before. Set property now.
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR,
                                         mImpl->GetBorderlineColor());
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET,
                                         mImpl->GetBorderlineOffset());

            // Make Blend mode ON_WITHOUT_CULL for transparent mix color.
            mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);

            // Change the shader must not be occured many times. we always have to use borderline feature.
            mImpl->mAlwaysUsingBorderline = true;

            // Change shader
            if(!IsUsingCustomShader())
            {
              needUpdateShader = true;
            }
          }
        }
        break;
      }
      case Ui::DevelVisual::Property::BORDERLINE_COLOR:
      {
        Vector4 color;
        if(value.Get(color) && (mImpl->mDecorationData != nullptr || color != Vector4::ZERO))
        {
          mImpl->SetBorderlineColor(color);
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForBorderline(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR,
                                       mImpl->GetBorderlineColor());
        }
        break;
      }
      case Ui::DevelVisual::Property::BORDERLINE_OFFSET:
      {
        float offset;
        if(value.Get(offset) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(offset)))
        {
          mImpl->SetBorderlineOffset(offset);
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForBorderline(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET,
                                       mImpl->GetBorderlineOffset());
        }
        break;
      }
      case Ui::DevelVisual::Property::CORNER_RADIUS:
      {
        if(value.GetType() == Property::VECTOR4)
        {
          // If CORNER_RADIUS Property is Vector4,
          // Each values mean the radius of
          // (top-left, top-right, bottom-right, bottom-left)
          Vector4 radius;
          if(value.Get(radius) && (mImpl->mDecorationData != nullptr || radius != Vector4::ZERO))
          {
            mImpl->SetCornerRadius(radius);
          }
        }
        else
        {
          // If CORNER_RADIUS Property is float,
          // Every corner radius have same value
          float radius;
          if(value.Get(radius) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(radius)))
          {
            mImpl->SetCornerRadius(Vector4(radius, radius, radius, radius));
          }
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForCornerRadius(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());

          // Check whether we must update shader.
          if(!mImpl->mAlwaysUsingCornerRadius && IsRoundedCornerRequired())
          {
            // Required to change shader mean, we didn't setup CORNER_RADIUS_POLICY into mRenderer before. Set property
            // now.
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY,
                                         static_cast<float>(mImpl->GetCornerRadiusPolicy()));

            // Change the shader must not be occured many times. we always have to use corner radius feature.
            mImpl->mAlwaysUsingCornerRadius = true;

            if(!IsBorderlineRequired())
            {
              // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't
              // overwrite it.
              mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
            }

            // Change shader
            if(!IsUsingCustomShader())
            {
              needUpdateShader = true;
            }
          }
        }

        break;
      }
      case Ui::DevelVisual::Property::CORNER_RADIUS_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          switch(policy)
          {
            case Ui::Visual::Transform::Policy::RELATIVE:
            case Ui::Visual::Transform::Policy::ABSOLUTE:
            {
              mImpl->SetCornerRadiusPolicy(policy);
              if(DALI_UNLIKELY(mImpl->mRenderer))
              {
                // Unusual case. SetProperty called after OnInitialize().
                // Assume that DoAction call UPDATE_PROPERTY.
                mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY,
                                             static_cast<float>(mImpl->GetCornerRadiusPolicy()));
              }
              break;
            }
            default:
            {
              DALI_LOG_ERROR("Unsupported policy: %d\n", policy);
              break;
            }
          }
        }
        break;
      }
      case Ui::DevelVisual::Property::CORNER_SQUARENESS:
      {
        if(value.GetType() == Property::VECTOR4)
        {
          // If CORNER_SQUARENESS Property is Vector4,
          // Each values mean the squareness of
          // (top-left, top-right, bottom-right, bottom-left)
          Vector4 squareness;
          if(value.Get(squareness) && (mImpl->mDecorationData != nullptr || squareness != Vector4::ZERO))
          {
            mImpl->SetCornerSquareness(squareness);
          }
        }
        else
        {
          // If CORNER_RADIUS Property is float,
          // Every corner radius have same value
          float squareness;
          if(value.Get(squareness) && (mImpl->mDecorationData != nullptr || !Dali::EqualsZero(squareness)))
          {
            mImpl->SetCornerSquareness(Vector4(squareness, squareness, squareness, squareness));
          }
        }

        if(DALI_UNLIKELY(mImpl->mRenderer && IsTypeAvailableForCornerRadius(mImpl->mType)))
        {
          // Unusual case. SetProperty called after OnInitialize().
          // Assume that DoAction call UPDATE_PROPERTY.
          mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS,
                                       mImpl->GetCornerSquareness());

          // Check whether we must update shader.
          if(!mImpl->mAlwaysUsingCornerSquareness && IsSquircleCornerRequired())
          {
            // Required to change shader mean, we didn't setup CORNER_RADIUS_POLICY into mRenderer before. Set property
            // now.
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS,
                                         static_cast<Vector4>(mImpl->GetCornerRadius()));
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY,
                                         static_cast<float>(mImpl->GetCornerRadiusPolicy()));

            // Change the shader must not be occured many times. we always have to use corner squreness feature.
            mImpl->mAlwaysUsingCornerSquareness = true;

            if(!IsBorderlineRequired())
            {
              // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't
              // overwrite it.
              mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
            }

            // Change shader
            if(!IsUsingCustomShader())
            {
              needUpdateShader = true;
            }
          }
        }

        break;
      }
    }
  }

  DoSetProperties(propertyMap);

  if(DALI_UNLIKELY(needUpdateShader))
  {
    UpdateShader();
  }
}

void Visual::Base::SetTransformAndSize(const Property::Map& transform, Size controlSize)
{
  mImpl->mControlSize = controlSize;
  mImpl->mTransformMapChanged |= !transform.Empty();
  if(mImpl->mTransformMapChanged && mImpl->mTransformMapUsingDefault)
  {
    mImpl->mTransformMapUsingDefault = false;
  }
  if(!mImpl->mTransformMapUsingDefault)
  {
    mImpl->GetOrCreateTransform().UpdatePropertyMap(transform);
  }

#if defined(DEBUG_ENABLED)
  std::ostringstream oss;
  oss << transform;
  DALI_LOG_INFO(gVisualBaseLogFilter, Debug::General,
                "Visual::Base::SetTransformAndSize(%s) - [\e[1;32mtransform: %s  controlSize: (%3.1f, %3.1f)]\e[0m\n",
                GetName().c_str(), oss.str().c_str(), controlSize.x, controlSize.y);
#endif

  OnSetTransform();

  // Reset flag here.
  mImpl->mTransformMapChanged = false;
}

void Visual::Base::SetName(const std::string& name)
{
  mImpl->mName = name;
}

const std::string& Visual::Base::GetName() const
{
  return mImpl->mName;
}

float Visual::Base::GetHeightForWidth(float width)
{
  float   aspectCorrectedHeight = 0.f;
  Vector2 naturalSize;
  GetNaturalSize(naturalSize);
  if(naturalSize.width > 0.0f)
  {
    aspectCorrectedHeight = naturalSize.height * width / naturalSize.width;
  }
  return aspectCorrectedHeight;
}

float Visual::Base::GetWidthForHeight(float height)
{
  float   aspectCorrectedWidth = 0.f;
  Vector2 naturalSize;
  GetNaturalSize(naturalSize);
  if(naturalSize.height > 0.0f)
  {
    aspectCorrectedWidth = naturalSize.width * height / naturalSize.height;
  }
  return aspectCorrectedWidth;
}

void Visual::Base::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize = Vector2::ZERO;
}

void Visual::Base::DoAction(const Property::Index actionId, const Property::Value& attributes)
{
  OnDoAction(actionId, attributes);

  // Check if action is valid for this visual type and perform action if possible
  switch(actionId)
  {
    case DevelVisual::Action::UPDATE_PROPERTY:
    {
      const Property::Map* map = attributes.GetMap();
      if(map)
      {
        SetProperties(*map);
      }
      break;
    }
  }
}

void Visual::Base::DoActionExtension(const Dali::Property::Index actionId, const Dali::Any& attributes)
{
  OnDoActionExtension(actionId, attributes);
}

void Visual::Base::SetDepthIndex(int index)
{
  // Clamp input value as valid range.
  Dali::ClampInPlace(index, static_cast<int>(Ui::DepthIndex::MINIMUM_DEPTH_INDEX),
                     static_cast<int>(Ui::DepthIndex::MAXIMUM_DEPTH_INDEX));

  if(mImpl->mDepthIndex != index)
  {
    mImpl->mDepthIndex = index;
    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex);
    }
  }
}

int Visual::Base::GetDepthIndex() const
{
  return mImpl->mDepthIndex;
}

void Visual::Base::SetOnScene(Actor& actor)
{
  if(!IsOnScene())
  {
    // To display the actor correctly, renderer should not be added to actor until all required resources are ready.
    // Thus the calling of actor.AddRenderer() should happen inside derived class as base class does not know the exact
    // timing.
    DoSetOnScene(actor);

    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
      if(mImpl->mDepthIndex == static_cast<int>(DepthIndex::AUTO_INDEX))
      {
        mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, static_cast<int>(DepthIndex::CONTENT));
      }
      else
      {
        mImpl->mRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, mImpl->mDepthIndex);
      }
    }

    mImpl->mFlags |= Impl::IS_ON_SCENE;

    mImpl->mConstraintFeatureList.UpdateAllApplyRate();
  }
}

void Visual::Base::SetOffScene(Actor& actor)
{
  if(IsOnScene())
  {
    DoSetOffScene(actor);
    mImpl->mFlags &= ~Impl::IS_ON_SCENE;
    mImpl->mConstraintFeatureList.SceneDisconnected();
  }
}

void Visual::Base::CreatePropertyMap(Property::Map& map) const
{
  if(mImpl->mRenderer)
  {
    // Update values from Renderer
    mImpl->mMixColor = mImpl->mRenderer.GetProperty<Vector4>(Renderer::Property::MIX_COLOR);

    const auto& rendererOffset = mImpl->mRenderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET);
    const auto& rendererSize   = mImpl->mRenderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);

    if(rendererSize != Vector2::ZERO || rendererSize != Vector2::ONE)
    {
      mImpl->GetOrCreateTransform().mOffset = rendererOffset;
      mImpl->GetOrCreateTransform().mSize   = rendererSize;
    }

    if(IsRoundedCornerRequired())
    {
      mImpl->SetCornerRadius(mImpl->mRenderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_RADIUS));
    }
    if(IsSquircleCornerRequired())
    {
      mImpl->SetCornerSquareness(
        mImpl->mRenderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::CORNER_SQUARENESS));
    }
    if(IsBorderlineRequired())
    {
      mImpl->SetBorderlineWidth(
        mImpl->mRenderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH));
      mImpl->SetBorderlineColor(
        mImpl->mRenderer.GetProperty<Vector4>(DecoratedVisualRenderer::Property::BORDERLINE_COLOR));
      mImpl->SetBorderlineOffset(
        mImpl->mRenderer.GetProperty<float>(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET));
    }
  }

  DoCreatePropertyMap(map);

  if(IsUsingCustomShader())
  {
    mImpl->CreateCustomShaderPropertyMap(map);
  }

  Property::Map transform;
  if(mImpl->mTransform)
  {
    mImpl->mTransform->GetPropertyMap(transform);
  }
  else
  {
    transform = Transform::GetDefaultTransformMap();
  }
  map.Insert(Ui::Visual::Property::TRANSFORM, transform);

  // Note, Color and Primitive will also insert their own mix color into the map
  // which is ok, because they have a different key value range, but uses same cached value anyway.
  map.Insert(Ui::Visual::Property::MIX_COLOR, mImpl->mMixColor); // vec4
  map.Insert(Ui::Visual::Property::OPACITY, mImpl->mMixColor.a);

  if(IsTypeAvailableForBorderline(mImpl->mType))
  {
    map.Insert(Ui::DevelVisual::Property::BORDERLINE_WIDTH, mImpl->GetBorderlineWidth());
    map.Insert(Ui::DevelVisual::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
    map.Insert(Ui::DevelVisual::Property::BORDERLINE_OFFSET, mImpl->GetBorderlineOffset());
  }

  if(IsTypeAvailableForCornerRadius(mImpl->mType))
  {
    map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
    map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY, mImpl->GetCornerRadiusPolicy());
    map.Insert(Ui::DevelVisual::Property::CORNER_SQUARENESS, mImpl->GetCornerSquareness());
  }
}

void Visual::Base::CreateInstancePropertyMap(Property::Map& map) const
{
  DoCreateInstancePropertyMap(map);

  if(IsUsingCustomShader())
  {
    mImpl->CreateCustomShaderPropertyMap(map);
  }
}

bool Visual::Base::IsPreMultipliedAlphaEnabled() const
{
  return mImpl->mFlags & Impl::IS_PRE_MULTIPLIED_ALPHA;
}

void Visual::Base::DoSetOffScene(Actor& actor)
{
  actor.RemoveRenderer(mImpl->mRenderer);
}

bool Visual::Base::IsOnScene() const
{
  return mImpl->mFlags & Impl::IS_ON_SCENE;
}

bool Visual::Base::IsRoundedCornerRequired() const
{
  // If VisualType doesn't support rounded corner, always return false.
  if(IsTypeAvailableForCornerRadius(mImpl->mType))
  {
    if(mImpl->mRenderer)
    {
      // Update values from Renderer
      Property::Value value = mImpl->mRenderer.GetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS);

      Vector4 retValue = Vector4::ZERO;
      if(value.Get(retValue))
      {
        if(mImpl->mDecorationData != nullptr || retValue != Vector4::ZERO)
        {
          mImpl->SetCornerRadius(retValue);
        }
      }
    }
    return mImpl->mAlwaysUsingCornerRadius || !(mImpl->GetCornerRadius() == Vector4::ZERO);
  }
  return false;
}

bool Visual::Base::IsSquircleCornerRequired() const
{
  // If VisualType doesn't support rounded corner, always return false.
  if(IsTypeAvailableForCornerRadius(mImpl->mType))
  {
    if(mImpl->mRenderer)
    {
      // Update values from Renderer
      Property::Value value = mImpl->mRenderer.GetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS);

      Vector4 retValue = Vector4::ZERO;
      if(value.Get(retValue))
      {
        if(mImpl->mDecorationData != nullptr || retValue != Vector4::ZERO)
        {
          mImpl->SetCornerSquareness(retValue);
        }
      }
    }
    return IsRoundedCornerRequired() &&
           (mImpl->mAlwaysUsingCornerSquareness || !(mImpl->GetCornerSquareness() == Vector4::ZERO));
  }
  return false;
}

bool Visual::Base::IsBorderlineRequired() const
{
  // If VisualType doesn't support borderline, always return false.
  if(IsTypeAvailableForBorderline(mImpl->mType))
  {
    if(mImpl->mRenderer)
    {
      // Update values from Renderer
      Property::Value value = mImpl->mRenderer.GetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH);

      float retValue = 0.0f;
      if(value.Get(retValue))
      {
        if(mImpl->mDecorationData != nullptr || !Dali::EqualsZero(retValue))
        {
          mImpl->SetBorderlineWidth(retValue);
        }
      }
    }
    return mImpl->mAlwaysUsingBorderline || !EqualsZero(mImpl->GetBorderlineWidth());
  }
  return false;
}

void Visual::Base::OnDoAction(const Property::Index actionId, const Property::Value& attributes)
{
  // May be overridden by derived class
}

void Visual::Base::OnDoActionExtension(const Property::Index actionId, const Dali::Any& attributes)
{
  // May be overridden by derived class
}

void Visual::Base::RegisterMixColor()
{
  if(mImpl->mRenderer)
  {
    // All visual renderers now use same mix color / opacity properties.
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR, mImpl->mMixColor);

    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());
  }
}

void Visual::Base::RegisterDecoration()
{
  if(mImpl->mRenderer)
  {
    if(IsTypeAvailableForCornerRadius(mImpl->mType))
    {
      if(mImpl->mAlwaysUsingCornerRadius || !(mImpl->GetCornerRadius() == Vector4::ZERO))
      {
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY,
                                     static_cast<float>(mImpl->GetCornerRadiusPolicy()));
      }
      if(mImpl->mAlwaysUsingCornerSquareness || !(mImpl->GetCornerSquareness() == Vector4::ZERO))
      {
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS,
                                     mImpl->GetCornerSquareness());
      }
    }
    if(IsTypeAvailableForBorderline(mImpl->mType))
    {
      if(mImpl->mAlwaysUsingBorderline || !EqualsZero(mImpl->GetBorderlineWidth()))
      {
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH, mImpl->GetBorderlineWidth());
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR, mImpl->GetBorderlineColor());
        mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET,
                                     mImpl->GetBorderlineOffset());
      }
    }
  }
}

void Visual::Base::SetMixColor(const Vector4& color)
{
  mImpl->mMixColor = color;

  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR, color);
  }
}

void Visual::Base::SetMixColor(const Vector3& color)
{
  mImpl->mMixColor.r = color.r;
  mImpl->mMixColor.g = color.g;
  mImpl->mMixColor.b = color.b;

  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR_RED, color.r);
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR_GREEN, color.g);
    mImpl->mRenderer.SetProperty(Renderer::Property::MIX_COLOR_BLUE, color.b);
  }
}

void Visual::Base::AddEventObserver(Visual::EventObserver& observer)
{
  mImpl->mEventObserver = &observer;
}

void Visual::Base::RemoveEventObserver(Visual::EventObserver& observer)
{
  mImpl->mEventObserver = NULL;
}

void Visual::Base::ResourceReady(Ui::Visual::ResourceStatus resourceStatus)
{
  if(mImpl->mResourceStatus != resourceStatus)
  {
    mImpl->mResourceStatus = resourceStatus;

    if(mImpl->mEventObserver)
    {
      // observer is currently a control impl
      mImpl->mEventObserver->ResourceReady(*this);
    }
  }
}

bool Visual::Base::IsResourceReady() const
{
  return (mImpl->mResourceStatus == Ui::Visual::ResourceStatus::READY ||
          mImpl->mResourceStatus == Ui::Visual::ResourceStatus::FAILED);
}

bool Visual::Base::IsSynchronousLoadingRequired() const
{
  return (mImpl->mFlags & Impl::IS_SYNCHRONOUS_RESOURCE_LOADING);
}

Ui::Visual::Type Visual::Base::GetType() const
{
  return mImpl->mType;
}

Ui::Visual::ResourceStatus Visual::Base::GetResourceStatus() const
{
  return mImpl->mResourceStatus;
}

bool Visual::Base::IsFittingModeRequired() const
{
  return mImpl->mFittingModeRequired;
}

bool Visual::Base::IsPixelAreaSetForFittingMode() const
{
  return mImpl->mPixelAreaSetByFittingMode;
}

void Visual::Base::SetPixelAreaForFittingMode(const Vector4& pixelArea)
{
  if(mImpl->mRenderer)
  {
    mImpl->mRenderer.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, pixelArea);
  }

  mImpl->mPixelAreaSetByFittingMode = (pixelArea != FULL_TEXTURE_RECT) ? true : false;
}

bool Visual::Base::IsTransformMapSetForFittingMode() const
{
  return mImpl->mTransformMapSetForFittingMode;
}

void Visual::Base::SetTransformMapUsageForFittingMode(bool used)
{
  mImpl->mTransformMapSetForFittingMode = used;
}

void Visual::Base::ApplyFittingMode(const Vector2& controlSize, const Extents& padding)
{
  if(GetType() != Ui::Visual::Type::TEXT)
  {
    SetViewSize(controlSize);
  }
}

void Visual::Base::DoApplyFittingMode(const Vector2& controlSize, const Extents& viewPadding, Ui::Image::FittingMode fittingMode)
{
  if(IsPixelAreaSetForFittingMode())
  {
    SetPixelAreaForFittingMode(FULL_TEXTURE_RECT);
  }

  Extents padding = (mImpl->mFlags & Impl::IS_FITTING_MODE_IGNORE_VIEW_PADDING) ? Extents() : viewPadding;

  Vector2 finalSize   = controlSize - Vector2(padding.start + padding.end, padding.top + padding.bottom);
  Vector2 finalOffset = Vector2(padding.start, padding.top);
  bool    zeroPadding = (padding == Extents());

  Property::Map transformMap;

  if((!zeroPadding) || (fittingMode != Ui::Image::FittingMode::FILL))
  {
    SetTransformMapUsageForFittingMode(true);

    Vector2 naturalSize;
    if(fittingMode != Ui::Image::FittingMode::FILL)
    {
      GetNaturalSize(naturalSize);
    }

    switch(fittingMode)
    {
      case Ui::Image::FittingMode::FIT_KEEP_ASPECT_RATIO:
      {
        auto availableVisualSize = finalSize;
        finalSize                = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                          (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        finalOffset += (availableVisualSize - finalSize) * 0.5f;
        transformMap.Add(Ui::Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Ui::Visual::Transform::Property::SIZE, finalSize);
        break;
      }
      case Ui::Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO:
      {
        auto availableVisualSize = finalSize;
        finalSize                = naturalSize * std::max((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                          (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        auto originalOffset      = finalOffset;
        if(!IsPixelAreaSetForFittingMode() && !Dali::EqualsZero(finalSize.width) && !Dali::EqualsZero(finalSize.height))
        {
          float   x           = std::abs((availableVisualSize.width - finalSize.width) / finalSize.width) * 0.5f;
          float   y           = std::abs((availableVisualSize.height - finalSize.height) / finalSize.height) * 0.5f;
          float   widthRatio  = 1.f - std::abs((availableVisualSize.width - finalSize.width) / finalSize.width);
          float   heightRatio = 1.f - std::abs((availableVisualSize.height - finalSize.height) / finalSize.height);
          Vector4 pixelArea(x, y, widthRatio, heightRatio);
          SetPixelAreaForFittingMode(pixelArea);
        }
        transformMap.Add(Ui::Visual::Transform::Property::OFFSET, originalOffset)
          .Add(Ui::Visual::Transform::Property::SIZE, availableVisualSize);
        break;
      }
      case Ui::Image::FittingMode::CENTER:
      {
        auto availableVisualSize = finalSize;
        if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
        {
          finalSize = naturalSize;
        }
        else
        {
          finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                             (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        }
        finalOffset += (availableVisualSize - finalSize) * 0.5f;
        transformMap.Add(Ui::Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Ui::Visual::Transform::Property::SIZE, finalSize);
        break;
      }
      case Ui::Image::FittingMode::FILL:
      default:
      {
        transformMap.Add(Ui::Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Ui::Visual::Transform::Property::SIZE, finalSize);
        break;
      }
    }

    transformMap
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::ORIGIN, Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::PIVOT, Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE));
  }
  else if(IsTransformMapSetForFittingMode() && zeroPadding)
  {
    SetTransformMapUsageForFittingMode(false);
    transformMap.Add(Ui::Visual::Transform::Property::OFFSET, Vector2::ZERO)
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::RELATIVE, Ui::Visual::Transform::Policy::RELATIVE))
      .Add(Ui::Visual::Transform::Property::SIZE, Vector2::ONE)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::RELATIVE, Ui::Visual::Transform::Policy::RELATIVE));
  }

  SetTransformAndSize(transformMap, controlSize);
}

void Visual::Base::SetControlSize(Size controlSize)
{
}

void Visual::Base::SetViewSize(Size viewSize)
{
  // Notify to visual's constraint that view's size is changed.
  UpdateApplyRate(Actor::Property::SIZE);

  const static Property::Map emptyMap;
  SetTransformAndSize(emptyMap, viewSize);
}

Visual::Base& Visual::Base::GetVisualObject()
{
  return *this;
}

VisualRenderer Visual::Base::GetRenderer() const
{
  return mImpl->mRenderer;
}

Property::Index Visual::Base::GetIntKey(Property::Key key)
{
  if(key.type == Property::Key::INDEX)
  {
    return key.indexKey;
  }

  Property::Index index = Internal::Visual::Transform::GetIntKey(key);

  if(index != Property::INVALID_INDEX)
  {
    return index;
  }

  if(key.stringKey == MIX_COLOR)
  {
    return Ui::Visual::Property::MIX_COLOR;
  }
  else if(key.stringKey == OPACITY)
  {
    return Ui::Visual::Property::OPACITY;
  }
  else if(key.stringKey == CUSTOM_SHADER)
  {
    return Ui::Visual::Property::SHADER;
  }
  else if(key.stringKey == TRANSFORM)
  {
    return Ui::Visual::Property::TRANSFORM;
  }
  else if(key.stringKey == CORNER_RADIUS)
  {
    return Ui::DevelVisual::Property::CORNER_RADIUS;
  }
  else if(key.stringKey == CORNER_RADIUS_POLICY)
  {
    return Ui::DevelVisual::Property::CORNER_RADIUS_POLICY;
  }
  else if(key.stringKey == CORNER_SQUARENESS)
  {
    return Ui::DevelVisual::Property::CORNER_SQUARENESS;
  }
  else if(key.stringKey == BORDERLINE_WIDTH)
  {
    return Ui::DevelVisual::Property::BORDERLINE_WIDTH;
  }
  else if(key.stringKey == BORDERLINE_COLOR)
  {
    return Ui::DevelVisual::Property::BORDERLINE_COLOR;
  }
  else if(key.stringKey == BORDERLINE_OFFSET)
  {
    return Ui::DevelVisual::Property::BORDERLINE_OFFSET;
  }

  return Property::INVALID_INDEX;
}

Property::Index Visual::Base::GetPropertyIndex(Property::Key key) const
{
  switch(GetIntKey(key))
  {
    case Dali::Ui::Visual::Transform::Property::OFFSET:
    {
      return VisualRenderer::Property::TRANSFORM_OFFSET;
    }
    case Dali::Ui::Visual::Transform::Property::SIZE:
    {
      return VisualRenderer::Property::TRANSFORM_SIZE;
    }
    case Dali::Ui::Visual::Transform::Property::ORIGIN:
    {
      return VisualRenderer::Property::TRANSFORM_ORIGIN;
    }
    case Dali::Ui::Visual::Transform::Property::PIVOT:
    {
      return VisualRenderer::Property::TRANSFORM_PIVOT;
    }
    case Dali::Ui::Visual::Property::MIX_COLOR:
    {
      return Renderer::Property::MIX_COLOR;
    }
    case Dali::Ui::Visual::Property::OPACITY:
    {
      return Renderer::Property::OPACITY;
    }
    case Dali::Ui::DevelVisual::Property::CORNER_RADIUS:
    {
      if(IsTypeAvailableForCornerRadius(mImpl->mType))
      {
        return DecoratedVisualRenderer::Property::CORNER_RADIUS;
      }
      break;
    }
    case Dali::Ui::DevelVisual::Property::CORNER_SQUARENESS:
    {
      if(IsTypeAvailableForCornerRadius(mImpl->mType))
      {
        return DecoratedVisualRenderer::Property::CORNER_SQUARENESS;
      }
      break;
    }
    case Dali::Ui::DevelVisual::Property::BORDERLINE_WIDTH:
    {
      if(IsTypeAvailableForBorderline(mImpl->mType))
      {
        return DecoratedVisualRenderer::Property::BORDERLINE_WIDTH;
      }
      break;
    }
    case Dali::Ui::DevelVisual::Property::BORDERLINE_COLOR:
    {
      if(IsTypeAvailableForBorderline(mImpl->mType))
      {
        return DecoratedVisualRenderer::Property::BORDERLINE_COLOR;
      }
      break;
    }
    case Dali::Ui::DevelVisual::Property::BORDERLINE_OFFSET:
    {
      if(IsTypeAvailableForBorderline(mImpl->mType))
      {
        return DecoratedVisualRenderer::Property::BORDERLINE_OFFSET;
      }
      break;
    }
  }

  // Fast-out for invalid key.
  if((key.type == Property::Key::INDEX && key.indexKey == Property::INVALID_KEY) ||
     (key.type == Property::Key::STRING && key.stringKey.Empty()))
  {
    return Property::INVALID_INDEX;
  }

  Property::Index index = mImpl->mRenderer.GetPropertyIndex(key);

  if(index == Property::INVALID_INDEX)
  {
    // Is it a shader property?
    Shader shader = mImpl->mRenderer.GetShader();
    index         = shader.GetPropertyIndex(key);
    if(index != Property::INVALID_INDEX)
    {
      // Yes - we should register it in the Renderer so it can be set / animated
      // independently, as shaders are shared across multiple renderers.
      Dali::String    keyName;
      Property::Index keyIndex(Property::INVALID_KEY);
      if(key.type == Property::Key::INDEX)
      {
        keyName  = shader.GetPropertyName(index);
        keyIndex = key.indexKey;
      }
      else
      {
        keyName = key.stringKey;
        // Leave keyIndex as INVALID_KEY - it can still be registered against the string key.
      }
      Property::Value value = shader.GetProperty(index);

      // We already know that mRenderer didn't have property. So we can assume that it is unique.
      index = mImpl->mRenderer.RegisterUniqueProperty(keyIndex, keyName, value);
    }
  }
  return index;
}

Dali::Property Visual::Base::GetPropertyObject(Dali::Property::Key key, bool changeProperties)
{
  if(!mImpl->mRenderer)
  {
    Handle handle;
    return Dali::Property(handle, Property::INVALID_INDEX);
  }

  switch(GetIntKey(key))
  {
    // Default animatable properties from VisualRenderer
    case Ui::Visual::Property::MIX_COLOR:
    {
      return Dali::Property(mImpl->mRenderer, Renderer::Property::MIX_COLOR);
    }
    case Ui::Visual::Property::OPACITY:
    {
      return Dali::Property(mImpl->mRenderer, Renderer::Property::OPACITY);
    }
    case Ui::Visual::Transform::Property::OFFSET:
    {
      // Need to change visual transform is not default anymore.
      if(changeProperties && mImpl->mTransformMapUsingDefault)
      {
        mImpl->mTransformMapUsingDefault = false;
      }

      return Dali::Property(mImpl->mRenderer, VisualRenderer::Property::TRANSFORM_OFFSET);
    }
    case Ui::Visual::Transform::Property::SIZE:
    {
      // Need to change visual transform is not default anymore.
      if(changeProperties && mImpl->mTransformMapUsingDefault)
      {
        mImpl->mTransformMapUsingDefault = false;
      }

      return Dali::Property(mImpl->mRenderer, VisualRenderer::Property::TRANSFORM_SIZE);
    }

    // Default animatable properties from DecoratedVisualRenderer
    case Ui::DevelVisual::Property::CORNER_RADIUS:
    {
      if(IsTypeAvailableForCornerRadius(mImpl->mType))
      {
        if(changeProperties)
        {
          const bool updateShader = !IsUsingCustomShader() && !IsRoundedCornerRequired();

          // CornerRadius is animated now. we always have to use corner radius feature.
          mImpl->mAlwaysUsingCornerRadius = true;

          if(updateShader)
          {
            // Update each values to renderer
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY,
                                         static_cast<float>(mImpl->GetCornerRadiusPolicy()));

            if(IsSquircleCornerRequired())
            {
              // Change the shader must not be occured many times. we always have to use corner squreness feature.
              mImpl->mAlwaysUsingCornerSquareness = true;

              mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS,
                                           mImpl->GetCornerSquareness());
            }

            // Change shader
            UpdateShader();
          }
          if(!IsBorderlineRequired())
          {
            // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite
            // it.
            mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
          }
        }
        return Dali::Property(mImpl->mRenderer, DecoratedVisualRenderer::Property::CORNER_RADIUS);
      }
      break;
    }
    case Ui::DevelVisual::Property::CORNER_SQUARENESS:
    {
      if(IsTypeAvailableForCornerRadius(mImpl->mType))
      {
        if(changeProperties)
        {
          const bool updateShader = !IsUsingCustomShader() && !IsSquircleCornerRequired();

          // CornerSquareness is animated now. we always have to use corner squareness feature.
          mImpl->mAlwaysUsingCornerSquareness = true;

          if(updateShader)
          {
            // Update each values to renderer
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS, mImpl->GetCornerRadius());
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_RADIUS_POLICY,
                                         static_cast<float>(mImpl->GetCornerRadiusPolicy()));
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::CORNER_SQUARENESS,
                                         mImpl->GetCornerSquareness());

            // Change shader
            UpdateShader();
          }
          if(!IsBorderlineRequired())
          {
            // If IsBorderlineRequired is true, BLEND_MODE is already BlendMode::ON_WITHOUT_CULL. So we don't overwrite
            // it.
            mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
          }
        }
        return Dali::Property(mImpl->mRenderer, DecoratedVisualRenderer::Property::CORNER_SQUARENESS);
      }
      break;
    }
    case Ui::DevelVisual::Property::BORDERLINE_WIDTH:
    case Ui::DevelVisual::Property::BORDERLINE_COLOR:
    case Ui::DevelVisual::Property::BORDERLINE_OFFSET:
    {
      if(IsTypeAvailableForBorderline(mImpl->mType))
      {
        if(changeProperties)
        {
          const bool updateShader = !IsUsingCustomShader() && !IsBorderlineRequired();

          // Borderline is animated now. we always have to use borderline feature.
          mImpl->mAlwaysUsingBorderline = true;

          if(updateShader)
          {
            // Update each values to renderer
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_WIDTH,
                                         mImpl->GetBorderlineWidth());
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_COLOR,
                                         mImpl->GetBorderlineColor());
            mImpl->mRenderer.SetProperty(DecoratedVisualRenderer::Property::BORDERLINE_OFFSET,
                                         mImpl->GetBorderlineOffset());

            // Change shader
            UpdateShader();
          }
          mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON_WITHOUT_CULL);
        }

        return Dali::Property(mImpl->mRenderer, GetPropertyIndex(key));
      }
      break;
    }
    default:
    {
      // Special case for MIX_COLOR
      if(key.type == Property::Key::INDEX &&
         ((mImpl->mType == Ui::Visual::PRIMITIVE && key.indexKey == PrimitiveVisual::Property::MIX_COLOR)))
      {
        return Dali::Property(mImpl->mRenderer, Renderer::Property::MIX_COLOR);
      }

      // Special case for PRE_MULTIPLIED_ALPHA (It is not animatable, but keep it just for logical flow)
      if((mImpl->mType == Ui::Visual::IMAGE || mImpl->mType == Ui::Visual::ANIMATED_IMAGE || mImpl->mType == Ui::Visual::N_PATCH) &&
         ((key.type == Property::Key::INDEX && key.indexKey == ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA) ||
          (key.type == Property::Key::STRING && key.stringKey == PRE_MULTIPLIED_ALPHA)))
      {
        return Dali::Property(mImpl->mRenderer, Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA);
      }

      // Special case for BLUR_RADIUS
      if(mImpl->mType == Ui::Visual::COLOR &&
         ((key.type == Property::Key::INDEX && key.indexKey == ColorVisualPropertyIndex::BLUR_RADIUS) ||
          (key.type == Property::Key::STRING && key.stringKey == BLUR_RADIUS_NAME)))
      {
        // Request to color-visual class
        return OnGetPropertyObject(key, changeProperties);
      }
    }
  }

  // If it is not VisualRenderer property, check registered Renderer and Shader property.
  Property::Index index = GetPropertyIndex(key);
  if(index != Property::INVALID_INDEX)
  {
    return Dali::Property(mImpl->mRenderer, index);
  }

  // We can't find the property in the base class.
  // Request to child class
  return OnGetPropertyObject(key, changeProperties);
}

bool Visual::Base::IsOffscreenRenderingCaptureEnabled() const
{
  const int depthIndex = mImpl->mDepthIndex;
  return depthIndex == DepthIndex::AUTO_INDEX ||
         (DepthIndex::BACKGROUND_EFFECT < depthIndex && depthIndex <= DepthIndex::DECORATION);
}

bool Visual::Base::IsCornerRadiusIgnoredAtOffscreenRendering() const
{
  return !IsTypeAvailableForCornerRadius(mImpl->mType) ||
         (IsOffscreenRenderingCaptureEnabled() && mImpl->mOffscreenRenderingIgnoreCornerRadius);
}

void Visual::Base::CornerRadiusIgnoredAtOffscreenRendering(bool ignored)
{
  mImpl->mOffscreenRenderingIgnoreCornerRadius = ignored;
}

void Visual::Base::AddConstraintObserver(Visual::ConstraintObserver& observer)
{
  mImpl->mConstraintFeatureList.SetObserver(observer);
}

void Visual::Base::RemoveConstraintObserver(Visual::ConstraintObserver& observer)
{
  mImpl->mConstraintFeatureList.ResetObserver(observer);
}

void Visual::Base::AddConstraintFeature(Dali::Constraint constraint, std::unordered_set<Property::Index> properties)
{
  mImpl->mConstraintFeatureList.AddFeature(std::move(constraint), std::move(properties));
}

void Visual::Base::RemoveConstraintFeature(Dali::Constraint constraint)
{
  mImpl->mConstraintFeatureList.RemoveFeature(std::move(constraint));
}

void Visual::Base::RemoveConstraintFeatureByIndex(Property::Index index)
{
  mImpl->mConstraintFeatureList.RemoveFeature(index);
}

void Visual::Base::UpdateApplyRate(Property::Index updatedProperty)
{
  if(IsOnScene())
  {
    mImpl->mConstraintFeatureList.UpdateApplyRateByIndex(updatedProperty);
  }
}

void Visual::Base::StartConstraintFeature(Property::Index index)
{
  mImpl->mConstraintFeatureList.StartFeature(index);
}

void Visual::Base::StopConstraintFeature(Property::Index index)
{
  mImpl->mConstraintFeatureList.StopFeature(index);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
