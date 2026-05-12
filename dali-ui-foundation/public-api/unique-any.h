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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/type-info-id.h>
#include <typeinfo>

namespace Dali
{
namespace Ui
{

/**
 * @brief Type-erased object stored as an attachment.
 *
 * UniqueAny is move-only. It owns one stored value and can hold copyable
 * and move-only value types.
 *
 * @note Use a stable namespace-scope type as the stored value type. Local types
 * and anonymous-namespace types should not be used when the value may be
 * retrieved from another translation unit, because they may not have a stable
 * cross-translation-unit type identity.
 */
class DALI_UI_API UniqueAny
{
public:
  /**
   * @brief Creates an empty UniqueAny.
   */
  UniqueAny();

  /**
   * @brief Creates a UniqueAny that owns @p value.
   *
   * The stored value is constructed from @p value. Passing an rvalue moves the
   * value into the UniqueAny, so move-only value types are supported.
   *
   * @tparam T The value type to store
   * @param[in] value The value to store
   */
  template<typename T>
  explicit UniqueAny(T&& value)
  : mHolder(new Holder<typename Dali::DecayType<T>::type>(Dali::Forward<T>(value)))
  {
  }

  /**
   * @brief Move constructor.
   */
  UniqueAny(UniqueAny&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   */
  UniqueAny& operator=(UniqueAny&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~UniqueAny();

  /**
   * @brief Checks whether this UniqueAny stores a value.
   *
   * @return True if no value is stored
   */
  bool Empty() const;

  /**
   * @brief Checks whether this UniqueAny stores a value.
   *
   * @return True if a value is stored
   */
  explicit operator bool() const;

  /**
   * @brief Clears the stored value.
   */
  void Reset();

  /**
   * @brief Gets a pointer to the stored value.
   *
   * The returned pointer is owned by this UniqueAny. The caller must not delete
   * it and must not use it after this UniqueAny is destroyed or moved from.
   *
   * @return Pointer to the stored value, or nullptr if the type does not match
   */
  template<typename T>
  typename Dali::DecayType<T>::type* Get()
  {
    using ValueType = typename Dali::DecayType<T>::type;
    return IsType<ValueType>() ? &static_cast<Holder<ValueType>*>(mHolder)->mValue : nullptr;
  }

  /**
   * @brief Gets a const pointer to the stored value.
   *
   * The returned pointer is owned by this UniqueAny. The caller must not delete
   * it and must not use it after this UniqueAny is destroyed or moved from.
   *
   * @return Pointer to the stored value, or nullptr if the type does not match
   */
  template<typename T>
  const typename Dali::DecayType<T>::type* Get() const
  {
    using ValueType = typename Dali::DecayType<T>::type;
    return IsType<ValueType>() ? &static_cast<const Holder<ValueType>*>(mHolder)->mValue : nullptr;
  }

  /**
   * @brief Detaches the stored value from this UniqueAny.
   *
   * If the stored value type matches @p T, ownership is transferred to the
   * returned UniquePtr and this UniqueAny becomes empty. If the type does not
   * match, this UniqueAny is left unchanged.
   *
   * @return The detached value, or nullptr if the type does not match
   */
  template<typename T>
  Dali::UniquePtr<typename Dali::DecayType<T>::type> Detach()
  {
    using ValueType = typename Dali::DecayType<T>::type;
    if(!IsType<ValueType>())
    {
      return Dali::UniquePtr<ValueType>();
    }

    Dali::UniquePtr<Holder<ValueType>> holder(static_cast<Holder<ValueType>*>(mHolder));
    mHolder = nullptr;
    return Dali::UniquePtr<ValueType>(new ValueType(Dali::Move(holder->mValue)));
  }

private:
  struct HolderBase
  {
    virtual ~HolderBase();
    virtual const TypeInfoId& GetType() const = 0;
  };

  template<typename T>
  struct Holder : public HolderBase
  {
    template<typename U>
    explicit Holder(U&& value)
    : mValue(Dali::Forward<U>(value))
    {
    }

    const TypeInfoId& GetType() const override
    {
      return GetTypeId<T>();
    }

    T mValue;
  };

  template<typename T>
  bool IsType() const
  {
    return mHolder && mHolder->GetType() == GetTypeId<T>();
  }

  template<typename T>
  static const TypeInfoId& GetTypeId()
  {
    static const Dali::String typeName = DemangleTypeName(typeid(T).name());
    static const TypeInfoId   typeId(Dali::HashFNV1a(typeName.CStr()), typeName.CStr());
    return typeId;
  }

  static DALI_UI_API Dali::String DemangleTypeName(const char* mangledName);

  UniqueAny(const UniqueAny&)            = delete;
  UniqueAny& operator=(const UniqueAny&) = delete;

  HolderBase* mHolder;
};

} // namespace Ui
} // namespace Dali
