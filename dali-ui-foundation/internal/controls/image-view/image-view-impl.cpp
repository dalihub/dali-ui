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
#include "image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/controls/image-view/image-view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
namespace
{
constexpr float FULL_OPACITY = 1.0f;
constexpr float LOW_OPACITY = 0.2f;

constexpr int PLACEHOLDER_DEPTH_INDEX = -2;
constexpr int PREVIOUS_VISUAL_DEPTH_INDEX = -1;
constexpr int CURRENT_VISUAL_DEPTH_INDEX = 0;

BaseHandle Create()
{
  return UI::ImageView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(UI::ImageView, UI::Control, Create);
DALI_PROPERTY_REGISTRATION(UI, ImageView, "image", MAP, IMAGE)
DALI_PROPERTY_REGISTRATION(UI, ImageView, "preMultipliedAlpha", BOOLEAN, PRE_MULTIPLIED_ALPHA)
DALI_PROPERTY_REGISTRATION(UI, ImageView, "placeholderImage", STRING, PLACEHOLDER_IMAGE)
DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT(UI, ImageView, "pixelArea", Vector4(0.f, 0.f, 1.f, 1.f), PIXEL_AREA)
DALI_TYPE_REGISTRATION_END()

/**
 * @brief Discard the given visual into VisualFactory. The visual will be destroyed at next idle time.
 *
 * @param[in,out] visual Visual to be discarded. It will be reset to an empty handle.
 */
void DiscardImageViewVisual(Dali::UI::Visual::Base& visual)
{
  if (DALI_LIKELY(Dali::Adaptor::IsAvailable() && visual))
  {
    Dali::UI::VisualFactory::Get().DiscardVisual(visual);
  }
  visual.Reset();
}

} // anonymous namespace

using namespace Dali;

ImageView::ImageView(ControlBehaviour additionalBehaviour)
  : Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT | additionalBehaviour)),
    mImageSize(),
    mImageReplaced(false)
{
}

ImageView::~ImageView()
{
  DiscardImageViewVisual(mVisual);
  DiscardImageViewVisual(mPreviousVisual);
  DiscardImageViewVisual(mPlaceholderVisual);
}

UI::ImageView ImageView::New(ControlBehaviour additionalBehaviour)
{
  ImageView* impl = new ImageView(additionalBehaviour);

  UI::ImageView handle = UI::ImageView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////

void ImageView::OnInitialize()
{
  // ImageView can relayout in the OnImageReady, alternative to a signal would be to have a upcall from the Control to
  // ImageView
  Dali::UI::Control handle(GetOwner());
  handle.ResourceReadySignal().Connect(this, &ImageView::OnResourceReady);

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::IMAGE);
}

