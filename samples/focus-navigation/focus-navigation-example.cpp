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

#include <algorithm>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

class FocusNavigationController : public ConnectionTracker
{
public:
  explicit FocusNavigationController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FocusNavigationController::OnInit);
  }

  ~FocusNavigationController()
  {
    FocusManager manager = FocusManager::Get();
    if(manager)
    {
      manager.SetFocusNavigationFallback({});
    }
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x101820));

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label guide = Label::New("No initial focus is requested. Press an arrow key or Tab.\n"
                             "The application fallback uses the custom order 1 → 4 → 2 → 3.");
    guide.SetRequestedX(30.0f);
    guide.SetRequestedY(20.0f);
    guide.SetRequestedWidth(560.0f);
    guide.SetRequestedHeight(80.0f);
    guide.SetTextColor(UiColor(0xFFFFFF));
    guide.SetMultiLine(true);
    root.Add(guide);

    View first = MakeFocusableView("1", 50.0f, 130.0f, 0xC84B31);
    View second = MakeFocusableView("2", 270.0f, 130.0f, 0x2D6A9F);
    View third = MakeFocusableView("3", 50.0f, 300.0f, 0x4C956C);
    View fourth = MakeFocusableView("4", 270.0f, 300.0f, 0x8F5DA2);

    root.Add(first);
    root.Add(second);
    root.Add(third);
    root.Add(fourth);
    window.Add(root);

    mTraversalOrder = {first, fourth, second, third};

    FocusManager manager = FocusManager::Get();
    manager.FocusChangedSignal().Connect(this, &FocusNavigationController::OnFocusChanged);
    manager.SetFocusNavigationFallback(
      FocusNavigationCallback::New(this, &FocusNavigationController::GetNextFocusableView));
  }

  View MakeFocusableView(const char* text, float x, float y, uint32_t color)
  {
    View view = View::New();
    view.SetFocusable(true);
    view.SetRequestedX(x);
    view.SetRequestedY(y);
    view.SetRequestedWidth(170.0f);
    view.SetRequestedHeight(120.0f);
    view.SetBackgroundColor(UiColor(color));
    view.SetOpacity(0.72f);

    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetFontSize(34.0f);
    label.SetTextColor(UiColor(0xFFFFFF));
    view.Add(label);
    return view;
  }

  FocusNavigationResult GetNextFocusableView(View current, FocusNavigationContext context)
  {
    if(mTraversalOrder.empty())
    {
      return FocusNavigationResult::NotHandled();
    }

    if(!current)
    {
      return FocusNavigationResult::MoveTo(mTraversalOrder.front());
    }

    auto currentIterator = std::find(mTraversalOrder.begin(), mTraversalOrder.end(), current);
    if(currentIterator == mTraversalOrder.end())
    {
      return FocusNavigationResult::NotHandled();
    }

    const FocusDirection direction = context.GetDirection();
    const bool forward = direction == FocusDirection::RIGHT ||
                         direction == FocusDirection::DOWN ||
                         direction == FocusDirection::FORWARD;
    const bool backward = direction == FocusDirection::LEFT ||
                          direction == FocusDirection::UP ||
                          direction == FocusDirection::BACKWARD;

    if(forward)
    {
      ++currentIterator;
      return currentIterator == mTraversalOrder.end()
               ? FocusNavigationResult::Stay()
               : FocusNavigationResult::MoveTo(*currentIterator);
    }

    if(backward)
    {
      if(currentIterator == mTraversalOrder.begin())
      {
        return FocusNavigationResult::Stay();
      }
      return FocusNavigationResult::MoveTo(*--currentIterator);
    }

    return FocusNavigationResult::NotHandled();
  }

  void OnFocusChanged(View previous, View current)
  {
    if(previous)
    {
      previous.SetOpacity(0.72f);
    }
    if(current)
    {
      current.SetOpacity(1.0f);
    }
  }

private:
  Application& mApplication;
  std::vector<View> mTraversalOrder;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  FocusNavigationController controller(application);
  application.MainLoop();
  return 0;
}
