#include "my-view.h"
#include "my-view-impl.h"

using namespace Dali;
using namespace Dali::Ui;

namespace MyViewSample
{

namespace
{
// Define conversion method
inline MyViewImpl& GetImpl(MyView& myView)
{
  Dali::RefObject& handle = myView.GetImplementation();
  return static_cast<MyViewImpl&>(handle);
}
}

MyView MyView::New()
{
  IntrusivePtr<MyViewImpl> impl = MyViewImpl::New();
  MyView handle(*impl); // handle이 impl의 소유권을 획득
  impl->Initialize();
  return handle;
}

MyView MyView::DownCast(BaseHandle handle)
{
  return View::DownCast<MyView, MyViewImpl>(handle);
}

MyView::MyView()
{
}

MyView::MyView(const MyView& myView)
: View(myView)
{
}

MyView::MyView(MyView&& rhs) noexcept
: View(std::move(rhs))
{
}

MyView::MyView(MyViewImpl& impl)  // New 수행시 필요
: View(impl)
{
}

MyView::MyView(Dali::Internal::CustomActor* customActor) // DownCast 수행시 필요
: View(customActor)
{
  VerifyCustomActorPointer<MyViewImpl>(customActor);
}

MyView::~MyView()
{
}

void MyView::ChangeBackground()
{
  GetImpl(*this).ChangeBackground();
}

}