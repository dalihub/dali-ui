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

#include <dali-ui-foundation/internal/visuals/transition-data-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui::Internal;

void utc_dali_transition_data_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_transition_data_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
Property::Map CreateAnimator(const Property::Value& property, const Property::Value& alphaFunction, const char* animationType)
{
  Property::Map timePeriod;
  timePeriod.Insert("delay", 0.25f);
  timePeriod.Insert("duration", 1.5f);

  Property::Map animator;
  animator.Insert("alphaFunction", alphaFunction);
  animator.Insert("timePeriod", timePeriod);
  animator.Insert("animationType", animationType);
  animator.Insert(17, true);

  Property::Map transition;
  transition.Insert("target", "targetActor");
  transition.Insert("property", property);
  transition.Insert("initialValue", Vector3::ZERO);
  transition.Insert("targetValue", Vector3::ONE);
  transition.Insert("animator", animator);
  transition.Insert(9, false);
  return transition;
}
} // unnamed namespace

int UtcDaliTransitionDataMapRoundTripP(void)
{
  Property::Map map = CreateAnimator(Property::Value("position"), Property::Value("EASE_IN_OUT_SINE"), "BY");
  TransitionDataPtr data = TransitionData::New(map);

  DALI_TEST_EQUALS(data->Count(), 1u, TEST_LOCATION);
  TransitionData::Animator* entry = *data->Begin();
  DALI_TEST_EQUALS(entry->objectName, std::string("targetActor"), TEST_LOCATION);
  DALI_TEST_EQUALS(entry->propertyKey.stringKey, Dali::String("position"), TEST_LOCATION);
  DALI_TEST_EQUALS(entry->timePeriodDelay, 0.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(entry->timePeriodDuration, 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(entry->animationType, TransitionData::AnimationType::BY, TEST_LOCATION);
  DALI_TEST_CHECK(entry->animate);
  DALI_TEST_CHECK(data->Begin() != data->End());

  Property::Map roundTrip = data->GetAnimatorAt(0u);
  DALI_TEST_CHECK(roundTrip.Find("target"));
  DALI_TEST_CHECK(roundTrip.Find("initialValue"));
  DALI_TEST_CHECK(roundTrip.Find("targetValue"));
  DALI_TEST_CHECK(roundTrip.Find("animator"));
  END_TEST;
}

int UtcDaliTransitionDataArrayAndAlphaFunctionsP(void)
{
  const char* alphaFunctions[] = {
    "LINEAR", "REVERSE", "EASE_IN", "EASE_OUT", "EASE_IN_SQUARE", "EASE_IN_OUT", "EASE_IN_OUT_SINE",
    "EASE_IN_SINE", "EASE_OUT_SQUARE", "EASE_OUT_SINE", "EASE_OUT_BACK", "BOUNCE", "SIN", "UNKNOWN"};

  Property::Array transitions;
  transitions.PushBack(42);
  for(const char* alphaFunction : alphaFunctions)
  {
    transitions.PushBack(CreateAnimator(Property::Value(Actor::Property::COLOR), Property::Value(alphaFunction), "TO"));
  }

  TransitionDataPtr data = TransitionData::New(transitions);
  DALI_TEST_EQUALS(data->Count(), sizeof(alphaFunctions) / sizeof(alphaFunctions[0]), TEST_LOCATION);

  for(auto iter = data->Begin(); iter != data->End(); ++iter)
  {
    DALI_TEST_CHECK((*iter)->propertyKey.indexKey == Actor::Property::COLOR);
    DALI_TEST_EQUALS((*iter)->animationType, TransitionData::AnimationType::TO, TEST_LOCATION);
    DALI_TEST_CHECK((*iter)->animate);
  }

  Property::Map noAnimator;
  noAnimator.Insert("target", "plain");
  noAnimator.Insert("property", 12);
  TransitionDataPtr plain = TransitionData::New(noAnimator);
  DALI_TEST_CHECK(!(*plain->Begin())->animate);
  Property::Map plainRoundTrip = plain->GetAnimatorAt(0u);
  DALI_TEST_CHECK(!plainRoundTrip.Find("initialValue"));
  DALI_TEST_CHECK(!plainRoundTrip.Find("targetValue"));
  DALI_TEST_CHECK(!plainRoundTrip.Find("animator"));
  END_TEST;
}

int UtcDaliTransitionDataBezierAndInvalidP(void)
{
  Property::Array validBezier;
  validBezier.PushBack(0.1f);
  validBezier.PushBack(0.2f);
  validBezier.PushBack(0.8f);
  validBezier.PushBack(0.9f);
  TransitionDataPtr arrayBezier = TransitionData::New(CreateAnimator(Property::Value("opacity"), validBezier, "BETWEEN"));
  DALI_TEST_CHECK((*arrayBezier->Begin())->animate);
  DALI_TEST_EQUALS((*arrayBezier->Begin())->alphaFunction.GetMode(), AlphaFunction::BEZIER, TEST_LOCATION);
  DALI_TEST_EQUALS((*arrayBezier->Begin())->animationType, TransitionData::AnimationType::BETWEEN, TEST_LOCATION);
  DALI_TEST_CHECK(arrayBezier->GetAnimatorAt(0u).Find("animator"));

  TransitionDataPtr vectorBezier = TransitionData::New(
    CreateAnimator(Property::Value("opacity"), Property::Value(Vector4(0.2f, 0.3f, 0.7f, 0.8f)), "INVALID"));
  DALI_TEST_EQUALS((*vectorBezier->Begin())->alphaFunction.GetMode(), AlphaFunction::BEZIER, TEST_LOCATION);

  Property::Array shortBezier;
  shortBezier.PushBack(0.1f);
  TransitionDataPtr invalidArray = TransitionData::New(CreateAnimator(Property::Value("opacity"), shortBezier, "TO"));
  DALI_TEST_CHECK(!(*invalidArray->Begin())->animate);

  Property::Array wrongBezier;
  wrongBezier.PushBack(0.1f);
  wrongBezier.PushBack("bad");
  wrongBezier.PushBack(0.8f);
  wrongBezier.PushBack(0.9f);
  TransitionDataPtr invalidElement = TransitionData::New(CreateAnimator(Property::Value("opacity"), wrongBezier, "TO"));
  DALI_TEST_CHECK(!(*invalidElement->Begin())->animate);

  TransitionDataPtr invalidType = TransitionData::New(CreateAnimator(Property::Value("opacity"), Property::Value(true), "TO"));
  DALI_TEST_CHECK(!(*invalidType->Begin())->animate);
  END_TEST;
}
