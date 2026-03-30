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
#include <dali-ui-foundation/public-api/view.h>

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
 * Add this effect to a view, clear manually to deactivate.
 *
 * Ui::View view = Ui::View::New();
 * parent.Add(view);
 * view.SetRenderEffect(MaskEffect::New(maskView)); // Activate
 * ...
 * view.ClearRenderEffect(); // Deactivate
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
   * @param[in] maskView The source View to affect mask.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static MaskEffect New(Ui::View maskView);

  /**
   * @brief Creates an initialized MaskEffect.
   *
   * @param[in] maskView The source View to affect mask.
   * @param[in] maskMode Defines pixel data type (alpha, luminance) used as the mask source.
   * @param[in] maskPosition The Position of mask source.
   * @param[in] maskScale The Scale of mask source.
   *
   * @return A handle to a newly allocated Dali resource
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
   * @brief Set whether the target should be rendered once(true) or every frame(false).
   * @param[in] targetMaskOnce If true, renders target once, else updates every frame.
   */
  void SetTargetMaskOnce(bool targetMaskOnce);

  /**
   * @brief Retrives whether the target should be rendered once(true) or every frame(false).
   */
  bool GetTargetMaskOnce() const;

  /**
   * @brief Set whether the source should be rendered once(true) or every frame(false).
   * @param[in] sourceMaskOnce If true, renders source once, else updates every frame.
   */
  void SetSourceMaskOnce(bool sourceMaskOnce);

  /**
   * @brief Retrives whether the source should be rendered once(true) or every frame(false).
   */
  bool GetSourceMaskOnce() const;

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
