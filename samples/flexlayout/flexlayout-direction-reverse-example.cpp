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
#include <dali-ui-foundation/public-api/flex-layout.h>
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/stack-layout-params.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * FlexLayout direction reverse sample.
 *
 * Demonstrates RowReverse and ColumnReverse directions.
 *
 * Layout: a vertical StackLayout holding two FlexLayout sections:
 *
 *   Section 1: FlexDirection::ROW_REVERSE
 *     - Items laid out right-to-left (1, 2, 3 appear as 3, 2, 1)
 *     - JustifyContent: FlexStart (items pack to the right/start of reverse)
 *
 *   Section 2: FlexDirection::COLUMN_REVERSE
 *     - Items laid out bottom-to-top (1, 2, 3 appear as 3, 2, 1)
 *     - JustifyContent: FlexStart (items pack to the bottom/start of reverse)
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutDirectionReverseController : public ConnectionTracker
{
public:
  FlexLayoutDirectionReverseController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutDirectionReverseController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Outer vertical StackLayout to hold two sections
    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.SetSpacing(16.0f);
    outer.SetViewPadding(Extents(16, 16, 16, 16));

    // Section 1: RowReverse
    FlexLayout rowReverse = FlexLayout::New();
    rowReverse.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    rowReverse.SetDirection(FlexDirection::ROW_REVERSE);
    rowReverse.SetJustifyContent(FlexJustify::FLEX_START);
    rowReverse.SetAlignItems(FlexAlign::CENTER);
    rowReverse.SetViewPadding(Extents(8, 8, 8, 8));
    rowReverse.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    View rrBox1 = View::New();
    rrBox1.SetBackgroundColor(Color::RED);
    rrBox1.SetRequestedWidth(100.0f);
    rrBox1.SetRequestedHeight(80.0f);
    rowReverse.AddView(rrBox1);

    View rrBox2 = View::New();
    rrBox2.SetBackgroundColor(Color::GREEN);
    rrBox2.SetRequestedWidth(100.0f);
    rrBox2.SetRequestedHeight(60.0f);
    rowReverse.AddView(rrBox2);

    View rrBox3 = View::New();
    rrBox3.SetBackgroundColor(Color::BLUE);
    rrBox3.SetRequestedWidth(100.0f);
    rrBox3.SetRequestedHeight(100.0f);
    rowReverse.AddView(rrBox3);

    outer.AddView(rowReverse);

    // Section 2: ColumnReverse
    FlexLayout columnReverse = FlexLayout::New();
    columnReverse.SetLayoutParams(StackLayoutParams::New().SetWeight(2.0f).SetAlignment(LayoutAlignment::FILL));
    columnReverse.SetDirection(FlexDirection::COLUMN_REVERSE);
    columnReverse.SetJustifyContent(FlexJustify::FLEX_START);
    columnReverse.SetAlignItems(FlexAlign::CENTER);
    columnReverse.SetViewPadding(Extents(8, 8, 8, 8));
    columnReverse.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    View crBox1 = View::New();
    crBox1.SetBackgroundColor(Color::YELLOW);
    crBox1.SetRequestedWidth(200.0f);
    crBox1.SetRequestedHeight(60.0f);
    columnReverse.AddView(crBox1);

    View crBox2 = View::New();
    crBox2.SetBackgroundColor(Color::CYAN);
    crBox2.SetRequestedWidth(160.0f);
    crBox2.SetRequestedHeight(60.0f);
    columnReverse.AddView(crBox2);

    View crBox3 = View::New();
    crBox3.SetBackgroundColor(Color::MAGENTA);
    crBox3.SetRequestedWidth(120.0f);
    crBox3.SetRequestedHeight(60.0f);
    columnReverse.AddView(crBox3);

    outer.AddView(columnReverse);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &FlexLayoutDirectionReverseController::OnKeyEvent);
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
  FlexLayoutDirectionReverseController controller(application);
  application.MainLoop();
  return 0;
}
