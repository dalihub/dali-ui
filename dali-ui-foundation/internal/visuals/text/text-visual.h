#ifndef DALI_UI_INTERNAL_TEXT_VISUAL_H
#define DALI_UI_INTERNAL_TEXT_VISUAL_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/async-text-interface.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-manager.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-gradient-data.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-reveal-data.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual-shader-factory.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class TextVisual;
typedef IntrusivePtr<TextVisual> TextVisualPtr;

/**
 * The visual which renders text
 *
 * The following properties are optional:
 *
 * | %Property Name      | Type    |
 * |---------------------|---------|
 * | text                | STRING  |
 * | fontFamily          | STRING  |
 * | fontStyle           | STRING  |
 * | pointSize           | FLOAT   |
 * | multiLine           | BOOLEAN |
 * | horizontalAlignment | STRING  |
 * | verticalAlignment   | STRING  |
 * | textColor           | VECTOR4 |
 * | shadow              | STRING  |
 * | underline           | STRING  |
 * | outline             | STRING  |
 * | lineThrough         | STRING  |
 * | background          | STRING  |
 *
 */
class TextVisual : public Visual::Base, public TextLoadObserver
{
public:
  /**
   * @brief Create a new text visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] shaderFactory The TextVisualShaderFactory object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static TextVisualPtr New(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory,
                           const Property::Map& properties);

  /**
   * @brief Retrieve the text's controller.
   * @param[in] visual The text visual.
   * @return The text controller
   */
  static Ui::Text::ControllerPtr GetController(Ui::Integration::Visual::Base visual)
  {
    return GetVisualObject(visual).mController;
  };

  /**
   * @brief Set the index of the animatable text color property.
   * @param[in] visual The text visual.
   * @param[in] animatablePropertyIndex The index of the animatable property
   */
  static void SetAnimatableTextColorProperty(Ui::Integration::Visual::Base visual, Property::Index animatablePropertyIndex)
  {
    GetVisualObject(visual).mAnimatableTextColorPropertyIndex = animatablePropertyIndex;
  };

  /**
   * @brief Sets the TextGradient start-offset source property index registered by the control.
   *
   * @param[in] visual The text visual.
   * @param[in] startOffsetPropertyIndex The source property index for uTextGradientStartOffset.
   */
  static void SetGradientAnimProperties(Ui::Integration::Visual::Base visual,
                                        Property::Index               startOffsetPropertyIndex)
  {
    GetVisualObject(visual).SetGradientAnimProperties(startOffsetPropertyIndex);
  };

  /**
   * @brief Sets the TextGradientOverlay start-offset source property index registered by the control.
   *
   * @param[in] visual The text visual.
   * @param[in] startOffsetPropertyIndex The source property index for uTextGradientOverlayStartOffset.
   */
  static void SetGradientOverlayAnimProperties(Ui::Integration::Visual::Base visual,
                                               Property::Index               startOffsetPropertyIndex)
  {
    GetVisualObject(visual).SetGradientOverlayAnimProperties(startOffsetPropertyIndex);
  };

  /**
   * @brief Configures finite reveal for synchronous and asynchronous Label paths.
   *
   * @param[in] visual The text visual to configure.
   * @param[in] unit The internal reveal unit, or DISABLED to remove reveal rendering.
   * @param[in] fadeDurationRatio The authored automatic sentinel or normalized fade duration.
   * @param[in] progressPropertyIndex The stable Label scene property used as progress.
   * @param[in] revision The revision used to reject stale asynchronous results.
   */
  static void ConfigureTextReveal(
    Ui::Integration::Visual::Base    visual,
    Ui::Text::Internal::Reveal::Unit unit,
    float                            fadeDurationRatio,
    Property::Index                  progressPropertyIndex,
    uint64_t                         revision)
  {
    GetVisualObject(visual).ConfigureTextReveal(unit, fadeDurationRatio, progressPropertyIndex, revision);
  }

  /**
   * @brief Set the flag to trigger the textures to be initialized and renderer to be added to the control.
   * @param[in] visual The text visual.
   */
  static void EnableRendererUpdate(Ui::Integration::Visual::Base visual)
  {
    GetVisualObject(visual).mRendererUpdateNeeded = true;
  };

