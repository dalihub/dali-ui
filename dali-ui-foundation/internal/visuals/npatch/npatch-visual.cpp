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
#include <dali-ui-foundation/internal/visuals/npatch/npatch-visual.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

#include <locale>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/utility/npatch-helper.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-loader.h>
#include <dali-ui-foundation/internal/visuals/rendering-addon.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToPropertyValue;

namespace Dali
{
namespace Ui
{
namespace Internal
{

/////////////////NPatchVisual////////////////

NPatchVisualPtr NPatchVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                  const VisualUrl& imageUrl, const Property::Map& properties)
{
  NPatchVisualPtr nPatchVisual(new NPatchVisual(factoryCache, shaderFactory, creationOptions));
  nPatchVisual->mImageUrl = imageUrl;
  nPatchVisual->SetProperties(properties);
  nPatchVisual->Initialize();
  return nPatchVisual;
}

NPatchVisualPtr NPatchVisual::New(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, Ui::Integration::VisualFactory::CreationOptions creationOptions,
                                  const VisualUrl& imageUrl)
{
  NPatchVisualPtr nPatchVisual(new NPatchVisual(factoryCache, shaderFactory, creationOptions));
  nPatchVisual->mImageUrl = imageUrl;
  nPatchVisual->Initialize();
  return nPatchVisual;
}

void NPatchVisual::LoadImages()
{
  TextureManager& textureManager     = mFactoryCache.GetTextureManager();
  bool            synchronousLoading = mImpl->mFlags & Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;

  if(mId == NPatchData::INVALID_NPATCH_DATA_ID)
  {
    bool preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !IsUsingCustomShader() ? true : false;
    mId                    = mLoader.Load(textureManager, this, mImageUrl, mBorder, preMultiplyOnLoad, synchronousLoading);

    NPatchDataPtr data;
    if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
    {
      EnablePreMultipliedAlpha(data->IsPreMultiplied());
    }
  }

  if(mAuxiliaryTextureId == TextureManager::INVALID_TEXTURE_ID && mAuxiliaryUrl.IsValid())
  {
    auto preMultiplyOnLoad = IsPreMultipliedAlphaEnabled() && !IsUsingCustomShader()
                               ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                               : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    if(mPreMultipliedAlphaIndex != Property::INVALID_INDEX)
    {
      mImpl->mRenderer.SetProperty(mPreMultipliedAlphaIndex, IsPreMultipliedAlphaEnabled() ? 1.0f : 0.0f);
    }
    else
    {
      // Register PRE_MULTIPLIED_ALPHA here.
      mPreMultipliedAlphaIndex = mImpl->mRenderer.RegisterProperty(
        Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled() ? 1.0f : 0.0f);
    }

    TextureManager::MaskingDataPointer maskingDataPtr = nullptr;
    bool                               loadingStatus  = false;

    // Load the auxiliary image
    mAuxiliaryTextureSet = textureManager.LoadTexture(mAuxiliaryUrl, Dali::ImageDimensions(),
                                                      SamplingMode::BOX_THEN_LINEAR, maskingDataPtr, synchronousLoading,
                                                      mAuxiliaryTextureId, loadingStatus, this, true,
                                                      TextureManager::ReloadPolicy::CACHED, preMultiplyOnLoad);

    // If synchronousLoading is true, we can check the auxiliaryResource's status now.
    if(synchronousLoading)
    {
      mAuxiliaryResourceStatus = (mAuxiliaryTextureSet && mAuxiliaryTextureSet.GetTextureCount() > 0u)
                                   ? Ui::Visual::ResourceStatus::READY
                                   : Ui::Visual::ResourceStatus::FAILED;
    }
  }
}

void NPatchVisual::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize.x = 0u;
  naturalSize.y = 0u;

