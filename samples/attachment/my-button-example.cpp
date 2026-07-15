/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <cstdio>

using namespace Dali;
using namespace Dali::Ui;

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
class MyButton : public InteractiveView
{
public:
  struct Data
  {
    Data(Label label, UiColor normalColor, UiColor pressedColor)
    : label(label),
      normalColor(normalColor),
      pressedColor(pressedColor)
    {
    }

    Label   label;
    UiColor normalColor;
    UiColor pressedColor;
    uint32_t clickCount{0u};
  };

  MyButton() = default;

  static MyButton New(const Dali::String& text)
  {
    MyButton button(InteractiveView::New());
    button.Initialize(text);
    return button;
  }

  static MyButton DownCast(BaseHandle handle)
  {
    InteractiveView view = InteractiveView::DownCast(handle);
    return view && view.GetAttachment<Data>(GetDataId()) ? MyButton(view) : MyButton();
  }

  void Initialize(const Dali::String& text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(18.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);

    Add(label);
    SetAttachment(GetDataId(), Dali::MakeUnique<Data>(label, UiColor(0x1565C0), UiColor(0x2E7D32)));
    SetBackgroundColor(UiColor(0x1565C0));
  }

  void IncrementClickCount()
  {
    Data* data = GetAttachment<Data>(GetDataId());
    if(data)
    {
      ++data->clickCount;
      SetBackgroundColor((data->clickCount % 2u) ? data->pressedColor : data->normalColor);

      char text[64];
      std::snprintf(text, sizeof(text), "MyButton: %u", data->clickCount);
      data->label.SetText(text);
    }
  }

private:
  explicit MyButton(InteractiveView view)
  : InteractiveView(view)
  {
  }

  static AttachmentId GetDataId()
  {
    static AttachmentId id = AttachmentId::Alloc();
    return id;
  }
};

class MyButtonExampleController : public ConnectionTracker
{
public:
  explicit MyButtonExampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &MyButtonExampleController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x202020));

    MyButton button = MyButton::New("MyButton: 0");
    button.SetRequestedWidth(220.0f);
    button.SetRequestedHeight(80.0f);
    button.SetRequestedX(60.0f);
    button.SetRequestedY(60.0f);
    button.ConnectClickedSignal(this, &MyButtonExampleController::OnClicked);
    window.Add(button);
  }

  void OnClicked(View view, InputEvent event)
  {
    MyButton button = MyButton::DownCast(view);
    if(button)
    {
      button.IncrementClickCount();
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  MyButtonExampleController controller(application);
  application.MainLoop();
  return 0;
}
