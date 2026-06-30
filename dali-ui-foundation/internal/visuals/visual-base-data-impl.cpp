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
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/devel-api/object/property-map-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/helpers/property-helper.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(SHADER_HINT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Shader::Hint, NONE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Shader::Hint, OUTPUT_IS_TRANSPARENT)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Shader::Hint, MODIFIES_GEOMETRY)
DALI_ENUM_TO_STRING_TABLE_END(SHADER_HINT)

Dali::Vector2 PointToVector2(Ui::Align::Type point, Ui::Integration::Direction::Type direction)
{
  // clang-format off
  static const float pointToVector2[] = {0.0f,0.0f,
                                         0.5f,0.0f,
                                         1.0f,0.0f,
                                         0.0f,0.5f,
                                         0.5f,0.5f,
                                         1.0f,0.5f,
                                         0.0f,1.0f,
                                         0.5f,1.0f,
                                         1.0f,1.0f};

  // clang-format on

  Vector2 result(&pointToVector2[point * 2]);
  if(direction == Dali::Ui::Integration::Direction::RIGHT_TO_LEFT)
  {
    result.x = 1.0f - result.x;
  }

  return result;
}

} // unnamed namespace

Internal::Visual::Base::Impl::Impl(Ui::Integration::InternalVisualType type)
: mEventObserver(nullptr),
  mConstraintFeatureList{},
  mTransform(nullptr),
  mMixColor(Color::WHITE),
  mControlSize(Vector2::ZERO),
  mDecorationData(nullptr),
  mDepthIndex(Ui::Integration::DepthIndex::AUTO_INDEX),
  mFlags(0),
  mViewEffectiveScale(1.0f),
  mResourceStatus(Ui::Visual::ResourceStatus::PREPARING),
  mType(type),
  mAlwaysUsingBorderline(false),
  mAlwaysUsingCornerRadius(false),
  mAlwaysUsingCornerSquareness(false),
  mFittingModeRequired(false),
  mPixelAreaSetByFittingMode(false),
  mTransformMapSetForFittingMode(false),
  mTransformMapUsingDefault(true),
  mTransformMapChanged(false),
  mOffscreenRenderingIgnoreCornerRadius(false)
{
}

Internal::Visual::Base::Impl::~Impl()
{
  mCustomShaders.clear();
  if(mDecorationData)
  {
    delete mDecorationData;
  }
}

Internal::Visual::Base::Impl::CustomShader::CustomShader(const Property::Map& map)
: mGridSize(1, 1),
  mHints(Shader::Hint::NONE),
  mRenderPassTag(0),
  mName("")
{
  SetPropertyMap(map);
}