  // load now if not already loaded
  NPatchDataPtr data;
  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() != NPatchData::LoadingState::LOADING)
  {
    naturalSize.x = static_cast<float>(data->GetCroppedWidth());
    naturalSize.y = static_cast<float>(data->GetCroppedHeight());
  }
  else
  {
    if(mImageUrl.IsValid())
    {
      ImageDimensions dimensions = Dali::GetOriginalImageSize(mImageUrl.GetUrl());
      if(dimensions != ImageDimensions(0, 0))
      {
        naturalSize.x = dimensions.GetWidth();
        naturalSize.y = dimensions.GetHeight();
      }
    }
  }

  if(mAuxiliaryTextureSet && mAuxiliaryTextureSet.GetTextureCount() > 0u)
  {
    naturalSize.x = std::max(naturalSize.x, float(mAuxiliaryTextureSet.GetTexture(0u).GetWidth()));
    naturalSize.y = std::max(naturalSize.y, float(mAuxiliaryTextureSet.GetTexture(0u).GetHeight()));
  }
}

void NPatchVisual::DoSetProperties(const Property::Map& propertyMap)
{
  // URL is already passed in via constructor

  Property::Value* borderOnlyValue = propertyMap.Find(Ui::ImageVisualPropertyIndex::BORDER_ONLY, BORDER_ONLY);
  if(borderOnlyValue)
  {
    borderOnlyValue->Get(mBorderOnly);
  }

  Property::Value* borderValue = propertyMap.Find(Ui::ImageVisualPropertyIndex::BORDER, BORDER);
  if(borderValue)
  {
    if(!borderValue->Get(mBorder)) // If value exists and is Extents (or Vector4), just set mBorder
    {
      // Not a extents so try rect
      Rect<int32_t> rect;
      if(borderValue->Get(rect))
      {
        mBorder.start  = static_cast<int16_t>(rect.x);
        mBorder.end    = static_cast<int16_t>(rect.y);
        mBorder.top    = static_cast<int16_t>(rect.width);
        mBorder.bottom = static_cast<int16_t>(rect.height);
      }
    }
    // Ensure the range of border valid.
    Dali::ClampInPlace(mBorder.start, static_cast<int16_t>(0), static_cast<int16_t>(0x7FFF));
    Dali::ClampInPlace(mBorder.end, static_cast<int16_t>(0), static_cast<int16_t>(0x7FFF));
    Dali::ClampInPlace(mBorder.top, static_cast<int16_t>(0), static_cast<int16_t>(0x7FFF));
    Dali::ClampInPlace(mBorder.bottom, static_cast<int16_t>(0), static_cast<int16_t>(0x7FFF));
  }

  Property::Value* auxImage = propertyMap.Find(Ui::ImageVisualPropertyIndex::AUXILIARY_IMAGE, AUXILIARY_IMAGE_NAME);
  if(auxImage)
  {
    std::string url;
    if(GetStdString(*auxImage, url))
    {
      mAuxiliaryUrl = url;
    }
  }

  Property::Value* auxImageAlpha =
    propertyMap.Find(Ui::ImageVisualPropertyIndex::AUXILIARY_IMAGE_ALPHA, AUXILIARY_IMAGE_ALPHA_NAME);
  if(auxImageAlpha)
  {
    auxImageAlpha->Get(mAuxiliaryImageAlpha);
  }

  Property::Value* synchronousLoading =
    propertyMap.Find(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, SYNCHRONOUS_LOADING);
  if(synchronousLoading)
  {
    bool sync = false;
    synchronousLoading->Get(sync);
    if(sync)
    {
      mImpl->mFlags |= Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
    else
    {
      mImpl->mFlags &= ~Visual::Base::Impl::IS_SYNCHRONOUS_RESOURCE_LOADING;
    }
  }

  Property::Value* releasePolicy = propertyMap.Find(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, RELEASE_POLICY_NAME);
  if(releasePolicy)
  {
    releasePolicy->Get(mReleasePolicy);
  }

  Property::Value* preMultiplied = propertyMap.Find(Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, PRE_MULTIPLIED_ALPHA);
  if(preMultiplied)
  {
    bool premultipliedAlpha = false;
    if(preMultiplied->Get(premultipliedAlpha))
    {
      EnablePreMultipliedAlpha(premultipliedAlpha);
    }
  }

  Property::Value* fittingMode = propertyMap.Find(Ui::ImageVisualPropertyIndex::FITTING_MODE, FITTING_MODE);
  if(fittingMode)
  {
    fittingMode->Get(mFittingMode);
  }
}

void NPatchVisual::DoSetOnScene(Actor& actor)
{
  // load when first go on stage
  LoadImages();

  // Set mPlacementActor now, because some case, LoadImages can use this information in LoadComplete API.
  // at this case, we try to SetResouce to mPlaceActor twice. so, we should avoid that case.
  mPlacementActor = actor;

  NPatchDataPtr data;
  if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data) &&
     data->GetLoadingState() != NPatchData::LoadingState::LOADING)
  {
    // If mAuxiliaryUrl need to be loaded, we should wait it until LoadComplete called.
    if(!mAuxiliaryUrl.IsValid() || mAuxiliaryResourceStatus != Ui::Visual::ResourceStatus::PREPARING)
    {
      SetResource();
    }
  }
}

