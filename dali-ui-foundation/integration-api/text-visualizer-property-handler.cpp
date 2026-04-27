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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text-visualizer-impl.h>
#include <dali-ui-foundation/integration-api/text-visualizer-property-handler.h>

namespace Dali::Ui::Integration
{

void TextVisualizerImpl::PropertyHandler::SetProperty(Ui::View view, Property::Index index, const Property::Value& value)
{
  TextVisualizerImpl& impl = static_cast<TextVisualizerImpl&>(GetImpl(view));

  switch(index)
  {
    case Text::TextVisualizerPropertyIndex::TEXT:
    {
      impl.SetText(value.Get<Dali::String>());
      break;
    }
    case Text::TextVisualizerPropertyIndex::FONT_FAMILY:
    {
      impl.SetFontFamily(value.Get<Dali::String>());
      break;
    }
    case Text::TextVisualizerPropertyIndex::FONT_SIZE:
    {
      impl.SetFontSize(value.Get<float>());
      break;
    }
    case Text::TextVisualizerPropertyIndex::TEXT_COLOR:
    {
      impl.SetTextColor(UiColor(value.Get<Vector4>()));
      break;
    }
    case Text::TextVisualizerPropertyIndex::LINE_HEIGHT:
    {
      impl.SetLineHeight(value.Get<float>());
      break;
    }
  }
}

Property::Value TextVisualizerImpl::PropertyHandler::GetProperty(Ui::View view, Property::Index index)
{
  Property::Value     value;
  TextVisualizerImpl& impl = static_cast<TextVisualizerImpl&>(GetImpl(view));

  switch(index)
  {
    case Text::TextVisualizerPropertyIndex::TEXT:
    {
      value = impl.GetText();
      break;
    }
    case Text::TextVisualizerPropertyIndex::FONT_FAMILY:
    {
      value = impl.GetFontFamily();
      break;
    }
    case Text::TextVisualizerPropertyIndex::FONT_SIZE:
    {
      value = impl.GetFontSize();
      break;
    }
    case Text::TextVisualizerPropertyIndex::TEXT_COLOR:
    {
      value = impl.GetTextColor().Resolve();
      break;
    }
    case Text::TextVisualizerPropertyIndex::LINE_HEIGHT:
    {
      value = impl.GetLineHeight();
      break;
    }
  }

  return value;
}

} // namespace Dali::Ui::Integration