void Internal::Visual::Base::Impl::CustomShader::SetPropertyMap(const Property::Map& shaderMap)
{
  mVertexShader.clear();
  mFragmentShader.clear();
  mGridSize = ImageDimensions(1, 1);
  mHints    = Shader::Hint::NONE;
  mName     = "";

  Property::Value* vertexShaderValue =
    shaderMap.Find(Ui::Visual::Shader::Property::VERTEX_SHADER, CUSTOM_VERTEX_SHADER);
  if(vertexShaderValue)
  {
    if(!GetStringFromProperty(*vertexShaderValue, mVertexShader))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a string\n", CUSTOM_VERTEX_SHADER);
    }
  }

  Property::Value* fragmentShaderValue =
    shaderMap.Find(Ui::Visual::Shader::Property::FRAGMENT_SHADER, CUSTOM_FRAGMENT_SHADER);
  if(fragmentShaderValue)
  {
    if(!GetStringFromProperty(*fragmentShaderValue, mFragmentShader))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a string\n", CUSTOM_FRAGMENT_SHADER);
    }
  }

  Property::Value* subdivideXValue =
    shaderMap.Find(Ui::Visual::Shader::Property::SUBDIVIDE_GRID_X, CUSTOM_SUBDIVIDE_GRID_X);
  if(subdivideXValue)
  {
    int subdivideX;
    if(!subdivideXValue->Get(subdivideX) || subdivideX < 1)
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a value greater than 1\n", CUSTOM_SUBDIVIDE_GRID_X);
    }
    else
    {
      mGridSize = ImageDimensions(subdivideX, mGridSize.GetY());
    }
  }

  Property::Value* subdivideYValue =
    shaderMap.Find(Ui::Visual::Shader::Property::SUBDIVIDE_GRID_Y, CUSTOM_SUBDIVIDE_GRID_Y);
  if(subdivideYValue)
  {
    int subdivideY;
    if(!subdivideYValue->Get(subdivideY) || subdivideY < 1)
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a value greater than 1\n", CUSTOM_SUBDIVIDE_GRID_Y);
    }
    else
    {
      mGridSize = ImageDimensions(mGridSize.GetX(), subdivideY);
    }
  }

  Property::Value* renderPassTagValue =
    shaderMap.Find(Ui::Visual::Shader::Property::RENDER_PASS_TAG, CUSTOM_RENDER_PASS_TAG);
  if(renderPassTagValue)
  {
    if(!renderPassTagValue->Get(mRenderPassTag) || mRenderPassTag < 0)
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a value greater than or equal 0\n",
                     CUSTOM_RENDER_PASS_TAG);
    }
  }

  Property::Value* hintsValue = shaderMap.Find(Ui::Visual::Shader::Property::HINTS, CUSTOM_SHADER_HINTS);
  if(hintsValue)
  {
    if(!Scripting::GetBitmaskEnumerationProperty(*hintsValue, SHADER_HINT_TABLE, SHADER_HINT_TABLE_COUNT, mHints))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a hint or an array of hint strings\n",
                     CUSTOM_SHADER_HINTS);
    }
  }

  Property::Value* nameValue = shaderMap.Find(Ui::Visual::Shader::Property::NAME, CUSTOM_SHADER_NAME);
  if(nameValue)
  {
    if(!GetStringFromProperty(*nameValue, mName))
    {
      DALI_LOG_ERROR("'%s' parameter does not correctly specify a string\n", CUSTOM_SHADER_NAME);
    }
  }
}

Property::Map Internal::Visual::Base::Impl::CustomShader::CreatePropertyMap() const
{
  Property::Map customShader;
  if(!mVertexShader.empty() || !mFragmentShader.empty())
  {
    if(!mVertexShader.empty())
    {
      customShader.Insert(Ui::Visual::Shader::Property::VERTEX_SHADER, ToPropertyValue(mVertexShader));
    }
    if(!mFragmentShader.empty())
    {
      customShader.Insert(Ui::Visual::Shader::Property::FRAGMENT_SHADER, ToPropertyValue(mFragmentShader));
    }

    if(mGridSize.GetWidth() != 1)
    {
      customShader.Insert(Ui::Visual::Shader::Property::SUBDIVIDE_GRID_X, mGridSize.GetWidth());
    }
    if(mGridSize.GetHeight() != 1)
    {
      customShader.Insert(Ui::Visual::Shader::Property::SUBDIVIDE_GRID_Y, mGridSize.GetHeight());
    }

    if(mRenderPassTag >= 0)
    {
      customShader.Insert(Ui::Visual::Shader::Property::RENDER_PASS_TAG, mRenderPassTag);
    }

    if(mHints != Dali::Shader::Hint::NONE)
    {
      customShader.Insert(Ui::Visual::Shader::Property::HINTS, static_cast<int>(mHints));
    }

    if(!mName.empty())
    {
      customShader.Insert(Ui::Visual::Shader::Property::NAME, ToPropertyValue(mName));
    }
  }
  return customShader;
}

void Internal::Visual::Base::Impl::SetTransformUniformsInternal(const Transform& transform, Dali::VisualRenderer renderer, Ui::Integration::Direction::Type direction)
{
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE, transform.mSize);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET,
                       direction == Ui::Integration::Direction::LEFT_TO_RIGHT ? transform.mOffset : transform.mOffset * Vector2(-1.0f, 1.0f));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET_SIZE_MODE, transform.mOffsetSizeMode);
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_ORIGIN,
                       PointToVector2(transform.mOrigin, direction) - Vector2(0.5, 0.5));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_PIVOT,
                       Vector2(0.5, 0.5) - PointToVector2(transform.mPivot, direction));
  renderer.SetProperty(VisualRenderer::Property::EXTRA_SIZE, transform.mExtraSize);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
