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
#include <dali-ui-foundation/integration-api/builder/builder.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visuals/color-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/text-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/builder/builder-impl.h>
#include <dali-ui-foundation/internal/builder/dictionary.h>
#include <dali-ui-foundation/internal/builder/style.h>
#include <dali-ui-foundation/internal/focus-manager/focus-finder.h>
#include <dali-ui-foundation/internal/views/view/inner-shadow.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_internal_coverage_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_coverage_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* BUILDER_JSON = R"JSON({
  "styles": {
    "baseStyle": {
      "visible": false,
      "color": [1.0, 0.0, 0.0, 1.0]
    },
    "derivedStyle": {
      "inherit": ["baseStyle"],
      "size": [320.0, 180.0, 0.0],
      "actors": {
        "directChild": { "visible": false }
      }
    }
  }
})JSON";
}

int UtcDaliBuilderStylePropertyExtractionP(void)
{
  UiTestApplication application(UiConfig::New());
  Dali::Ui::Integration::Builder builder = Dali::Ui::Integration::Builder::New();
  auto& impl = Dali::Ui::GetImpl(builder);
  Property::Map properties;
  Actor actor = Actor::New();

  DALI_TEST_CHECK(!impl.GetStyleProperties("baseStyle", actor, properties));
  DALI_TEST_CHECK(properties.Empty());
  builder.LoadFromString(BUILDER_JSON);
  DALI_TEST_CHECK(impl.LookupStyleName("BASESTYLE"));
  DALI_TEST_CHECK(!impl.LookupStyleName("missing"));
  DALI_TEST_CHECK(!impl.GetStyleProperties("missing", actor, properties));
  DALI_TEST_CHECK(properties.Empty());

  DALI_TEST_CHECK(impl.GetStyleProperties("derivedStyle", actor, properties));
  const Property::Value* size = properties.Find(Actor::Property::SIZE);
  DALI_TEST_CHECK(size);
  DALI_TEST_EQUALS(size->Get<Vector3>(), Vector3(320.0f, 180.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_CHECK(!properties.Find(Dali::String("actors")));

  DALI_TEST_CHECK(impl.GetStyleProperties("baseStyle", Handle(), properties));
  const Property::Value* visible = properties.Find(Dali::String("visible"));
  DALI_TEST_CHECK(visible);
  DALI_TEST_EQUALS(visible->Get<bool>(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliBuilderStyleAppliesVisualInstanceAndActorPropertiesP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);

  Dali::Ui::Internal::StylePtr style = Dali::Ui::Internal::Style::New();
  style->properties.Insert(Actor::Property::NAME, "styled-view");
  Property::Map image;
  image.Insert(Ui::Integration::Visual::Property::TYPE, static_cast<int>(Ui::Integration::InternalVisualType::IMAGE));
  image.Insert(Ui::Integration::ImageVisual::Property::URL, "style-image.png");
  image.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 80);
  image.Insert(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, 40);
  DALI_TEST_CHECK(style->visuals.Add("background", image));

  Property::Map instance = image;
  instance.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 120);
  Dali::Ui::Internal::Dictionary<Property::Map> instances;
  DALI_TEST_CHECK(instances.Add("background", instance));
  style->ApplyVisualsAndPropertiesRecursively(view, instances);
  DALI_TEST_EQUALS(view.GetProperty<String>(Actor::Property::NAME), String("styled-view"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 120.0f, TEST_LOCATION);

  Property::Map differentType;
  differentType.Insert(Ui::Integration::Visual::Property::TYPE, static_cast<int>(Ui::Integration::InternalVisualType::COLOR));
  differentType.Insert(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, 200);
  Dali::Ui::Internal::Style::ApplyVisual(view, "background", image, &differentType);
  DALI_TEST_EQUALS(view.Measure(1000.0f, 1000.0f).GetWidth(), 80.0f, TEST_LOCATION);

  Dali::Ui::Internal::Style::ApplyVisual(view, "notAProperty", image, &instance);
  DALI_TEST_EQUALS(view.GetProperty<String>(Actor::Property::NAME), String("styled-view"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliBuilderDictionaryMutationAndLookupP(void)
{
  UiTestApplication application;
  Dali::Ui::Internal::Dictionary<Property::Map> values;
  Property::Map first;
  first.Insert("value", 1);
  Property::Map second;
  second.Insert("value", 2);
  DALI_TEST_CHECK(values.Add("First", first));
  DALI_TEST_CHECK(!values.Add("First", second));
  DALI_TEST_CHECK(!values.Add(nullptr, first));
  DALI_TEST_CHECK(values.FindConst("first"));
  DALI_TEST_CHECK(!values.FindConst("missing"));
  DALI_TEST_CHECK(!values.FindConst(""));

  Dali::Ui::Internal::Dictionary<Property::Map> replacement;
  DALI_TEST_CHECK(replacement.Add("First", second));
  DALI_TEST_CHECK(replacement.Add("Second", first));
  values.Merge(replacement);
  DALI_TEST_EQUALS(values.FindConst("First")->Find("value")->Get<int>(), 2, TEST_LOCATION);
  Dali::Ui::Internal::DictionaryKeys keys;
  values.GetKeys(keys);
  DALI_TEST_EQUALS(keys.size(), 2u, TEST_LOCATION);
  values.Remove("First");
  values.Remove("absent");
  values.Remove("");
  DALI_TEST_CHECK(!values.FindConst("First"));
  DALI_TEST_CHECK(values.FindConst("Second"));

  Dali::Ui::Internal::DictionaryKeys merged{"alpha", "beta"};
  Dali::Ui::Internal::Merge(merged, Dali::Ui::Internal::DictionaryKeys{"beta", "gamma"});
  DALI_TEST_EQUALS(merged.size(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFocusFinderDirectionalBeamSelectionP(void)
{
  UiTestApplication application;
  View parent = View::New();
  parent.SetParentOrigin(ParentOrigin::TOP_LEFT);
  parent.SetPivot(Pivot::TOP_LEFT);
  parent.SetRequestedWidth(400.0f);
  parent.SetRequestedHeight(400.0f);
  application.GetScene().Add(parent);

  auto makeView = [&](float x, float y)
  {
    View view = View::New();
    view.SetFocusable(true);
    view.SetParentOrigin(ParentOrigin::TOP_LEFT);
    view.SetPivot(Pivot::TOP_LEFT);
    view.SetRequestedX(x);
    view.SetRequestedY(y);
    view.SetRequestedWidth(40.0f);
    view.SetRequestedHeight(40.0f);
    parent.Add(view);
    return view;
  };

  View center = makeView(150.0f, 150.0f);
  makeView(210.0f, 220.0f);
  makeView(90.0f, 220.0f);
  makeView(220.0f, 90.0f);
  makeView(80.0f, 90.0f);
  View left = makeView(60.0f, 150.0f);
  View right = makeView(240.0f, 150.0f);
  View up = makeView(150.0f, 60.0f);
  View down = makeView(150.0f, 240.0f);
  application.SendNotification();
  application.Render();

  Actor root = application.GetScene().GetRootLayer();
  DALI_TEST_EQUALS(Dali::Ui::Internal::FocusFinder::GetNearestFocusableView(root, center, FocusDirection::LEFT), left, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::FocusFinder::GetNearestFocusableView(root, center, FocusDirection::RIGHT), right, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::FocusFinder::GetNearestFocusableView(root, center, FocusDirection::UP), up, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Internal::FocusFinder::GetNearestFocusableView(root, center, FocusDirection::DOWN), down, TEST_LOCATION);
  END_TEST;
}

int UtcDaliImageVisualNPatchAuxiliaryPropertiesP(void)
{
  UiTestApplication application;
  ImageVisual visual = ImageVisual::New();
  visual.SetResourceUrl("../samples/image-view/res/tooltip.9.png");
  visual.SetNPatchAuxiliaryImage("../samples/image-view/res/mask.png");
  visual.SetNPatchAuxiliaryImageAlpha(0.5f);
  visual.SetFittingMode(Image::FittingMode::CENTER);

  View view = View::New();
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(120.0f);
  DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  auto internalVisual = GetImplementation(visual).GetVisual();
  DALI_TEST_CHECK(internalVisual);
  Property::Map map;
  internalVisual.CreatePropertyMap(map);
  const Property::Value* type = map.Find(Dali::Ui::Integration::Visual::Property::TYPE);
  const Property::Value* auxiliary = map.Find(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE);
  const Property::Value* alpha = map.Find(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE_ALPHA);
  const Property::Value* fitting = map.Find(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE);
  DALI_TEST_CHECK(type && auxiliary && alpha && fitting);
  DALI_TEST_EQUALS(type->Get<int>(), static_cast<int>(Dali::Ui::Integration::InternalVisualType::N_PATCH), TEST_LOCATION);
  DALI_TEST_EQUALS(auxiliary->Get<Dali::String>(), Dali::String("../samples/image-view/res/mask.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(alpha->Get<float>(), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(fitting->Get<int>(), static_cast<int>(Image::FittingMode::CENTER), TEST_LOCATION);

  Property::Map instanceMap;
  Dali::Ui::GetImplementation(internalVisual).CreateInstancePropertyMap(instanceMap);
  DALI_TEST_CHECK(instanceMap.Find(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE));
  END_TEST;
}

int UtcDaliInnerShadowDirectVisualFactoryP(void)
{
  UiTestApplication application;
  DALI_TEST_CHECK(!Dali::Ui::Internal::InnerShadow::CreateVisual(InnerShadow::None()));
  const UiColor color(0.1f, 0.2f, 0.3f, 0.5f);
  InnerShadow shadow(Insets(20.0f, -10.0f, 30.0f, 5.0f), 8.0f, color);
  ColorVisual visual = Dali::Ui::Internal::InnerShadow::CreateVisual(shadow);
  DALI_TEST_CHECK(visual);
  DALI_TEST_EQUALS(visual.GetBlurRadius(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 15.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 12.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraWidth(), 146.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraHeight(), 121.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineWidth(), 78.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineColor(), color, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCutoutPolicy(), CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVisualBaseInstancePropertyMapsP(void)
{
  UiTestApplication application;

  auto getInstanceMap = [&](VisualBase visual)
  {
    View view = View::New();
    view.SetRequestedWidth(200.0f);
    view.SetRequestedHeight(120.0f);
    DALI_TEST_CHECK(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND));
    application.GetScene().Add(view);
    application.SendNotification();
    application.Render();

    auto internalVisual = GetImplementation(visual).GetVisual();
    DALI_TEST_CHECK(internalVisual);
    Property::Map map;
    Dali::Ui::GetImplementation(internalVisual).CreateInstancePropertyMap(map);
    return map;
  };

  ColorVisual color = ColorVisual::New();
  DALI_TEST_CHECK(getInstanceMap(color).Empty());

  TextVisual text = TextVisual::New();
  text.SetText("instance text");
  Property::Map textMap = getInstanceMap(text);
  const Property::Value* textType = textMap.Find(Dali::Ui::Integration::Visual::Property::TYPE);
  const Property::Value* textValue = textMap.Find(Dali::Ui::Integration::TextVisual::Property::TEXT);
  DALI_TEST_CHECK(textType && textValue);
  DALI_TEST_EQUALS(textType->Get<int>(), static_cast<int>(Dali::Ui::Integration::InternalVisualType::TEXT), TEST_LOCATION);
  DALI_TEST_EQUALS(textValue->Get<Dali::String>(), Dali::String("instance text"), TEST_LOCATION);

  ImageVisual image = ImageVisual::New();
  image.SetResourceUrl("../samples/image-view/res/sample.jpg");
  image.SetDesiredWidth(64);
  image.SetDesiredHeight(48);
  Property::Map imageMap = getInstanceMap(image);
  const Property::Value* imageType = imageMap.Find(Dali::Ui::Integration::Visual::Property::TYPE);
  const Property::Value* imageWidth = imageMap.Find(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH);
  DALI_TEST_CHECK(imageType && imageWidth);
  DALI_TEST_EQUALS(imageType->Get<int>(), static_cast<int>(Dali::Ui::Integration::InternalVisualType::IMAGE), TEST_LOCATION);
  DALI_TEST_EQUALS(imageWidth->Get<int>(), 64, TEST_LOCATION);

  AnimatedImageVisual animated = AnimatedImageVisual::New();
  animated.SetResourceUrl("../samples/visual-base/res/animatedLoading.gif");
  animated.SetDesiredWidth(80);
  Property::Map animatedMap = getInstanceMap(animated);
  const Property::Value* animatedType = animatedMap.Find(Dali::Ui::Integration::Visual::Property::TYPE);
  DALI_TEST_CHECK(animatedType);
  DALI_TEST_EQUALS(animatedType->Get<int>(), static_cast<int>(Dali::Ui::Integration::InternalVisualType::ANIMATED_IMAGE), TEST_LOCATION);
  END_TEST;
}
