#pragma once

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
#include <dali-ui-foundation/integration-api/selectable-image-interface-impl.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Internal implementation for SelectableLottieAnimationView.
 *
 * A plain BaseObject that implements Integration::SelectableImageInterfaceImpl by COMPOSING a single
 * Ui::LottieAnimationView: it drives that glyph between a "selected" and a "deselected" state
 * by playing (or snapping to) the configured frame segments, and recolours the checked inner
 * fill per-frame with the pre-resolved state colours pushed by the owning component. It
 * inherits the SetSelected() template from SelectableImageInterfaceImpl and reimplements only
 * OnSelectedChanged() plus the drawing/state accessors.
 *
 * @see Dali::Ui::SelectableLottieAnimationView
 */
class SelectableLottieAnimationViewImpl : public SelectableImageInterfaceImpl
{
public:
  using FrameRange = Ui::SelectableLottieImage::FrameRange;

  /**
   * @brief Creates a new SelectableLottieAnimationViewImpl.
   *
   * @param[in] url           The URL of the Lottie JSON file
   * @param[in] selectRange   The segment played (or snapped to) when becoming selected
   * @param[in] deselectRange The segment played (or snapped to) when becoming deselected
   * @param[in] keyPath       The inner-fill recolour key path; empty keeps the generic default
   * @return A raw pointer to the newly allocated implementation (reference count 0)
   */
  static SelectableLottieAnimationViewImpl* New(const Dali::String& url,
                                                const FrameRange&   selectRange,
                                                const FrameRange&   deselectRange,
                                                const Dali::String& keyPath = Dali::String());

public: // From Integration::SelectableImageInterfaceImpl
  /**
   * @copydoc Dali::Ui::Integration::SelectableImageInterfaceImpl::GetView
   */
  Ui::View GetView() const override;

  /**
   * @copydoc Dali::Ui::Integration::SelectableImageInterfaceImpl::SetStateColors
   */
  void SetStateColors(const Vector4& deselected, const Vector4& selected) override;

  /**
   * @copydoc Dali::Ui::Integration::SelectableImageInterfaceImpl::IsTransitioning
   */
  bool IsTransitioning() const override;

  /**
   * @copydoc Dali::Ui::Integration::SelectableImageInterfaceImpl::TransitionFinishedSignal
   */
  SelectableImageInterface::TransitionFinishedSignalType& TransitionFinishedSignal() override;

protected: // From Integration::SelectableImageInterfaceImpl
  /**
   * @copydoc Dali::Ui::Integration::SelectableImageInterfaceImpl::OnSelectedChanged
   *
   * Plays (or snaps to) the select/deselect segment and re-seats the inner-fill recolour.
   */
  void OnSelectedChanged(bool selected, bool animated) override;

protected: // Construction & Destruction
  /**
   * @brief SelectableLottieAnimationViewImpl constructor.
   */
  SelectableLottieAnimationViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  ~SelectableLottieAnimationViewImpl() override;

private:
  /**
   * @brief Sets the frame segments played on select and on deselect.
   *
   * @param[in] selectRange   The segment played (or snapped to) when becoming selected
   * @param[in] deselectRange The segment played (or snapped to) when becoming deselected
   */
  void SetFrameRanges(const FrameRange& selectRange, const FrameRange& deselectRange);

  /**
   * @brief (Re)registers the per-frame inner-fill recolour dynamic property from the
   * current state colours, key path and frame ranges.
   */
  void RegisterInnerFillRecolor();

private:
  // Not copyable or movable
  SelectableLottieAnimationViewImpl(const SelectableLottieAnimationViewImpl&)            = delete;
  SelectableLottieAnimationViewImpl(SelectableLottieAnimationViewImpl&&)                 = delete;
  SelectableLottieAnimationViewImpl& operator=(const SelectableLottieAnimationViewImpl&) = delete;
  SelectableLottieAnimationViewImpl& operator=(SelectableLottieAnimationViewImpl&&)      = delete;

private:                                     // Data
  Ui::LottieAnimationView mLottie;           ///< the composed single Lottie glyph that draws the states
  Dali::String            mInnerFillKeyPath; ///< asset key path of the checked inner fill to recolour

  Vector4 mDeselectedColor; ///< pre-resolved deselected inner-fill colour
  Vector4 mSelectedColor;   ///< pre-resolved selected inner-fill colour

  int32_t mSelectStart;   ///< select-segment start frame
  int32_t mSelectEnd;     ///< select-segment end frame
  int32_t mDeselectStart; ///< deselect-segment start frame
  int32_t mDeselectEnd;   ///< deselect-segment end frame

  bool mLastSelected{false}; ///< last requested logical state; drives the inner-fill recolour

  int32_t mDynamicPropertyId; ///< unique id keying this instance's recolour callback
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
