// /*
//  * Copyright (c) 2026 Samsung Electronics Co., Ltd.
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  * http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  *
//  */

// #include <stdlib.h>
// #include <iostream>

// #include <dali.h>
// #include <dali-test-suite-utils.h>
// #include <dali-ui-foundation/dali-ui-foundation.h>
// #include <dali-ui-foundation/integration-api/view-impl.h>

// using namespace Dali;
// using namespace Dali::Ui;
// using namespace Dali::Ui::Integration;

// namespace
// {

// // ============================================================================
// // Helpers
// // ============================================================================

// struct StateChangedData
// {
//   StateChangedData()
//   : callCount(0)
//   {
//   }

//   void Reset()
//   {
//     callCount = 0;
//     view      = View();
//     prev      = UiState::Normal;
//     cur       = UiState::Normal;
//   }

//   int     callCount;
//   View    view;
//   UiState prev;
//   UiState cur;
// };

// struct StateChangedFunctor
// {
//   StateChangedFunctor(StateChangedData& data)
//   : mData(data)
//   {
//   }

//   void operator()(View view, const StateEvent& e)
//   {
//     ++mData.callCount;
//     mData.view = view;
//     mData.prev = e.GetPrev();
//     mData.cur  = e.GetCurrent();
//   }

//   StateChangedData& mData;
// };

// /**
//  * @brief A helper class that implements ConnectionTracker for tracking signal lifetime.
//  */
// class StateHandlerTracker : public ConnectionTracker
// {
// public:
//   StateHandlerTracker(StateChangedData& data)
//   : mData(data)
//   {
//   }

//   void HandleStateChanged(View view, const StateEvent& e)
//   {
//     ++mData.callCount;
//     mData.view = view;
//     mData.prev = e.GetPrev();
//     mData.cur  = e.GetCurrent();
//   }

//   StateChangedData& mData;
// };

// View CreateView(TestApplication& application)
// {
//   View view = View::New();
//   view.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
//   application.GetScene().Add(view);
//   application.SendNotification();
//   application.Render();
//   return view;
// }

// } // namespace

// void utc_dali_viewstatehandler_startup(void)
// {
//   test_return_value = TET_UNDEF;
// }

// void utc_dali_viewstatehandler_cleanup(void)
// {
//   test_return_value = TET_PASS;
// }

// // ============================================================================
// // WhenStateChanged — basic registration and dispatch (member function, #2)
// // ============================================================================

// int UtcDaliViewWhenStateChangedMemberFunctionP(void)
// {
//   TestApplication    application;
//   View               view = CreateView(application);
//   StateChangedData   data;
//   StateHandlerTracker tracker(data);

//   View& result = GetImpl(view).WhenStateChanged("TestHandler", &tracker, &StateHandlerTracker::HandleStateChanged);
//   DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

//   // Trigger a state change
//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
//   DALI_TEST_CHECK(data.view == view);
//   DALI_TEST_CHECK(UiState::Focused.WasAdded(data.prev, data.cur));

//   END_TEST;
// }

// // ============================================================================
// // WhenStateChanged — lambda form (#4)
// // ============================================================================

// int UtcDaliViewWhenStateChangedLambdaP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   StateChangedData data;
//   ConnectionTracker tracker;

//   GetImpl(view).WhenStateChanged("TestHandler", &tracker, StateChangedFunctor(data));

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
//   DALI_TEST_CHECK(UiState::Focused.WasAdded(data.prev, data.cur));

//   END_TEST;
// }

// // ============================================================================
// // WhenStateChanged — method chaining returns *this
// // ============================================================================

// int UtcDaliViewWhenStateChangedChainingP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData data;

//   View& result = view
//     .WhenStateChanged("Handler1", &tracker, StateChangedFunctor(data))
//     .WhenStateChanged("Handler2", &tracker, StateChangedFunctor(data));

