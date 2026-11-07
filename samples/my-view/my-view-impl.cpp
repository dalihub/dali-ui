#include "my-view-impl.h"
#include <dali-ui-foundation/public-api/views/view-registration.h>

using namespace Dali;
using namespace Dali::Ui;

namespace MyViewSample
{

namespace
{
// For type Registration
BaseHandle Create()
{
  return MyView::New();
}

// Type registration (optional): without it MyView still works as a plain View.
// Registering makes the type discoverable and creatable by the name "MyView".
// The type is registered under the handle class's name (MyView), while instances
// are recognised through the implementation class's (MyViewImpl) RTTI.
Dali::Ui::ViewRegistration myViewRegistration(DALI_TYPE_ID(MyView), DALI_TYPE_ID(MyViewImpl), DALI_TYPE_ID(Dali::Ui::ViewImpl), Create);

} // namespace

IntrusivePtr<MyViewImpl> MyViewImpl::New()
{
  IntrusivePtr<MyViewImpl> impl(new MyViewImpl());

  // The arrange producer's default policy is ArrangePolicy::IF_CHANGED. This class
  // does not override OnArrange, so the default arrangement can be cached without
  // any further setup. If OnArrange reads ancestor/world coordinates (such as
  // SCREEN_POSITION) or updates a surface outside the actor tree, call
  // SetArrangePolicy(ArrangePolicy::ALWAYS) in the constructor.

  return impl;
}

MyViewImpl::MyViewImpl()
: ViewImpl()
{
  // [NOTE] The handle cannot be accessed at this point; it is available from OnInitialize onwards.
}

void MyViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  MyView handle = MyView::DownCast(Self()); // Get handle
  handle.SetBackgroundColor(UiColor(0x00FFFF));
  handle.SetRequestedWidth(200_spx);
  handle.SetRequestedHeight(200_spx);
}

void MyViewImpl::ChangeBackground()
{
  static UiColor colors[] = {
    UiColor(0x00FFFF),
    UiColor(0xFF00FF),
    UiColor(0xFFFF00),
    UiColor(0xFF0000),
    UiColor(0x00FF00),
    UiColor(0x0000FF)};
  static const int size = sizeof(colors) / sizeof(colors[0]);

  mChangeCount = (mChangeCount + 1) % size;

  MyView handle = MyView::DownCast(Self());
  handle.SetBackgroundColor(colors[mChangeCount]);
}
} //namespace MyViewSample
