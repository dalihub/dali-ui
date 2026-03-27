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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

class LayoutCallbacksImpl;

/**
 * @brief Internal handle for LayoutCallbacks trait.
 */
class LayoutCallbacksTrait : public Trait
{
public:
  LayoutCallbacksTrait() = default;

  static LayoutCallbacksTrait New(LayoutCallbacksImpl* impl);

private:
  explicit LayoutCallbacksTrait(LayoutCallbacksImpl* impl);
};

/**
 * @brief Internal implementation of LayoutCallbacks trait.
 *
 * Stores measure/arrange callbacks as CallbackBase pointers.
 */
class LayoutCallbacksImpl : public Integration::TraitImpl
{
public:
  LayoutCallbacksImpl();

  void SetMeasureCallback(CallbackBase* callback);
  void SetArrangeCallback(CallbackBase* callback);

  bool HasMeasureCallback() const;
  bool HasArrangeCallback() const;

  MeasuredSize InvokeMeasure(View view, float widthConstraint, float heightConstraint);
  MeasuredSize InvokeArrange(View view, const LayoutRect& bounds);

protected:
  ~LayoutCallbacksImpl() override;

  void OnBeforeAttached(Integration::TraitId id, View& view) override;
  void OnAttached(Integration::TraitId id, View& view) override;
  void OnDetached(Integration::TraitId id, View& view) override;
  void OnViewDestroying(Integration::ViewImpl* viewImpl) override;

private:
  UniquePtr<CallbackBase> mOnMeasure;
  UniquePtr<CallbackBase> mOnArrange;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
