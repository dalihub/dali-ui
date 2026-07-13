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

namespace
{
const AttachmentId COUNTER_DATA_ID = AttachmentId::Alloc();

struct CounterData
{
  explicit CounterData(Label label)
  : label(label)
  {
  }

  Label    label;
  uint32_t count{0u};
};

void UpdateLabel(View view)
{
  CounterData* data = view.GetAttachment<CounterData>(COUNTER_DATA_ID);
  if(data)
  {
    char text[64];
    std::snprintf(text, sizeof(text), "Count: %u", data->count);
    data->label.SetText(text);
  }
}
} // unnamed namespace

class AttachmentExampleController : public ConnectionTracker
{
public:
  explicit AttachmentExampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AttachmentExampleController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x202020));

    Label label = Label::New("Count: 0");
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetFontSize(18.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);

    InteractiveView counter = InteractiveView::New();
    counter.SetAttachment(COUNTER_DATA_ID, Dali::MakeUnique<CounterData>(label));
    counter.SetRequestedWidth(220.0f);
    counter.SetRequestedHeight(80.0f);
    counter.SetRequestedPositionX(60.0f);
    counter.SetRequestedPositionY(60.0f);
    counter.SetBackgroundColor(UiColor(0x1565C0));
    counter.Add(label);
    counter.ConnectClickedSignal(this, &AttachmentExampleController::OnCounterClicked);

    Label resetLabel = Label::New("Detach and Reset");
    resetLabel.SetRequestedWidth(MATCH_PARENT);
    resetLabel.SetRequestedHeight(MATCH_PARENT);
    resetLabel.SetFontSize(16.0f);
    resetLabel.SetTextColor(UiColor(0xFFFFFF));
    resetLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    resetLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    InteractiveView reset = InteractiveView::New();
    reset.SetRequestedWidth(220.0f);
    reset.SetRequestedHeight(64.0f);
    reset.SetRequestedPositionX(60.0f);
    reset.SetRequestedPositionY(160.0f);
    reset.SetBackgroundColor(UiColor(0x424242));
    reset.ConnectClickedSignal(this, &AttachmentExampleController::OnResetClicked);
    reset.Add(resetLabel);

    mCounter = counter;
    window.Add(counter);
    window.Add(reset);
  }

  void OnCounterClicked(View view, InputEvent event)
  {
    CounterData* data = view.GetAttachment<CounterData>(COUNTER_DATA_ID);
    if(data)
    {
      ++data->count;
      view.SetBackgroundColor((data->count % 2u) ? UiColor(0x2E7D32) : UiColor(0x1565C0));
      UpdateLabel(view);
    }
  }

  void OnResetClicked(View view, InputEvent event)
  {
    Dali::UniquePtr<CounterData> data = mCounter.DetachAttachment<CounterData>(COUNTER_DATA_ID);
    if(data)
    {
      data->count = 0u;
      data->label.SetText("Count: 0");
      mCounter.SetAttachment(COUNTER_DATA_ID, Dali::Move(data));
      mCounter.SetBackgroundColor(UiColor(0x1565C0));
    }
  }

private:
  Application& mApplication;
  View         mCounter;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  AttachmentExampleController controller(application);
  application.MainLoop();
  return 0;
}
