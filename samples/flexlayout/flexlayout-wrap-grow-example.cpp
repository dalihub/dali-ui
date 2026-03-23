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

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * FlexLayout wrap and flex-grow sample.
 *
 * Demonstrates FlexWrap::WRAP so items flow to the next line when
 * there is not enough room, combined with flex-grow to distribute
 * remaining space on each line.
 *
 * - Row direction with Wrap enabled
 * - 6 boxes with flex-basis 150px each
 * - Boxes with different flex-grow values (items grow to fill the line)
 * - AlignContent: SpaceAround for spacing between wrapped lines
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutWrapGrowController : public ConnectionTracker
{
public:
  FlexLayoutWrapGrowController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutWrapGrowController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    FlexLayout root = FlexLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetDirection(FlexDirection::ROW);
    root.SetWrap(FlexWrap::WRAP);
    root.SetJustifyContent(FlexJustify::FLEX_START);
    root.SetAlignContent(FlexAlign::CENTER);
    root.SetViewPadding(Extents(15, 15, 15, 15));

    // Box 1: Red, basis 150, grow 1
    View box1 = View::New();
    box1.SetBackgroundColor(Color::RED);
    box1.SetRequestedHeight(80.0f);
    box1.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(150.0f).SetFlexGrow(1.0f));
    root.Add(box1);

    // Box 2: Green, basis 150, grow 2
    View box2 = View::New();
    box2.SetBackgroundColor(Color::GREEN);
    box2.SetRequestedHeight(80.0f);
    box2.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(150.0f).SetFlexGrow(2.0f));
    root.Add(box2);

    // Box 3: Blue, basis 150, grow 1
    View box3 = View::New();
    box3.SetBackgroundColor(Color::BLUE);
    box3.SetRequestedHeight(80.0f);
    box3.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(150.0f).SetFlexGrow(1.0f));
    root.Add(box3);

    // Box 4: Yellow, basis 150, grow 1
    View box4 = View::New();
    box4.SetBackgroundColor(Color::YELLOW);
    box4.SetRequestedHeight(80.0f);
    box4.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(150.0f).SetFlexGrow(1.0f));
    root.Add(box4);

    // Box 5: Cyan, basis 150, grow 1
    View box5 = View::New();
    box5.SetBackgroundColor(Color::CYAN);
    box5.SetRequestedHeight(80.0f);
    box5.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(150.0f).SetFlexGrow(1.0f));
    root.Add(box5);

    // Box 6: Magenta, basis 150, grow 3
    View box6 = View::New();
    box6.SetBackgroundColor(Color::MAGENTA);
    box6.SetRequestedHeight(80.0f);
    box6.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(150.0f).SetFlexGrow(3.0f));
    root.Add(box6);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &FlexLayoutWrapGrowController::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  FlexLayoutWrapGrowController controller(application);
  application.MainLoop();
  return 0;
}
