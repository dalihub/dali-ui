/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <test-harness.h>

extern int UtcDaliDialogShowingBridgeSameHandleEmitsOnceP(void);
extern int UtcDaliDialogShowingBridgeHiddenOffsceneAndRemovalP(void);
extern int UtcDaliDialogShowingBridgeReplacementOrdersSourcesP(void);
extern int UtcDaliDialogShowingBridgeTransferImmediateFalseDeferredTrueP(void);
extern int UtcDaliDialogShowingBridgeRapidHideShowCoalescesLatestP(void);
extern int UtcDaliDialogShowingBridgeCaptureStartStopIsIdempotentP(void);
extern int UtcDaliDialogShowingBridgeWorldAlphaTransitionsP(void);
extern int UtcDaliDialogShowingBridgeCullingTransitionsP(void);

// Separate opt-in transport tests: no generated normal-suite header and no
// adaptor-internal/GIO dependency added to the normal Components executable.
static testcase tc_array[] = {
  {"UtcDaliDialogShowingBridgeSameHandleEmitsOnceP", UtcDaliDialogShowingBridgeSameHandleEmitsOnceP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeHiddenOffsceneAndRemovalP", UtcDaliDialogShowingBridgeHiddenOffsceneAndRemovalP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeReplacementOrdersSourcesP", UtcDaliDialogShowingBridgeReplacementOrdersSourcesP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeTransferImmediateFalseDeferredTrueP", UtcDaliDialogShowingBridgeTransferImmediateFalseDeferredTrueP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeRapidHideShowCoalescesLatestP", UtcDaliDialogShowingBridgeRapidHideShowCoalescesLatestP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeCaptureStartStopIsIdempotentP", UtcDaliDialogShowingBridgeCaptureStartStopIsIdempotentP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeWorldAlphaTransitionsP", UtcDaliDialogShowingBridgeWorldAlphaTransitionsP, nullptr, nullptr},
  {"UtcDaliDialogShowingBridgeCullingTransitionsP", UtcDaliDialogShowingBridgeCullingTransitionsP, nullptr, nullptr},
  {nullptr, nullptr, nullptr, nullptr}};

int main(int argc, char* const argv[])
{
  return TestHarness::RunTests(argc, argv, tc_array);
}
