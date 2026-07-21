#ifndef DALI_UI_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H
#define DALI_UI_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/internal/text/rendering/text-renderer.h>
#include <dali-ui-foundation/internal/text/text-model.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace Dali::Ui::Internal
{
class CommonTextUtils
{
public:
  /**
   * Common method to render text, setting up background, foreground actors with decorators/stencil.
   * @param[in] textActor The InputEditor or InputField
   * @param[in] renderer pointer to the text renderer
   * @param[in] controller pointer to the text controller
   * @param[in] decorator pointer to the text decorator
   * @param[in,out] alignmentOffset Alignment offset
   * @param[in,out] renderableActor Actor for rendering text
   * @param[in,out] backgroundActor Actor for rendering background
   * @param[in,out] cursorLayerActor Actor for rendering the cursor
   * @param[in,out] stencil Clipping actor
   * @param[in,out] clippingDecorationActors Clipping decoration actors
   * @param[in,out] anchorActors Anchor actors
   * @param[in] updateTextType How the text has been updated
   * @param[in] atlasFrameState Atlas gradient state for the current frame
   * @param[in] viewSize Current view size
   */
  static void RenderText(Actor textActor, Ui::Text::RendererPtr renderer, Ui::Text::ControllerPtr controller,
                         Ui::Text::DecoratorPtr decorator, float& alignmentOffset, Actor& renderableActor,
                         Actor& backgroundActor, Actor& cursorLayerActor, Actor& stencil,
                         std::vector<Actor>& clippingDecorationActors, std::vector<Ui::TextAnchor>& anchorActors,
                         Ui::Text::Controller::UpdateTextType                 updateTextType,
                         const Ui::Text::Internal::Gradient::AtlasFrameState& atlasFrameState,
                         const Vector2&                                       viewSize);

  /**
   * @brief Synchronizes actor scrolling and atlas gradient bounds without rebuilding glyph geometry.
   */
  static void UpdateTextRenderPosition(Actor textActor, Ui::Text::RendererPtr renderer, Ui::Text::ControllerPtr controller,
                                       float alignmentOffset, Actor renderableActor, Actor stencil,
                                       const Ui::Text::Internal::Gradient::AtlasFrameState& atlasFrameState,
                                       const Vector2&                                       viewSize);

  /**
   * Common method to synchronize TextAnchor actors with Anchor objects in text's logical model.
   * @param[in] parent The actor that is a parent of anchor actors
   * @param[in] controller pointer to the text controller
   * @param[in,out] anchorActors Anchor actors
   */
  static void SynchronizeTextAnchorsInParent(Actor parent, Ui::Text::ControllerPtr controller,
                                             std::vector<Ui::TextAnchor>& anchorActors);

  /**
   * @brief Gets the bounding box of a specific text range.
   *
   * @param[in] model pointer to the text model.
   * @param[in] startIndex start index of the text requested to get bounding box to.
   * @param[in] endIndex end index(included) of the text requested to get bounding box to.
   * @return bounding box of the requested text.
   */
  static Bounds GetTextBoundingRectangle(Ui::Text::ModelPtr model, TextAbstraction::CharacterIndex startIndex,
                                         TextAbstraction::CharacterIndex endIndex);
};

} // namespace Dali::Ui::Internal

#endif // DALI_UI_INTERNAL_TEXT_CONTROLS_COMMON_TEXT_UTILS_H
