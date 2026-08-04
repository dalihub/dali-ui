/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */

// EXTERNAL INCLUDES
#include <dali.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/common-text-utils.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.h>
#include <dali-ui-foundation/internal/text/text-atlas-gradient-state.h>
#include <dali-ui-foundation/internal/text/text-model.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;

namespace
{
namespace PublicGradient = Dali::Ui::Gradient;
namespace TextGradient   = Dali::Ui::Integration::Text::Gradient;
namespace AtlasGradient  = Dali::Ui::Text::Internal::Gradient;

PublicGradient::Linear MakeGradient(const Vector4& first = Color::RED, const Vector4& second = Color::BLUE)
{
  PublicGradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({PublicGradient::StopNode(0.0f, Dali::Ui::UiColor(first)),
                         PublicGradient::StopNode(1.0f, Dali::Ui::UiColor(second))});
  return gradient;
}

void SetStops(PublicGradient::Base& gradient,
              const Vector4&        first  = Color::RED,
              const Vector4&        second = Color::BLUE)
{
  gradient.SetStopNodes({PublicGradient::StopNode(0.0f, Dali::Ui::UiColor(first)),
                         PublicGradient::StopNode(1.0f, Dali::Ui::UiColor(second))});
}

class UnsupportedGradientRenderer : public Dali::Ui::Text::Renderer
{
public:
  static Dali::Ui::Text::RendererPtr New()
  {
    return new UnsupportedGradientRenderer();
  }

  Actor Render(Dali::Ui::Text::ViewInterface&, Actor, Property::Index, float&, int) override
  {
    return Actor();
  }

  bool SetAtlasGradientState(const AtlasGradient::AtlasRendererState&) override
  {
    ++setAtlasGradientStateCount;
    return false;
  }

  uint32_t setAtlasGradientStateCount{0u};

protected:
  ~UnsupportedGradientRenderer() override = default;
};

class RecordingGradientRenderer : public Dali::Ui::Text::Renderer
{
public:
  Actor Render(Dali::Ui::Text::ViewInterface&, Actor, Property::Index, float& alignmentOffset, int) override
  {
    ++renderCount;
    alignmentOffset = 0.0f;
    if(!renderableActor)
    {
      renderableActor = Actor::New();
    }
    return renderableActor;
  }

  void UpdateAtlasGradient(const Vector2& coordinateSize, const Vector4& bounds) override
  {
    ++updateAtlasGradientCount;
    lastCoordinateSize = coordinateSize;
    lastBounds         = bounds;
  }

  Actor    renderableActor;
  uint32_t renderCount{0u};
  uint32_t updateAtlasGradientCount{0u};
  Vector2  lastCoordinateSize{Vector2::ZERO};
  Vector4  lastBounds{Vector4::ZERO};

protected:
  ~RecordingGradientRenderer() override = default;
};

bool ApplyAtlasGradientForTest(Dali::Ui::Text::RendererPtr              renderer,
                               const AtlasGradient::AtlasRendererState& state,
                               TextGradient::AtlasApplyState&           applied)
{
  if(AtlasGradient::MatchesAtlasApplyState(applied, state))
  {
    return true;
  }

  if(!state.IsEnabled())
  {
    AtlasGradient::SetAtlasApplyState(applied, state);
    return true;
  }

  if(renderer && renderer->SetAtlasGradientState(state))
  {
    AtlasGradient::SetAtlasApplyState(applied, state);
  }
  else
  {
    AtlasGradient::SetAtlasApplyStateAsSolidFallback(applied, state);
  }
  return true;
}
} // namespace

