#ifndef DALI_UI_VISUAL_FACTORY_IMPL_H
#define DALI_UI_VISUAL_FACTORY_IMPL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/precompile-shader-option.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/styling/style-manager.h>

namespace Dali
{
namespace UI
{
namespace Internal
{
class VisualFactoryCache;
class ImageVisualShaderFactory;
class TextVisualShaderFactory;
class ColorVisualShaderFactory;
class NpatchShaderFactory;
class CustomShaderFactory;

/**
 * @copydoc UI::VisualFactory
 */
class VisualFactory : public BaseObject, public ConnectionTracker
{
public:
  /**
   * @brief Constructor
   *
   * @param[in] debugEnabled If true, use debug renderer to replace all the concrete renderer.
   */
  VisualFactory(bool debugEnabled);

  /**
   * @brief StyleChanged callback
   *
   * @param[in] styleManager Handle for style manager.
   * @param[in] type Style change type.
   */
  void OnStyleChangedSignal(UI::StyleManager styleManager, StyleChange::Type type);

  /**
   * @brief BrokenImageChanged callback
   *
   * @param[in] styleManager Handle for style manager.
   */
  void OnBrokenImageChangedSignal(UI::StyleManager styleManager);

  /**
   * @copydoc UI::VisualFactory::CreateVisual( const Property::Map& )
   */
  UI::Visual::Base CreateVisual(const Property::Map& propertyMap);

  /**
   * @copydoc UI::VisualFactory::CreateVisual( const Property::Map&, UI::VisualFactory::CreationOptions )
   */
  UI::Visual::Base CreateVisual(const Property::Map& propertyMap, UI::VisualFactory::CreationOptions creationOptions);

  /**
   * @copydoc UI::VisualFactory::CreateVisual( const std::string&, ImageDimensions )
   */
  UI::Visual::Base CreateVisual(const std::string& image, ImageDimensions size);

  /**
   * @copydoc UI::VisualFactory::CreateVisual( const std::string&, ImageDimensions,
   * UI::VisualFactory::CreationOptions )
   */
  UI::Visual::Base CreateVisual(const std::string& image, ImageDimensions size,
                                UI::VisualFactory::CreationOptions creationOptions);

  /**
   * @copydoc UI::VisualFactory::GetDefaultQuadGeometry()
   */
  Dali::Geometry GetDefaultQuadGeometry();

  /**
   * @copydoc UI::VisualFactory::SetPreMultiplyOnLoad()
   */
  void SetPreMultiplyOnLoad(bool preMultiply);

  /**
   * @copydoc UI::VisualFactory::GetPreMultiplyOnLoad()
   */
  bool GetPreMultiplyOnLoad() const;

  /**
   * @copydoc UI::VisualFactory::SetDefaultCreationOptions( UI::VisualFactory::CreationOptions )
   */
  void SetDefaultCreationOptions(UI::VisualFactory::CreationOptions creationOptions);

  /**
   * @copydoc UI::VisualFactory::GetDefaultCreationOptions()
   */
  UI::VisualFactory::CreationOptions GetDefaultCreationOptions() const;

  /**
   * @copydoc UI::VisualFactory::DiscardVisual()
   */
  void DiscardVisual(UI::Visual::Base visual);

  /**
   * @copydoc UI::VisualFactory::AddPrecompileShader()
   */
  bool AddPrecompileShader(const Property::Map& map);

  /**
   * @copydoc UI::VisualFactory::UsePreCompiledShader()
   */
  void UsePreCompiledShader();

  /**
   * @return the reference to texture manager
   */
  Internal::TextureManager& GetTextureManager();

  /**
   * @return the reference to svg loader
   */
  Internal::SvgLoader& GetSvgLoader();

  /**
   * Get the factory cache, creating it if necessary.
   */
  Internal::VisualFactoryCache& GetFactoryCache();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~VisualFactory() override;

private:
  /**
   * @brief Set the Broken Image url
   * @param[in] styleManager The instance of StyleManager
   */
  void SetBrokenImageUrl(UI::StyleManager& styleManager);

  /**
   * Get the image visual shader factory, creating it if necessary.
   */
  ImageVisualShaderFactory& GetImageVisualShaderFactory();

  /**
   * Get the text visual shader factory, creating it if necessary.
   */
  TextVisualShaderFactory& GetTextVisualShaderFactory();

