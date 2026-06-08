#pragma once

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace Example
{

class LabelButtonImpl : public ConnectionTracker
{
public:
  LabelButtonImpl(UiColor normalColor, UiColor pressedColor)
  : mNormalColor(normalColor),
    mPressedColor(pressedColor)
  {
  }

  void Initialize(LabelButton label)
  {
    label.SetRequestedWidth(220.0f);
    label.SetRequestedHeight(80.0f);
    label.SetFontSize(18.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetBackgroundColor(UiColor(0x1565C0));

    label.TouchedSignal().Connect(this, &LabelButtonImpl::OnTouched);
  }

  bool OnTouched(Actor actor, TouchEvent e)
  {
    Label label = Label::DownCast(actor);

    if(e.GetState(0) == PointState::Type::DOWN)
    {
      label.SetBackgroundColor(mNormalColor);
    }
    else if(e.GetState(0) == PointState::Type::UP)
    {
      label.SetBackgroundColor(mPressedColor);

      char text[64];
      std::snprintf(text, sizeof(text), "LabelButton: %u", ++mClickCount);
      label.SetText(text);
    }
    return true;
  }

private:
  // WeakHandle<LabelButton> mOwner;
  UiColor   mNormalColor;
  UiColor mPressedColor;
  uint32_t mClickCount{0u};
};

}