void NPatchVisual::DoSetOffScene(Actor& actor)
{
  if(mReleasePolicy == Ui::Image::ReleasePolicy::DETACHED)
  {
    if(mId != NPatchData::INVALID_NPATCH_DATA_ID)
    {
      mLoader.RequestRemove(mId, this);
      mImpl->mResourceStatus = Ui::Visual::ResourceStatus::PREPARING;
      mId                    = NPatchData::INVALID_NPATCH_DATA_ID;
    }
    if(mAuxiliaryTextureId != TextureManager::INVALID_TEXTURE_ID)
    {
      TextureManager& textureManager = mFactoryCache.GetTextureManager();
      textureManager.RequestRemove(mAuxiliaryTextureId, this);
      mAuxiliaryTextureId      = TextureManager::INVALID_TEXTURE_ID;
      mAuxiliaryResourceStatus = Ui::Visual::ResourceStatus::PREPARING;
      mAuxiliaryTextureSet.Reset();
    }

    // Remove textureset now.
    mImpl->mRenderer.RemoveTextures();
  }

  actor.RemoveRenderer(mImpl->mRenderer);
  mPlacementActor.Reset();
}

void NPatchVisual::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  mFittingMode = fittingMode;
}

void NPatchVisual::OnApplyFittingMode(const Vector2& controlSize, const Insets& padding, float effectiveScale)
{
  DoApplyFittingMode(controlSize, padding, effectiveScale, mFittingMode);
}

void NPatchVisual::OnSetTransform()
{
  if(mImpl->mRenderer && mImpl->mTransformMapChanged)
  {
    mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);
  }
}

void NPatchVisual::UpdateShader()
{
  if(mImpl->mRenderer)
  {
    Shader shader = CreateShader();
    mImpl->mRenderer.SetShader(shader);
  }
}

void NPatchVisual::DoCreatePropertyMap(Property::Map& map) const
{
  map.Clear();
  bool sync = IsSynchronousLoadingRequired();
  map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, sync);
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::N_PATCH);
  map.Insert(Ui::ImageVisualPropertyIndex::URL, ToPropertyValue(mImageUrl.GetUrl()));
  map.Insert(Ui::ImageVisualPropertyIndex::BORDER_ONLY, mBorderOnly);
  map.Insert(Ui::ImageVisualPropertyIndex::BORDER, mBorder);
  map.Insert(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, mReleasePolicy);
  map.Insert(Ui::ImageVisualPropertyIndex::FITTING_MODE, mFittingMode);
  map.Insert(Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, IsPreMultipliedAlphaEnabled());

  if(mAuxiliaryUrl.IsValid())
  {
    map.Insert(Ui::ImageVisualPropertyIndex::AUXILIARY_IMAGE, ToPropertyValue(mAuxiliaryUrl.GetUrl()));
    map.Insert(Ui::ImageVisualPropertyIndex::AUXILIARY_IMAGE_ALPHA, mAuxiliaryImageAlpha);
  }
}

void NPatchVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  if(mAuxiliaryUrl.IsValid())
  {
    map.Insert(Ui::ImageVisualPropertyIndex::AUXILIARY_IMAGE, ToPropertyValue(mAuxiliaryUrl.GetUrl()));
    map.Insert(Ui::ImageVisualPropertyIndex::AUXILIARY_IMAGE_ALPHA, mAuxiliaryImageAlpha);
  }
}

void NPatchVisual::EnablePreMultipliedAlpha(bool preMultiplied)
{
  if(mImpl->mRenderer && mPreMultipliedAlphaIndex != Property::INVALID_INDEX)
  {
    mImpl->mRenderer.SetProperty(mPreMultipliedAlphaIndex, preMultiplied ? 1.0f : 0.0f);
  }

  if(preMultiplied)
  {
    mImpl->mFlags |= Visual::Base::Impl::IS_PRE_MULTIPLIED_ALPHA;
  }
  else
  {
    mImpl->mFlags &= ~Visual::Base::Impl::IS_PRE_MULTIPLIED_ALPHA;
  }
}

NPatchVisual::NPatchVisual(VisualFactoryCache& factoryCache, ImageVisualShaderFactory& shaderFactory, Ui::Integration::VisualFactory::CreationOptions creationOptions)
: Visual::Base(factoryCache, Ui::Integration::InternalVisualType::N_PATCH),
  mPlacementActor(),
  mLoader(factoryCache.GetNPatchLoader()),
  mImageVisualShaderFactory(shaderFactory),
  mImageUrl(),
  mAuxiliaryUrl(),
  mId(NPatchData::INVALID_NPATCH_DATA_ID),
  mAuxiliaryTextureSet(),
  mAuxiliaryTextureId(TextureManager::INVALID_TEXTURE_ID),
  mAuxiliaryResourceStatus(Ui::Visual::ResourceStatus::PREPARING),
  mPreMultipliedAlphaIndex(Property::INVALID_INDEX),
  mBorderOnly(false),
  mBorder(),
  mAuxiliaryImageAlpha(0.0f),
  mReleasePolicy(Ui::Image::ReleasePolicy::DETACHED),
  mFittingMode(Ui::Image::FittingMode::FILL)
{
  EnablePreMultipliedAlpha(mFactoryCache.GetPreMultiplyOnLoad());

  mImpl->mFittingModeRequired = true;

  if(creationOptions & Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_IGNORE_VIEW_PADDING)
  {
    mImpl->mFlags |= Visual::Base::Impl::IS_FITTING_MODE_IGNORE_VIEW_PADDING;
  }
}

NPatchVisual::~NPatchVisual()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    if(mReleasePolicy != Ui::Image::ReleasePolicy::NEVER)
    {
      if(mId != NPatchData::INVALID_NPATCH_DATA_ID)
      {
        mLoader.RequestRemove(mId, this);
        mId = NPatchData::INVALID_NPATCH_DATA_ID;
      }
      if(mAuxiliaryTextureId != TextureManager::INVALID_TEXTURE_ID)
      {
        TextureManager& textureManager = mFactoryCache.GetTextureManager();

        textureManager.RequestRemove(mAuxiliaryTextureId, this);
        mAuxiliaryTextureId = TextureManager::INVALID_TEXTURE_ID;
        mAuxiliaryTextureSet.Reset();
      }
    }
  }
}

void NPatchVisual::OnInitialize()
{
  // Get basic geometry and shader
  Geometry geometry                        = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  auto     imageVisualShaderFeatureBuilder = ImageVisualShaderFeature::FeatureBuilder();
  Shader   shader                          = mImageVisualShaderFactory.GetShader(mFactoryCache, imageVisualShaderFeatureBuilder);

  mImpl->mRenderer = VisualRenderer::New(geometry, shader);

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);
}

