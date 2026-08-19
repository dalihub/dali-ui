#include "my-view-impl.h"
#include <dali/devel-api/object/type-registry-helper.h>

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

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN_FULL(MyView, MyViewImpl, Dali::Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

IntrusivePtr<MyViewImpl> MyViewImpl::New()
{
  IntrusivePtr<MyViewImpl> impl(new MyViewImpl());

  // Arrange producer의 기본 정책은 ArrangePolicy::IF_CHANGED입니다. 이 클래스는
  // OnArrange를 재정의하지 않으므로 별도 설정 없이 기본 배치를 캐시할 수 있습니다.
  // OnArrange가 조상/월드 좌표(SCREEN_POSITION 등)를 읽거나 actor 트리 밖의
  // 표면을 갱신한다면 생성자에서
  // SetArrangePolicy(ArrangePolicy::ALWAYS)를 호출하십시오.

  return impl;
}

MyViewImpl::MyViewImpl()
: ViewImpl()
{
  // [NOTE] 이 시점에는 핸들에 접근할 수 없습니다. OnInitialize 부터 가능합니다.
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
