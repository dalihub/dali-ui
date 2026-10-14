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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layout.h>
#include <dali-ui-foundation/public-api/flex-layout.h>
#include <dali/integration-api/debug.h>

#include <dali-ui-components/public-api/page.h>
#include <dali-ui-components/public-api/navigator.h>
#include <dali-ui-components/public-api/app-bar.h>
#include <dali-ui-components/public-api/content-page.h>
#include <dali-ui-components/public-api/dialog-page.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::UI;
using Dali::UI::View;

/**
 * This example demonstrates the Navigation API using Navigator, ContentPage, AppBar, and DialogPage.
 */
class NavigationController : public ConnectionTracker
{
public:

  NavigationController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &NavigationController::Create);
  }

  ~NavigationController() = default;

  void Create(Dali::Application& application)
  {
    Dali::Window window = application.GetWindow();
    window.SetBackgroundColor(Dali::Color::WHITE);

    // Create the main Navigator
    mNavigator =
        UI::Navigator::New().SetSizeWidth(window.GetSize().GetWidth()).SetSizeHeight(window.GetSize().GetHeight());

    window.Add(mNavigator);

    // Push the initial page
    mNavigator.Push(CreateMainPage());
  }

  // Create the Main ContentPage
  UI::ContentPage CreateMainPage()
  {
    DALI_LOG_ERROR("[CreateMainPage] Called\n");
    UI::ContentPage mainPage = UI::ContentPage::New();
    mainPage.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
    mainPage.SetSizeWidth(mNavigator.GetSizeWidth());
    mainPage.SetSizeHeight(mNavigator.GetSizeHeight());

    UI::AppBar appBar = UI::AppBar::New()
                            .BackgroundColor(Dali::Color::BLUE)
                            .SetSizeHeight(60_spx)
                            .SetSizeWidth(mNavigator.GetSizeWidth());
    appBar.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);

    // In a real implementation this would tie into an AppBar Title property
    View titleText = View::New().BackgroundColor(Dali::Color::CYAN).SetSizeWidth(200_spx).SetSizeHeight(40_spx);
    appBar.Add(titleText);

    UI::FlexLayout contentArea = UI::FlexLayout::New()
                                     .Direction(UI::FlexDirection::Column)
                                     .AlignItems(UI::FlexAlign::Center)
                                     .JustifyContent(UI::FlexJustify::Center);
    contentArea.SetSizeWidth(mNavigator.GetSizeWidth());
    contentArea.SetSizeHeight(mNavigator.GetSizeHeight() - 60_spx);
    contentArea.SetPositionY(60_spx);
    contentArea.SetBackgroundColor(Dali::Color::LIGHT_GRAY); // Added to visualize ContentArea bounds
    contentArea.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);

    // Button to Push Next Page
    View pushButton = View::New()
                          .BackgroundColor(Dali::Color::GREEN)
                          .SetSizeWidth(200_spx)
                          .SetSizeHeight(80_spx)
                          .AsClickable(this,
                                       [this](View view, const UI::InputEvent& inputEvent)
                                       {
                                         if (inputEvent.GetInputEventType() == UI::InputEventType::TOUCH_EVENT)
                                         {
                                           const Dali::TouchEvent& event = inputEvent.GetTouchEvent();
                                           DALI_LOG_ERROR("[pushButton] Touch State: %d\n", event.GetState(0));
                                           if (event.GetPointCount() > 0 && event.GetState(0) == Dali::PointState::DOWN)
                                           {
                                             DALI_LOG_ERROR("[pushButton] Pushing SecondPage\n");
                                             mNavigator.Push(CreateSecondPage());
                                             return true;
                                           }
                                         }
                                         return false;
                                       });

    // Button to show Dialog Page
    View dialogButton =
        View::New()
            .BackgroundColor(Dali::Color::YELLOW)
            .SetSizeWidth(200_spx)
            .SetSizeHeight(80_spx)
            .AsClickable(this,
                         [this](View view, const UI::InputEvent& inputEvent)
                         {
                           if (inputEvent.GetInputEventType() == UI::InputEventType::TOUCH_EVENT)
                           {
                             const Dali::TouchEvent& event = inputEvent.GetTouchEvent();
                             DALI_LOG_ERROR("[dialogButton] Touch State: %d\n", event.GetState(0));
                             if (event.GetPointCount() > 0 && event.GetState(0) == Dali::PointState::DOWN)
                             {
                               DALI_LOG_ERROR("[dialogButton] Pushing DialogPage\n");
                               mNavigator.Push(CreateDialogPage());
                               return true;
                             }
                           }
                           return false;
                         });

    contentArea.Contents({pushButton, dialogButton});

    mainPage.Add(appBar);
    mainPage.Add(contentArea);

    return mainPage;
  }

  // Create a Second ContentPage
  UI::ContentPage CreateSecondPage()
  {
    DALI_LOG_ERROR("[CreateSecondPage] Called\n");
    UI::ContentPage secondPage = UI::ContentPage::New();
    secondPage.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
    secondPage.SetSizeWidth(mNavigator.GetSizeWidth());
    secondPage.SetSizeHeight(mNavigator.GetSizeHeight());

    UI::AppBar appBar = UI::AppBar::New()
                            .BackgroundColor(Dali::Color::MAGENTA)
                            .SetSizeHeight(60_spx)
                            .SetSizeWidth(mNavigator.GetSizeWidth());
    appBar.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);

    UI::FlexLayout contentArea = UI::FlexLayout::New()
                                     .Direction(UI::FlexDirection::Column)
                                     .AlignItems(UI::FlexAlign::Center)
                                     .JustifyContent(UI::FlexJustify::Center);
    contentArea.SetSizeWidth(mNavigator.GetSizeWidth());
    contentArea.SetSizeHeight(mNavigator.GetSizeHeight() - 60_spx);
    contentArea.SetPositionY(60_spx);
    contentArea.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);

    // Button to Pop back
    View popButton = View::New()
                         .BackgroundColor(Dali::Color::RED)
                         .SetSizeWidth(200_spx)
                         .SetSizeHeight(80_spx)
                         .AsClickable(this,
                                      [this](View view, const UI::InputEvent& inputEvent)
                                      {
                                        if (inputEvent.GetInputEventType() == UI::InputEventType::TOUCH_EVENT)
                                        {
                                          const Dali::TouchEvent& event = inputEvent.GetTouchEvent();
                                          DALI_LOG_ERROR("[popButton] Touch State: %d\n", event.GetState(0));
                                          if (event.GetPointCount() > 0 && event.GetState(0) == Dali::PointState::DOWN)
                                          {
                                            DALI_LOG_ERROR("[popButton] Popping Nav\n");
                                            mNavigator.Pop();
                                            return true;
                                          }
                                        }
                                        return false;
                                      });

    contentArea.AddView(popButton);

    secondPage.Add(appBar);
    secondPage.Add(contentArea);

    return secondPage;
  }

  // Create a DialogPage
  UI::DialogPage CreateDialogPage()
  {
    DALI_LOG_ERROR("[CreateDialogPage] Called\n");
    UI::DialogPage dialogPage = UI::DialogPage::New();
    dialogPage.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
    dialogPage.SetSizeWidth(mNavigator.GetSizeWidth());
    dialogPage.SetSizeHeight(mNavigator.GetSizeHeight());

    // Scrim (dark transparent background)
    View scrim = View::New()
                     .BackgroundColor(Dali::Vector4(0.0f, 0.0f, 0.0f, 0.5f)) // Half transparent black
                     .SetSizeWidth(mNavigator.GetSizeWidth())
                     .SetSizeHeight(mNavigator.GetSizeHeight());
    scrim.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);
    scrim.AsClickable(this,
                      [this](View view, const UI::InputEvent& inputEvent)
                      {
                        if (inputEvent.GetInputEventType() == UI::InputEventType::TOUCH_EVENT)
                        {
                          const Dali::TouchEvent& event = inputEvent.GetTouchEvent();
                          DALI_LOG_ERROR("[scrim] Touch State: %d\n", event.GetState(0));
                          if (event.GetPointCount() > 0 && event.GetState(0) == Dali::PointState::DOWN)
                          {
                            DALI_LOG_ERROR("[scrim] Popping Dialog\n");
                            // Touching outside the dialog pops it
                            mNavigator.Pop();
                            return true;
                          }
                        }
                        return false;
                      });

    // The actual dialog box
    UI::FlexLayout dialogBox = UI::FlexLayout::New()
                                   .Direction(UI::FlexDirection::Column)
                                   .AlignItems(UI::FlexAlign::Center)
                                   .JustifyContent(UI::FlexJustify::Center);
    dialogBox.SetBackgroundColor(Dali::Color::WHITE);
    dialogBox.SetSizeWidth(300_spx);
    dialogBox.SetSizeHeight(200_spx);
    dialogBox.SetPositionX((mNavigator.GetSizeWidth() - 300_spx) / 2.0f);
    dialogBox.SetPositionY((mNavigator.GetSizeHeight() - 200_spx) / 2.0f);
    dialogBox.SetParentOrigin(Dali::ParentOrigin::TOP_LEFT);

    View okButton = View::New()
                        .BackgroundColor(Dali::Color::BLUE)
                        .SetSizeWidth(150_spx)
                        .SetSizeHeight(50_spx)
                        .AsClickable(this,
                                     [this](View view, const UI::InputEvent& inputEvent)
                                     {
                                       if (inputEvent.GetInputEventType() == UI::InputEventType::TOUCH_EVENT)
                                       {
                                         const Dali::TouchEvent& event = inputEvent.GetTouchEvent();
                                         DALI_LOG_ERROR("[okButton] Touch State: %d\n", event.GetState(0));
                                         if (event.GetPointCount() > 0 && event.GetState(0) == Dali::PointState::DOWN)
                                         {
                                           DALI_LOG_ERROR("[okButton] Popping Dialog\n");
                                           mNavigator.Pop();
                                           return true;
                                         }
                                       }
                                       return false;
                                     });

    dialogBox.AddView(okButton);

    dialogPage.Add(scrim);
    dialogPage.Add(dialogBox);

    return dialogPage;
  }

  void OnKeyEvent(const Dali::KeyEvent& event)
  {
    DALI_LOG_ERROR("[OnKeyEvent] %s state: %d\n", event.GetKeyName().c_str(), event.GetState());
    if (event.GetState() == Dali::KeyEvent::DOWN)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        if (mNavigator.GetPageCount() > 1)
        {
          mNavigator.Pop();
        }
        else
        {
          mApplication.Quit();
        }
      }
    }
  }

private:
  Dali::Application& mApplication;
  UI::Navigator mNavigator;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Dali::Application application = Dali::Application::New(&argc, &argv);
  NavigationController test(application);
  application.MainLoop();
  return 0;
}
