#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace MyViewSample
{

class MyViewImpl;

class MyView : public View
{
public:

  static MyView New();
  static MyView DownCast(BaseHandle handle);

  MyView();
  MyView(const MyView& DummyComponent);
  MyView(MyView&& rhs) noexcept;
  DALI_INTERNAL MyView(MyViewImpl& impl);
  DALI_INTERNAL MyView(Dali::Internal::CustomActor* customActor);

  ~MyView();

  MyView& operator=(const MyView& handle) = default;
  MyView& operator=(MyView&& rhs) noexcept = default;

  void ChangeBackground();

  // 체인 메소드 재정의 매크로 호출
  DALI_UI_CHAIN_VIEW_METHODS(MyView)

  // [IMPORTANT] 데이터 필드를 넣지 않아야 함
};

}