  /**
   * @brief Set the text to be always rendered
   * @param[in] visual The text visual.
   * @param[in] requireRender Whether to text always rendered.
   */
  static void SetRequireRender(Ui::Integration::Visual::Base visual, bool requireRender)
  {
    GetVisualObject(visual).SetRequireRender(requireRender);
  };

  /**
   * @brief Instantly updates the renderer
   * @param[in] visual The text visual.
   */
  static void UpdateRenderer(Ui::Integration::Visual::Base visual)
  {
    GetVisualObject(visual).UpdateRenderer();
  };

  /**
   * @brief Instantly updates the async renderer
   * @param[in] visual The text visual.
   * @param[in] parameters The async text parameters.
   * @return true if the async text render request was successful, false otherwise.
   */
  static bool UpdateAsyncRenderer(Ui::Integration::Visual::Base visual, Ui::Text::AsyncTextParameters& parameters)
  {
    return GetVisualObject(visual).UpdateAsyncRenderer(parameters);
  };

  /**
   * @brief Instantly requests the async size computation.
   * @param[in] visual The text visual.
   * @param[in] parameters The async text parameters.
   */
  static void RequestAsyncSizeComputation(Ui::Integration::Visual::Base visual, Ui::Text::AsyncTextParameters& parameters)
  {
    GetVisualObject(visual).RequestAsyncSizeComputation(parameters);
  };

  /**
   * @brief Set the control's async text interface.
   * @param[in] visual The text visual.
   * @param[in] asyncTextInterface The async text interface.
   */
  static void SetAsyncTextInterface(Ui::Integration::Visual::Base visual, Ui::Integration::Text::AsyncTextInterface* asyncTextInterface)
  {
    GetVisualObject(visual).SetAsyncTextInterface(asyncTextInterface);
  };

  /**
   * @brief Set the visual constraints need to be applied always or not.
   * @param[in] visual The text visual.
   * @param[in] applyAlways True if constraint need to be applied always. False if we need once only.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  static void SetConstraintApplyAlways(Ui::Integration::Visual::Base visual, bool applyAlways, bool notifyToConstraint = false)
  {
    GetVisualObject(visual).SetConstraintApplyAlways(applyAlways, notifyToConstraint);
  };

  /**
   * @brief Sets whether TextGradient animation constraints should be applied every frame.
   *
   * @param[in] visual The text visual.
   * @param[in] applyAlways True to use APPLY_ALWAYS, false to use APPLY_ONCE.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  static void SetGradientAnimApplyAlways(Ui::Integration::Visual::Base visual, bool applyAlways, bool notifyToConstraint = false)
  {
    GetVisualObject(visual).SetGradientAnimApplyAlways(applyAlways, notifyToConstraint);
  };

  /**
   * @brief Sets whether TextGradientOverlay animation constraints should be applied every frame.
   *
   * @param[in] visual The text visual.
   * @param[in] applyAlways True to use APPLY_ALWAYS, false to use APPLY_ONCE.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  static void SetGradientOverlayAnimApplyAlways(Ui::Integration::Visual::Base visual, bool applyAlways, bool notifyToConstraint = false)
  {
    GetVisualObject(visual).SetGradientOverlayAnimApplyAlways(applyAlways, notifyToConstraint);
  };

  /**
   * @brief Store the TextGradient style snapshot for a future gradient-enabled render path.
   * @param[in] visual The text visual.
   * @param[in] style The TextGradient style snapshot.
   */
  static void SetTextGradientStyle(Ui::Integration::Visual::Base visual, const Ui::Text::Internal::Gradient::Style& style)
  {
    GetVisualObject(visual).SetTextGradientStyle(style);
  };

  /**
   * @brief Store the TextGradient bounds mode used by Label rendering.
   * @param[in] visual The text visual.
   * @param[in] mode The TextGradient bounds mode.
   */
  static void SetTextGradientBoundsMode(Ui::Integration::Visual::Base visual, Ui::Text::GradientBoundsMode mode)
  {
    GetVisualObject(visual).SetTextGradientBoundsMode(mode);
  };

