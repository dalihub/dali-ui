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
 */

// CLASS HEADER
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-ui-foundation/internal/visuals/animated-vector-image/animated-vector-image-visual.h>
#include <dali-ui-foundation/internal/visuals/arc/arc-visual.h>
#include <dali-ui-foundation/internal/visuals/border/border-visual.h>
#include <dali-ui-foundation/internal/visuals/color/color-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/color/color-visual.h>
#include <dali-ui-foundation/internal/visuals/custom-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/gradient/gradient-visual.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual.h>
#include <dali-ui-foundation/internal/visuals/mesh/mesh-visual.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-visual.h>
#include <dali-ui-foundation/internal/visuals/primitive/primitive-visual.h>
#include <dali-ui-foundation/internal/visuals/svg/svg-visual.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>
#include <dali-ui-foundation/internal/visuals/wireframe/wireframe-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

BaseHandle Create()
{
  BaseHandle handle = Ui::VisualFactory::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Ui::VisualFactory, Dali::BaseHandle, Create, true)
DALI_TYPE_REGISTRATION_END()
const char* const BROKEN_IMAGE_FILE_NAME = "broken.png"; ///< The file name of the broken image.

} // namespace

VisualFactory::VisualFactory(bool debugEnabled)
: mFactoryCache(),
  mImageVisualShaderFactory(),
  mTextVisualShaderFactory(),
  mColorVisualShaderFactory(),
  mSlotDelegate(this),
  mIdleCallback(nullptr),
  mDefaultCreationOptions(Ui::VisualFactory::CreationOptions::NONE),
  mAdaptorInitialized(false),
  mDebugEnabled(debugEnabled),
  mPreMultiplyOnLoad(true),
  mPrecompiledShaderRequested(false)
{
  Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
  if(DALI_LIKELY(lifecycleController))
  {
    lifecycleController.PreInitSignal().Connect(this, &VisualFactory::OnAdaptorInitialized);
    lifecycleController.TerminateSignal().Connect(this, &VisualFactory::OnAdaptorTerminated);
  }
}

VisualFactory::~VisualFactory()
{
  if(Adaptor::IsAvailable())
  {
    if(mIdleCallback)
    {
      // Removes the callback from the callback manager in case the control is destroyed before the callback is
      // executed.
      Adaptor::Get().RemoveIdle(mIdleCallback);
      mIdleCallback = nullptr;
    }
  }
}

Ui::Visual::Base VisualFactory::CreateVisual(const Property::Map& propertyMap)
{
  return CreateVisual(propertyMap, mDefaultCreationOptions);
}

