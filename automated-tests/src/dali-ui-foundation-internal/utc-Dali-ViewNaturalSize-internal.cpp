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
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/builder/style.h>
#include <dali-ui-foundation/internal/views/view/view-visual-data.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <unordered_set>

using namespace Dali;
using namespace Dali::Ui;

// A visual that contributes to its owner's natural size and only resolves LATE is the
// case these tests pin. ViewDataImpl::GetNaturalSize() reads the BACKGROUND visual, and
// the default measure path feeds that natural size straight into the measured size, so
// a background that finishes loading after the view has already been measured leaves a
// stale entry in the measure cache. Nothing else invalidates it: the resource-ready
// path used to raise only a dali-core legacy relayout request, which does not touch the
// dali-ui measure cache -- and it raised even that only while the view was connected to
// a scene, although the measure cache is scene independent.
namespace
{
// Visual::Base with a natural size the test drives directly, standing in for a resource
// whose real dimensions are only known once the load completes.
class NaturalSizeTestVisual : public Dali::Ui::Internal::Visual::Base
{
public:
  using Ptr = IntrusivePtr<NaturalSizeTestVisual>;

  static Ptr New(Dali::Ui::Internal::VisualFactoryCache& factoryCache)
  {
    Ptr visual(new NaturalSizeTestVisual(factoryCache));
    visual->Initialize();
    return visual;
  }

  // Answered by GetNaturalSize(); starts at ZERO, exactly like an image with no texture.
  Vector2 naturalSize{Vector2::ZERO};

  void GetNaturalSize(Vector2& outNaturalSize) override
  {
    outNaturalSize = naturalSize;
  }

protected:
  NaturalSizeTestVisual(Dali::Ui::Internal::VisualFactoryCache& factoryCache)
  : Dali::Ui::Internal::Visual::Base(factoryCache, Dali::Ui::Integration::InternalVisualType::IMAGE)
  {
  }

  ~NaturalSizeTestVisual() override = default;

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
};
} // namespace

void utc_dali_view_natural_size_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_natural_size_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// INC-A. The view is never added to a window. A late resource-ready must still drop the
// measure cache: measure and its cache are scene independent, so gating the
// invalidation on scene connection would make "loaded while detached" a permanently
// wrong size.
int UtcDaliViewNaturalSizeResourceReadyOffSceneInvalidatesMeasure(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto                                testVisual = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base visual(testVisual.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, visual);

  // Not ready yet, so natural size is ZERO. This settles the measure cache.
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetHeight(), 0.0f, TEST_LOCATION);

  // The resource resolves. The view is still off-scene.
  DALI_TEST_EQUALS(view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE), false, TEST_LOCATION);
  testVisual->naturalSize = Vector2(120.0f, 60.0f);
  testVisual->ResourceReady(Ui::Visual::ResourceStatus::READY);

  // Same constraints as before: a surviving cache entry would still answer 0x0.
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetHeight(), 60.0f, TEST_LOCATION);

  END_TEST;
}

// INC-A, on-scene half: a background that resolves AFTER the tree has settled must move
// the view's arranged geometry, through the normal layout pass, with no other stimulus.
int UtcDaliViewNaturalSizeResourceReadyAfterSettleUpdatesArrangedSize(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto                                testVisual = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base visual(testVisual.Get());

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, visual);

  window.Add(view);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 0.0f, TEST_LOCATION);

  // Late load.
  testVisual->naturalSize = Vector2(140.0f, 70.0f);
  testVisual->ResourceReady(Ui::Visual::ResourceStatus::READY);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_WIDTH), 140.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 70.0f, TEST_LOCATION);

  END_TEST;
}

