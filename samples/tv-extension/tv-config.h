/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#pragma once

#include <dali-ui-foundation/public-api/ui-config.h>

class TVConfig : public Dali::Ui::UiConfig
{
public:
  TVConfig() = default;

  explicit TVConfig(const Dali::Ui::UiConfig& config)
  : Dali::Ui::UiConfig(config)
  {
  }

  static TVConfig New();
  static bool IsApplied();

  void Apply();

private:
  static bool mApplied;
};
