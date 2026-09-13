/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DALI_UI_COMPONENTS_TEST_ATSPI_CAPTURE_H
#define DALI_UI_COMPONENTS_TEST_ATSPI_CAPTURE_H

#include <dali/integration-api/scene.h>
#include <memory>
#include <string>
#include <vector>

namespace ComponentsTest
{
struct AtspiStateEvent
{
  std::string path;
  std::string interface;
  std::string member;
  std::string signature;
  std::string state;
  int         value;
  int         reserved;
  int         payload;
  std::string sourceBus;
  std::string sourcePath;
};

// Captures actual BridgeObject serialized emissions at the transport boundary.
// It does NOT send to a bus or prove AT/screen-reader receipt. Construct before
// UiTestApplication, call Stop before application teardown, destroy afterward.
// Intended for the normal fork-isolated Components UTC runner.
class AtspiCapture
{
public:
  AtspiCapture();
  ~AtspiCapture();
  AtspiCapture(const AtspiCapture&) = delete;
  AtspiCapture& operator=(const AtspiCapture&) = delete;

  void Start(Dali::Integration::Scene scene);
  void Stop();
  void Tick(unsigned int count);
  void Clear();
  const std::vector<AtspiStateEvent>& Events() const;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};
}
#endif
