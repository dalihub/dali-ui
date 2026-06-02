#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>
#include "my-view.h"

using namespace Dali;
using namespace Dali::Ui;

namespace MyViewSample
{

class MyViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<MyViewImpl> New();

  MyViewImpl();
  virtual ~MyViewImpl() = default;

  void ChangeBackground();

protected:
  void OnInitialize() override;

private:
  int mChangeCount;
};

}
