#ifndef DALI_UI_INTERNAL_VISUAL_BASE_IMPL_H
#define DALI_UI_INTERNAL_VISUAL_BASE_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

#include <unordered_set>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/direction-enums.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/visuals/visual-constraint-observer.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Visual
{
class EventObserver;

/**
 * Base class for all Control rendering logic. A control may have multiple visuals.
 *
 * Note: The visual responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 *
 * The following properties are optional
 *
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | customShader            | MAP              |
 *
 * where custom-shader is a map with the following properties:
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | vertexShader            | STRING           |
 * | fragmentShader          | STRING           |
 * | subdivideGridX          | INT              |
 * | subdivideGridY          | INT              |
 * | shaderHints             | INT              |
 * | name                    | STRING           |
 */
class Base : public BaseObject
{
public:
  /**
   * Setting the properties of the visual, this API should only called by the VisualFactory
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  void SetProperties(const Property::Map& propertyMap);

  /**
   * @copydoc Ui::Visual::Base::SetName
   */
  void SetName(const std::string& name);

  /**
   * @copydoc Ui::Visual::Base::GetName
   */
  const std::string& GetName() const;

  /**
   * @copydoc Ui::Visual::Base::SetSize
   */
  void SetTransformAndSize(const Property::Map& transform, Size controlSize);

  /**
   * @copydoc Ui::Visual::Base::GetHeightForWidth
   */
  virtual float GetHeightForWidth(float width);

  /**
   * @copydoc Ui::Visual::Base::GetWidthForHeight
   */
  virtual float GetWidthForHeight(float height);

  /**
   * @copydoc Ui::Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize(Vector2& naturalSize);

  /**
   * @copydoc Ui::Visual::Base::SetDepthIndex
   */
  void SetDepthIndex(int index);

  /**
   * @copydoc Ui::Visual::Base::GetDepthIndex
   */
  int GetDepthIndex() const;

  /**
   * @copydoc Ui::Visual::Base::SetOnScene
   * @pre Impl->mGeometry must be created before this method is called
   */
  void SetOnScene(Actor& actor);

  /**
   * @copydoc Ui::Visual::Base::SetOffScene
   */
  void SetOffScene(Actor& actor);

  /**
   * @copydoc Ui::Visual::Base::CreatePropertyMap
   */
  void CreatePropertyMap(Property::Map& map) const;

  /**
   * @copydoc Ui::Visual::Base::DoAction
   */
  void DoAction(const Dali::Property::Index actionId, const Dali::Property::Value& attributes);

  /**
   * @copydoc Ui::Visual::Base::DoActionExtension
   */
  void DoActionExtension(const Dali::Property::Index actionId, const Dali::Any& attributes);

  /**
   * @brief Gets the on scene state for this Visual
   *
   * @return Returns true if this Visual is on the scene, false if it is off the scene
   */
  bool IsOnScene() const;

  /**
   * @brief Create a property map containing per-instance visual properties.
   *
   * This will enable creation of new visuals on control state change with
   * any alternative style properties and the relevant instance properties
   * (e.g. for image visual, the desired size, and for text visual, the actual text).
   * @param[in] map The property map into which to write
   */
  void CreateInstancePropertyMap(Property::Map& map) const;

  /**
   * @brief Query whether alpha is pre-multiplied.
   *
   * @return True is alpha is pre-multiplied, false otherwise.
   */
  bool IsPreMultipliedAlphaEnabled() const;

  /**
   * @brief Sets properties of custom shader
   * @param[in] propertyMap Property map containing the custom shader data
   */
  void SetCustomShader(const Property::Map& propertyMap);

  /**
   * @brief Sets properties of custom shader
   * @param[in] propertyArray Property array containing multiple custom shader data
   */
  void SetCustomShader(const Property::Array& propertyArray);

  /**
   * @brief Retrieves whether the visual uses Custom shader or not.
   * @return True if the visual uses Custom Shader.
   */
  bool IsUsingCustomShader() const;

  /**
   * @copydoc Ui::Visual::Base::SetProperty
   */
  void SetProperty(Dali::Property::Index index, const Dali::Property::Value& propertyValue);

  /**
   * @copydoc Ui::Visual::Base::GetProperty
   */
  Dali::Property::Value GetProperty(Dali::Property::Index index);

  /**
   * Gets currently staged renderer, or an empty handle if not staged
   */
  VisualRenderer GetRenderer() const;

  /**
   * Convert all string keys to int keys
   * @param[in] key The key to convert
   * @return the index key supplied or matching, or INVALID_INDEX if no match
   */
  static Property::Index GetIntKey(Property::Key key);

  /**
   * Sets the mix color ( including opacity )  of the visual.
   * @param[in] mixColor The new mix color
   */
  void SetMixColor(const Vector4& color);

  /**
   * Sets the mix color of the visual.
   * @param[in] mixColor The new mix color
   */
  void SetMixColor(const Vector3& color);

  /**
   * @brief Add an observer to watch for when the Visuals have events to notify
   * Currently only supports a single observer
   */
  void AddEventObserver(Visual::EventObserver& observer);

  /**
   * @brief Remove an observer
   */
  void RemoveEventObserver(Visual::EventObserver& observer);

  /**
   * @brief Called when the visuals resources are loaded / ready
   */
  void ResourceReady(Ui::Visual::ResourceStatus resourceStatus);

  /**
   * @brief Called when the visuals resources are loaded / ready or failed (mean, broken image ready)
   * @return true if ready or failed (mean, broken image ready), false otherwise
   */
  virtual bool IsResourceReady() const;

  /**
   * @brief Get the loading state of the visual resource
   * @return Return the loading status (PREPARING, READY and FAILED) of visual resource
   */
  Ui::Visual::ResourceStatus GetResourceStatus() const;

  /**
   * @brief Change the fitting mode to this visual.
   *
   * Default visuals do nothing for this API.
   * Subclasses that support FittingMode (e.g. ImageVisual) override this to apply
   * the fitting mode variables.
   *
   * @param[in] fittingMode The fitting mode.
   */
  virtual void SetFittingMode(Ui::Image::FittingMode fittingMode)
  {
  }

  /**
   * @brief Applies the fitting mode transform to this visual.
   *
   * Default implementation calls SetViewSize for non-TEXT visuals.
   * Subclasses that support FittingMode (e.g. ImageVisual) override this to apply
   * the fitting algorithm using their stored mFittingMode.
   *
   * @param[in] controlSize The size of the parent control.
   * @param[in] viewPadding     The padding of the parent control (start, end, top, bottom).
   *                        For RTL layouts, start/end should already be swapped by the caller.
   */
  virtual void ApplyFittingMode(const Vector2& controlSize, const Extents& viewPadding);

  /**
   * @brief Query whether the fittingMode is ignored.
   *
   * @return Returns true if the fittingMode is ignored, false otherwise.
   */
  bool IsIgnoreFittingMode() const;

  /**
   * @brief Query whether the pixel area is set by fitting mode.
   */
  bool IsPixelAreaSetForFittingMode() const;

  /**
   * @brief Set the pixel area of the visual.
   *
   * @param[in] pixelArea The pixel area of the visual.
   */
  void SetPixelAreaForFittingMode(const Vector4& pixelArea);

  /**
   * @brief Checks whether the trasformMap is being used in the FittingMode situation.
   */
  bool IsTransformMapSetForFittingMode() const;

  /**
   * @brief Set the flag to use transformMap in the FittingMode.
   */
  void SetTransformMapUsageForFittingMode(bool used);

  /**
   * @brief Sets the control size
   * It will call SetTransformAndSize with empty transform map.
   *
   * @param[in] controlSize The size of the parent control for visuals that need to scale internally.
   */
  void SetControlSize(Size controlSize);

  /**
   * @brief Sets the view size
   * It will call SetTransformAndSize with empty transform map.
   *
   * @param[in] viewSize The size of the parent view for visuals that need to scale internally.
   */
  void SetViewSize(Size viewSize);

  /**
   * @brief Get the actual Visual Object.
   * @return The actual visual object
   * @note Should be overridden by deriving controls if they are acting as a proxy to other visual objects.
   */
  virtual Base& GetVisualObject();

  /**
   * @brief Query whether resources requires to be loaded synchronously.
   * @return Returns true if synchronous resource loading is required, false otherwise.
   */
  bool IsSynchronousLoadingRequired() const;

  /**
   * @brief Get the type of this visual.
   *
   * @return The the type of this visual.
   */
  Ui::Visual::Type GetType() const;

  /**
   * @brief Retrieve the property object associated with the property key.
   *
   * @param[in] key The Property key of the visual.
   * @return The Property object
   */
  Dali::Property GetPropertyObject(Dali::Property::Key key, bool changeProperties = true);

  /**
   * @brief Query whether this visual could be rendered in control's offscreen-rendering result
   *
   * @return True if this visual rendered at offscreen-rendering render task.
   *         False if this visual will be rendered at on-screen even if control set offscreen rendering type.
   */
  bool IsOffscreenRenderingCaptureEnabled() const;

  /**
   * @brief Query whether this visual could not apply control's corner radius during control's offscreen-rendering
   * result
   *
   * @return True if this visual ignore corner radius rendered at offscreen-rendering render task. False otherwise.
   */
  bool IsCornerRadiusIgnoredAtOffscreenRendering() const;

  /**
   * @brief Set whether this visual could not apply control's corner radius during control's offscreen-rendering result
   *
   * @param[in] ignored True if this visual ignore corner radius rendered at offscreen-rendering render task. False
   * otherwise.
   */
  void CornerRadiusIgnoredAtOffscreenRendering(bool ignored);

  /**
   * @brief Add an observer to watch for when the Visuals have constraint to notify
   * Currently only supports a single observer
   */
  void AddConstraintObserver(Visual::ConstraintObserver& observer);

  /**
   * @brief Remove an observer
   */
  void RemoveConstraintObserver(Visual::ConstraintObserver& observer);

  /**
   * @brief Add new constraint that this visual will control apply rate.
   */
  void AddConstraintFeature(Dali::Constraint constraint, std::unordered_set<Property::Index> properties);

  /**
   * @brief Remove constraint feature by constraint handle.
   */
  void RemoveConstraintFeature(Dali::Constraint constraint);

  /**
   * @brief Remove constraint feature by property index.
   */
  void RemoveConstraintFeatureByIndex(Property::Index index);

  /**
   * @brief Notify to visuals who has constraints that given ConstraintObserver's property updated
   */
  void UpdateApplyRate(Property::Index updatedProperty);

  /**
   * @brief Apply constraints by specific property index
   */
  void StartConstraintFeature(Property::Index index);

  /**
   * @brief Remove constraints by specific property index
   */
  void StopConstraintFeature(Property::Index index);

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] type The type of the this visual
   */
  Base(VisualFactoryCache& factoryCache, Ui::Visual::Type type);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~Base() override;

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

protected:
  /**
   * @brief Called by Initialize() allowing sub classes to initialize them.
   */
  virtual void OnInitialize() = 0;

  /**
   * @brief Called by CreatePropertyMap() allowing sub classes to respond to the CreatePropertyMap event
   *
   * @param[out] map The visual property map.
   */
  virtual void DoCreatePropertyMap(Property::Map& map) const = 0;

  /**
   * @brief Called by CreateInstancePropertyMap() allowing derived
   * classes to store instanced data (separate to styled data) that
   * needs copying between visuals on state change.
   *
   * @param[out] map The visual property map
   */
  virtual void DoCreateInstancePropertyMap(Property::Map& map) const = 0;

  /**
   * @brief Called by SetProperties() allowing sub classes to set their properties
   *
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  virtual void DoSetProperties(const Property::Map& propertyMap) = 0;

  /**
   * @brief Called when transform or control size changes
   * ( Of use to SVG and Text visuals )
   */
  virtual void OnSetTransform() = 0;

  /**
   * @brief Called by SetOnScene() allowing sub classes to respond to the SetOnScene event
   *
   * @note The derived class is required to create the renderer, and add it to the actor when all the resources are in
   * place.
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOnScene(Actor& actor) = 0;

  /**
   * @brief Called by SetOffScene() allowing sub classes to respond to the SetOffScene event
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOffScene(Actor& actor);

  /**
   * @brief Called by DoAction() allowing sub classes to do the given action.
   *
   * @param[in] actionId The action to perform
   * @param[in] attributes The list of attributes for the action. ( optional for this data structure to have content )
   */
  virtual void OnDoAction(const Property::Index actionId, const Property::Value& attributes);

  /**
   * @brief Called by DoActionExtension() allowing sub classes to do the given action.
   *
   * @param[in] actionId The action to perform
   * @param[in] attributes The list of attributes for the action. ( optional for this data structure to have content )
   */
  virtual void OnDoActionExtension(const Property::Index actionId, const Dali::Any& attributes);

  /**
   * @brief Update the shader when some properties are changed.
   */
  virtual void UpdateShader()
  {
  }

  /**
   * @brief Generate a shader by the current properties from each sub classes's own shader-generate logic.
   * @return If the function defined, Generated shader by the current properties. Else, empty shader.
   * @todo This function should be removed after refactoring the shader generation logic.
   */
  virtual Dali::Shader GenerateShader() const
  {
    return Dali::Shader();
  }

  /**
   * @brief Called by GetPropertyObject() allowing sub classes to respond to the GetPropertyObject event
   * @note The derived class is required to register the given property.
   * @param[in] key The key of the visual's property.
   * @return The Property object
   */
  virtual Dali::Property OnGetPropertyObject(Dali::Property::Key key, bool changeProperties)
  {
    Handle handle;
    return Dali::Property(handle, Property::INVALID_INDEX);
  }

protected:
  /**
   * @brief Shared fitting mode algorithm used by subclasses that support FittingMode.
   *
   * Computes the transform map for the given control size, padding, and fitting mode,
   * then calls SetTransformAndSize. Intended to be called from ApplyFittingMode overrides.
   *
   * @param[in] controlSize The size of the parent control.
   * @param[in] viewPadding     The padding of the parent control. For RTL, start/end already swapped.
   * @param[in] fittingMode The fitting mode to apply.
   */
  void DoApplyFittingMode(const Vector2& controlSize, const Extents& viewPadding, Ui::Image::FittingMode fittingMode);

  /**
   * @brief Query whether the corners of the visual requires to be rounded.
   *
   * @return Returns true if the rounded corner is required, false otherwise.
   */
  bool IsRoundedCornerRequired() const;

  /**
   * @brief Query whether the corners of the visual requires to be squircle.
   *
   * @return Returns true if the rounded corner is required, false otherwise.
   */
  bool IsSquircleCornerRequired() const;

  /**
   * @brief Query whether the borderline of the visual requires to be rendered.
   *
   * @return Returns true if the outline is required, false otherwise.
   */
  bool IsBorderlineRequired() const;

private:
  /**
   * Register the mix color uniform on the Renderer and store the property index.
   * Note, this is not used by Color or Primitive Visuals, which will use their
   * own property index.
   */
  void RegisterMixColor();

  /**
   * Register the uniform on the DecoratedVisualRenderer and store the property index if necessary.
   */
  void RegisterDecoration();

  /**
   * Find the matching property on the renderer or shader. If it's a shader
   * property, register it on the renderer in order to animate it for this
   * visual independently.
   * @param[in] key The key to match.
   * @return the matching index, or INVALID_INDEX if it's not found
   */
  Property::Index GetPropertyIndex(Property::Key key) const;

  // Undefined
  Base(const Visual::Base& visual);

  // Undefined
  Base& operator=(const Visual::Base& visual);

protected:
  struct Impl;
  Impl*               mImpl;
  VisualFactoryCache& mFactoryCache;
};

typedef IntrusivePtr<Base> BasePtr;

} // namespace Visual

} // namespace Internal

inline const Internal::Visual::Base& GetImplementation(const Ui::Visual::Base& visualBase)
{
  DALI_ASSERT_ALWAYS(visualBase && "visual base handle is empty");

  const BaseObject& handle = visualBase.GetBaseObject();

  return static_cast<const Internal::Visual::Base&>(handle);
}

inline Internal::Visual::Base& GetImplementation(Ui::Visual::Base& visualBase)
{
  DALI_ASSERT_ALWAYS(visualBase && "visual base handle is empty");

  BaseObject& handle = visualBase.GetBaseObject();

  return static_cast<Internal::Visual::Base&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_VISUAL_BASE_IMPL_H
