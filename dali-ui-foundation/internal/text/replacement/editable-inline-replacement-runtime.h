#ifndef DALI_UI_EDITABLE_INLINE_REPLACEMENT_RUNTIME_H
#define DALI_UI_EDITABLE_INLINE_REPLACEMENT_RUNTIME_H

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
 * @brief Enumerates the actions for an editable replacement runtime update.
 */
enum class EditableInlineReplacementUpdate
{
  CLEAR,
  WAIT_FOR_LAYOUT,
  APPLY
};

/**
 * @brief Resolves the runtime action for authored and rendered replacement state.
 *
 * @param[in] source The current authored replacement source.
 * @param[in] state The current replacement render state.
 * @param[in] suppressed Whether replacement rendering is explicitly suppressed.
 * @return The action for the editable replacement runtime.
 */
EditableInlineReplacementUpdate ResolveEditableInlineReplacementUpdate(
  const Ui::Text::ReplacementSourceSnapshot& source,
  const Ui::Text::ReplacementRenderState&    state,
  bool                                       suppressed);

/**
 * @brief Stores the on-demand ImageVisual runtime of an editable text control.
 */
struct EditableInlineReplacementRuntime
{
  explicit EditableInlineReplacementRuntime(Ui::View owner);
  ~EditableInlineReplacementRuntime();

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
 * @brief Gets an editable inline replacement runtime.
 *
 * @param[in] owner The visual owner.
 * @return The runtime, or nullptr.
 */
EditableInlineReplacementRuntime* GetEditableInlineReplacementRuntime(Ui::View owner);

/**
 * @brief Gets an editable inline replacement runtime during owner destruction.
 *
 * @param[in] owner The visual owner implementation.
 * @return The runtime, or nullptr.
 */
EditableInlineReplacementRuntime* GetEditableInlineReplacementRuntime(Ui::ViewImpl& owner);

/**
 * @brief Gets or creates an editable inline replacement runtime.
 *
 * @param[in] owner The visual owner.
 * @return The runtime.
 */
EditableInlineReplacementRuntime& GetOrCreateEditableInlineReplacementRuntime(Ui::View owner);

/**
 * @brief Removes an editable inline replacement runtime.
 *
 * @param[in] owner The visual owner.
 */
void RemoveEditableInlineReplacementRuntime(Ui::View owner);

/**
 * @brief Removes an editable inline replacement runtime during owner destruction.
 *
 * @param[in] owner The visual owner implementation.
 */
void RemoveEditableInlineReplacementRuntime(Ui::ViewImpl& owner);

} // namespace Dali::Ui::Internal::Text

#endif // DALI_UI_EDITABLE_INLINE_REPLACEMENT_RUNTIME_H