  /**
   * Get the color visual shader factory, creating it if necessary.
   */
  ColorVisualShaderFactory& GetColorVisualShaderFactory();

  /**
   * Get the npatch shader factory, creating it if necessary.
   */
  NpatchShaderFactory& GetNpatchShaderFactory();

  /**
   * Get the custom shader factory, creating it if necessary.
   */
  CustomShaderFactory& GetCustomShaderFactory();

  /**
   * @brief Add precompiled shader
   * @param[in] shaderOption The option of precompiled shader
   *
   * @return True if add success, false otherwise.
   */
  bool AddPrecompileShader(PrecompileShaderOption& shaderOption);

  /**
   * @brief Callbacks called for clear discarded visuals.
   */
  void OnDiscardCallback();

  /**
   * @brief Register idle callback for discard visuals if need.
   */
  void RegisterDiscardCallback();

  /**
   * @brief Callbacks called when adaptor is initialized.
   */
  void OnAdaptorInitialized();

  /**
   * @brief Callbacks called when adaptor is terminated.
   */
  void OnAdaptorTerminated();

  VisualFactory(const VisualFactory&) = delete;

  VisualFactory& operator=(const VisualFactory& rhs) = delete;

private:
  std::unique_ptr<VisualFactoryCache> mFactoryCache;
  std::unique_ptr<ImageVisualShaderFactory> mImageVisualShaderFactory;
  std::unique_ptr<TextVisualShaderFactory> mTextVisualShaderFactory;
  std::unique_ptr<ColorVisualShaderFactory> mColorVisualShaderFactory;
  std::unique_ptr<NpatchShaderFactory> mNpatchShaderFactory;
  std::unique_ptr<CustomShaderFactory> mCustomShaderFactory;
  SlotDelegate<VisualFactory> mSlotDelegate;
  CallbackBase* mIdleCallback;
  using DiscardedVisualContainer = std::vector<UI::Visual::Base>;
  DiscardedVisualContainer mDiscardedVisuals{};

  UI::VisualFactory::CreationOptions mDefaultCreationOptions : 2;

  bool mAdaptorInitialized : 1;
  bool mDebugEnabled : 1;
  bool mPreMultiplyOnLoad : 1; ///< Local store for this flag
  bool mPrecompiledShaderRequested : 1;
};

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType0 The type of first argument passed to the CreateVisual()
 * @tparam ParameterType1 The type of second argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the visual will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param0 First template based argument passed to the visual factory
 * @param[in] param1 Second template based argument passed to the visual factory
 */
template <class ParameterType0, class ParameterType1>
void InitializeVisual(Actor& actor, UI::Visual::Base& visual, ParameterType0& param0, ParameterType1& param1)
{
  if (actor)
  {
    UI::GetImplementation(visual).SetOffScene(actor);
  }
  visual = UI::VisualFactory::Get().CreateVisual(param0, param1);
  if (visual && actor && actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    UI::GetImplementation(visual).SetOnScene(actor);
  }
}

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType The type of argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the visual will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param Template based argument passed to the visual factory
 */
template <class ParameterType>
void InitializeVisual(Actor& actor, UI::Visual::Base& visual, ParameterType& param)
{
  if (actor && visual)
  {
    UI::GetImplementation(visual).SetOffScene(actor);
  }
  visual = UI::VisualFactory::Get().CreateVisual(param);
  if (visual && actor && actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    UI::GetImplementation(visual).SetOnScene(actor);
  }
}

} // namespace Internal

inline const Internal::VisualFactory& GetImplementation(const UI::VisualFactory& factory)
{
  DALI_ASSERT_ALWAYS(factory && "VisualFactory handle is empty");

  const BaseObject& handle = factory.GetBaseObject();

  return static_cast<const Internal::VisualFactory&>(handle);
}

inline Internal::VisualFactory& GetImplementation(UI::VisualFactory& factory)
{
  DALI_ASSERT_ALWAYS(factory && "VisualFactory handle is empty");

  BaseObject& handle = factory.GetBaseObject();

  return static_cast<Internal::VisualFactory&>(handle);
}

} // namespace UI

} // namespace Dali

#endif /* DALI_UI_VISUAL_FACTORY_IMPL_H */
