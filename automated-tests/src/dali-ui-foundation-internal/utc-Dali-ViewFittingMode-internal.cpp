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

#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
class FittingModeTestVisual : public Dali::Ui::Internal::Visual::Base
{
public:
  using Ptr = IntrusivePtr<FittingModeTestVisual>;

  static Ptr New(Dali::Ui::Internal::VisualFactoryCache&         factoryCache,
                 Dali::Ui::Integration::InternalVisualType type = Dali::Ui::Integration::InternalVisualType::IMAGE)
  {
    Ptr visual(new FittingModeTestVisual(factoryCache, type));
    visual->Initialize();
    return visual;
  }

  int     applyCount{0};
  Vector2 lastControlSize{Vector2::ZERO};

protected:
  FittingModeTestVisual(Dali::Ui::Internal::VisualFactoryCache& factoryCache,
                        Dali::Ui::Integration::InternalVisualType type)
  : Dali::Ui::Internal::Visual::Base(factoryCache, type)
  {
    mImpl->mFittingModeRequired = true;
  }

  ~FittingModeTestVisual() override = default;

  void OnInitialize() override
  {
  }

  void DoCreatePropertyMap(Property::Map&) const override
  {
  }

  void DoCreateInstancePropertyMap(Property::Map&) const override
  {
  }

  void DoSetProperties(const Property::Map&) override
  {
  }

  void OnSetTransform() override
  {
  }

  void DoSetOnScene(Actor&) override
  {
  }

  void OnApplyFittingMode(const Vector2& controlSize, const Insets&, float) override
  {
    ++applyCount;
    lastControlSize = controlSize;
  }
};
} // namespace

