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
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Primary template — not defined; use one of the specialisations below.
 *
 * Supported forms:
 *   Callback<void()>
 *   Callback<Ret()>
 *   Callback<void(Arg0)>
 *   Callback<Ret(Arg0)>
 *   Callback<void(Arg0, Arg1)>
 *   Callback<Ret(Arg0, Arg1)>
 *   Callback<void(Arg0, Arg1, Arg2)>
 *   Callback<Ret(Arg0, Arg1, Arg2)>
 */
template<typename Signature>
class Callback;

// ---------------------------------------------------------------------------
// 0 parameters, void return
// ---------------------------------------------------------------------------

template<>
class DALI_UI_API Callback<void()>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<void()> New(void (*func)())
  {
    return Callback<void()>(MakeCallback(func));
  }

  template<class T>
  static Callback<void()> New(T* obj, void (T::*func)())
  {
    return Callback<void()>(MakeCallback(obj, func));
  }

  void Invoke()
  {
    CallbackBase::Execute(*mCallback);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 0 parameters, non-void return
// ---------------------------------------------------------------------------

template<typename Ret>
class DALI_UI_API Callback<Ret()>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<Ret()> New(Ret (*func)())
  {
    return Callback<Ret()>(MakeCallback(func));
  }

  template<class T>
  static Callback<Ret()> New(T* obj, Ret (T::*func)())
  {
    return Callback<Ret()>(MakeCallback(obj, func));
  }

  Ret Invoke()
  {
    return CallbackBase::ExecuteReturn<Ret>(*mCallback);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 1 parameter, void return
// ---------------------------------------------------------------------------

template<typename Arg0>
class DALI_UI_API Callback<void(Arg0)>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<void(Arg0)> New(void (*func)(Arg0))
  {
    return Callback<void(Arg0)>(MakeCallback(func));
  }

  template<class T>
  static Callback<void(Arg0)> New(T* obj, void (T::*func)(Arg0))
  {
    return Callback<void(Arg0)>(MakeCallback(obj, func));
  }

  void Invoke(Arg0 arg0)
  {
    CallbackBase::template Execute<Arg0>(*mCallback, arg0);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 1 parameter, non-void return
// ---------------------------------------------------------------------------

template<typename Ret, typename Arg0>
class DALI_UI_API Callback<Ret(Arg0)>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<Ret(Arg0)> New(Ret (*func)(Arg0))
  {
    return Callback<Ret(Arg0)>(MakeCallback(func));
  }

  template<class T>
  static Callback<Ret(Arg0)> New(T* obj, Ret (T::*func)(Arg0))
  {
    return Callback<Ret(Arg0)>(MakeCallback(obj, func));
  }

  Ret Invoke(Arg0 arg0)
  {
    return CallbackBase::template ExecuteReturn<Ret, Arg0>(*mCallback, arg0);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 2 parameters, void return
// ---------------------------------------------------------------------------

template<typename Arg0, typename Arg1>
class DALI_UI_API Callback<void(Arg0, Arg1)>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<void(Arg0, Arg1)> New(void (*func)(Arg0, Arg1))
  {
    return Callback<void(Arg0, Arg1)>(MakeCallback(func));
  }

  template<class T>
  static Callback<void(Arg0, Arg1)> New(T* obj, void (T::*func)(Arg0, Arg1))
  {
    return Callback<void(Arg0, Arg1)>(MakeCallback(obj, func));
  }

  void Invoke(Arg0 arg0, Arg1 arg1)
  {
    CallbackBase::template Execute<Arg0, Arg1>(*mCallback, arg0, arg1);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 2 parameters, non-void return
// ---------------------------------------------------------------------------

template<typename Ret, typename Arg0, typename Arg1>
class DALI_UI_API Callback<Ret(Arg0, Arg1)>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<Ret(Arg0, Arg1)> New(Ret (*func)(Arg0, Arg1))
  {
    return Callback<Ret(Arg0, Arg1)>(MakeCallback(func));
  }

  template<class T>
  static Callback<Ret(Arg0, Arg1)> New(T* obj, Ret (T::*func)(Arg0, Arg1))
  {
    return Callback<Ret(Arg0, Arg1)>(MakeCallback(obj, func));
  }

  Ret Invoke(Arg0 arg0, Arg1 arg1)
  {
    return CallbackBase::template ExecuteReturn<Ret, Arg0, Arg1>(*mCallback, arg0, arg1);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 3 parameters, void return
// ---------------------------------------------------------------------------

template<typename Arg0, typename Arg1, typename Arg2>
class DALI_UI_API Callback<void(Arg0, Arg1, Arg2)>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<void(Arg0, Arg1, Arg2)> New(void (*func)(Arg0, Arg1, Arg2))
  {
    return Callback<void(Arg0, Arg1, Arg2)>(MakeCallback(func));
  }

  template<class T>
  static Callback<void(Arg0, Arg1, Arg2)> New(T* obj, void (T::*func)(Arg0, Arg1, Arg2))
  {
    return Callback<void(Arg0, Arg1, Arg2)>(MakeCallback(obj, func));
  }

  void Invoke(Arg0 arg0, Arg1 arg1, Arg2 arg2)
  {
    CallbackBase::template Execute<Arg0, Arg1, Arg2>(*mCallback, arg0, arg1, arg2);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

// ---------------------------------------------------------------------------
// 3 parameters, non-void return
// ---------------------------------------------------------------------------

template<typename Ret, typename Arg0, typename Arg1, typename Arg2>
class DALI_UI_API Callback<Ret(Arg0, Arg1, Arg2)>
{
public:
  Callback(Callback&&) noexcept            = default;
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  static Callback<Ret(Arg0, Arg1, Arg2)> New(Ret (*func)(Arg0, Arg1, Arg2))
  {
    return Callback<Ret(Arg0, Arg1, Arg2)>(MakeCallback(func));
  }

  template<class T>
  static Callback<Ret(Arg0, Arg1, Arg2)> New(T* obj, Ret (T::*func)(Arg0, Arg1, Arg2))
  {
    return Callback<Ret(Arg0, Arg1, Arg2)>(MakeCallback(obj, func));
  }

  Ret Invoke(Arg0 arg0, Arg1 arg1, Arg2 arg2)
  {
    return CallbackBase::template ExecuteReturn<Ret, Arg0, Arg1, Arg2>(*mCallback, arg0, arg1, arg2);
  }

  const CallbackBase& Get() const
  {
    return *mCallback;
  }

  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* cb)
  : mCallback(cb)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

} // namespace Ui
} // namespace Dali
