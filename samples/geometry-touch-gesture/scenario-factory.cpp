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

// INTERNAL INCLUDES
#include "sample-common.h"

namespace GeometryTouchGestureSample
{
std::unique_ptr<Scenario> CreateTouchRoutingScenario(EventLog& eventLog, const Dali::Vector2& canvasSize);
std::unique_ptr<Scenario> CreateSharedDetectorScenario(EventLog& eventLog, const Dali::Vector2& canvasSize);
std::unique_ptr<Scenario> CreateTapLongPressScenario(EventLog& eventLog, const Dali::Vector2& canvasSize);
std::unique_ptr<Scenario> CreateTapPanScenario(EventLog& eventLog, const Dali::Vector2& canvasSize);
std::unique_ptr<Scenario> CreateNestedOrthogonalPanScenario(EventLog& eventLog, const Dali::Vector2& canvasSize);
std::unique_ptr<Scenario> CreateSameDirectionHandoffScenario(EventLog& eventLog, const Dali::Vector2& canvasSize);

std::unique_ptr<Scenario> CreateScenario(std::size_t index, EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  switch(index)
  {
    case 0u:
      return CreateTouchRoutingScenario(eventLog, canvasSize);
    case 1u:
      return CreateSharedDetectorScenario(eventLog, canvasSize);
    case 2u:
      return CreateTapLongPressScenario(eventLog, canvasSize);
    case 3u:
      return CreateTapPanScenario(eventLog, canvasSize);
    case 4u:
      return CreateNestedOrthogonalPanScenario(eventLog, canvasSize);
    case 5u:
      return CreateSameDirectionHandoffScenario(eventLog, canvasSize);
  }
  return nullptr;
}

} // namespace GeometryTouchGestureSample