//   DALI_TEST_EQUALS(&result, &view, TEST_LOCATION);

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   // Both handlers should fire
//   DALI_TEST_EQUALS(data.callCount, 2, TEST_LOCATION);

//   END_TEST;
// }

// // ============================================================================
// // WhenStateChanged — same ID replaces previous handler
// // ============================================================================

// int UtcDaliViewWhenStateChangedReplaceP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData data1, data2;

//   GetImpl(view).WhenStateChanged("MyHandler", &tracker, StateChangedFunctor(data1));
//   // Register with the same ID — should replace the first
//   GetImpl(view).WhenStateChanged("MyHandler", &tracker, StateChangedFunctor(data2));

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(data1.callCount, 0, TEST_LOCATION); // old handler NOT called
//   DALI_TEST_EQUALS(data2.callCount, 1, TEST_LOCATION); // new handler called

//   END_TEST;
// }

// // ============================================================================
// // WhenStateChanged — no dispatch when state does not change
// // ============================================================================

// int UtcDaliViewWhenStateChangedNoChangeN(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData data;

//   GetImpl(view).WhenStateChanged("MyHandler", &tracker, StateChangedFunctor(data));

//   // Set same state twice — second call should not dispatch
//   GetImpl(view).SetViewState(UiState::Focused, true);
//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);

//   END_TEST;
// }

// // ============================================================================
// // UnsetStateHandler — removes a handler by ID
// // ============================================================================

// int UtcDaliViewUnsetStateHandlerP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData data;

//   GetImpl(view).WhenStateChanged("MyHandler", &tracker, StateChangedFunctor(data));

//   bool removed = GetImpl(view).UnsetStateHandler("MyHandler");
//   DALI_TEST_CHECK(removed);

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION); // not called after removal

//   END_TEST;
// }

// int UtcDaliViewUnsetStateHandlerNotFoundN(void)
// {
//   TestApplication application;
//   View            view = CreateView(application);

//   // No handlers registered — should return false
//   bool removed = GetImpl(view).UnsetStateHandler("NonExistent");
//   DALI_TEST_CHECK(!removed);

//   END_TEST;
// }

// // ============================================================================
// // UnsetStateHandler — only removes the specified ID, others intact
// // ============================================================================

// int UtcDaliViewUnsetStateHandlerSeleciveP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData dataA, dataB;

//   GetImpl(view).WhenStateChanged("HandlerA", &tracker, StateChangedFunctor(dataA));
//   GetImpl(view).WhenStateChanged("HandlerB", &tracker, StateChangedFunctor(dataB));

//   GetImpl(view).UnsetStateHandler("HandlerA");

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(dataA.callCount, 0, TEST_LOCATION); // removed
//   DALI_TEST_EQUALS(dataB.callCount, 1, TEST_LOCATION); // still active

//   END_TEST;
// }

// // ============================================================================
// // UnsetStateHandlerWhenNotProcessing — skip removal during own handler
// // ============================================================================

// int UtcDaliViewUnsetStateHandlerWhenNotProcessingSkipsP(void)
// {
//   TestApplication  application;
//   View             view    = CreateView(application);
//   ConnectionTracker tracker;
//   bool             handlerCalled = false;
//   bool             unsetResult   = true; // intentionally set to true to verify it becomes false

//   GetImpl(view).WhenStateChanged("BackgroundColor", &tracker, [&](View v, const StateEvent&) {
//     handlerCalled = true;
//     // Try to unset self during processing — should be skipped
//     unsetResult = GetImpl(v).UnsetStateHandlerWhenNotProcessing("BackgroundColor");
//   });

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_CHECK(handlerCalled);
//   DALI_TEST_CHECK(!unsetResult); // returns false because it is processing

//   // Handler should still be active after skipped removal
//   handlerCalled = false;
//   GetImpl(view).SetViewState(UiState::Pressed, true);
//   DALI_TEST_CHECK(handlerCalled);

//   END_TEST;
// }

// int UtcDaliViewUnsetStateHandlerWhenNotProcessingSucceedsP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData data;