// INC-A. The invalidation is scoped by the visual's own "resource completion requires
// owner relayout" flag, the same gate the legacy relayout request already used. A
// visual that has opted out (the inline text replacement visuals do) must not shake the
// measure cache when it resolves.
int UtcDaliViewNaturalSizeResourceReadyRespectsRelayoutOptOut(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto                                testVisual = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base visual(testVisual.Get());
  testVisual->SetResourceReadyRelayoutRequired(false);

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));
  viewData.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, visual);

  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);

  testVisual->naturalSize = Vector2(120.0f, 60.0f);
  testVisual->ResourceReady(Ui::Visual::ResourceStatus::READY);

  // Opted out: the cache is deliberately left standing.
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);

  // Non-vacuity: an unrelated invalidation still picks the new natural size up, so the
  // assertion above is about the cache and not about the visual being ignored.
  view.InvalidateMeasure();
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 120.0f, TEST_LOCATION);

  END_TEST;
}

// INC-A. Registering and unregistering the BACKGROUND visual is itself a natural-size
// change and must drop the measure cache, through the single RegisterVisual /
// UnregisterVisual funnel that every background path goes through.
int UtcDaliViewNaturalSizeRegisterUnregisterInvalidatesMeasure(void)
{
  UiTestApplication application;

  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);

  auto  factory      = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();

  auto                                testVisual = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base visual(testVisual.Get());
  testVisual->naturalSize = Vector2(80.0f, 40.0f);

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view));

  // No background at all.
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);

  viewData.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, visual);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 80.0f, TEST_LOCATION);

  viewData.UnregisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);

  // A visual at any other index is not a natural-size input and invalidates nothing.
  auto                                shadowVisual = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base shadow(shadowVisual.Get());
  shadowVisual->naturalSize = Vector2(500.0f, 500.0f);
  viewData.RegisterVisual(Dali::Ui::Integration::View::Property::SHADOW, shadow);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliViewVisualDataStateStyleReplacementP(void)
{
  UiTestApplication application;
  View view = View::New();
  DALI_TEST_CHECK(view.GetPropertyIndex("background") != Property::INVALID_INDEX);
  auto& data = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view)).EnsureVisualData();

  auto makeColorMap = [](const Vector4& color)
  {
    Property::Map map;
    map.Add(Dali::Ui::Integration::Visual::Property::TYPE, VisualType::COLOR);
    map.Add(Dali::Ui::Integration::Visual::Property::MIX_COLOR, color);
    return map;
  };
  auto normal = Dali::Ui::Internal::Style::New();
  normal->visuals.Add("background", makeColorMap(Color::RED));
  auto focused = Dali::Ui::Internal::Style::New();
  focused->visuals.Add("background", makeColorMap(Color::BLUE));
  normal->subStates.Add("focus", focused);
  data.ReplaceStateVisualsAndProperties({}, normal, "focus");
  DALI_TEST_CHECK(view.GetProperty<Property::Map>(Dali::Ui::Integration::View::Property::BACKGROUND).Count() > 0u);

  auto changed = Dali::Ui::Internal::Style::New();
  changed->visuals.Add("background", makeColorMap(Color::GREEN));
  data.ReplaceStateVisualsAndProperties(normal, changed, "focus");
  DALI_TEST_CHECK(view.GetProperty<Property::Map>(Dali::Ui::Integration::View::Property::BACKGROUND).Count() > 0u);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  data.ReplaceStateVisualsAndProperties(changed, normal, "focus");
  DALI_TEST_CHECK(view.IsResourceReady());
  data.ReplaceStateVisualsAndProperties(normal, {}, "focus");
  END_TEST;
}

