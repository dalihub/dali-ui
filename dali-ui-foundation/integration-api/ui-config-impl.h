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
#include <cstdint>
#include <string>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-config.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class UiConfigImpl;
using UiConfigImplPtr = IntrusivePtr<UiConfigImpl>;

/**
 * @brief Internal implementation of UiConfig.
 *
 * Supports a "freeze" mechanism: once Freeze() is called (by UiConfigManager::Init()),
 * all subsequent setter calls will trigger DALI_ASSERT_ALWAYS.
 */
class DALI_UI_API UiConfigImpl : public BaseObject
{
public:
  /**
   * @brief Creates a new UiConfig implementation.
   *
   * @return An IntrusivePtr to the new UiConfigImpl
   */
  static UiConfigImplPtr New();

  /**
   * @brief Marks this config as frozen.
   *
   * Setter calls after this point will trigger an assertion failure.
   */
  void Freeze();

  /**
   * @brief Returns whether this config is frozen.
   *
   * @return True if frozen
   */
  bool IsFrozen() const;

  /**
   * @brief Sets the scaling factor.
   *
   * @pre Must not be frozen.
   * @param[in] scalingFactor The scaling factor
   */
  void SetScalingFactor(float scalingFactor);

  /**
   * @brief Retrieves the scaling factor.
   *
   * @return The scaling factor
   */
  float GetScalingFactor() const;

  /**
   * @brief Sets the screen DPI.
   *
   * @pre Must not be frozen.
   * @param[in] dpi The DPI value
   */
  void SetDpi(int dpi);

  /**
   * @brief Retrieves the screen DPI.
   *
   * @return The DPI value
   */
  int GetDpi() const;

  /**
   * @brief Sets the baseline DPI.
   *
   * @pre Must not be frozen.
   * @param[in] baselineDpi The baseline DPI
   */
  void SetBaselineDpi(int baselineDpi);

  /**
   * @brief Retrieves the baseline DPI.
   *
   * @return The baseline DPI value
   */
  int GetBaselineDpi() const;

  /**
   * @brief Computes the DPI factor (dpi / baselineDpi).
   *
   * @return The computed DPI factor
   */
  float GetDpiFactor() const;

  /**
   * @brief Computes the scaled DPI factor (dpiFactor * scalingFactor).
   *
   * @return The computed scaled DPI factor
   */
  float GetScaledDpiFactor() const;

  /**
   * @brief Sets the default key click policy.
   *
   * @pre Must not be frozen.
   * @param[in] policy The key click policy
   */
  void SetKeyClickPolicy(KeyClickPolicy policy);

  /**
   * @brief Retrieves the default key click policy.
   *
   * @return The key click policy
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @brief Sets the execution key predicate.
   *
   * @pre Must not be frozen.
   * @param[in] predicate A function pointer with signature bool(const std::string&)
   */
  void SetExecutionKeyPredicate(ExecutionKeyPredicate predicate);

  /**
   * @brief Retrieves the execution key predicate.
   *
   * @return The current execution key predicate function pointer
   */
  ExecutionKeyPredicate GetExecutionKeyPredicate() const;

  /**
   * @brief Sets the minimum key repeat count for long-press.
   *
   * @pre Must not be frozen.
   * @param[in] count The minimum repeat count
   */
  void SetMinLongPressKeyCount(uint32_t count);

  /**
   * @brief Retrieves the minimum key repeat count for long-press.
   *
   * @return The minimum repeat count
   */
  uint32_t GetMinLongPressKeyCount() const;

  /**
   * @brief Sets the tap recognizer time limit in milliseconds.
   *
   * @pre Must not be frozen.
   * @param[in] timeMs The time limit in milliseconds
   */
  void SetTapRecognizerTime(uint32_t timeMs);

  /**
   * @brief Retrieves the tap recognizer time limit.
   *
   * @return The time limit in milliseconds
   */
  uint32_t GetTapRecognizerTime() const;

  /**
   * @brief Called after this config is applied via UiConfig::Apply().
   *
   * Derived config implementations override this to register themselves
   * with their own layer-specific config manager.
   * The base implementation does nothing.
   */
  virtual void OnInitialized();

protected:
  /**
   * @brief Constructor with default configuration values.
   */
  UiConfigImpl();

  /**
   * @brief Destructor.
   */
  ~UiConfigImpl() override;

private:
  UiConfigImpl(const UiConfigImpl&) = delete;
  UiConfigImpl(UiConfigImpl&&) = delete;
  UiConfigImpl& operator=(const UiConfigImpl&) = delete;
  UiConfigImpl& operator=(UiConfigImpl&&) = delete;

private:
  ExecutionKeyPredicate mExecutionKeyPredicate;
  float mScalingFactor;
  int mDpi;
  int mBaselineDpi;
  KeyClickPolicy mKeyClickPolicy;
  uint32_t mMinLongPressKeyCount;
  uint32_t mTapRecognizerTime;
  bool mFrozen;
};

} // namespace Integration

/**
 * @brief Retrieves the UiConfigImpl from a UiConfig handle.
 *
 * @param[in] obj The UiConfig handle
 * @return A reference to the internal implementation
 */
inline Integration::UiConfigImpl& GetImpl(UiConfig& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UiConfigImpl&>(handle);
}

/**
 * @brief Retrieves the UiConfigImpl from a const UiConfig handle.
 *
 * @param[in] obj The UiConfig handle
 * @return A const reference to the internal implementation
 */
inline const Integration::UiConfigImpl& GetImpl(const UiConfig& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UiConfigImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