  /**
   * @brief Store the TextGradientOverlay style snapshot used by Label rendering.
   * @param[in] visual The text visual.
   * @param[in] style The TextGradientOverlay style snapshot.
   */
  static void SetTextGradientOverlayStyle(Ui::Integration::Visual::Base visual, const Ui::Text::Internal::Gradient::Style& style)
  {
    GetVisualObject(visual).SetTextGradientOverlayStyle(style);
  };

  /**
   * @brief Store the TextGradientOverlay bounds mode used by Label rendering.
   * @param[in] visual The text visual.
   * @param[in] mode The TextGradientOverlay bounds mode.
   */
  static void SetTextGradientOverlayBoundsMode(Ui::Integration::Visual::Base visual, Ui::Text::GradientBoundsMode mode)
  {
    GetVisualObject(visual).SetTextGradientOverlayBoundsMode(mode);
  };

  /**
   * @brief Store the TextGradientOverlay mode used by Label rendering.
   * @param[in] visual The text visual.
   * @param[in] mode The TextGradientOverlay mode.
   */
  static void SetTextGradientOverlayMode(Ui::Integration::Visual::Base visual, Ui::Text::GradientOverlayMode mode)
  {
    GetVisualObject(visual).SetTextGradientOverlayMode(mode);
  };

  /**
   * @brief Calculates VIEW_BOUND gradient bounds in a caller-supplied coordinate space.
   *
   * @param[in] visual The text visual.
   * @param[in] coordinateSize The coordinate space size used by the target shader.
   * @return Bounds that map the Label view into the target coordinate space.
   */
  static Vector4 CalculateGradientViewBounds(Ui::Integration::Visual::Base visual, const Vector2& coordinateSize);

  /**
   * @brief Returns the coordinate size used for VIEW_BOUND gradient mapping.
   *
   * @param[in] visual The text visual.
   * @return The visual size in Label coordinates after transform constraints.
   */
  static Vector2 GetGradientViewCoordinateSize(Ui::Integration::Visual::Base visual);

  /**
   * @brief Render marquee text PixelData with the TextVisual-owned Typesetter.
   *
   * This is for Label's sync marquee preparation only. It reuses TextVisual's
   * Typesetter without exposing it outside TextVisual.
   *
   * @param[in] visual The text visual.
   * @param[in] size The marquee texture size.
   * @param[in] textDirection The direction of the text.
   * @param[in] behaviour The behaviour of how to render the text.
   * @param[in] ignoreHorizontalAlignment Whether to ignore horizontal alignment.
   * @param[in] pixelFormat The pixel format of the rendered PixelData.
   * @param[in] originSize The origin size for calculating vertical alignment.
   * @param[in] marqueeStartAnchor Retained source anchor to resolve in the rendered texture.
   * @param[out] marqueeTextureAnchor Optional resolved texture-space anchor.
   * @return A pixel data with the requested marquee text content rendered.
   */
  static PixelData RenderMarqueeText(Ui::Integration::Visual::Base         visual,
                                     const Vector2&                        size,
                                     Ui::Text::Direction                   textDirection,
                                     Ui::Text::Typesetter::RenderBehaviour behaviour,
                                     bool                                  ignoreHorizontalAlignment,
                                     Pixel::Format                         pixelFormat,
                                     const Vector2&                        originSize,
                                     const Ui::Text::MarqueeStartAnchor&   marqueeStartAnchor,
                                     Ui::Text::MarqueeTextureAnchor*       marqueeTextureAnchor = nullptr);

  /**
   * @brief Render preserved-color marquee PixelData with the TextVisual-owned Typesetter.
   */
  static PixelData RenderMarqueeTextGradientPreserved(Ui::Integration::Visual::Base visual,
                                                      const Vector2&                size,
                                                      Ui::Text::Direction           textDirection,
                                                      bool                          ignoreHorizontalAlignment,
                                                      Pixel::Format                 pixelFormat,
                                                      const Vector2&                originSize);