void utc_dali_view_fitting_mode_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_fitting_mode_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewFittingModeAppliedAfterLayout(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  View              view   = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto fittingVisualA = FittingModeTestVisual::New(factoryCache);
  auto fittingVisualB = FittingModeTestVisual::New(factoryCache);

  Dali::Ui::Integration::Visual::Base visualA(fittingVisualA.Get());
  Dali::Ui::Integration::Visual::Base visualB(fittingVisualB.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(View::Property::BACKGROUND, visualA);
  viewData.RegisterVisual(View::Property::SHADOW, visualB);

  window.Add(view);

  LayoutController& controller = LayoutController::Get(window);
  controller.ProcessLayouts();

  DALI_TEST_EQUALS(fittingVisualA->applyCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualB->applyCount, 0, TEST_LOCATION);

  application.SendNotification();

  const Vector2 arrangedSize(
    view.GetProperty<float>(Actor::Property::SIZE_WIDTH),
    view.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

  DALI_TEST_CHECK(arrangedSize.width > 0.0f);
  DALI_TEST_CHECK(arrangedSize.height > 0.0f);
  DALI_TEST_EQUALS(fittingVisualA->applyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualB->applyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualA->lastControlSize, arrangedSize, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingVisualB->lastControlSize, arrangedSize, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewFittingModeAfterLayoutSkipsText(void)
{
  UiTestApplication application;
  View              view = View::New();

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto fittingImageVisual = FittingModeTestVisual::New(factoryCache);
  auto fittingTextVisual  = FittingModeTestVisual::New(factoryCache, Dali::Ui::Integration::InternalVisualType::TEXT);

  Dali::Ui::Integration::Visual::Base imageVisual(fittingImageVisual.Get());
  Dali::Ui::Integration::Visual::Base textVisual(fittingTextVisual.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(View::Property::BACKGROUND, imageVisual);
  viewData.RegisterVisual(View::Property::SHADOW, textVisual);

  viewData.EmitLayoutFinishedSignal(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(fittingImageVisual->applyCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fittingTextVisual->applyCount, 0, TEST_LOCATION);

  END_TEST;
}

// A fitting processor runs once for each explicit request and does not become
// implicit per-frame work.
int UtcDaliViewFittingModeProcessorRunsOncePerRequest(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto                                fittingVisual = FittingModeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base visual(fittingVisual.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(View::Property::BACKGROUND, visual);

  viewData.SizeOrUiScaleChanged();

  application.SendNotification();
  DALI_TEST_EQUALS(fittingVisual->applyCount, 1, TEST_LOCATION);

  application.SendNotification();
  application.SendNotification();
  DALI_TEST_EQUALS(fittingVisual->applyCount, 1, TEST_LOCATION);

  // A fresh request after the run registers again and applies again.
  viewData.SizeOrUiScaleChanged();
  application.SendNotification();
  DALI_TEST_EQUALS(fittingVisual->applyCount, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualLabelFittingCoalescesInitialRaster(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);
  gl.ResetTextureCallStack();

  Label label = Label::New("A plain Label must raster only once during its initial fitting pass");
  label.SetRequestedWidth(720.0f);
  label.SetRequestedHeight(174.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);

  // The initial text resource must be published only once.
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 2, TEST_LOCATION);

  Renderer renderer = label.GetRendererAt(0u);
  Texture  texture  = renderer.GetTextures().GetTexture(0u);

  // Reapplying unchanged fitting must retain the published resource without
  // creating per-frame work.
  gl.ResetTextureCallStack();
  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(label));
  viewData.SizeOrUiScaleChanged();
  application.SendNotification();
  application.Render(16);
  for(uint32_t frame = 0u; frame < 100u; ++frame)
  {
    application.SendNotification();
    application.Render(16);
  }

  DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTexture(0u) == texture);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualLabelFittingPreservesInvalidation(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  const float originalScale = UiScaleManager::Get().GetScale();
  Label       label         = Label::New("Initial text at fixed geometry");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  gl.ResetTextureCallStack();
  label.SetText("Changed text at the same fixed geometry");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);

  // Same-size text changes must publish their own glyph resources.
  for(const char* text : {"\xED\x95\x9C\xEA\xB8\x80 geometry",
                          "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7 geometry",
                          "Cafe\xCC\x81 geometry",
                          "Emoji \xF0\x9F\x98\x80 geometry",
                          "Plain text geometry"})
  {
    gl.ResetTextureCallStack();
    label.SetText(text);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  }

  gl.ResetTextureCallStack();
  UiScaleManager::Get().SetScale(originalScale * 1.25f);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  application.SendNotification();
  application.Render(16);

  for(uint32_t iteration = 0u; iteration < 10u; ++iteration)
  {
    label.Unparent();
    application.SendNotification();
    application.Render(16);
    if(iteration == 3u)
    {
      label.SetText("The latest off-scene content must win when the Label reconnects");
      label.SetPadding(Insets(9.0f, 13.0f, 5.0f, 7.0f));
    }
    application.GetScene().Add(label);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() > 0u);
  }

  END_TEST;
}

int UtcDaliTextVisualLabelFittingCoalescesWrapContentUpdate(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  Label label = Label::New("Short paragraph");
  label.SetRequestedWidth(180.0f);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetMultiLine(true);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  const float initialHeight = label.GetCurrentSize().height;

  gl.ResetTextureCallStack();
  label.SetText("A substantially longer paragraph changes wrapping, line count, and the wrap-content height of this Label.");
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetCurrentSize().height > initialHeight);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);

  // The updated wrap-content result must publish one text resource.
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualStandaloneFittingStillApplies(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);
  gl.ResetTextureCallStack();

  View view = View::New();
  view.SetRequestedWidth(360.0f);
  view.SetRequestedHeight(174.0f);

  TextVisual visual = TextVisual::New();
  visual.SetText("Standalone transformed TextVisual");
  visual.SetOffsetX(13.0f);
  visual.SetOffsetY(7.0f);
  visual.SetWidth(0.75f);
  visual.SetHeight(0.5f);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(view.GetRendererAt(0u).GetTextures().GetTextureCount() > 0u);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  END_TEST;
}

int UtcDaliTextVisualFittingFinalizesMeasuredLayout(void)
{
  UiTestApplication application;

  Label label = Label::New(
    "A measured wrapping Label must finish its controller layout before an unchanged height-for-width query");
  label.SetRequestedWidth(260.0f);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetMultiLine(true);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  auto& viewData   = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(label));
  auto  visual     = viewData.GetVisual(Ui::Text::LabelPropertyIndex::TEXT);
  auto  controller = Dali::Ui::Internal::TextVisual::GetController(visual);
  auto& impl       = Dali::Ui::Text::Controller::Impl::GetImplementation(*controller.Get());

  DALI_TEST_EQUALS(impl.mOperationsPending, Dali::Ui::Text::Controller::NO_OPERATION, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mTextUpdateInfo.mClearAll);
  DALI_TEST_CHECK(!impl.mTextUpdateInfo.mFullRelayoutNeeded);

  // An unchanged height-for-width query must not restore pending layout state.
  const float measuredWidth  = label.GetCurrentSize().width;
  const float measuredHeight = label.GetHeightForWidth(measuredWidth);
  DALI_TEST_CHECK(measuredHeight > 0.0f);
  DALI_TEST_EQUALS(impl.mOperationsPending, Dali::Ui::Text::Controller::NO_OPERATION, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mTextUpdateInfo.mClearAll);
  DALI_TEST_CHECK(!impl.mTextUpdateInfo.mFullRelayoutNeeded);

  END_TEST;
}

int UtcDaliTextVisualSameSizePaddingChangeInvalidatesResource(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  Label label = Label::New(
    "Padding must change this deliberately long paragraph's available width and wrapping without changing its outer geometry");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(240.0f);
  label.SetFontSize(20.0f);
  label.SetMultiLine(true);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  const Vector3 initialSize      = label.GetCurrentSize();
  const int     initialLineCount = label.GetLineCount();
  Texture       initialTexture   = label.GetRendererAt(0u).GetTextures().GetTexture(0u);

  gl.ResetTextureCallStack();
  label.SetPadding(Insets(100.0f, 110.0f, 7.0f, 11.0f));
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(label.GetCurrentSize(), initialSize, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetLineCount() > initialLineCount);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTexture(0u) != initialTexture);

  Texture paddedTexture = label.GetRendererAt(0u).GetTextures().GetTexture(0u);
  gl.ResetTextureCallStack();
  label.SetPadding(Insets());
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(label.GetCurrentSize(), initialSize, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetLineCount(), initialLineCount, TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTexture(0u) != paddedTexture);

  Label sameLineLabel = Label::New("Short text");
  sameLineLabel.SetRequestedWidth(420.0f);
  sameLineLabel.SetRequestedHeight(96.0f);
  sameLineLabel.SetFontSize(20.0f);
  application.GetScene().Add(sameLineLabel);
  application.SendNotification();
  application.Render(16);

  const int sameLineCount = sameLineLabel.GetLineCount();
  gl.ResetTextureCallStack();
  sameLineLabel.SetPadding(Insets(30.0f, 40.0f, 7.0f, 11.0f));
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(sameLineLabel.GetLineCount(), sameLineCount, TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  Label ellipsisLabel = Label::New(
    "Padding changes this initially fitting single line into a deterministic END ellipsis result");
  ellipsisLabel.SetFontSize(20.0f);
  ellipsisLabel.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  const Vector3 ellipsisNaturalSize = ellipsisLabel.GetNaturalSize();
  DALI_TEST_CHECK(ellipsisNaturalSize.width > 0.0f);
  ellipsisLabel.SetRequestedWidth(ellipsisNaturalSize.width + 20.0f);
  ellipsisLabel.SetRequestedHeight(ellipsisNaturalSize.height + 4.0f);
  application.GetScene().Add(ellipsisLabel);
  application.SendNotification();
  application.Render(16);

  auto& ellipsisViewData =
    Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(ellipsisLabel));
  auto ellipsisVisual = ellipsisViewData.GetVisual(Ui::Text::LabelPropertyIndex::TEXT);
  auto ellipsisController = Dali::Ui::Internal::TextVisual::GetController(ellipsisVisual);
  const auto* ellipsisModel = ellipsisController->GetRenderTextModel();
  DALI_TEST_CHECK(ellipsisModel && ellipsisModel->GetNumberOfLines() > 0u);
  DALI_TEST_CHECK(!ellipsisModel->GetLines()[0u].ellipsis);

  gl.ResetTextureCallStack();
  const float ellipsisPadding = ellipsisNaturalSize.width * 0.35f;
  ellipsisLabel.SetPadding(Insets(ellipsisPadding, ellipsisPadding, 0.0f, 0.0f));
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  ellipsisModel = ellipsisController->GetRenderTextModel();
  DALI_TEST_CHECK(ellipsisModel && ellipsisModel->GetNumberOfLines() > 0u);
  DALI_TEST_CHECK(ellipsisModel->GetLines()[0u].ellipsis);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  Label revealLabel = Label::New(
    "Reveal metadata must follow padding-driven wrapping rather than retaining the previously published schedule");
  revealLabel.SetRequestedWidth(420.0f);
  revealLabel.SetRequestedHeight(160.0f);
  revealLabel.SetFontSize(20.0f);
  revealLabel.SetMultiLine(true);
  revealLabel.SetTextReveal(Text::Reveal());
  revealLabel.SetTextRevealProgress(0.4f);
  application.GetScene().Add(revealLabel);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(revealLabel.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);
  Texture revealTexture = revealLabel.GetRendererAt(0u).GetTextures().GetTexture(0u);

  gl.ResetTextureCallStack();
  revealLabel.SetPadding(Insets(80.0f, 90.0f, 5.0f, 9.0f));
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_CHECK(revealLabel.GetRendererAt(0u).GetTextures().GetTexture(0u) != revealTexture);
  DALI_TEST_CHECK(revealLabel.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);
  DALI_TEST_EQUALS(revealLabel.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualSameSizeRenderStateChangeInvalidatesResource(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  Label label = Label::New("Geometry remains fixed while render-only state changes");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  const Vector3 fixedSize = label.GetCurrentSize();

  gl.ResetTextureCallStack();
  Text::Outline outline;
  outline.SetWidth(2.0f);
  label.SetTextOutline(outline);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetCurrentSize(), fixedSize, TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  gl.ResetTextureCallStack();
  label.SetTextReveal(Text::Reveal());
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetCurrentSize(), fixedSize, TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);

  Renderer renderer = label.GetRendererAt(0u);
  gl.ResetTextureCallStack();
  label.SetTextRevealProgress(0.5f);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextVisualEmptyOrZeroToRenderablePublishesResource(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  Label label = Label::New("");
  label.SetRequestedWidth(0.0f);
  label.SetRequestedHeight(0.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);

  gl.ResetTextureCallStack();
  label.SetText("The latest text must publish after an empty zero-size state");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() > 0u);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  label.SetText("");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);

  gl.ResetTextureCallStack();
  label.SetText("Emoji \xF0\x9F\x98\x80 after empty content");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() > 0u);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  label.SetRequestedWidth(0.0f);
  label.SetRequestedHeight(0.0f);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);

  gl.ResetTextureCallStack();
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTextureCount() > 0u);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);

  END_TEST;
}

int UtcDaliTextVisualNonFittingControllerMutationInvalidatesResource(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  Label label = Label::New("Initial controller content");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);

  auto& viewData   = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(label));
  auto  visual     = viewData.GetVisual(Ui::Text::LabelPropertyIndex::TEXT);
  auto  controller = Dali::Ui::Internal::TextVisual::GetController(visual);

  gl.ResetTextureCallStack();
  controller->SetText("A direct non-fitting controller mutation must not inherit persistent fitting state");
  Dali::Ui::Internal::TextVisual::UpdateRenderer(visual);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);

  END_TEST;
}
