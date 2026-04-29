#ifndef DALI_HIDDEN_TEXT_H
#define DALI_HIDDEN_TEXT_H

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
#include <dali/public-api/adaptor-framework/timer.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Default password mask character.
 */
static constexpr uint32_t DEFAULT_PASSWORD_MASK_CHARACTER = 0x2A; // '*'

/**
 * @brief Default duration for revealing the last entered password character.
 */
static constexpr int DEFAULT_PASSWORD_REVEAL_DURATION = 1000;

/**
 * @brief Class to handle hidden or password-style text substitution.
 */
class HiddenText : public ConnectionTracker
{
public:
  /**
   * @brief Internal hidden text display mode.
   *
   * This preserves the previous hidden input behavior without exposing
   * property-map based HiddenInput settings.
   */
  enum class Mode : uint8_t
  {
    /**
     * @brief Displays the source text without masking.
     */
    NONE = 0,

    /**
     * @brief Masks all characters.
     */
    HIDE_ALL,

    /**
     * @brief Masks the first configured number of characters.
     */
    HIDE_COUNT,

    /**
     * @brief Shows the first configured number of characters and masks the rest.
     */
    SHOW_COUNT,

    /**
     * @brief Temporarily reveals the last entered character before masking it.
     */
    REVEAL_LAST_CHARACTER
  };

  class Observer
  {
  public:
    /**
     * @brief Invoked when the time to reveal the last character has expired.
     */
    virtual void DisplayTimeExpired() = 0;
  };

  /**
   * @brief Constructor.
   *
   * @param[in] observer The observer.
   */
  HiddenText(Observer* observer);

public: // Password API
  /**
   * @brief Sets the password display mode.
   *
   * @param[in] mode The password display mode.
   */
  void SetPasswordMode(PasswordMode mode);

  /**
   * @brief Gets the password display mode.
   *
   * If the current hidden text mode cannot be represented by PasswordMode,
   * PasswordMode::NONE is returned.
   *
   * @return The password display mode.
   */
  PasswordMode GetPasswordMode() const;

  /**
   * @brief Sets the character used to mask password text.
   *
   * @param[in] character The Unicode code point used as the password mask character.
   */
  void SetPasswordMaskCharacter(uint32_t character);

  /**
   * @brief Gets the character used to mask password text.
   *
   * @return The Unicode code point used as the password mask character.
   */
  uint32_t GetPasswordMaskCharacter() const;

  /**
   * @brief Sets the duration for which the last entered password character remains visible.
   *
   * This value is used when PasswordMode is REVEAL_LAST_CHARACTER.
   *
   * @param[in] duration The duration in milliseconds.
   */
  void SetPasswordRevealDuration(int duration);

  /**
   * @brief Gets the duration for which the last entered password character remains visible.
   *
   * @return The duration in milliseconds.
   */
  int GetPasswordRevealDuration() const;

public: // Hidden text behavior
  /**
   * @brief Clears hidden text substitution.
   */
  void ClearHiddenText();

  /**
   * @brief Masks all characters.
   */
  void HideAll();

  /**
   * @brief Masks the first count characters.
   *
   * Characters after count remain visible.
   *
   * @param[in] count The number of characters to mask from the start.
   */
  void HideFirstCharacters(uint32_t count);

  /**
   * @brief Shows the first count characters and masks the remaining characters.
   *
   * @param[in] count The number of characters to show from the start.
   */
  void ShowFirstCharacters(uint32_t count);

  /**
   * @brief Temporarily reveals the last entered character before masking it.
   */
  void RevealLastCharacter();

  /**
   * @brief Gets the current hidden text mode.
   *
   * @return The current hidden text mode.
   */
  Mode GetMode() const;

  /**
   * @brief Gets the character count used by HIDE_COUNT and SHOW_COUNT modes.
   *
   * @return The configured character count.
   */
  uint32_t GetSubstituteCount() const;

public:
  /**
   * @brief Converts source text to destination text according to the current mode.
   *
   * @param[in] source The original text.
   * @param[out] destination The substituted text.
   * @param[in] currentCursorIndex The current cursor index.
   */
  void Substitute(const Vector<Character>& source, Vector<Character>& destination, Length currentCursorIndex);

  /**
   * @brief Initializes the previous text count.
   */
  void InitPreviousTextCount();

  /**
   * @brief Invoked when the timer expires.
   *
   * @return False to stop the timer.
   */
  bool OnTick();

private:
  /**
   * @brief Converts PasswordMode to the internal hidden text mode.
   *
   * @param[in] mode The password display mode.
   * @return The hidden text mode.
   */
  Mode ToHiddenTextMode(PasswordMode mode) const;

  /**
   * @brief Converts an internal hidden text mode to PasswordMode.
   *
   * @param[in] mode The hidden text mode.
   * @return The password display mode.
   */
  PasswordMode ToPasswordMode(Mode mode) const;

private:
  Timer     mTimer;
  Observer* mObserver;

  Mode mMode;

  uint32_t mPasswordMaskCharacter;
  int      mPasswordRevealDuration;

  // Used by HIDE_COUNT and SHOW_COUNT.
  uint32_t mSubstituteCount;

  Length mPreviousTextCount;
  bool   mIsLastCharacterRevealed;
};

} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_HIDDEN_TEXT_H