Ui::Visual::Base VisualFactory::CreateVisual(const Property::Map&               propertyMap,
                                             Ui::VisualFactory::CreationOptions creationOptions)
{
  Visual::BasePtr visualPtr;

  Property::Value*      typeValue  = propertyMap.Find(Ui::Visual::Property::TYPE, VISUAL_TYPE);
  Ui::DevelVisual::Type visualType = Ui::DevelVisual::IMAGE; // Default to IMAGE type.
  if(typeValue)
  {
    Scripting::GetEnumerationProperty(*typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType);
  }

  switch(visualType)
  {
    case Ui::Visual::BORDER:
    {
      visualPtr = BorderVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Ui::Visual::COLOR:
    {
      visualPtr = ColorVisual::New(GetFactoryCache(), GetColorVisualShaderFactory(), propertyMap);
      break;
    }

    case Ui::Visual::GRADIENT:
    {
      visualPtr = GradientVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Ui::Visual::IMAGE:
    case Ui::Visual::ANIMATED_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find(Ui::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue)
      {
        if(GetStdString(*imageURLValue, imageUrl))
        {
          if(!imageUrl.empty())
          {
            VisualUrl visualUrl(imageUrl);

            switch(visualUrl.GetType())
            {
              case VisualUrl::N_PATCH:
              {
                visualPtr = NPatchVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                break;
              }
              case VisualUrl::TVG:
              case VisualUrl::SVG:
              {
                visualPtr = SvgVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                break;
              }
              case VisualUrl::JSON:
              {
                visualPtr = AnimatedVectorImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl,
                                                           propertyMap);
                break;
              }
              case VisualUrl::GIF:
              case VisualUrl::WEBP:
              {
                if(visualType == Ui::DevelVisual::ANIMATED_IMAGE ||
                   !(creationOptions & Ui::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY))
                {
                  visualPtr = AnimatedImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl,
                                                       propertyMap);
                  break;
                }
                DALI_FALLTHROUGH;
              }
              case VisualUrl::REGULAR_IMAGE:
              {
                visualPtr = ImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                break;
              }
            }
          }
        }
        else
        {
          Property::Array* array = imageURLValue->GetArray();
          if(array && array->Count() > 0)
          {
            visualPtr = AnimatedImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), *array, propertyMap);
          }
        }
      }
      break;
    }

    case Ui::Visual::MESH:
    {
      visualPtr = MeshVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Ui::Visual::PRIMITIVE:
    {
      visualPtr = PrimitiveVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Ui::Visual::WIREFRAME:
    {
      visualPtr = WireframeVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Ui::Visual::TEXT:
    {
      visualPtr = TextVisual::New(GetFactoryCache(), GetTextVisualShaderFactory(), propertyMap);
      break;
    }

    case Ui::Visual::N_PATCH:
    {
      Property::Value* imageURLValue = propertyMap.Find(Ui::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue && GetStdString(*imageURLValue, imageUrl))
      {
        if(!imageUrl.empty())
        {
          visualPtr = NPatchVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
        }
      }
      break;
    }

    case Ui::Visual::SVG:
    {
      Property::Value* imageURLValue = propertyMap.Find(Ui::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue && GetStdString(*imageURLValue, imageUrl))
      {
        if(!imageUrl.empty())
        {
          visualPtr = SvgVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
        }
      }
      break;
    }

    case Ui::DevelVisual::ANIMATED_VECTOR_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find(Ui::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue && GetStdString(*imageURLValue, imageUrl))
      {
        if(!imageUrl.empty())
        {
          visualPtr =
            AnimatedVectorImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
        }
      }
      break;
    }

    case Ui::DevelVisual::ARC:
    {
      visualPtr = ArcVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    default:
    {
      break;
    }
  }

  DALI_LOG_INFO(
    gLogFilter, Debug::Concise, "VisualFactory::CreateVisual( VisualType:%s %s%s)\n",
    Scripting::GetEnumerationName<Ui::DevelVisual::Type>(visualType, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT),
    (visualType == Ui::DevelVisual::IMAGE) ? "url:" : "",
    ((visualType == Ui::DevelVisual::IMAGE)
       ? ((
           [&]()
  {
    // Return URL if present in PropertyMap else return "not
    // found message"
    Property::Value* imageURLValue = propertyMap.Find(Ui::ImageVisual::Property::URL, IMAGE_URL_NAME);
    return (imageURLValue) ? ToStdString(*imageURLValue)
                           : std::string("url not found in PropertyMap");
  })())
       : std::string(""))
      .c_str());

  if(!visualPtr)
  {
    DALI_LOG_ERROR("VisualType unknown\n");
  }

  if(mDebugEnabled && visualType != Ui::DevelVisual::WIREFRAME)
  {
    // Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New(GetFactoryCache(), visualPtr, propertyMap);
  }

  return Ui::Visual::Base(visualPtr.Get());
}

Ui::Visual::Base VisualFactory::CreateVisual(const std::string& url, ImageDimensions size)
{
  return CreateVisual(url, size, mDefaultCreationOptions);
}

Ui::Visual::Base VisualFactory::CreateVisual(const std::string& url, ImageDimensions size,
                                             Ui::VisualFactory::CreationOptions creationOptions)
{
  Visual::BasePtr visualPtr;

  if(!url.empty())
  {
    // first resolve url type to know which visual to create
    VisualUrl visualUrl(url);
    switch(visualUrl.GetType())
    {
      case VisualUrl::N_PATCH:
      {
        visualPtr = NPatchVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl);
        break;
      }
      case VisualUrl::TVG:
      case VisualUrl::SVG:
      {
        visualPtr = SvgVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
        break;
      }
      case VisualUrl::JSON:
      {
        visualPtr = AnimatedVectorImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
        break;
      }
      case VisualUrl::GIF:
      case VisualUrl::WEBP:
      {
        if(!(creationOptions & Ui::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY))
        {
          visualPtr = AnimatedImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
          break;
        }
        DALI_FALLTHROUGH;
      }
      case VisualUrl::REGULAR_IMAGE:
      {
        visualPtr = ImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
        break;
      }
    }
  }

  if(mDebugEnabled)
  {
    // Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New(GetFactoryCache(), visualPtr);
  }

  return Ui::Visual::Base(visualPtr.Get());
}

