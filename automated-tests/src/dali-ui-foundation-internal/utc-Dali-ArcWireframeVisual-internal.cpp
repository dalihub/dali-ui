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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/visuals/arc/arc-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-cache.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>
#include <dali-ui-foundation/internal/visuals/wireframe/wireframe-visual.h>
#undef protected
#undef private

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

void utc_dali_arc_wireframe_visual_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_arc_wireframe_visual_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
VisualFactoryCache& GetArcWireframeFactoryCache()
{
  auto visualFactory = Ui::Integration::VisualFactory::Get();
  return GetImplementation(visualFactory).GetFactoryCache();
}
} // unnamed namespace

int UtcDaliArcVisualPropertiesAndSceneP(void)
{
  UiTestApplication application;
  Property::Map properties;
  properties.Insert(Ui::Integration::ArcVisual::Property::THICKNESS, 8.0f);
  properties.Insert(Ui::Integration::ArcVisual::Property::START_ANGLE, 25.0f);
  properties.Insert(Ui::Integration::ArcVisual::Property::SWEEP_ANGLE, 220.0f);
  properties.Insert(Ui::Integration::ArcVisual::Property::CAP, "ROUND");
  ArcVisualPtr visual = ArcVisual::New(GetArcWireframeFactoryCache(), properties);

  DALI_TEST_EQUALS(visual->mThickness, 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mStartAngle, 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mSweepAngle, 220.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->mCapType, Ui::Integration::ArcVisual::Cap::ROUND, TEST_LOCATION);
  DALI_TEST_CHECK(visual->mImpl->mRenderer);

  Property::Map transform;
  transform.Insert(Ui::Integration::Visual::Transform::Property::SIZE, Vector2(0.5f, 0.75f));
  visual->SetTransformAndSize(transform, Vector2(200.0f, 100.0f), 1.0f);
  visual->OnSetTransform();
  DALI_TEST_CHECK(visual->mRadius > 0.0f);

  Property::Map output;
  visual->DoCreatePropertyMap(output);
  DALI_TEST_EQUALS(output.Count(), 5u, TEST_LOCATION);
  Property::Map instance;
  instance.Insert(1, 2);
  visual->DoCreateInstancePropertyMap(instance);
  DALI_TEST_EQUALS(instance.Count(), 1u, TEST_LOCATION);

  Actor actor = Actor::New();
  visual->DoSetOnScene(actor);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  visual->DoSetOffScene(actor);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 0u, TEST_LOCATION);

  properties.Clear();
  properties.Insert("thickness", 4.0f);
  properties.Insert("startAngle", 10.0f);
  properties.Insert("sweepAngle", 90.0f);
  visual->DoSetProperties(properties);
  DALI_TEST_EQUALS(visual->mThickness, 4.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliArcVisualDefaultsInvalidAndShaderCacheP(void)
{
  UiTestApplication application;
  ArcVisualPtr visual(new ArcVisual(GetArcWireframeFactoryCache()));
  Property::Map output;
  visual->DoCreatePropertyMap(output);
  DALI_TEST_EQUALS(output.Count(), 5u, TEST_LOCATION);

  Property::Map invalid;
  invalid.Insert(Ui::Integration::ArcVisual::Property::THICKNESS, "bad");
  invalid.Insert(Ui::Integration::ArcVisual::Property::START_ANGLE, Vector2::ZERO);
  invalid.Insert(Ui::Integration::ArcVisual::Property::SWEEP_ANGLE, true);
  invalid.Insert(Ui::Integration::ArcVisual::Property::CAP, Ui::Integration::ArcVisual::Cap::BUTT);
  visual->DoSetProperties(invalid);
  visual->OnInitialize();
  DALI_TEST_CHECK(visual->mImpl->mRenderer);

  Property::Map defaults;
  ArcVisualPtr cached = ArcVisual::New(GetArcWireframeFactoryCache(), defaults);
  DALI_TEST_CHECK(cached->mImpl->mRenderer);
  END_TEST;
}

int UtcDaliWireframeVisualStandaloneP(void)
{
  UiTestApplication application;
  Property::Map properties;
  properties.Insert(Ui::Integration::Visual::Property::TRANSFORM, "bad");
  WireframeVisualPtr visual = WireframeVisual::New(GetArcWireframeFactoryCache(), properties);

  DALI_TEST_CHECK(visual->mImpl->mRenderer);
  DALI_TEST_EQUALS(visual->GetHeightForWidth(80.0f), 0.0f, TEST_LOCATION);
  Vector2 naturalSize;
  visual->GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2::ZERO, TEST_LOCATION);

  Property::Map output;
  visual->DoCreatePropertyMap(output);
  DALI_TEST_EQUALS(output.Count(), 1u, TEST_LOCATION);
  Property::Map instance;
  instance.Insert(1, 2);
  visual->DoCreateInstancePropertyMap(instance);
  DALI_TEST_EQUALS(instance.Count(), 1u, TEST_LOCATION);

  Property::Map color;
  color.Insert(Ui::Integration::Visual::Property::MIX_COLOR, Vector4(0.2f, 0.4f, 0.6f, 0.8f));
  visual->DoSetProperties(color);
  visual->mImpl->mTransformMapChanged = true;
  visual->OnSetTransform();
  DALI_TEST_CHECK(&visual->GetVisualObject() == visual.Get());

  Actor actor = Actor::New();
  visual->DoSetOnScene(actor);
  DALI_TEST_EQUALS(actor.GetRendererCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWireframeVisualProxyAndCacheP(void)
{
  UiTestApplication application;
  Property::Map arcProperties;
  arcProperties.Insert(Ui::Integration::ArcVisual::Property::THICKNESS, 3.0f);
  ArcVisualPtr arc = ArcVisual::New(GetArcWireframeFactoryCache(), arcProperties);

  WireframeVisualPtr proxy = WireframeVisual::New(GetArcWireframeFactoryCache(), Ui::Internal::Visual::BasePtr(arc.Get()));
  DALI_TEST_CHECK(&proxy->GetVisualObject() == arc.Get());
  DALI_TEST_EQUALS(proxy->GetHeightForWidth(64.0f), arc->GetHeightForWidth(64.0f), TEST_LOCATION);
  Vector2 proxySize;
  Vector2 arcSize;
  proxy->GetNaturalSize(proxySize);
  arc->GetNaturalSize(arcSize);
  DALI_TEST_EQUALS(proxySize, arcSize, TEST_LOCATION);

  Property::Map output;
  proxy->DoCreatePropertyMap(output);
  DALI_TEST_CHECK(output.Find(Ui::Integration::Visual::Property::TYPE));

  Property::Map transform;
  transform.Insert(Ui::Integration::Visual::Transform::Property::OFFSET, Vector2(0.1f, 0.2f));
  Property::Map properties;
  properties.Insert(Ui::Integration::Visual::Property::TRANSFORM, transform);
  WireframeVisualPtr transformed =
    WireframeVisual::New(GetArcWireframeFactoryCache(), Ui::Internal::Visual::BasePtr(arc.Get()), properties);
  DALI_TEST_CHECK(transformed->mImpl->mRenderer);
  END_TEST;
}
