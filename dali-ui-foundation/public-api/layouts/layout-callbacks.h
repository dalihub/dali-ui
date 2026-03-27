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
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{

namespace Ui
{

class View;

/**
 * @brief Callback wrapper for custom layout measurement.
 *
 * Created via the template New() factory which accepts a member function
 * or static function. Follows the same pattern as DALi's RenderCallback
 * and VertexBufferUpdateCallback.
 *
 * @code
 * // Static function
 * auto cb = LayoutMeasureCallback::New(&MyClass::OnMeasure);
 *
 * // Member function
 * auto cb = LayoutMeasureCallback::New(this, &MyClass::OnMeasure);
 * @endcode
 *
 * @note Signature: MeasuredSize(View view, float widthConstraint, float heightConstraint)
 */
class DALI_UI_API LayoutMeasureCallback
{
public:
  /**
   * @brief Member function type.
   */
  template<class T>
  using FuncType = MeasuredSize (T::*)(View, float, float);

  /**
   * @brief Creates a callback from a member function.
   *
   * @param[in] object The object instance
   * @param[in] func The member function
   * @return A unique pointer to the callback
   */
  template<class T>
  static ::Dali::UniquePtr<LayoutMeasureCallback> New(T* object, FuncType<T> func)
  {
    return New(MakeCallback(object, func));
  }

  /**
   * @brief Creates a callback from a static function.
   *
   * @param[in] func The static function
   * @return A unique pointer to the callback
   */
  static ::Dali::UniquePtr<LayoutMeasureCallback> New(MeasuredSize (*func)(View, float, float))
  {
    return New(MakeCallback(func));
  }

  /**
   * @brief Returns the internal CallbackBase.
   *
   * Releases ownership — caller takes ownership of the returned pointer.
   * @return The CallbackBase pointer
   */
  CallbackBase* GetCallback()
  {
    return mCallback.Release();
  }

private:
  static ::Dali::UniquePtr<LayoutMeasureCallback> New(CallbackBase* callback);

  LayoutMeasureCallback(CallbackBase* callback)
  : mCallback(callback)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

/**
 * @brief Callback wrapper for custom layout arrangement.
 *
 * @note Signature: MeasuredSize(View view, const LayoutRect& bounds)
 */
class DALI_UI_API LayoutArrangeCallback
{
public:
  /**
   * @brief Member function type.
   */
  template<class T>
  using FuncType = MeasuredSize (T::*)(View, const LayoutRect&);

  /**
   * @brief Creates a callback from a member function.
   */
  template<class T>
  static ::Dali::UniquePtr<LayoutArrangeCallback> New(T* object, FuncType<T> func)
  {
    return New(MakeCallback(object, func));
  }

  /**
   * @brief Creates a callback from a static function.
   */
  static ::Dali::UniquePtr<LayoutArrangeCallback> New(MeasuredSize (*func)(View, const LayoutRect&))
  {
    return New(MakeCallback(func));
  }

  /**
   * @brief Returns the internal CallbackBase.
   *
   * Releases ownership — caller takes ownership of the returned pointer.
   * @return The CallbackBase pointer
   */
  CallbackBase* GetCallback()
  {
    return mCallback.Release();
  }

private:
  static ::Dali::UniquePtr<LayoutArrangeCallback> New(CallbackBase* callback);

  LayoutArrangeCallback(CallbackBase* callback)
  : mCallback(callback)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

} // namespace Ui

} // namespace Dali
