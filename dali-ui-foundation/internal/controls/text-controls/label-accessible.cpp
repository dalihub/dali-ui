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

#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/controls/text-controls/label-accessible.h>
#include <dali-ui-foundation/internal/text/anchor/anchor-interaction-data.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui::Integration
{
std::pair<std::string, bool> LabelAccessible::GetNameRaw() const
{
  return {GetWholeText(), false};
}

Property::Index LabelAccessible::GetNamePropertyIndex()
{
  return Ui::Text::LabelPropertyIndex::TEXT;
}

const std::vector<Ui::TextAnchor>& LabelAccessible::GetTextAnchors() const
{
  return Internal::Text::GetA11yAnchors(Ui::View::DownCast(Self()));
}

Ui::Text::ControllerPtr LabelAccessible::GetTextController() const
{
  auto  self = Ui::View::DownCast(Self());
  auto& impl = static_cast<LabelImpl&>(Ui::GetImpl(self));
  return impl.mController;
}

} // namespace Dali::Ui::Integration