Geometry NPatchVisual::CreateGeometry()
{
  Geometry      geometry;
  NPatchDataPtr data;
  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    if(data->GetStretchPixelsX().Size() == 1 && data->GetStretchPixelsY().Size() == 1)
    {
      if(DALI_UNLIKELY(mBorderOnly))
      {
        geometry = GetNinePatchGeometry(VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY);
      }
      else
      {
        if(data->GetRenderingMap())
        {
          uint32_t elementCount[2];
          geometry = RenderingAddOn::Get().CreateGeometryGrid(data->GetRenderingMap(), Uint16Pair(3, 3), elementCount);
          if(mImpl->mRenderer)
          {
            RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
          }
        }
        else
        {
          geometry = GetNinePatchGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
        }
      }
    }
    else if(data->GetStretchPixelsX().Size() > 0 || data->GetStretchPixelsY().Size() > 0)
    {
      Uint16Pair gridSize(static_cast<uint32_t>(2u * data->GetStretchPixelsX().Size() + 1u), static_cast<uint32_t>(2u * data->GetStretchPixelsY().Size() + 1u));
      if(!data->GetRenderingMap())
      {
        geometry = !mBorderOnly ? VisualFactoryCache::CreateGridGeometry(gridSize, false)
                                : VisualFactoryCache::CreateBorderGeometry(gridSize);
      }
      else
      {
        uint32_t elementCount[2];
        geometry = !mBorderOnly
                     ? RenderingAddOn::Get().CreateGeometryGrid(data->GetRenderingMap(), gridSize, elementCount)
                     : VisualFactoryCache::CreateBorderGeometry(gridSize);
        if(mImpl->mRenderer)
        {
          RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
        }
      }
    }
  }
  else
  {
    // no N patch data so use default geometry
    geometry = GetNinePatchGeometry(VisualFactoryCache::NINE_PATCH_GEOMETRY);
  }
  return geometry;
}