int UtcDaliViewVisualDataRegistrationAndOverridesP(void)
{
  UiTestApplication application;
  View view = View::New();
  auto factory = Dali::Ui::Integration::VisualFactory::Get();
  auto& factoryCache = Dali::Ui::GetImplementation(factory).GetFactoryCache();
  auto& data = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view)).EnsureVisualData();
  auto firstImpl = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base first(firstImpl.Get());
  data.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, first, false);
  DALI_TEST_CHECK(!data.IsVisualEnabled(Dali::Ui::Integration::View::Property::BACKGROUND));
  DALI_TEST_CHECK(data.GetVisual("background") == first);
  data.EnableVisual(Dali::Ui::Integration::View::Property::BACKGROUND, true);
  DALI_TEST_CHECK(data.IsVisualEnabled(Dali::Ui::Integration::View::Property::BACKGROUND));
  data.EnableReadyTransitionOverridden(first, true);
  data.EnableReadyTransitionOverridden(first, true);
  data.EnableReadyTransitionOverridden(first, false);

  auto replacementImpl = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base replacement(replacementImpl.Get());
  data.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, replacement, 7);
  DALI_TEST_EQUALS(replacement.GetDepthIndex(), 7, TEST_LOCATION);
  DALI_TEST_CHECK(data.GetVisual(Dali::Ui::Integration::View::Property::BACKGROUND) == replacement);

  auto shadowImpl = NaturalSizeTestVisual::New(factoryCache);
  Dali::Ui::Integration::Visual::Base shadow(shadowImpl.Get());
  data.RegisterVisual(Dali::Ui::Integration::View::Property::SHADOW, shadow, true, 8);
  DALI_TEST_CHECK(data.IsVisualEnabled(Dali::Ui::Integration::View::Property::SHADOW));
  data.UnregisterVisual(Dali::Ui::Integration::View::Property::SHADOW);
  DALI_TEST_CHECK(!data.GetVisual(Dali::Ui::Integration::View::Property::SHADOW));
  END_TEST;
}

int UtcDaliViewVisualDataAnimationConstraintReferenceCountsP(void)
{
  UiTestApplication application;
  View view = View::New();
  auto& data = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view)).EnsureVisualData();
  Animation animation = Animation::New(1.0f);
  const auto property = View::Property::BORDERLINE_WIDTH;
  const std::unordered_set<Property::Index> properties{property};
  DALI_TEST_CHECK(!data.IsAnyPropertyAnimate(properties));
  data.CreateAnimationConstraints(animation.GetBaseObject(), property);
  DALI_TEST_CHECK(data.IsAnyPropertyAnimate(properties));
  data.CreateAnimationConstraints(animation.GetBaseObject(), property);
  data.ClearAnimationConstraints(animation.GetBaseObject(), property);
  DALI_TEST_CHECK(data.IsAnyPropertyAnimate(properties));
  data.ClearAnimationConstraints(animation.GetBaseObject(), property);
  DALI_TEST_CHECK(!data.IsAnyPropertyAnimate(properties));
  data.ClearAnimationConstraints(animation.GetBaseObject(), property);
  END_TEST;
}

int UtcDaliViewVisualDataCornerConstraintTransitionsP(void)
{
  UiTestApplication application;
  View view = View::New();
  auto& data = Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view)).EnsureVisualData();
  auto factory = Dali::Ui::Integration::VisualFactory::Get();
  Property::Map map;
  map.Add(Dali::Ui::Integration::Visual::Property::TYPE, VisualType::COLOR);
  map.Add(Dali::Ui::Integration::Visual::Property::MIX_COLOR, Color::RED);
  auto visual = factory.CreateVisual(map);
  DALI_TEST_CHECK(visual);
  data.RegisterVisual(Dali::Ui::Integration::View::Property::BACKGROUND, visual, true);
  data.EnableVisual(Property::INVALID_INDEX, true);
  data.DoActionExtension(Dali::Ui::Integration::View::Property::BACKGROUND, 999, Dali::Any());
  data.EnableCornerPropertiesOverridden(visual, true, {});
  view.SetProperty(View::Property::CORNER_RADIUS, Vector4(3.0f, 4.0f, 5.0f, 6.0f));
  view.SetProperty(View::Property::CORNER_SQUARENESS, Vector4(0.2f, 0.3f, 0.4f, 0.5f));
  data.NotifyConstraintPropertyChanged(View::Property::CORNER_RADIUS, false);
  data.NotifyConstraintPropertyChanged(View::Property::CORNER_RADIUS, true);
  data.NotifyConstraintPropertyChanged(View::Property::CORNER_SQUARENESS, false);
  data.NotifyConstraintPropertyChanged(View::Property::CORNER_SQUARENESS, true);
  data.OffscreenRenderingEnabled(true);
  data.OffscreenRenderingEnabled(false);
  data.EnableCornerPropertiesOverridden(visual, false, {});
  DALI_TEST_CHECK(data.GetVisual(Dali::Ui::Integration::View::Property::BACKGROUND) == visual);
  END_TEST;
}