void ImageView::SetImage(const Property::Map& map)
{
  if (mVisual)
  {
    DiscardImageViewVisual(mPreviousVisual);
    mPreviousVisual = mVisual;
  }

  // Comparing a property map is too expensive so just creating a new visual
  mPropertyMap = map;
  mUrl.clear();

  mImageReplaced = true;

  if (!mVisual)
  {
    ShowPlaceholderImage();
  }

  UI::Visual::Base visual = UI::VisualFactory::Get().CreateVisual(mPropertyMap);
  if (visual)
  {
    Internal::Visual::Base& visualImpl = UI::GetImplementation(visual);
    if (visualImpl.GetFittingMode() == Visual::FittingMode::DONT_CARE)
    {
      visualImpl.SetFittingMode(Visual::FittingMode::FILL);
    }

    // Don't set mVisual until it is ready and shown. Getters will still use current visual.
    if (!mVisual)
    {
      mVisual = visual;
    }

    if (!mShaderMap.Empty())
    {
      visualImpl.SetCustomShader(mShaderMap);
    }

    DevelControl::RegisterVisual(*this, UI::ImageView::Property::IMAGE, visual, DepthIndex::CONTENT);

    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
    controlDataImpl.EnableCornerPropertiesOverridden(visual, true);
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual(*this, UI::ImageView::Property::IMAGE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }
  // Signal that a Relayout may be needed
}

void ImageView::SetImage(const std::string& url, ImageDimensions size)
{
  if (mVisual)
  {
    DiscardImageViewVisual(mPreviousVisual);
    mPreviousVisual = mVisual;
  }

  // Don't bother comparing if we had a visual previously, just drop old visual and create new one
  mUrl = url;
  mImageSize = size;
  mPropertyMap.Clear();

  mImageReplaced = true;

  if (!mVisual)
  {
    ShowPlaceholderImage();
  }

  // Don't set mVisual until it is ready and shown. Getters will still use current visual.
  UI::Visual::Base visual = UI::VisualFactory::Get().CreateVisual(url, size);
  if (visual)
  {
    Internal::Visual::Base& visualImpl = UI::GetImplementation(visual);
    if (visualImpl.GetFittingMode() == Visual::FittingMode::DONT_CARE)
    {
      visualImpl.SetFittingMode(Visual::FittingMode::FILL);
    }

    // Don't set mVisual until it is ready and shown. Getters will still use current visual.
    if (!mVisual)
    {
      mVisual = visual;
    }

    if (!mShaderMap.Empty())
    {
      visualImpl.SetCustomShader(mShaderMap);
    }

    DevelControl::RegisterVisual(*this, UI::ImageView::Property::IMAGE, visual, DepthIndex::CONTENT);

    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
    controlDataImpl.EnableCornerPropertiesOverridden(visual, true);
  }
  else
  {
    // Unregister the exsiting visual
    DevelControl::UnregisterVisual(*this, UI::ImageView::Property::IMAGE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    RelayoutRequest();
  }
  // Signal that a Relayout may be needed
}

void ImageView::ClearImageVisual()
{
  // Clear cached properties
  mPropertyMap.Clear();
  mUrl.clear();

  DiscardImageViewVisual(mVisual);

  // Unregister the exsiting visual
  DevelControl::UnregisterVisual(*this, UI::ImageView::Property::IMAGE);

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  RelayoutRequest();
}

void ImageView::EnablePreMultipliedAlpha(bool preMultipled)
{
  if (mVisual)
  {
    UI::GetImplementation(mVisual).EnablePreMultipliedAlpha(preMultipled);
  }
}

bool ImageView::IsPreMultipliedAlphaEnabled() const
{
  if (mVisual)
  {
    return UI::GetImplementation(mVisual).IsPreMultipliedAlphaEnabled();
  }
  return false;
}

void ImageView::SetDepthIndex(int depthIndex)
{
  if (mVisual)
  {
    mVisual.SetDepthIndex(depthIndex);
  }
}

void ImageView::SetPlaceholderUrl(const std::string& url)
{
  mPlaceholderUrl = url;
  if (!url.empty())
  {
    DiscardImageViewVisual(mPlaceholderVisual);
    CreatePlaceholderImage();
  }
  else
  {
    // Clear current placeholder image
    UI::Visual::Base visual = DevelControl::GetVisual(*this, UI::ImageView::Property::PLACEHOLDER_IMAGE);
    if (visual)
    {
      DevelControl::UnregisterVisual(*this, UI::ImageView::Property::PLACEHOLDER_IMAGE);
    }

    DiscardImageViewVisual(mPlaceholderVisual);
    mPlaceholderUrl = url;
  }
}

std::string ImageView::GetPlaceholderUrl() const
{
  return mPlaceholderUrl;
}

Vector3 ImageView::GetNaturalSize()
{
  if (mVisual)
  {
    Vector2 rendererNaturalSize;
    mVisual.GetNaturalSize(rendererNaturalSize);

    Extents padding;
    padding = Self().GetProperty<Extents>(UI::Control::Property::PADDING);

    rendererNaturalSize.width += (padding.start + padding.end);
    rendererNaturalSize.height += (padding.top + padding.bottom);
    return Vector3(rendererNaturalSize);
  }

  // if no visual then use Control's natural size
  return Control::GetNaturalSize();
}

float ImageView::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(UI::Control::Property::PADDING);

  if (mVisual)
  {
    return mVisual.GetHeightForWidth(width) + padding.top + padding.bottom;
  }
  else
  {
    return Control::GetHeightForWidth(width) + padding.top + padding.bottom;
  }
}

float ImageView::GetWidthForHeight(float height)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(UI::Control::Property::PADDING);

  if (mVisual)
  {
    return mVisual.GetWidthForHeight(height) + padding.start + padding.end;
  }
  else
  {
    return Control::GetWidthForHeight(height) + padding.start + padding.end;
  }
}

