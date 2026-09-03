#ifndef DALI_UI_MASK_EFFECT_H
#define DALI_UI_MASK_EFFECT_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/render-effects/render-effect.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class MaskEffectImpl;
} // namespace Internal DALI_INTERNAL

/**
 * @brief MaskEffect is a visual effect that masks owner View.
 * This class is a concrete class from RenderEffect interface.
 * Set this effect on a view, clear it manually to remove.
 *
 * @code
 * Ui::View view = Ui::View::New();
 * parent.Add(view);
 * view.SetRenderEffect(MaskEffect::New(maskView)); // Set on the view, activated automatically
 * ...
 * view.ClearRenderEffect(); // Remove from the view, deactivated automatically
 * @endcode
 */
class DALI_UI_API MaskEffect : public RenderEffect
{
public:
  /**
   * @brief Enumeration for selecting how the mask source interprets pixel data.
   */
  enum MaskMode
  {
    ALPHA,     ///< Uses the alpha channel of the mask texture. (Default)
    LUMINANCE, ///< Converts RGB to grayscale and uses the luminance as mask value.
  };

  /**
   * @brief Creates an initialized MaskEffect with view, using default settings. The default settings are:
   *
   * maskMode = MaskMode::ALPHA
   * maskPosition = Vector2(0.f, 0.f)
   * maskScale = Vector2(1.f, 1.f)
   *
   * @param[in] maskView The View used as the mask source.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static MaskEffect New(Ui::View maskView);

  /**
   * @brief Creates an initialized MaskEffect.
   *
   * @param[in] maskView The View used as the mask source.
   * @param[in] maskMode Defines pixel data type (alpha, luminance) used as the mask source.
   * @param[in] maskPosition The offset of the mask source against the owner View, in pixels.
   * @param[in] maskScale The scale of the mask source. Vector2(1.f, 1.f) leaves it at its own size.
   *
   * @return A handle to a newly allocated Dali resource
   * @note A zero component in maskScale is clamped to a small non-zero value internally, so it
   * neither divides by zero nor disables the effect.
   */
  static MaskEffect New(Ui::View maskView, MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale);

  /**
   * @brief Creates an uninitialized mask effect.
   */
  MaskEffect();

  /**
   * @brief Copy constructor.
   */
  MaskEffect(const MaskEffect& handle);

  /**
   * @brief Destructor
   */
  ~MaskEffect();

  /**
   * @brief Set whether the target, the owner View of this effect, is rendered once or every frame.
   * @param[in] renderOnce If true, renders target once, else updates every frame.
   * @note The masking itself is still applied every frame. This only controls how often the target
   * is captured into the offscreen buffer used as the mask input.
   */
  void SetTargetRenderOnce(bool renderOnce);

  /**
   * @brief Retrieves whether the target is rendered once(true) or every frame(false).
   * @return Whether the target is rendered once or every frame.
   */
  bool IsTargetRenderOnce() const;

  /**
   * @brief Set whether the source, the mask View, is rendered once or every frame.
   * @param[in] renderOnce If true, renders source once, else updates every frame.
   * @note The masking itself is still applied every frame. This only controls how often the source
   * is captured into the offscreen buffer used as the mask input.
   */
  void SetSourceRenderOnce(bool renderOnce);

  /**
   * @brief Retrieves whether the source is rendered once(true) or every frame(false).
   * @return Whether the source is rendered once or every frame.
   */
  bool IsSourceRenderOnce() const;

public: // Not intended for use by Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Ui::Internal implementation.
   * @param[in] maskEffectImpl The mask effect internal implementation.
   */
  explicit DALI_INTERNAL MaskEffect(Internal::MaskEffectImpl* maskEffectImpl);
  ///@endcond
};
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_MASK_EFFECT_H