  /**
   * @brief Render TextGradient mask marquee PixelData with the TextVisual-owned Typesetter.
   */
  static PixelData RenderMarqueeTextGradientMask(Ui::Integration::Visual::Base visual,
                                                 const Vector2&                size,
                                                 Ui::Text::Direction           textDirection,
                                                 bool                          ignoreHorizontalAlignment,
                                                 Pixel::Format                 pixelFormat,
                                                 const Vector2&                originSize);

  /**
   * @brief Retrieve the stored TextGradient mask PixelData for internal rendering/tests.
   * @param[in] visual The text visual.
   * @return The stored TextGradient mask PixelData.
   */
  static PixelData GetTextGradientMaskPixelData(Ui::Integration::Visual::Base visual)
  {
    return GetVisualObject(visual).GetTextGradientMaskPixelData();
  };

public: // from Visual::Base
  /**
   * @copydoc Visual::Base::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Visual::Base::GetNaturalSize()
   */
  void GetNaturalSize(Vector2& naturalSize) override;

  /**
   * @copydoc Visual::Base::CreatePropertyMap()
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::SetFittingMode
   */
  void SetFittingMode(Ui::Image::FittingMode fittingMode) override;

  /**
   * @copydoc Visual::Base::OnApplyFittingMode
   */
  void OnApplyFittingMode(const Vector2& controlSize, const Insets& padding, float effectiveScale) override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache The VisualFactoryCache object
   * @param[in] shaderFactory The TextVisualShaderFactory object
   */
  TextVisual(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~TextVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  // from Visual::Base

  /**
   * @copydoc Visual::Base::DoSetProperties()
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::DoSetOnScene()
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene()
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

private:
  struct TilingInfo
  {
    PixelData textPixelData;
    PixelData stylePixelData;
    PixelData overlayStylePixelData;
    PixelData maskPixelData;
    PixelData revealPixelData;
    int32_t   width;
    int32_t   height;
    uint32_t  offsetHeight;
    Vector2   transformOffset;

    TilingInfo(int32_t width, int32_t height)
    : textPixelData(),
      stylePixelData(),
      overlayStylePixelData(),
      maskPixelData(),
      revealPixelData(),
      width(width),
      height(height),
      offsetHeight(0u),
      transformOffset(0.f, 0.f)
    {
    }

    ~TilingInfo()
    {
    }
  };

  /**
   * @brief Set the individual property to the given value.
   *
   * @param[in] index The index key used to reference this value within the initial property map.
   *
   * @param[in] propertyValue The value to set.
   */
  void DoSetProperty(Dali::Property::Index index, const Dali::Property::Value& propertyValue);

  /**
   * @brief Updates the effective line height based on the current LineHeightMode.
   */
  void UpdateLineHeight();

  /**
   * @brief Updates the text's renderer.
   */
  void UpdateRenderer();

  /**
   * @brief Updates the text's async renderer.
   * @param[in] parameters The async text parameters.
   * @return true if the async text render request was successful, false otherwise.
   */
  bool UpdateAsyncRenderer(Ui::Text::AsyncTextParameters& parameters);

  /**
   * @brief Requests the async size computation.
   * @param[in] parameters The async text parameters.
   */
  void RequestAsyncSizeComputation(Ui::Text::AsyncTextParameters& parameters);

  /**
   * @brief Set the control's async text interface.
   * @param[in] asyncTextInterface The async text interface.
   */
  void SetAsyncTextInterface(Ui::Integration::Text::AsyncTextInterface* asyncTextInterface);

  void SetTextGradientStyle(const Ui::Text::Internal::Gradient::Style& style);

  void SetTextGradientBoundsMode(Ui::Text::GradientBoundsMode mode);

  void SetTextGradientOverlayStyle(const Ui::Text::Internal::Gradient::Style& style);

  void SetTextGradientOverlayBoundsMode(Ui::Text::GradientBoundsMode mode);

  void SetTextGradientOverlayMode(Ui::Text::GradientOverlayMode mode);

  PixelData GetTextGradientMaskPixelData() const;

  /**
   * @brief Sets the TextGradient start-offset source property index registered by the control.
   */
  void SetGradientAnimProperties(Property::Index startOffsetPropertyIndex);

  /**
   * @brief Sets the TextGradientOverlay start-offset source property index registered by the control.
   */
  void SetGradientOverlayAnimProperties(Property::Index startOffsetPropertyIndex);

  /**
   * @brief Set the visual constraints need to be applied always or not.
   * @param[in] applyAlways True if constraint need to be applied always. False if we need once only.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetConstraintApplyAlways(bool applyAlways, bool notifyToConstraint);

  /**
   * @brief Sets whether TextGradient animation constraints should be applied every frame.
   *
   * @param[in] applyAlways True to use APPLY_ALWAYS, false to use APPLY_ONCE.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientAnimApplyAlways(bool applyAlways, bool notifyToConstraint);

  /**
   * @brief Sets whether TextGradientOverlay animation constraints should be applied every frame.
   *
   * @param[in] applyAlways True to use APPLY_ALWAYS, false to use APPLY_ONCE.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientOverlayAnimApplyAlways(bool applyAlways, bool notifyToConstraint);

  /**
   * @brief Removes TextGradient animation constraints.
   */
  void RemoveGradientAnimConstraints();

  /**
   * @brief Removes TextGradientOverlay animation constraints.
   */
  void RemoveGradientOverlayAnimConstraints();

  /**
   * @brief Rebinds TextGradient animation constraints to the current renderer uniforms.
   */
  void RebindGradientAnimConstraints();

  /**
   * @brief Rebinds TextGradientOverlay animation constraints to the current renderer uniforms.
   */
  void RebindGradientOverlayAnimConstraints();

  /**
   * @brief Binds TextGradient animation constraints to registered renderer uniform properties.
   */
  void BindGradientAnimConstraints(VisualRenderer& renderer,
                                   Property::Index startOffsetIndex);

  /**
   * @brief Binds TextGradientOverlay animation constraints to registered renderer uniform properties.
   */
  void BindGradientOverlayAnimConstraints(VisualRenderer& renderer,
                                          Property::Index startOffsetIndex);

  /**
   * @brief Applies reveal configuration to this visual.
   *
   * A changed configuration invalidates renderer output but retains the
   * caller-owned progress property. DISABLED removes active renderer
   * constraints immediately.
   *
   * @param[in] unit The reveal unit, or DISABLED.
   * @param[in] fadeDurationRatio The authored automatic sentinel or normalized fade duration.
   * @param[in] progressPropertyIndex The Label scene progress property index.
   * @param[in] revision The current reveal configuration revision.
   */
  void ConfigureTextReveal(Ui::Text::Internal::Reveal::Unit unit,
                           float                            fadeDurationRatio,
                           Property::Index                  progressPropertyIndex,
                           uint64_t                         revision);

  /**
   * @brief Removes all constraints that bind reveal progress to renderers.
   */
  void RemoveTextRevealConstraints();

  /**
   * @brief Binds the stable Label reveal progress property to a renderer.
   *
   * The renderer receives the current fade duration and an APPLY_ALWAYS
   * constraint for progress.
   *
   * @param[in] renderer The renderer receiving reveal uniforms.
   */
  void BindTextRevealConstraint(VisualRenderer& renderer);

  /**
   * @brief Builds the source-glyph reveal plan for the synchronous visual path.
   *
   * CHARACTER avoids segmentation. WORD lazily creates a Segmentation::New()
   * instance owned by this visual instead of using SingletonService.
   *
   * @return The plan indexed by source glyph before final elision projection.
   */
  Ui::Text::Internal::Reveal::Plan BuildTextRevealSourcePlan();

  /**
   * @brief Removes the text's renderer.
   */
  void RemoveRenderer(Actor& actor, bool removeDefaultRenderer);

  /**
   * @brief Create a texture in textureSet and add it.
   * @param[in] textureSet The textureSet to which the texture will be added.
   * @param[in] data The PixelData to be uploaded to texture.
   * @param[in] sampler The sampler.
   * @param[in] textureSetIndex The Index of TextureSet.
   */
  void AddTexture(TextureSet& textureSet, PixelData& data, Sampler& sampler, unsigned int textureSetIndex);

  /**
   * @brief Create a texture in textureSet and add it.
   * @param[in] textureSet The textureSet to which the texture will be added.
   * @param[in] tilingInfo The tiling infomation to be uploaded to texture.
   * @param[in] data The PixelData to be uploaded to texture.
   * @param[in] sampler The sampler.
   * @param[in] textureSetIndex The Index of TextureSet.
   */
  void AddTilingTexture(TextureSet& textureSet, TilingInfo& tilingInfo, PixelData& data, Sampler& sampler,
                        unsigned int textureSetIndex);

  /**
   * @brief Create the text's texture. It will use cached shader feature for text visual.
   * @param[in] info This is the information you need to create a Tiling.
   * @param[in] renderer The renderer to which the TextureSet will be added.
   * @param[in] sampler The sampler.
   */
  void CreateTextureSet(TilingInfo& info, VisualRenderer& renderer, Sampler& sampler);

  /**
   * Create renderer of the text for rendering.
   * @param[in] actor The actor.
   * @param[in] size The texture size.
   * @param[in] hasMultipleTextColors Whether the text contains multiple colors.
   * @param[in] containsColorGlyph Whether the text contains color glyph.
   * @param[in] styleEnabled Whether the legacy renderer should use the style feature.
   * @param[in] styleTextureEnabled Whether style texture plane generation is required.
   * @param[in] styleBlocksTextGradient Whether the current style state blocks TextGradient composition.
   * @param[in] isOverlayStyle Whether the style needs to overlay on the text (e.g. strikethrough, underline, etc.).
   * @param[in] embossEnabled Whether the style contains emboss.
   */
  void AddRenderer(Actor& actor, const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph,
                   bool styleEnabled, bool styleTextureEnabled, bool styleBlocksTextGradient, bool isOverlayStyle,
                   bool embossEnabled);

  /**
   * @brief Whether simple TextGradient shader composition is supported for the current render.
   */
  bool IsTextGradientCompositionSupported(const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph,
                                          bool styleBlocksTextGradient, bool isOverlayStyle, bool embossEnabled,
                                          bool isHeightTiling, bool isMarqueeEnabled, bool isCutoutEnabled) const;

  /**
   * @brief Whether mixed TextGradient shader composition is supported for the current render.
   */
  bool IsTextGradientMixedCompositionSupported(const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph,
                                               bool styleBlocksTextGradient, bool styleTextureEnabled,
                                               bool isOverlayStyle, bool embossEnabled, bool isHeightTiling,
                                               bool isMarqueeEnabled, bool isCutoutEnabled) const;

  /**
   * @brief Whether TextGradientOverlay shader composition is supported for the current render.
   */
  bool IsTextGradientOverlayCompositionSupported(const Vector2& size, bool isHeightTiling,
                                                 bool isMarqueeEnabled, bool isCutoutEnabled) const;

  /**
   * @brief Calculates CONTENT_BOUND gradient bounds inside the text texture.
   */
  Vector4 CalculateGradientContentBounds(const Vector2& textureSize) const;

  /**
   * @brief Resolves the active TextGradient bounds mode for the texture.
   */
  Vector4 ResolveTextGradientBounds(const Vector2& textureSize, const Vector4& contentBounds) const;

  /**
   * @brief Resolves the active TextGradientOverlay bounds mode for the texture.
   */
  Vector4 ResolveTextGradientOverlayBounds(const Vector2& textureSize, const Vector4& contentBounds) const;

  /**
   * @brief Register TextGradient uniforms on a gradient-enabled renderer.
   */
  void ApplyTextGradientUniforms(VisualRenderer& renderer, const Vector2& textureSize, const Vector4& textBounds);

  /**
   * @brief Register TextGradientOverlay uniforms on an overlay-enabled renderer.
   */
  void ApplyTextGradientOverlayUniforms(VisualRenderer& renderer, const Vector2& textureSize, const Vector4& textBounds);

  /**
   * Get the texture of the text for rendering. It will use cached shader feature for text visual.
   * @param[in] size The texture size.
   */
  TextureSet GetTextTexture(const Vector2& size);

  /**
   * Get the text rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current text shader's features. It will be cached as text visual.
   */
  Shader GetTextShader(VisualFactoryCache& factoryCache, TextVisualShaderFeature::FeatureBuilder& featureBuilder);

  /**
   * @brief Set the text to be always rendered
   * @param[in] requireRender Whether to text always rendered.
   */
  void SetRequireRender(bool requireRender);

  /**
   * @brief Retrieve the TextVisual object.
   * @param[in] visual A handle to the TextVisual
   * @return The TextVisual object
   */
  static TextVisual& GetVisualObject(Ui::Integration::Visual::Base visual)
  {
    return static_cast<TextVisual&>(Ui::GetImplementation(visual).GetVisualObject());
  };

  /**
   * @copydoc TextLoadObserver::LoadComplete
   *
   * Called when the TextLoadingTask's work is complete.
   *
   * @param[in] success True if the load was successful, false otherwise.
   * @param[in] textInformation The text information including render info and parameters.
   */
  void LoadComplete(bool success, const TextInformation& textInformation) override;

private:
  typedef std::vector<VisualRenderer> RendererContainer;
  typedef std::vector<Constraint>     ConstraintContainer;

private:
  Ui::Text::ControllerPtr                    mController;         ///< The text's controller.
  Ui::Text::TypesetterPtr                    mTypesetter;         ///< The text's typesetter.
  Ui::Integration::Text::AsyncTextInterface* mAsyncTextInterface; ///< The text's async interface.
  TextVisualGradientDataPtr                  mGradientData;       ///< Lazily allocated TextGradient rendering state.
  TextVisualRevealDataPtr                    mRevealData;         ///< Lazily allocated TextReveal rendering state.

  TextVisualShaderFactory&                mTextVisualShaderFactory; ///< The shader factory for text visual.
  TextVisualShaderFeature::FeatureBuilder mTextShaderFeatureCache;  ///< The cached shader feature for text visual.

  WeakHandle<Actor> mControl;                    ///< The control where the renderer is added.
  Constraint        mColorConstraint{};          ///< Color constraint
  Constraint        mOpacityConstraint{};        ///< Opacity constraint
  Property::Index   mHasMultipleTextColorsIndex; ///< The index of uHasMultipleTextColors proeprty.
  Property::Index
                      mAnimatableTextColorPropertyIndex; ///< The index of animatable text color property registered by the control.
  Property::Index     mTextColorAnimatableIndex;         ///< The index of uTextColorAnimatable property.
  Property::Index     mTextRequireRenderPropertyIndex;   ///< The index of requireRender property.
  RendererContainer   mRendererList;
  ConstraintContainer mColorConstraintList;
  ConstraintContainer mOpacityConstraintList;

  float                    mLineHeight;
  Ui::Text::LineHeightMode mLineHeightMode;
  Ui::Text::OverflowMode   mOverflowMode;
  uint32_t                 mTextLoadingTaskId;               ///< The currently requested text loading(render) task Id.
  uint32_t                 mNaturalSizeTaskId;               ///< The currently requested natural size task Id.
  uint32_t                 mHeightForWidthTaskId;            ///< The currently requested height for width task Id.
  bool                     mRendererUpdateNeeded : 1;        ///< The flag to indicate whether the renderer needs to be updated.
  bool                     mApplyingFittingMode : 1;         ///< Whether renderer update is running from OnApplyFittingMode().
  bool                     mTextRequireRender : 1;           ///< The flag to indicate whether the text needs to be rendered.
  bool                     mIsConstraintAppliedAlways : 1;   ///< Whether the constraint need to be applied always.
  bool                     mIsTextLoadingTaskRunning : 1;    ///< Whether the requested text loading task is running or not.
  bool                     mIsNaturalSizeTaskRunning : 1;    ///< Whether the requested natural size task is running or not.
  bool                     mIsHeightForWidthTaskRunning : 1; ///< Whether the requested height for width task is running or not.
};

} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif /* DALI_UI_INTERNAL_TEXT_VISUAL_H */