void ImageView::OnUpdateVisualProperties(
    const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
{
  UI::Visual::Base visual = DevelControl::GetVisual(*this, UI::ImageView::Property::IMAGE);
  if (visual)
  {
    Dali::UI::Control handle(GetOwner());

    for (auto&& data : properties)
    {
      if (data.first == UI::ImageView::Property::IMAGE)
      {
        DevelControl::DoAction(handle, UI::ImageView::Property::IMAGE, DevelVisual::Action::UPDATE_PROPERTY,
                               data.second);
        break;
      }
    }
  }
}

void ImageView::OnResourceReady(UI::Control control)
{
  // In case of placeholder, we need to skip this call.
  // TODO: In case of placeholder, it needs to be modified not to call OnResourceReady()
  if (control.GetVisualResourceStatus(UI::ImageView::Property::IMAGE) != UI::Visual::ResourceStatus::READY)
  {
    return;
  }

  // Visual ready so update visual attached to this ImageView, following call to RelayoutRequest will use this visual.
  auto currentVisual = DevelControl::GetVisual(*this, UI::ImageView::Property::IMAGE);
  if (mVisual != currentVisual)
  {
    // If the current visual is not the same as the previous holded visual, then we need to discard old one.
    DiscardImageViewVisual(mVisual);
  }
  mVisual = currentVisual;

  // Signal that a Relayout may be needed
}

void ImageView::CreatePlaceholderImage()
{
  Property::Map propertyMap;
  propertyMap.Insert(UI::Visual::Property::TYPE, UI::Visual::IMAGE);
  propertyMap.Insert(UI::ImageVisual::Property::URL, mPlaceholderUrl);
  // propertyMap.Insert(UI::ImageVisual::Property::LOAD_POLICY, UI::ImageVisual::LoadPolicy::IMMEDIATE); //
  // TODO: need to enable this property
  propertyMap.Insert(UI::ImageVisual::Property::RELEASE_POLICY, UI::ImageVisual::ReleasePolicy::DESTROYED);
  propertyMap.Insert(UI::DevelImageVisual::Property::ENABLE_BROKEN_IMAGE, false);
  mPlaceholderVisual = UI::VisualFactory::Get().CreateVisual(propertyMap);
  if (mPlaceholderVisual)
  {
    mPlaceholderVisual.SetName("placeholder");
    mPlaceholderVisual.SetDepthIndex(mPlaceholderVisual.GetDepthIndex() + PLACEHOLDER_DEPTH_INDEX);
  }
  else
  {
    DevelControl::UnregisterVisual(*this, UI::ImageView::Property::PLACEHOLDER_IMAGE);
    DiscardImageViewVisual(mPlaceholderVisual);
  }
}

void ImageView::ShowPlaceholderImage()
{
  if (mPlaceholderVisual)
  {
    DevelControl::RegisterVisual(*this, UI::ImageView::Property::PLACEHOLDER_IMAGE, mPlaceholderVisual, false);

    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
    controlDataImpl.EnableCornerPropertiesOverridden(mPlaceholderVisual, true);

    Actor self = Self();
    UI::GetImplementation(mPlaceholderVisual).SetOnScene(self);
  }
}

void ImageView::HidePlaceholderImage()
{
  if (mPlaceholderVisual)
  {
    DevelControl::UnregisterVisual(*this, UI::ImageView::Property::PLACEHOLDER_IMAGE);

    // Hide placeholder
    Actor self = Self();
    UI::GetImplementation(mPlaceholderVisual).SetOffScene(self);
  }
}

///////////////////////////////////////////////////////////
//
// Properties
//

void ImageView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  UI::ImageView imageView = UI::ImageView::DownCast(Dali::BaseHandle(object));

  if (imageView)
  {
    ImageView& impl = GetImpl(imageView);
    switch (index)
    {
      case UI::ImageView::Property::IMAGE:
      {
        std::string imageUrl;
        const Property::Map* map;
        if (value.Get(imageUrl))
        {
          impl.SetImage(imageUrl, ImageDimensions());
        }
        // if its not a string then get a Property::Map from the property if possible.
        else
        {
          map = value.GetMap();
          if (DALI_LIKELY(map))
          {
            // the property map is emtpy map. Unregister visual.
            if (DALI_UNLIKELY(map->Count() == 0u))
            {
              impl.ClearImageVisual();
            }
            else
            {
              Property::Value* shaderValue = map->Find(UI::Visual::Property::SHADER, CUSTOM_SHADER);
              // set image only if property map contains image information other than custom shader
              if (map->Count() > 1u || !shaderValue)
              {
                impl.SetImage(*map);
              }
              // the property map contains only the custom shader
              else if ((map->Count() == 1u) && (shaderValue))
              {
                Property::Map* shaderMap = shaderValue->GetMap();
                if (shaderMap)
                {
                  impl.mShaderMap = *shaderMap;

                  if (!impl.mUrl.empty())
                  {
                    impl.SetImage(impl.mUrl, impl.mImageSize);
                  }
                  else if (!impl.mPropertyMap.Empty())
                  {
                    impl.SetImage(impl.mPropertyMap);
                  }
                }
              }
            }
          }
          else
          {
            // invalid property value comes. Unregister visual.
            impl.ClearImageVisual();
          }
        }
        break;
      }

      case UI::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        bool isPre;
        if (value.Get(isPre))
        {
          impl.EnablePreMultipliedAlpha(isPre);
        }
        break;
      }

      case UI::ImageView::Property::PLACEHOLDER_IMAGE:
      {
        std::string placeholderUrl;
        if (value.Get(placeholderUrl))
        {
          impl.SetPlaceholderUrl(placeholderUrl);
        }
        break;
      }
    }
  }
}

Property::Value ImageView::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  UI::ImageView imageview = UI::ImageView::DownCast(Dali::BaseHandle(object));

  if (imageview)
  {
    ImageView& impl = GetImpl(imageview);
    switch (propertyIndex)
    {
      case UI::ImageView::Property::IMAGE:
      {
        if (!impl.mUrl.empty())
        {
          value = impl.mUrl;
        }
        else
        {
          Property::Map map;
          UI::Visual::Base visual = DevelControl::GetVisual(impl, UI::ImageView::Property::IMAGE);
          if (visual)
          {
            visual.CreatePropertyMap(map);
          }
          value = map;
        }
        break;
      }

      case UI::ImageView::Property::PRE_MULTIPLIED_ALPHA:
      {
        value = impl.IsPreMultipliedAlphaEnabled();
        break;
      }

      case UI::ImageView::Property::PLACEHOLDER_IMAGE:
      {
        value = impl.GetPlaceholderUrl();
        break;
      }
    }
  }

  return value;
}

} // namespace Internal
} // namespace UI
} // namespace Dali