Dali::Geometry VisualFactory::GetDefaultQuadGeometry()
{
  return GetFactoryCache().GetGeometry(VisualFactoryCache::GeometryType::QUAD_GEOMETRY);
}

void VisualFactory::SetPreMultiplyOnLoad(bool preMultiply)
{
  if(mPreMultiplyOnLoad != preMultiply)
  {
    GetFactoryCache().SetPreMultiplyOnLoad(preMultiply);
  }
  mPreMultiplyOnLoad = preMultiply;
}

bool VisualFactory::GetPreMultiplyOnLoad() const
{
  return mPreMultiplyOnLoad;
}

void VisualFactory::SetDefaultCreationOptions(Ui::VisualFactory::CreationOptions creationOptions)
{
  mDefaultCreationOptions = creationOptions;
}

Ui::VisualFactory::CreationOptions VisualFactory::GetDefaultCreationOptions() const
{
  return mDefaultCreationOptions;
}

void VisualFactory::DiscardVisual(Ui::Visual::Base visual)
{
  mDiscardedVisuals.emplace_back(visual);

  RegisterDiscardCallback();
}

bool VisualFactory::AddPrecompileShader(const Property::Map& map)
{
  PrecompileShaderOption shaderOption(map);
  auto                   type = shaderOption.GetShaderType();
  if(type == PrecompileShaderOption::ShaderType::UNKNOWN)
  {
    DALI_LOG_ERROR("AddPrecompileShader is failed. we can't find shader type\n");
    return false;
  }

  return AddPrecompileShader(shaderOption);
}

void VisualFactory::UsePreCompiledShader()
{
  if(mPrecompiledShaderRequested)
  {
    return;
  }
  mPrecompiledShaderRequested = true;

  ShaderPreCompiler::Get().Enable(true);

  // Get image shader
  ShaderPreCompiler::RawShaderDataList rawShaderList;
  ShaderPreCompiler::RawShaderData     imageShaderData;
  GetImageVisualShaderFactory().GetPreCompiledShader(imageShaderData);
  rawShaderList.emplace_back(std::move(imageShaderData));

  // Get text shader
  ShaderPreCompiler::RawShaderData textShaderData;
  GetTextVisualShaderFactory().GetPreCompiledShader(textShaderData);
  rawShaderList.emplace_back(std::move(textShaderData));

  // Get color shader
  ShaderPreCompiler::RawShaderData colorShaderData;
  GetColorVisualShaderFactory().GetPreCompiledShader(colorShaderData);
  rawShaderList.emplace_back(std::move(colorShaderData));

  // Get npatch shader
  ShaderPreCompiler::RawShaderData npatchShaderData;
  GetNpatchShaderFactory().GetPreCompiledShader(npatchShaderData);
  rawShaderList.emplace_back(std::move(npatchShaderData));

  // Get 3D shader
  // TODO

  // Get Custom shader
  ShaderPreCompiler::RawShaderData customShaderData;
  GetCustomShaderFactory().GetPreCompiledShader(customShaderData);
  rawShaderList.emplace_back(std::move(customShaderData));

  // Save all shader
  ShaderPreCompiler::Get().SavePreCompileShaderList(std::move(rawShaderList));
}

Internal::TextureManager& VisualFactory::GetTextureManager()
{
  return GetFactoryCache().GetTextureManager();
}

Internal::SvgLoader& VisualFactory::GetSvgLoader()
{
  return GetFactoryCache().GetSvgLoader();
}

Internal::VisualFactoryCache& VisualFactory::GetFactoryCache()
{
  if(!mFactoryCache)
  {
    mFactoryCache = std::unique_ptr<VisualFactoryCache>(new VisualFactoryCache(mPreMultiplyOnLoad));

    // Get broken image urls
    auto imageDirPath   = AssetManager::GetDaliImagePath();
    auto brokenImageUrl = imageDirPath + BROKEN_IMAGE_FILE_NAME;

    std::vector<std::string> customBrokenImageUrlList{};
    auto&                    uiConfigManager = Integration::UiConfigManager::Get();
    if(uiConfigManager.IsInitialized())
    {
      customBrokenImageUrlList = Integration::UiConfigManager::Get().GetBrokenImageUrlList();
    }

    // Add default image
    mFactoryCache->SetBrokenImageUrl(brokenImageUrl, customBrokenImageUrlList);
  }

  return *mFactoryCache;
}