//   GetImpl(view).WhenStateChanged("BackgroundColor", &tracker, StateChangedFunctor(data));

//   // Called from outside a handler — should succeed
//   bool removed = GetImpl(view).UnsetStateHandlerWhenNotProcessing("BackgroundColor");
//   DALI_TEST_CHECK(removed);

//   GetImpl(view).SetViewState(UiState::Focused, true);
//   DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION);

//   END_TEST;
// }

// // ============================================================================
// // Auto-disconnect when tracker is destroyed
// // ============================================================================

// int UtcDaliViewWhenStateChangedAutoDisconnectP(void)
// {
//   TestApplication application;
//   View            view = CreateView(application);
//   StateChangedData data;

//   {
//     ConnectionTracker tracker;
//     GetImpl(view).WhenStateChanged("MyHandler", &tracker, StateChangedFunctor(data));

//     GetImpl(view).SetViewState(UiState::Focused, true);
//     DALI_TEST_EQUALS(data.callCount, 1, TEST_LOCATION);
//     data.Reset();

//     // tracker goes out of scope — auto-disconnect
//   }

//   // After tracker destroyed, handler should no longer fire
//   GetImpl(view).SetViewState(UiState::Pressed, true);
//   DALI_TEST_EQUALS(data.callCount, 0, TEST_LOCATION);

//   END_TEST;
// }

// // ============================================================================
// // Multiple state changes — prev/cur correctness
// // ============================================================================

// int UtcDaliViewWhenStateChangedPrevCurP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData data;

//   GetImpl(view).WhenStateChanged("Observer", &tracker, StateChangedFunctor(data));

//   GetImpl(view).SetViewState(UiState::Focused, true);
//   DALI_TEST_CHECK(!data.prev.Contains(UiState::Focused));
//   DALI_TEST_CHECK(data.cur.Contains(UiState::Focused));
//   data.Reset();

//   GetImpl(view).SetViewState(UiState::Pressed, true);
//   DALI_TEST_CHECK(data.prev.Contains(UiState::Focused));
//   DALI_TEST_CHECK(data.cur.Contains(UiState::Focused));
//   DALI_TEST_CHECK(data.cur.Contains(UiState::Pressed));

//   END_TEST;
// }

// // ============================================================================
// // Multiple handlers with different IDs all called on state change
// // ============================================================================

// int UtcDaliViewWhenStateChangedMultipleHandlersP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData dataA, dataB, dataC;

//   GetImpl(view).WhenStateChanged("A", &tracker, StateChangedFunctor(dataA));
//   GetImpl(view).WhenStateChanged("B", &tracker, StateChangedFunctor(dataB));
//   GetImpl(view).WhenStateChanged("C", &tracker, StateChangedFunctor(dataC));

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(dataA.callCount, 1, TEST_LOCATION);
//   DALI_TEST_EQUALS(dataB.callCount, 1, TEST_LOCATION);
//   DALI_TEST_EQUALS(dataC.callCount, 1, TEST_LOCATION);

//   END_TEST;
// }

// // ============================================================================
// // Named handlers and StateChangedSignal both fire
// // ============================================================================

// int UtcDaliViewWhenStateChangedAndSignalBothFireP(void)
// {
//   TestApplication  application;
//   View             view = CreateView(application);
//   ConnectionTracker tracker;
//   StateChangedData namedData, signalData;

//   // Named handler
//   GetImpl(view).WhenStateChanged("Observer", &tracker, StateChangedFunctor(namedData));
//   // Anonymous signal
//   view.StateChangedSignal().Connect(&tracker, StateChangedFunctor(signalData));

//   GetImpl(view).SetViewState(UiState::Focused, true);

//   DALI_TEST_EQUALS(namedData.callCount, 1, TEST_LOCATION);
//   DALI_TEST_EQUALS(signalData.callCount, 1, TEST_LOCATION);

//   END_TEST;
// }