Shader NPatchVisual::CreateShader()
{
  Shader        shader;
  NPatchDataPtr data;
  // 0 is either no data (load failed?) or no stretch regions on image
  // for both cases we use the default shader
  Dali::Ui::Integration::NPatchUtility::StretchRanges::SizeType xStretchCount = 0;
  Dali::Ui::Integration::NPatchUtility::StretchRanges::SizeType yStretchCount = 0;

  auto fragmentShader = mAuxiliaryResourceStatus == Ui::Visual::ResourceStatus::READY
                          ? SHADER_NPATCH_VISUAL_MASK_SHADER_FRAG
                          : SHADER_NPATCH_VISUAL_SHADER_FRAG;
  auto shaderType     = mAuxiliaryResourceStatus == Ui::Visual::ResourceStatus::READY
                          ? VisualFactoryCache::NINE_PATCH_MASK_SHADER
                          : VisualFactoryCache::NINE_PATCH_SHADER;

  // ask loader for the regions
  if(mLoader.GetNPatchData(mId, data))
  {
    xStretchCount = data->GetStretchPixelsX().Count();
    yStretchCount = data->GetStretchPixelsY().Count();
  }

  if(DALI_LIKELY(!IsUsingCustomShader()))
  {
    if(DALI_LIKELY((xStretchCount == 1 && yStretchCount == 1) || (xStretchCount == 0 && yStretchCount == 0)))
    {
      shader = mFactoryCache.GetShader(shaderType);
      if(DALI_UNLIKELY(!shader))
      {
        shader = mFactoryCache.GenerateAndSaveShader(shaderType, SHADER_NPATCH_VISUAL_3X3_SHADER_VERT, fragmentShader);

        shader.ReserveCustomProperties(2);
        shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
        shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
      }
    }
    else if(xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream shaderName;
      shaderName.imbue(std::locale::classic());
      shaderName << "N_PATCH_" << xStretchCount << "x" << yStretchCount;

      std::stringstream vertexShader;
      vertexShader.imbue(std::locale::classic());
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << SHADER_NPATCH_VISUAL_SHADER_VERT;

      shader = Shader::New(ToDaliStringView(vertexShader.str()), ToDaliStringView(fragmentShader), Dali::Shader::Hint::NONE, ToDaliStringView(shaderName.str()));

      shader.ReserveCustomProperties(2);
      shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
      shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
    }
  }
  else
  {
    Dali::Shader::Hint::Value hints = Dali::Shader::Hint::NONE;

    if(!mImpl->GetCustomShaderAt(0)->mFragmentShader.empty())
    {
      fragmentShader = mImpl->GetCustomShaderAt(0)->mFragmentShader.c_str();
    }
    hints = mImpl->GetCustomShaderAt(0)->mHints;

    /* Apply Custom Vertex Shader only if image is 9-patch */
    if((xStretchCount == 1 && yStretchCount == 1) || (xStretchCount == 0 && yStretchCount == 0))
    {
      const char* vertexShader = SHADER_NPATCH_VISUAL_3X3_SHADER_VERT.data();

      if(!mImpl->GetCustomShaderAt(0)->mVertexShader.empty())
      {
        vertexShader = mImpl->GetCustomShaderAt(0)->mVertexShader.c_str();
      }
      shader = Shader::New(vertexShader, ToDaliStringView(fragmentShader), hints);
    }
    else if(xStretchCount > 0 || yStretchCount > 0)
    {
      std::stringstream shaderName;
      shaderName.imbue(std::locale::classic());
      shaderName << "N_PATCH_" << xStretchCount << "x" << yStretchCount;

      std::stringstream vertexShader;
      vertexShader.imbue(std::locale::classic());
      vertexShader << "#define FACTOR_SIZE_X " << xStretchCount + 2 << "\n"
                   << "#define FACTOR_SIZE_Y " << yStretchCount + 2 << "\n"
                   << SHADER_NPATCH_VISUAL_SHADER_VERT;

      shader = Shader::New(ToDaliStringView(vertexShader.str()), ToDaliStringView(fragmentShader), hints, ToDaliStringView(shaderName.str()));
    }

    shader.ReserveCustomProperties(2);
    shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
    shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
  }

  return shader;
}

void NPatchVisual::ApplyTextureAndUniforms()
{
  NPatchDataPtr data;
  TextureSet    textureSet;

  if(mLoader.GetNPatchData(mId, data) && data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    textureSet = data->GetTextures();
    Dali::Ui::Integration::NPatchHelper::ApplyTextureAndUniforms(mImpl->mRenderer, data.Get());

    if(mAuxiliaryResourceStatus == Ui::Visual::ResourceStatus::READY)
    {
      DALI_ASSERT_ALWAYS(mAuxiliaryTextureId != TextureManager::INVALID_TEXTURE_ID);
      DALI_ASSERT_ALWAYS(mAuxiliaryTextureSet && mAuxiliaryTextureSet.GetTextureCount() > 0u);

      // TODO : This code exist due to the texture cache manager hold TextureSet, not Texture.
      // If we call textureSet.SetTexture(1, texture) directly, the cached TextureSet also be changed.
      // We should make pass utc-Dali-VisualFactory.cpp UtcDaliNPatchVisualAuxiliaryImage02().
      TextureSet tempTextureSet = TextureSet::New();
      tempTextureSet.SetTexture(1, mAuxiliaryTextureSet.GetTexture(0));
      tempTextureSet.SetTexture(0, textureSet.GetTexture(0));
      textureSet = tempTextureSet;

      mImpl->mRenderer.RegisterProperty(ImageVisualPropertyIndex::AUXILIARY_IMAGE_ALPHA, AUXILIARY_IMAGE_ALPHA_NAME,
                                        mAuxiliaryImageAlpha);
    }
    mImpl->mRenderer.SetTextures(textureSet);
  }
  else
  {
    DALI_LOG_ERROR("The N patch image '%s' is not a valid N patch image\n", mImageUrl.GetUrl().c_str());
    Actor   actor     = mPlacementActor.GetHandle();
    Vector2 imageSize = Vector2::ZERO;
    if(actor)
    {
      imageSize = actor.GetProperty(Actor::Property::SIZE).Get<Vector2>();
    }
    mFactoryCache.UpdateBrokenImageRenderer(mImpl->mRenderer, imageSize, false);
  }

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, Dali::Ui::Integration::Direction::LEFT_TO_RIGHT);
}

