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
 *
 */

// CLASS HEADER
#include <dali-ui/ui-test-application.h>

// EXTERNAL INCLUDES
#include <memory>
#include <utility>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui/ui-adaptor-impl.h>
#include <dali-ui/ui-lifecycle-controller.h>

namespace Dali
{
bool UiTestApplication::ADD_IDLE_SUCCESS = true;

using AdaptorImpl = Dali::Internal::Adaptor::Adaptor;

UiTestApplication::UiTestApplication(size_t surfaceWidth, size_t surfaceHeight, float horizontalDpi, float verticalDpi)
: UiTestApplication(Ui::UiConfig::New(), surfaceWidth, surfaceHeight, horizontalDpi, verticalDpi)
{
}

UiTestApplication::UiTestApplication(Ui::UiConfig config, size_t surfaceWidth, size_t surfaceHeight, float horizontalDpi, float verticalDpi)
: TestApplication(surfaceWidth, surfaceHeight, horizontalDpi, verticalDpi, false /* Do not Initialize Core */),
  mMainWindow(),
  mAdaptor(nullptr)
{
  config.Apply();

  InitializeAdaptor(); // Need to create Adaptor first as many singletons in dali-adaptor need it

  // Create Core next
  CreateCore();

  // Create Scene from main window
  // Must not call TestApplication::CreateScene().
  CreateSceneFromMainWindow();

  // Core needs to be initialized next before we start the adaptor
  InitializeCore();

  // This will also emit the window created signals
  EmitApplicationInitialize();
}

UiTestApplication::~UiTestApplication()
{
  Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
  lifecycleController.TerminateSignal().Emit();

  // Stop adaptor after terminate signal emit
  if(Dali::Adaptor::IsAvailable() && mAdaptor)
  {
    mAdaptor->Stop();
  }

  // Need to delete core before we delete the adaptor.
  delete mCore;
  mCore = NULL;
}

void UiTestApplication::InitializeAdaptor()
{
  mAdaptor.reset(&AdaptorImpl::New());
}

void UiTestApplication::CreateSceneFromMainWindow()
{
  // Override Scene creation in TestApplication by creating a window.
  // The window will create a Scene & surface and set up the scene's surface appropriately.
  mMainWindow = Window::New(PositionSize(0, 0, mSurfaceWidth, mSurfaceHeight), "");

  mScene = AdaptorImpl::GetScene(mMainWindow);
  mScene.SetDpi(Vector2(mDpi.x, mDpi.y));

  // Create render target for the scene
  Graphics::RenderTargetCreateInfo rtInfo{};
  rtInfo.SetExtent({mSurfaceWidth, mSurfaceHeight});
  mScene.SetSurfaceRenderTarget(rtInfo);

  mScenes.push_back(mScene);
}

void UiTestApplication::EmitApplicationInitialize()
{
  AdaptorImpl::GetImpl(*mAdaptor).Start(mMainWindow);
  AdaptorImpl::GetImpl(*mAdaptor).SetApplication(*this);

  Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
  lifecycleController.PreInitSignal().Emit();
  lifecycleController.InitSignal().Emit();
}

void UiTestApplication::RunIdles()
{
  AdaptorImpl::GetImpl(*mAdaptor.get()).RunIdles();
}

} // namespace Dali
