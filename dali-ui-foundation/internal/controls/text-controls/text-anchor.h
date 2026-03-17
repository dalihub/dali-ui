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
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class TextAnchorImpl;
}

/**
 * @brief A view which renders anchor (hyperlink) in hypertext.
 */
class TextAnchor : public View
{
public:
  /**
   * @brief The start and end property ranges for this view.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = PROPERTY_REGISTRATION_START_INDEX + 1001,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000 ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextAnchor class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The index of a character in text at which an anchor starts.
       * @details Name "startCharacterIndex", type INTEGER.
       */
      START_CHARACTER_INDEX = PROPERTY_START_INDEX,

      /**
       * @brief The index of a character in text that stands one position after the anchor's last character.
       * @details Name "endCharacterIndex", type INTEGER.
       */
      END_CHARACTER_INDEX,

      /**
       * @brief The URI associated with an anchor.
       * @details Name "uri", type STRING.
       */
      URI
    };
  };

public:
  /**
   * @brief Creates an empty handle.
   */
  TextAnchor();

  /**
   * @brief Creates the TextAnchor view.
   * @return A handle to the TextAnchor view.
   */
  static TextAnchor New();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextAnchor(const TextAnchor& handle);

  /**
   * @brief Move constructor.
   *
   * @param[in] handle The handle to move from.
   */
  TextAnchor(TextAnchor&& handle);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextAnchor();

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextAnchor& operator=(const TextAnchor& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] handle The handle to move from.
   * @return A reference to this.
   */
  TextAnchor& operator=(TextAnchor&& handle);

  /**
   * @brief Downcast a handle to TextAnchor.
   *
   * If the BaseHandle points is a TextAnchor the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextAnchor or an empty handle
   */
  static TextAnchor DownCast(BaseHandle handle);

public: // Not intended for application developers
  /**
   * @brief Creates a handle using the Ui::Internal implementation.
   *
   * @param[in] implementation The View implementation.
   */
  DALI_INTERNAL TextAnchor(Internal::TextAnchorImpl& implementation);

  /**
   * @brief Allows the creation of this View from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextAnchor(Dali::Internal::CustomActor* internal);

}; // Class TextAnchor

} // namespace Ui

} // namespace Dali