Geometry NPatchVisual::GetNinePatchGeometry(VisualFactoryCache::GeometryType subType)
{
  Geometry geometry = mFactoryCache.GetGeometry(subType);
  if(!geometry)
  {
    if(DALI_LIKELY(VisualFactoryCache::NINE_PATCH_GEOMETRY == subType))
    {
      geometry = VisualFactoryCache::CreateGridGeometry(Uint16Pair(3, 3), false);
    }
    else if(VisualFactoryCache::NINE_PATCH_BORDER_GEOMETRY == subType)
    {
      geometry = VisualFactoryCache::CreateBorderGeometry(Uint16Pair(3, 3));
    }
    mFactoryCache.SaveGeometry(subType, geometry);
  }
  return geometry;
}

void NPatchVisual::SetResource()
{
  NPatchDataPtr data;
  if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data))
  {
    Geometry geometry = CreateGeometry();
    Shader   shader   = CreateShader();

    mImpl->mRenderer.SetGeometry(geometry);
    mImpl->mRenderer.SetShader(shader);

    if(RenderingAddOn::Get().IsValid())
    {
      RenderingAddOn::Get().SubmitRenderTask(mImpl->mRenderer, data->GetRenderingMap());
    }
    Actor actor = mPlacementActor.GetHandle();
    if(actor)
    {
      ApplyTextureAndUniforms();
      actor.AddRenderer(mImpl->mRenderer);
      mPlacementActor.Reset();
    }

    // npatch loaded and ready to display
    if(data->GetLoadingState() != NPatchData::LoadingState::LOAD_COMPLETE)
    {
      ResourceReady(Ui::Visual::ResourceStatus::FAILED);
    }
    else
    {
      ResourceReady(Ui::Visual::ResourceStatus::READY);
    }
  }
}

void NPatchVisual::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  if(textureInformation.url.length() > 0) // For the Url.
  {
    if(DALI_UNLIKELY(mId == NPatchData::INVALID_NPATCH_DATA_ID))
    {
      // Special case when mLoader.Load call LoadComplete function before mId setup.
      // We can overwrite mId.
      mId = static_cast<NPatchData::NPatchDataId>(textureInformation.textureId);
    }
    if(loadSuccess)
    {
      EnablePreMultipliedAlpha(textureInformation.preMultiplied);
    }
  }
  else // For the AuxiliaryUrl
  {
    if(DALI_UNLIKELY(mAuxiliaryTextureId == TextureManager::INVALID_TEXTURE_ID))
    {
      // Special case when TextureManager.LoadTexture call LoadComplete function before mAuxiliaryTextureId setup.
      // We can overwrite mAuxiliaryTextureId.
      mAuxiliaryTextureId = textureInformation.textureId;
    }
    if(loadSuccess)
    {
      mAuxiliaryTextureSet     = textureInformation.textureSet;
      mAuxiliaryResourceStatus = Ui::Visual::ResourceStatus::READY;
    }
    else
    {
      mAuxiliaryResourceStatus = Ui::Visual::ResourceStatus::FAILED;
    }
  }

  // If auxiliaryUrl didn't required OR auxiliaryUrl load done.
  if(!mAuxiliaryUrl.IsValid() || mAuxiliaryResourceStatus != Ui::Visual::ResourceStatus::PREPARING)
  {
    NPatchDataPtr data;
    // and.. If Url loading done.
    if(mImpl->mRenderer && mLoader.GetNPatchData(mId, data) &&
       data->GetLoadingState() != NPatchData::LoadingState::LOADING)
    {
      SetResource();
    }
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