ImageVisualShaderFactory& VisualFactory::GetImageVisualShaderFactory()
{
  if(!mImageVisualShaderFactory)
  {
    mImageVisualShaderFactory = std::unique_ptr<ImageVisualShaderFactory>(new ImageVisualShaderFactory());
  }
  return *mImageVisualShaderFactory;
}

TextVisualShaderFactory& VisualFactory::GetTextVisualShaderFactory()
{
  if(!mTextVisualShaderFactory)
  {
    mTextVisualShaderFactory = std::unique_ptr<TextVisualShaderFactory>(new TextVisualShaderFactory());
  }
  return *mTextVisualShaderFactory;
}

ColorVisualShaderFactory& VisualFactory::GetColorVisualShaderFactory()
{
  if(!mColorVisualShaderFactory)
  {
    mColorVisualShaderFactory = std::unique_ptr<ColorVisualShaderFactory>(new ColorVisualShaderFactory());
  }
  return *mColorVisualShaderFactory;
}

NpatchShaderFactory& VisualFactory::GetNpatchShaderFactory()
{
  if(!mNpatchShaderFactory)
  {
    mNpatchShaderFactory = std::unique_ptr<NpatchShaderFactory>(new NpatchShaderFactory());
  }
  return *mNpatchShaderFactory;
}

CustomShaderFactory& VisualFactory::GetCustomShaderFactory()
{
  if(!mCustomShaderFactory)
  {
    mCustomShaderFactory = std::unique_ptr<CustomShaderFactory>(new CustomShaderFactory());
  }
  return *mCustomShaderFactory;
}

bool VisualFactory::AddPrecompileShader(PrecompileShaderOption& option)
{
  auto type = option.GetShaderType();
  bool ret  = false;
  switch(type)
  {
    case PrecompileShaderOption::ShaderType::COLOR:
    {
      ret = GetColorVisualShaderFactory().AddPrecompiledShader(option);
      break;
    }
    case PrecompileShaderOption::ShaderType::IMAGE:
    {
      ret = GetImageVisualShaderFactory().AddPrecompiledShader(option);
      break;
    }
    case PrecompileShaderOption::ShaderType::TEXT:
    {
      ret = GetTextVisualShaderFactory().AddPrecompiledShader(option);
      break;
    }
    case PrecompileShaderOption::ShaderType::NPATCH:
    {
      ret = GetNpatchShaderFactory().AddPrecompiledShader(option);
      break;
    }
    case PrecompileShaderOption::ShaderType::MODEL_3D:
    {
      // TODO
      break;
    }
    case PrecompileShaderOption::ShaderType::CUSTOM:
    {
      ret = GetCustomShaderFactory().AddPrecompiledShader(option);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("AddPrecompileShader is failed. we can't find shader factory type:%d\n", type);
      break;
    }
  }

  return ret;
}

void VisualFactory::OnDiscardCallback()
{
  mIdleCallback = nullptr;

  // Discard visual now.
  mDiscardedVisuals.clear();
}

void VisualFactory::OnAdaptorInitialized()
{
  mAdaptorInitialized = true;
}

void VisualFactory::OnAdaptorTerminated()
{
  mAdaptorInitialized = false;

  if(DALI_UNLIKELY(mIdleCallback))
  {
    OnDiscardCallback();
  }

  if(mFactoryCache)
  {
    mFactoryCache->FinalizeVectorAnimationManager();
  }
}

void VisualFactory::RegisterDiscardCallback()
{
  if(!mAdaptorInitialized)
  {
    // If the adaptor is not initialized, we cannot add idle. Discard visuals immediately.
    OnDiscardCallback();
    return;
  }
  if(!mIdleCallback && Adaptor::IsAvailable())
  {
    // The callback manager takes the ownership of the callback object.
    mIdleCallback = MakeCallback(this, &VisualFactory::OnDiscardCallback);

    Adaptor& adaptor = Adaptor::Get();

    if(DALI_UNLIKELY(!adaptor.AddIdle(mIdleCallback, false)))
    {
      DALI_LOG_ERROR("Fail to add idle callback for visual factory. Call it synchronously.\n");
      OnDiscardCallback();
    }
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
