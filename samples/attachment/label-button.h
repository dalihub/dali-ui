#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace Example
{

// Most DALi public classes are lightweight handles over internal implementations.
// A derived handle should not add fields for persistent state because the state
// is lost when the object is copied or upcast to the base handle type. Attachments
// provide a way to keep custom data in the implementation-side object while still
// deriving only from the public handle class.
//
// Attachment data follows the lifetime of the object it is attached to. Avoid
// attaching data that strongly references the owner, its parent, or an ancestor,
// as that can create a reference cycle. Keeping a child handle is fine because
// DALi children do not strongly reference their parent.
class LabelButton : public Label
{
public:
  LabelButton() = default;

  // [MANDATORY] Define New
  static LabelButton New(const Dali::String& text);

  // [MANDATORY] Define DownCast
  static LabelButton DownCast(BaseHandle handle);

  // [MANDATORY] Overload chain method
  DALI_UI_CHAIN_LABEL_METHODS(LabelButton)

private:
  explicit LabelButton(Label view);

  static AttachmentId GetImplId();
};

}