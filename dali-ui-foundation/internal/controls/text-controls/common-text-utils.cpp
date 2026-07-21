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
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h> // LCOV_EXCL_LINE
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/internal/controls/text-controls/common-text-utils.h>
#include <dali-ui-foundation/internal/text/text-atlas-gradient-state.h>
#include <dali-ui-foundation/internal/text/text-geometry.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace IntegrationView = Dali::Ui::Integration::View;

namespace Dali::Ui::Internal
{
namespace
{
Vector2 CalculateRenderablePosition(Actor textActor, Ui::Text::ControllerPtr controller, float alignmentOffset, Actor stencil)
{
  const Vector2& scrollOffset = controller->GetRenderTextModel()->GetScrollPosition();
  if(stencil)
  {
    return Vector2(scrollOffset.x + alignmentOffset, scrollOffset.y);
  }

  Vector4    propertyPadding = textActor.GetProperty<Vector4>(Ui::View::Property::PADDING);
  Insets     padding(propertyPadding.x, propertyPadding.y, propertyPadding.z, propertyPadding.w);
  const auto layoutDirection = static_cast<Dali::LayoutDirection::Type>(
    textActor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }
  return Vector2(scrollOffset.x + alignmentOffset + padding.start, scrollOffset.y + padding.top);
}

void UpdateAtlasGradient(Ui::Text::RendererPtr                                renderer,
                         Ui::Text::ControllerPtr                              controller,
                         Actor                                                stencil,
                         const Vector2&                                       renderablePosition,
                         float                                                minLineOffset,
                         const Ui::Text::Internal::Gradient::AtlasFrameState& state,
                         const Vector2&                                       viewSize)
{
  if(!renderer || !state.enabled)
  {
    return;
  }

  const Vector2 layoutSize = controller->GetView().GetLayoutSize();
  Vector4       bounds;
  if(state.boundsMode == Ui::Text::GradientBoundsMode::VIEW_BOUND)
  {
    Vector2 contentOffset = renderablePosition;
    if(stencil)
    {
      const Vector3 stencilPosition = stencil.GetProperty<Vector3>(Actor::Property::POSITION);
      contentOffset += Vector2(stencilPosition.x, stencilPosition.y);
    }
    bounds = Ui::Text::Internal::CalculateGradientViewBounds(layoutSize, viewSize, contentOffset);
  }
  else
  {
    const auto model = controller->GetRenderTextModel();
    bounds           = Ui::Text::Internal::CalculateAtlasGradientContentBounds(
      layoutSize, model->GetLines(), model->GetNumberOfLines(), minLineOffset);
  }

  renderer->UpdateAtlasGradient(layoutSize, bounds);
}
} // namespace

Bounds CommonTextUtils::GetTextBoundingRectangle(Ui::Text::ModelPtr model, TextAbstraction::CharacterIndex startIndex,
                                                 TextAbstraction::CharacterIndex endIndex)
{
  Vector<Vector2> sizeList;
  Vector<Vector2> positionList;

  GetTextGeometry(model, startIndex, endIndex, sizeList, positionList);

  if(sizeList.Empty() || sizeList.Size() != positionList.Size())
  {
    return {0, 0, 0, 0};
  }

  auto controlWidth = model->mVisualModel->mControlSize.width;
  auto minX         = positionList[0].x;
  auto minY         = positionList[0].y;
  auto maxRight     = positionList[0].x + sizeList[0].x;
  auto maxBottom    = positionList[0].y + sizeList[0].y;

  for(unsigned int i = 1; i < sizeList.Size(); i++)
  {
    minX      = std::min(minX, positionList[i].x);
    minY      = std::min(minY, positionList[i].y);
    maxRight  = std::max(maxRight, positionList[i].x + sizeList[i].x);
    maxBottom = std::max(maxBottom, positionList[i].y + sizeList[i].y);
  }

  if(minX < 0.0f)
  {
    minX = 0.0f;
  }

  if(maxRight > controlWidth)
  {
    maxRight = controlWidth;
  }

  return {minX, minY, maxRight - minX, maxBottom - minY};
}

void CommonTextUtils::SynchronizeTextAnchorsInParent(Actor parent, Ui::Text::ControllerPtr controller,
                                                     std::vector<Ui::TextAnchor>& anchorActors)
{
  for(auto& anchorActor : anchorActors)
  {
    parent.Remove(anchorActor);
  }
  anchorActors.clear();

  if(Dali::Integration::Accessibility::IsUp()) // LCOV_EXCL_LINE
  {
    controller->GetAnchorActors(anchorActors);
    for(auto& anchorActor : anchorActors)
    {
      parent.Add(anchorActor);
    }
  }
}

void CommonTextUtils::RenderText(Actor textActor, Ui::Text::RendererPtr renderer, Ui::Text::ControllerPtr controller,
                                 Ui::Text::DecoratorPtr decorator, float& alignmentOffset, Actor& renderableActor,
                                 Actor& backgroundActor, Actor& cursorLayerActor, Actor& stencil,
                                 std::vector<Actor>&                                  clippingDecorationActors,
                                 std::vector<Ui::TextAnchor>&                         anchorActors,
                                 Ui::Text::Controller::UpdateTextType                 updateTextType,
                                 const Ui::Text::Internal::Gradient::AtlasFrameState& atlasFrameState,
                                 const Vector2&                                       viewSize)
{
  Actor newRenderableActor;

  if(Ui::Text::Controller::NONE_UPDATED != (Ui::Text::Controller::MODEL_UPDATED & updateTextType))
  {
    if(renderer)
    {
      newRenderableActor = renderer->Render(controller->GetView(), textActor,
                                            Property::INVALID_INDEX, // Animatable property not supported
                                            alignmentOffset, Dali::Ui::Integration::DepthIndex::CONTENT);
    }

    if(renderableActor != newRenderableActor)
    {
      UnparentAndReset(backgroundActor);
      UnparentAndReset(renderableActor);
      renderableActor = newRenderableActor;

      if(renderableActor)
      {
        backgroundActor = controller->CreateBackgroundActor();
      }
    }
  }

  if(renderableActor)
  {
    const Vector2 renderableActorPosition = CalculateRenderablePosition(textActor, controller, alignmentOffset, stencil);
    renderableActor.SetProperty(Actor::Property::POSITION, renderableActorPosition);
    UpdateAtlasGradient(renderer, controller, stencil, renderableActorPosition, alignmentOffset, atlasFrameState, viewSize);

    // Make sure the actors are parented correctly with/without clipping.
    // When stencil is null, `self` is the text View itself; use the Integration
    // helper so the View-only OnChildAdd assert is bypassed for these internal
    // text-rendering Actors.
    Actor self = stencil ? stencil : textActor;

    auto addChild = [&](Actor child)
    {
      if(stencil)
      {
        self.Add(child);
      }
      else
      {
        IntegrationView::AddActorChild(Ui::View::DownCast(self), child);
      }
    };

    Actor highlightActor;

    for(std::vector<Actor>::iterator it = clippingDecorationActors.begin(), endIt = clippingDecorationActors.end();
        it != endIt; ++it)
    {
      addChild(*it);
      it->LowerToBottom();

      if(it->GetProperty(Dali::Actor::Property::NAME) == Dali::String("HighlightActor"))
      {
        highlightActor = *it;
      }
    }
    clippingDecorationActors.clear();

    addChild(renderableActor);

    if(backgroundActor)
    {
      if(decorator && decorator->IsHighlightVisible())
      {
        addChild(backgroundActor);
        backgroundActor.SetProperty(
          Actor::Property::POSITION,
          renderableActorPosition); // In input field's coords.
        backgroundActor.LowerBelow(highlightActor);
      }
      else
      {
        renderableActor.Add(backgroundActor);
        backgroundActor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f)); // In renderable actor's coords.
        backgroundActor.LowerToBottom();
      }
    }

    if(cursorLayerActor)
    {
      cursorLayerActor.RaiseToTop();
    }

    SynchronizeTextAnchorsInParent(textActor, controller, anchorActors);
  }
}

void CommonTextUtils::UpdateTextRenderPosition(
  Actor textActor, Ui::Text::RendererPtr renderer, Ui::Text::ControllerPtr controller, float alignmentOffset,
  Actor renderableActor, Actor stencil, const Ui::Text::Internal::Gradient::AtlasFrameState& atlasFrameState,
  const Vector2& viewSize)
{
  if(!renderableActor || !renderer)
  {
    return;
  }

  const Vector2 position = CalculateRenderablePosition(textActor, controller, alignmentOffset, stencil);
  renderableActor.SetProperty(Actor::Property::POSITION, position);
  UpdateAtlasGradient(renderer, controller, stencil, position, alignmentOffset, atlasFrameState, viewSize);
}

} // namespace Dali::Ui::Internal