void utc_dali_text_atlas_gradient_state_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_atlas_gradient_state_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextAtlasGradientStateLutInvalidationP(void)
{
  UiTestApplication            application;
  AtlasGradient::AtlasResource prepared;
  auto                         gradient = MakeGradient();

  DALI_TEST_CHECK(prepared.Set(gradient));
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 1u, TEST_LOCATION);
  const uint64_t initialStyleRevision  = prepared.GetStyleRevision();
  const uint64_t initialLookupRevision = prepared.GetLookupRevision();

  DALI_TEST_CHECK(!prepared.Set(gradient));
  DALI_TEST_EQUALS(prepared.GetStyleRevision(), initialStyleRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupRevision(), initialLookupRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 1u, TEST_LOCATION);

  gradient.SetSpreadMethod(PublicGradient::SpreadMethod::REPEAT);
  DALI_TEST_CHECK(prepared.Set(gradient));
  DALI_TEST_EQUALS(prepared.GetStyleRevision(), initialStyleRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupRevision(), initialLookupRevision + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 1u, TEST_LOCATION);

  gradient.SetUnits(PublicGradient::Units::USER_SPACE);
  gradient.SetStartOffset(0.25f);
  gradient.SetStartAndEndPosition(Vector2(10.0f, 5.0f), Vector2(80.0f, 5.0f));
  DALI_TEST_CHECK(prepared.Set(gradient));
  DALI_TEST_EQUALS(prepared.GetStyleRevision(), initialStyleRevision + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupRevision(), initialLookupRevision + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 1u, TEST_LOCATION);

  gradient = MakeGradient(Color::GREEN, Color::YELLOW);
  DALI_TEST_CHECK(prepared.Set(gradient));
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 2u, TEST_LOCATION);

  DALI_TEST_CHECK(prepared.Set(PublicGradient::Base::None()));
  DALI_TEST_CHECK(!prepared.GetRendererState().IsEnabled());
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 2u, TEST_LOCATION);

  gradient = MakeGradient();
  DALI_TEST_CHECK(prepared.Set(gradient));
  DALI_TEST_CHECK(prepared.GetRendererState().IsEnabled());
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextAtlasGradientStateRadialAndConicGeometryInvalidationP(void)
{
  UiTestApplication            application;
  AtlasGradient::AtlasResource prepared;

  PublicGradient::Radial radial(Vector2(0.0f, 0.0f), 0.5f);
  SetStops(radial);
  DALI_TEST_CHECK(prepared.Set(radial));
  const uint64_t radialStyleRevision  = prepared.GetStyleRevision();
  const uint64_t radialLookupRevision = prepared.GetLookupRevision();
  radial.SetCenterAndRadius(Vector2(0.2f, -0.1f), 0.8f);
  DALI_TEST_CHECK(prepared.Set(radial));
  DALI_TEST_EQUALS(prepared.GetStyleRevision(), radialStyleRevision + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupRevision(), radialLookupRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 1u, TEST_LOCATION);

  PublicGradient::Conic conic(Vector2(0.0f, 0.0f), Radian(0.0f));
  SetStops(conic);
  DALI_TEST_CHECK(prepared.Set(conic));
  const uint64_t conicStyleRevision  = prepared.GetStyleRevision();
  const uint64_t conicLookupRevision = prepared.GetLookupRevision();
  conic.SetCenterAndStartAngle(Vector2(-0.2f, 0.3f), Radian(1.0f));
  DALI_TEST_CHECK(prepared.Set(conic));
  DALI_TEST_EQUALS(prepared.GetStyleRevision(), conicStyleRevision + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupRevision(), conicLookupRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextAtlasGradientStateNormalPlaceholderIndependentP(void)
{
  UiTestApplication                     application;
  AtlasGradient::EditableAtlasResources state;
  state.SetTextGradient(MakeGradient(Color::RED, Color::BLUE));
  state.SetPlaceholderGradient(MakeGradient(Color::GREEN, Color::YELLOW));
  state.SetBoundsMode(Dali::Ui::Text::GradientBoundsMode::VIEW_BOUND);

  const auto normal      = state.GetRendererState(false);
  const auto placeholder = state.GetRendererState(true);
  DALI_TEST_CHECK(normal.IsEnabled());
  DALI_TEST_CHECK(placeholder.IsEnabled());
  DALI_TEST_CHECK(normal.resourceId != placeholder.resourceId);
  DALI_TEST_EQUALS(normal.style.stops[0u].color, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(placeholder.style.stops[0u].color, Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(state.GetFrameState(false).boundsMode,
                   Dali::Ui::Text::GradientBoundsMode::VIEW_BOUND,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(state.GetFrameState(true).boundsMode,
                   Dali::Ui::Text::GradientBoundsMode::VIEW_BOUND,
                   TEST_LOCATION);

  const auto normalBeforeInactiveUpdate = state.GetRendererState(false);
  DALI_TEST_CHECK(state.SetPlaceholderGradient(MakeGradient(Color::CYAN, Color::MAGENTA)));
  const auto normalAfterInactiveUpdate = state.GetRendererState(false);
  DALI_TEST_EQUALS(normalBeforeInactiveUpdate.resourceId, normalAfterInactiveUpdate.resourceId, TEST_LOCATION);
  DALI_TEST_EQUALS(normalBeforeInactiveUpdate.styleRevision, normalAfterInactiveUpdate.styleRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(normalBeforeInactiveUpdate.lookupRevision, normalAfterInactiveUpdate.lookupRevision, TEST_LOCATION);
  DALI_TEST_CHECK(normalBeforeInactiveUpdate.lookupTexture == normalAfterInactiveUpdate.lookupTexture);

  DALI_TEST_CHECK(!state.SetBoundsMode(Dali::Ui::Text::GradientBoundsMode::VIEW_BOUND));
  DALI_TEST_CHECK(state.SetBoundsMode(Dali::Ui::Text::GradientBoundsMode::CONTENT_BOUND));
  END_TEST;
}

int UtcDaliTextAtlasGradientStateSnapshotLifetimeP(void)
{
  UiTestApplication                 application;
  AtlasGradient::AtlasRendererState snapshot;

  {
    AtlasGradient::AtlasResource prepared;
    DALI_TEST_CHECK(prepared.Set(MakeGradient(Color::RED, Color::BLUE)));
    snapshot = prepared.GetRendererState();
    DALI_TEST_CHECK(snapshot.IsEnabled());

    DALI_TEST_CHECK(prepared.Set(PublicGradient::Base::None()));
    DALI_TEST_CHECK(!prepared.GetRendererState().IsEnabled());
    DALI_TEST_CHECK(snapshot.IsEnabled());
    DALI_TEST_EQUALS(snapshot.style.stops[0u].color, Color::RED, TEST_LOCATION);
  }

  DALI_TEST_CHECK(snapshot.IsEnabled());
  DALI_TEST_CHECK(snapshot.lookupTexture);
  DALI_TEST_CHECK(snapshot.lookupSampler);
  DALI_TEST_EQUALS(snapshot.style.stops[1u].color, Color::BLUE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextAtlasGradientStateInvalidAuthoredValueCanonicalizedP(void)
{
  UiTestApplication            application;
  AtlasGradient::AtlasResource prepared;

  DALI_TEST_CHECK(prepared.Set(MakeGradient()));
  PublicGradient::Linear oneStop(Vector2::ZERO, Vector2::ONE);
  oneStop.SetStopNodes({PublicGradient::StopNode(0.5f, Dali::Ui::UiColor(Color::RED))});
  DALI_TEST_CHECK(prepared.Set(oneStop));
  DALI_TEST_EQUALS(prepared.Get().GetType(), PublicGradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(!prepared.IsEnabled());

  PublicGradient::Radial zeroRadius(Vector2(0.25f, 0.75f), 0.0f);
  SetStops(zeroRadius);
  DALI_TEST_CHECK(prepared.Set(zeroRadius));
  DALI_TEST_EQUALS(prepared.Get().GetType(), PublicGradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_CHECK(!prepared.IsEnabled());
  END_TEST;
}

int UtcDaliTextAtlasGradientStateAuthoredOnlyChangeP(void)
{
  UiTestApplication            application;
  AtlasGradient::AtlasResource prepared;

  PublicGradient::Linear direct(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  direct.SetStopNodes({PublicGradient::StopNode(0.0f, Dali::Ui::UiColor(Color::TRANSPARENT)),
                       PublicGradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE))});
  DALI_TEST_CHECK(prepared.Set(direct));

  const uint64_t styleRevision         = prepared.GetStyleRevision();
  const uint64_t lookupRevision        = prepared.GetLookupRevision();
  const uint64_t lookupGenerationCount = prepared.GetLookupGenerationCount();

  PublicGradient::Linear token(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  token.SetStopNodes({PublicGradient::StopNode(0.0f, Dali::Ui::UiColor("MissingTextGradientToken")),
                      PublicGradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE))});
  DALI_TEST_CHECK(prepared.Set(token));

  DALI_TEST_EQUALS(prepared.GetStyleRevision(), styleRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupRevision(), lookupRevision, TEST_LOCATION);
  DALI_TEST_EQUALS(prepared.GetLookupGenerationCount(), lookupGenerationCount, TEST_LOCATION);
  DALI_TEST_CHECK(prepared.Get().GetStopNodes()[0u].GetColor().HasColorId());
  DALI_TEST_EQUALS(prepared.Get().GetStopNodes()[0u].GetColor().GetColorId(), "MissingTextGradientToken", TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextAtlasGradientStateAppliedVersionP(void)
{
  UiTestApplication                     application;
  AtlasGradient::EditableAtlasResources resources;
  TextGradient::AtlasApplyState         applied;

  resources.SetTextGradient(MakeGradient(Color::RED, Color::BLUE));
  resources.SetPlaceholderGradient(MakeGradient(Color::GREEN, Color::YELLOW));

  const auto& normal = resources.GetRendererState(false);
  DALI_TEST_CHECK(!AtlasGradient::MatchesAtlasApplyState(applied, normal));
  AtlasGradient::SetAtlasApplyState(applied, normal);
  DALI_TEST_CHECK(AtlasGradient::MatchesAtlasApplyState(applied, normal));
  DALI_TEST_CHECK(applied.IsGradientApplied());

  const auto& placeholder = resources.GetRendererState(true);
  DALI_TEST_CHECK(!AtlasGradient::MatchesAtlasApplyState(applied, placeholder));
  AtlasGradient::SetAtlasApplyState(applied, placeholder);
  DALI_TEST_CHECK(AtlasGradient::MatchesAtlasApplyState(applied, placeholder));

  auto changedNormal = MakeGradient(Color::CYAN, Color::MAGENTA);
  resources.SetTextGradient(changedNormal);
  DALI_TEST_CHECK(!AtlasGradient::MatchesAtlasApplyState(applied, resources.GetRendererState(false)));

  applied.Reset();
  DALI_TEST_CHECK(!applied.initialized);
  DALI_TEST_CHECK(!AtlasGradient::MatchesAtlasApplyState(applied, placeholder));
  END_TEST;
}

int UtcDaliTextAtlasGradientUnsupportedRendererSolidFallbackP(void)
{
  UiTestApplication                     application;
  AtlasGradient::EditableAtlasResources resources;
  TextGradient::AtlasApplyState         applied;

  resources.SetTextGradient(MakeGradient(Color::RED, Color::BLUE));
  resources.SetPlaceholderGradient(MakeGradient(Color::GREEN, Color::YELLOW));

  UnsupportedGradientRenderer* unsupportedRenderer = new UnsupportedGradientRenderer();
  Dali::Ui::Text::RendererPtr  renderer(unsupportedRenderer);
  DALI_TEST_CHECK(renderer);

  const auto& normal = resources.GetRendererState(false);
  DALI_TEST_CHECK(ApplyAtlasGradientForTest(renderer, normal, applied));
  DALI_TEST_CHECK(AtlasGradient::MatchesAtlasApplyState(applied, normal));
  DALI_TEST_CHECK(applied.IsSolidFallback());
  DALI_TEST_CHECK(!applied.IsGradientApplied());
  DALI_TEST_CHECK(!resources.GetFrameState(false, applied).enabled);
  DALI_TEST_EQUALS(unsupportedRenderer->setAtlasGradientStateCount, 1u, TEST_LOCATION);

  DALI_TEST_CHECK(ApplyAtlasGradientForTest(renderer, normal, applied));
  DALI_TEST_EQUALS(unsupportedRenderer->setAtlasGradientStateCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(resources.GetTextGradient().GetType(), PublicGradient::Type::LINEAR, TEST_LOCATION);

  const auto& placeholder = resources.GetRendererState(true);
  DALI_TEST_CHECK(ApplyAtlasGradientForTest(renderer, placeholder, applied));
  DALI_TEST_CHECK(AtlasGradient::MatchesAtlasApplyState(applied, placeholder));
  DALI_TEST_CHECK(applied.IsSolidFallback());
  DALI_TEST_CHECK(!resources.GetFrameState(true, applied).enabled);
  DALI_TEST_EQUALS(resources.GetPlaceholderGradient().GetType(), PublicGradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(unsupportedRenderer->setAtlasGradientStateCount, 2u, TEST_LOCATION);

  DALI_TEST_CHECK(resources.SetTextGradient(MakeGradient(Color::CYAN, Color::MAGENTA)));
  DALI_TEST_CHECK(ApplyAtlasGradientForTest(renderer, resources.GetRendererState(false), applied));
  DALI_TEST_CHECK(applied.IsSolidFallback());
  DALI_TEST_EQUALS(unsupportedRenderer->setAtlasGradientStateCount, 3u, TEST_LOCATION);

  DALI_TEST_CHECK(resources.SetTextGradient(PublicGradient::Base::None()));
  DALI_TEST_CHECK(ApplyAtlasGradientForTest(renderer, resources.GetRendererState(false), applied));
  DALI_TEST_CHECK(!applied.IsSolidFallback());
  DALI_TEST_CHECK(!resources.GetFrameState(false, applied).enabled);
  DALI_TEST_EQUALS(unsupportedRenderer->setAtlasGradientStateCount, 3u, TEST_LOCATION);

  resources.SetTextGradient(MakeGradient(Color::YELLOW, Color::CYAN));
  applied.Reset();
  Dali::Ui::Text::RendererPtr atlasRenderer = Dali::Ui::Text::AtlasRenderer::New();
  DALI_TEST_CHECK(ApplyAtlasGradientForTest(atlasRenderer, resources.GetRendererState(false), applied));
  DALI_TEST_CHECK(applied.IsGradientApplied());
  DALI_TEST_CHECK(resources.GetFrameState(false, applied).enabled);
  END_TEST;
}

int UtcDaliTextAtlasGradientRendererOwnsSnapshotP(void)
{
  UiTestApplication           application;
  Dali::Ui::Text::RendererPtr renderer = Dali::Ui::Text::AtlasRenderer::New();
  DALI_TEST_CHECK(renderer);

  {
    AtlasGradient::AtlasResource prepared;
    prepared.Set(MakeGradient(Color::RED, Color::BLUE));
    DALI_TEST_CHECK(renderer->SetAtlasGradientState(prepared.GetRendererState()));
  }

  renderer->UpdateAtlasGradient(Vector2(200.0f, 50.0f), Vector4(0.1f, 0.0f, 0.8f, 1.0f));
  DALI_TEST_CHECK(renderer);
  renderer.Reset();
  END_TEST;
}

int UtcDaliTextAtlasGradientGenericRendererRejectsStateP(void)
{
  UiTestApplication            application;
  AtlasGradient::AtlasResource prepared;
  DALI_TEST_CHECK(prepared.Set(MakeGradient()));

  Dali::Ui::Text::RendererPtr renderer = UnsupportedGradientRenderer::New();
  DALI_TEST_CHECK(renderer);
  DALI_TEST_CHECK(!renderer->SetAtlasGradientState(prepared.GetRendererState()));
  renderer.Reset();
  END_TEST;
}

int UtcDaliTextAtlasGradientCommonTextUtilsBackgroundScrollP(void)
{
  UiTestApplication application;

  Dali::Ui::View textActor = Dali::Ui::View::New();
  textActor.SetProperty(Actor::Property::SIZE, Vector2(120.0f, 48.0f));
  textActor.SetProperty(Dali::Ui::View::Property::PADDING, Extents(0, 0, 0, 0));
  application.GetWindow().Add(textActor);

  Dali::Ui::Text::ControllerPtr controller = Dali::Ui::Text::Controller::New();
  controller->SetDefaultFontSize(18.0f, Dali::Ui::Text::Controller::PIXEL_SIZE);
  controller->SetHorizontalScrollEnabled(true);
  controller->SetStyledText(Dali::Ui::Text::StyledText::FromMarkup(
    "<background color='red'>scroll background span keeps content offset while this text is much wider than the field</background>"));
  controller->Relayout(Size(120.0f, 48.0f));

  RecordingGradientRenderer*  recordingRenderer = new RecordingGradientRenderer();
  Dali::Ui::Text::RendererPtr renderer(recordingRenderer);

  float                             alignmentOffset = 0.0f;
  Actor                             renderableActor;
  Actor                             backgroundActor;
  Actor                             cursorLayerActor;
  Actor                             stencil;
  std::vector<Actor>                clippingDecorationActors;
  std::vector<Dali::Ui::TextAnchor> anchorActors;
  AtlasGradient::AtlasFrameState    frameState{true, Dali::Ui::Text::GradientBoundsMode::CONTENT_BOUND};

  Dali::Ui::Internal::CommonTextUtils::RenderText(
    textActor, renderer, controller, nullptr, alignmentOffset, renderableActor, backgroundActor, cursorLayerActor,
    stencil, clippingDecorationActors, anchorActors, Dali::Ui::Text::Controller::MODEL_UPDATED, frameState,
    Vector2(120.0f, 48.0f));

  DALI_TEST_CHECK(renderableActor);
  DALI_TEST_CHECK(backgroundActor);
  DALI_TEST_CHECK(backgroundActor.GetParent() == renderableActor);
  DALI_TEST_EQUALS(backgroundActor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(recordingRenderer->renderCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recordingRenderer->updateAtlasGradientCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(recordingRenderer->lastCoordinateSize.x > 120.0f);

  auto* model = const_cast<Dali::Ui::Text::Model*>(
    dynamic_cast<const Dali::Ui::Text::Model*>(controller->GetRenderTextModel()));
  DALI_TEST_CHECK(model);
  model->mScrollPosition = Vector2(-24.0f, -6.0f);

  Dali::Ui::Internal::CommonTextUtils::UpdateTextRenderPosition(
    textActor, renderer, controller, alignmentOffset, renderableActor, stencil, frameState, Vector2(120.0f, 48.0f));

  DALI_TEST_CHECK(backgroundActor.GetParent() == renderableActor);
  DALI_TEST_EQUALS(backgroundActor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(renderableActor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3(-24.0f, -6.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(recordingRenderer->renderCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(recordingRenderer->updateAtlasGradientCount, 2u, TEST_LOCATION);

  frameState.boundsMode = Dali::Ui::Text::GradientBoundsMode::VIEW_BOUND;
  Dali::Ui::Internal::CommonTextUtils::UpdateTextRenderPosition(
    textActor, renderer, controller, alignmentOffset, renderableActor, stencil, frameState, Vector2(120.0f, 48.0f));

  DALI_TEST_CHECK(backgroundActor.GetParent() == renderableActor);
  DALI_TEST_EQUALS(backgroundActor.GetProperty<Vector3>(Actor::Property::POSITION), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(recordingRenderer->updateAtlasGradientCount, 3u, TEST_LOCATION);
  END_TEST;
}
