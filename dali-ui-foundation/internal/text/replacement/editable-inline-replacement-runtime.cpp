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

#include <dali-ui-foundation/internal/text/replacement/editable-inline-replacement-runtime.h>

#include <dali-ui-foundation/internal/text/replacement/replacement-render-state.h>

#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali/public-api/common/unique-ptr.h>

namespace Dali::Ui::Internal::Text
{
namespace
{
const AttachmentId EDITABLE_INLINE_REPLACEMENT_RUNTIME_ATTACHMENT_ID = AttachmentId::Alloc();
}

EditableInlineReplacementUpdate ResolveEditableInlineReplacementUpdate(
  const Ui::Text::ReplacementSourceSnapshot& source,
  const Ui::Text::ReplacementRenderState&    state,
  bool                                       suppressed)
{
  if(!source.hasValidReplacementSource || source.runs.Empty() || suppressed)
  {
    return EditableInlineReplacementUpdate::CLEAR;
  }

  if(!state.attempted || state.sourceRevision != source.sourceRevision)
  {
    return EditableInlineReplacementUpdate::WAIT_FOR_LAYOUT;
  }

  return state.processingModel && state.projection.HasReplacements()
           ? EditableInlineReplacementUpdate::APPLY
           : EditableInlineReplacementUpdate::CLEAR;
}

EditableInlineReplacementRuntime::EditableInlineReplacementRuntime(Ui::View owner)
: visualLayer(Ui::View::New()),
  host(visualLayer, Dali::Ui::Integration::DepthIndex::CONTENT)
{
  visualLayer.SetProperty(Actor::Property::NAME, "EditableInlineReplacementLayer");
  visualLayer.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  visualLayer.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  visualLayer.SetProperty(Actor::Property::SENSITIVE, false);
  visualLayer.SetUiScalePolicy(UiScalePolicy::DISABLED);
}

EditableInlineReplacementRuntime::~EditableInlineReplacementRuntime()
{
  manager.Clear();
  visualLayer.Unparent();
}

void EditableInlineReplacementRuntime::PlaceVisualLayer(Actor          contentParent,
                                                        Actor          textActor,
                                                        Actor          cursorLayer,
                                                        const Vector2& contentSize)
{
  if(!contentParent || !visualLayer)
  {
    return;
  }

  if(visualLayer.GetParent() != contentParent)
  {
    visualLayer.Unparent();
    contentParent.Add(visualLayer);
  }
  visualLayer.SetProperty(Actor::Property::POSITION, Vector2::ZERO);
  visualLayer.SetProperty(Actor::Property::SIZE, contentSize);

  if(textActor && textActor.GetParent() == contentParent)
  {
    visualLayer.LowerBelow(textActor);
  }
  else if(cursorLayer && cursorLayer.GetParent() == contentParent)
  {
    visualLayer.LowerBelow(cursorLayer);
  }
}

EditableInlineReplacementRuntime* GetEditableInlineReplacementRuntime(Ui::View owner)
{
  return owner ? owner.GetAttachment<EditableInlineReplacementRuntime>(EDITABLE_INLINE_REPLACEMENT_RUNTIME_ATTACHMENT_ID)
               : nullptr;
}

EditableInlineReplacementRuntime* GetEditableInlineReplacementRuntime(Ui::ViewImpl& owner)
{
  using StoredType      = Dali::UniquePtr<EditableInlineReplacementRuntime>;
  UniqueAny* attachment = Internal::ViewDataImpl::Get(owner).GetAttachment(
    EDITABLE_INLINE_REPLACEMENT_RUNTIME_ATTACHMENT_ID);
  StoredType* data = attachment ? attachment->Get<StoredType>() : nullptr;
  return data ? data->Get() : nullptr;
}

EditableInlineReplacementRuntime& GetOrCreateEditableInlineReplacementRuntime(Ui::View owner)
{
  DALI_ASSERT_ALWAYS(owner && "Inline replacement runtime requires a valid owner");
  EditableInlineReplacementRuntime* data = GetEditableInlineReplacementRuntime(owner);
  if(!data)
  {
    owner.SetAttachment(EDITABLE_INLINE_REPLACEMENT_RUNTIME_ATTACHMENT_ID,
                        Dali::MakeUnique<EditableInlineReplacementRuntime>(owner));
    data = GetEditableInlineReplacementRuntime(owner);
  }
  DALI_ASSERT_ALWAYS(data && "Inline replacement runtime creation failed");
  return *data;
}

void RemoveEditableInlineReplacementRuntime(Ui::View owner)
{
  if(owner)
  {
    owner.RemoveAttachment(EDITABLE_INLINE_REPLACEMENT_RUNTIME_ATTACHMENT_ID);
  }
}

void RemoveEditableInlineReplacementRuntime(Ui::ViewImpl& owner)
{
  Internal::ViewDataImpl::Get(owner).RemoveAttachment(EDITABLE_INLINE_REPLACEMENT_RUNTIME_ATTACHMENT_ID);
}

} // namespace Dali::Ui::Internal::Text
