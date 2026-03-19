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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/internal/layout/layout-params-impl.h>
#include <dali-ui-foundation/public-api/flex-layout-params.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores FlexLayout child parameters.
 */
class FlexLayoutParamsImpl : public LayoutParamsImpl
{
public:
  /**
   * @brief Constructs with default flex properties (grow=0, shrink=1, basis=WRAP_CONTENT, alignSelf=AUTO).
   */
  FlexLayoutParamsImpl()
  : LayoutParamsImpl(),
    mFlexGrow(0.0f),
    mFlexShrink(1.0f),
    mFlexBasis(WRAP_CONTENT),
    mAlignSelf(FlexAlign::AUTO)
  {
  }

  FlexLayoutParamsImpl(const FlexLayoutParamsImpl& other)
  : LayoutParamsImpl(),
    mFlexGrow(other.mFlexGrow),
    mFlexShrink(other.mFlexShrink),
    mFlexBasis(other.mFlexBasis),
    mAlignSelf(other.mAlignSelf)
  {
  }

  Integration::TraitId GetTraitId() const override
  {
    return Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS;
  }

  /**
   * @brief Sets the flex grow factor for distributing remaining space.
   * @param[in] grow The grow factor (0 means no growing).
   */
  void SetFlexGrow(float grow)
  {
    mFlexGrow = grow;
  }

  /**
   * @brief Gets the flex grow factor.
   * @return The grow factor.
   */
  float GetFlexGrow() const
  {
    return mFlexGrow;
  }

  /**
   * @brief Sets the flex shrink factor for distributing negative space.
   * @param[in] shrink The shrink factor (1 means proportional shrinking).
   */
  void SetFlexShrink(float shrink)
  {
    mFlexShrink = shrink;
  }

  /**
   * @brief Gets the flex shrink factor.
   * @return The shrink factor.
   */
  float GetFlexShrink() const
  {
    return mFlexShrink;
  }

  /**
   * @brief Sets the initial main size of the flex item before grow/shrink.
   * @param[in] basis The flex basis value.
   */
  void SetFlexBasis(float basis)
  {
    mFlexBasis = basis;
  }

  /**
   * @brief Gets the flex basis value.
   * @return The flex basis.
   */
  float GetFlexBasis() const
  {
    return mFlexBasis;
  }

  /**
   * @brief Sets the cross-axis alignment override for this child.
   * @param[in] align The alignment (AUTO defers to parent's alignItems).
   */
  void SetAlignSelf(FlexAlign align)
  {
    mAlignSelf = align;
  }

  /**
   * @brief Gets the cross-axis alignment override for this child.
   * @return The align-self value.
   */
  FlexAlign GetAlignSelf() const
  {
    return mAlignSelf;
  }

  /**
   * @brief Retrieves the FlexLayoutParams trait attached to a view, if any.
   * @param[in] viewImpl The view implementation to query.
   * @return Pointer to the params, or nullptr if not attached.
   */
  static FlexLayoutParamsImpl* Get(Integration::ViewImpl& viewImpl)
  {
    Trait trait = viewImpl.GetTrait(Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS);
    if(trait)
    {
      return static_cast<FlexLayoutParamsImpl*>(&Ui::GetImpl(trait));
    }
    return nullptr;
  }

  /**
   * @brief Retrieves or creates the FlexLayoutParams trait on a view.
   * @param[in] viewImpl The view implementation.
   * @return Reference to the params (newly created if not already attached).
   */
  static FlexLayoutParamsImpl& GetOrCreate(Integration::ViewImpl& viewImpl)
  {
    FlexLayoutParamsImpl* existing = Get(viewImpl);
    if(existing)
    {
      return *existing;
    }
    IntrusivePtr<FlexLayoutParamsImpl> impl(new FlexLayoutParamsImpl());
    Trait                              trait = Trait::DownCast(BaseHandle(impl.Get()));
    viewImpl.SetTrait(Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS, trait);
    return *impl;
  }

protected:
  ~FlexLayoutParamsImpl() override = default;

private:
  float     mFlexGrow;
  float     mFlexShrink;
  float     mFlexBasis;
  FlexAlign mAlignSelf;
};

} // namespace Internal

inline Internal::FlexLayoutParamsImpl& GetImpl(FlexLayoutParams& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::FlexLayoutParamsImpl&>(handle);
}

inline const Internal::FlexLayoutParamsImpl& GetImpl(const FlexLayoutParams& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::FlexLayoutParamsImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
