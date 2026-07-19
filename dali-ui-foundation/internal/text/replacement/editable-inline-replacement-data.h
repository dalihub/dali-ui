#ifndef DALI_UI_EDITABLE_INLINE_REPLACEMENT_DATA_H
#define DALI_UI_EDITABLE_INLINE_REPLACEMENT_DATA_H

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
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui
{
class ViewImpl;
}

namespace Dali::Ui::Text
{
struct ReplacementRenderState;
}

namespace Dali::Ui::Internal::Text
{
/**
 * @brief Enumerates actions for updating editable inline replacement data.
 */
enum class EditableInlineReplacementUpdate
{
  CLEAR,
  WAIT_FOR_LAYOUT,
  APPLY
};

/**
 * @brief Resolves the update action for authored and rendered replacement state.
 *
 * @param[in] source The current authored replacement source.
 * @param[in] state The current replacement render state.
 * @param[in] suppressed Whether replacement rendering is explicitly suppressed.
 * @return The editable inline replacement update action.
 */
EditableInlineReplacementUpdate ResolveEditableInlineReplacementUpdate(
  const Ui::Text::ReplacementSourceSnapshot& source,
  const Ui::Text::ReplacementRenderState&    state,
  bool                                       suppressed);

/**
 * @brief Stores on-demand inline replacement data for an editable text control.
 */
struct EditableInlineReplacementData
{
  /**
   * @brief Creates inline replacement data for an editable visual owner.
   *
   * @param[in] owner The visual owner.
   */
  explicit EditableInlineReplacementData(Ui::View owner);

  /**
   * @brief Destroys the inline replacement data and its visual layer.
   */
  ~EditableInlineReplacementData();

  /**
   * @brief Places the visual layer in the editable content hierarchy.
   *
   * @param[in] contentParent The clipped content parent.
   * @param[in] textActor The text render actor.
   * @param[in] cursorLayer The clipped cursor layer.
   * @param[in] contentSize The editable content size.
   */
  void PlaceVisualLayer(Actor contentParent, Actor textActor, Actor cursorLayer, const Vector2& contentSize);

  Ui::View visualLayer;

  InlineReplacementViewHost host;
  InlineReplacementManager  manager;
  bool                      resourceReadyConnected{false};
};

/**
 * @brief Gets editable inline replacement data.
 *
 * @param[in] owner The visual owner.
 * @return The data, or nullptr.
 */
EditableInlineReplacementData* GetEditableInlineReplacementData(Ui::View owner);

/**
 * @brief Gets editable inline replacement data from a visual owner implementation.
 *
 * @param[in] owner The visual owner implementation.
 * @return The data, or nullptr.
 */
EditableInlineReplacementData* GetEditableInlineReplacementData(Ui::ViewImpl& owner);

/**
 * @brief Gets or creates editable inline replacement data.
 *
 * @param[in] owner The visual owner.
 * @return The data.
 */
EditableInlineReplacementData& GetOrCreateEditableInlineReplacementData(Ui::View owner);

/**
 * @brief Removes editable inline replacement data.
 *
 * @param[in] owner The visual owner.
 */
void RemoveEditableInlineReplacementData(Ui::View owner);

/**
 * @brief Removes editable inline replacement data from a visual owner implementation.
 *
 * @param[in] owner The visual owner implementation.
 */
void RemoveEditableInlineReplacementData(Ui::ViewImpl& owner);

} // namespace Dali::Ui::Internal::Text

#endif // DALI_UI_EDITABLE_INLINE_REPLACEMENT_DATA_H
