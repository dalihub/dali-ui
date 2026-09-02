#ifndef DALI_UI_TEXT_RENDERER_H
#define DALI_UI_TEXT_RENDERER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal::Gradient
{
struct AtlasFrameState;
struct AtlasRendererState;
} //namespace Internal::Gradient

class Renderer;
typedef IntrusivePtr<Renderer> RendererPtr;

class ViewInterface;

/**
 * @brief Abstract base class for Text renderers.
 *
 * This is reponsible for rendering the glyphs from a ViewInterface in the specified positions.
 * It is implemented by returning an Actor intended as the child of a UI control.
 */
class Renderer : public RefObject
{
public:
  /**
   * @brief Render the glyphs from a ViewInterface.
   *
   * @param[in] view The interface to a view.
   * @param[in] textControl handle to the text control
   * @param[in] animatablePropertyIndex textControl specific animatable property
   * @param[out] alignmentOffset Offset used to internally align the placement actor.
   * @param[in] depth The depth in the tree of the parent.
   * @return The Renderable actor used to position the text.
   */
  virtual Actor Render(ViewInterface& view, Actor textContol, Property::Index animatablePropertyIndex,
                       float& alignmentOffset, int depth) = 0;

  /**
   * @brief Supplies the active normal/placeholder atlas gradient snapshot.
   *
   * @return True if this renderer accepted and owns the supplied snapshot. False
   * if this renderer does not support atlas gradients; callers must keep the
   * authored value and render the text with the existing solid color fallback.
   *
   * Other renderer backends intentionally keep the default unsupported behavior.
   */
  virtual bool SetAtlasGradientState(const Internal::Gradient::AtlasRendererState& state);

  /**
   * @brief Updates bounds-dependent uniforms without rebuilding text geometry.
   */
  virtual void UpdateAtlasGradient(const Vector2& coordinateSize, const Vector4& bounds);

  /**
   * @brief Updates GradientSpan VIEW_BOUND uniforms without rebuilding text geometry.
   *
   * Unsupported renderer backends intentionally keep the default no-op behavior.
   */
  virtual void UpdateAtlasGradientSpanViewBounds(const Vector2& coordinateSize, const Vector4& bounds);

  /**
   * @brief Binds a control-owned source property to the atlas text gradient start offset uniform.
   *
   * Unsupported renderer backends intentionally keep the default no-op behavior.
   * Passing an empty actor or invalid property index clears the current binding.
   *
   * @param[in] sourceActor The actor owning the source property.
   * @param[in] startOffsetPropertyIndex Source property index for uTextGradientStartOffset.
   */
  virtual void SetAtlasGradientAnimProperties(Actor sourceActor, Property::Index startOffsetPropertyIndex);

  /**
   * @brief Updates atlas text gradient start offset constraint apply rate.
   *
   * @param[in] applyAlways True while an animation or constraint is actively driving the source property.
   * @param[in] notifyToConstraint True to reapply the current rate to existing constraints.
   */
  virtual void SetAtlasGradientAnimApplyAlways(bool applyAlways, bool notifyToConstraint = false);

protected:
  /**
   * @brief Constructor.
   */
  Renderer();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Renderer();

private:
  // Undefined
  Renderer(const Renderer& handle);

  // Undefined
  Renderer& operator=(const Renderer& handle);
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_RENDERER_H
