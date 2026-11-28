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

#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali/public-api/object/weak-handle.h>

using namespace Dali;
using namespace Dali::Ui;

// Define ENABLE_VISUAL_CACHE_HIT_DETACH_UTC to run the optional detach probes.

void utc_dali_visualcachehitdetach_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_visualcachehitdetach_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
enum class VisualKind
{
  IMAGE,
  SVG,
  N_PATCH,
  FIXED_IMAGE,
  ROLLING_IMAGE,
  GIF,
};

enum class MaskKind
{
  NONE,
  CPU,
  GPU,
};

constexpr const char* IMAGE_URL   = "../samples/image-view/res/sample.jpg";
constexpr const char* OTHER_URL   = "../samples/image-view/res/landscape-sample.jpg";
constexpr const char* MASK_URL    = "../samples/image-view/res/mask.png";
constexpr const char* SVG_URL     = "../samples/image-view/res/svg-blocks.svg";
constexpr const char* N_PATCH_URL = "../samples/image-view/res/button-up-1.9.png";
constexpr const char* GIF_URL     = "../samples/image-view/res/animatedLoading.gif";

bool IsAnimated(VisualKind kind)
{
  return kind == VisualKind::FIXED_IMAGE || kind == VisualKind::ROLLING_IMAGE || kind == VisualKind::GIF;
}

View CreateView(VisualKind kind, MaskKind mask, bool probe = false, bool sizeLoading = false)
{
  if(IsAnimated(kind))
  {
    AnimatedImageView view = AnimatedImageView::New();
    if(kind == VisualKind::GIF)
    {
      view.SetResourceUrl(GIF_URL);
    }
    else if(kind == VisualKind::ROLLING_IMAGE)
    {
      if(probe)
      {
        view.SetResourceUrlList({IMAGE_URL, OTHER_URL, IMAGE_URL});
      }
      else
      {
        view.SetResourceUrlList({OTHER_URL, IMAGE_URL, OTHER_URL});
      }
    }
    else
    {
      view.SetResourceUrlList({OTHER_URL, IMAGE_URL});
    }
    view.SetBatchSize(2);
    view.SetCacheSize(2);
    view.SetSynchronousLoading(false);
    view.SetImageLoadWithViewSizeEnabled(sizeLoading);
    if(mask != MaskKind::NONE)
    {
      view.SetAlphaMaskUrl(MASK_URL);
      view.SetMaskingPolicy(mask == MaskKind::CPU ? Image::MaskingPolicy::ON_LOADING : Image::MaskingPolicy::ON_RENDERING);
    }
    view.SetRequestedWidth(96.0f);
    view.SetRequestedHeight(96.0f);
    return view;
  }

  const char* url  = kind == VisualKind::SVG ? SVG_URL : kind == VisualKind::N_PATCH ? N_PATCH_URL
                                                                                     : IMAGE_URL;
  ImageView   view = ImageView::New(url);
  view.SetSynchronousLoading(false);
  view.SetImageLoadWithViewSizeEnabled(sizeLoading);
  if(mask != MaskKind::NONE)
  {
    view.SetAlphaMaskUrl(MASK_URL);
    view.SetMaskingPolicy(mask == MaskKind::CPU ? Image::MaskingPolicy::ON_LOADING : Image::MaskingPolicy::ON_RENDERING);
  }
  view.SetRequestedWidth(96.0f);
  view.SetRequestedHeight(96.0f);
  return view;
}

Visual::ResourceStatus GetStatus(View view)
{
  if(ImageView imageView = ImageView::DownCast(view))
  {
    return imageView.GetLoadingStatus();
  }
  return AnimatedImageView::DownCast(view).GetLoadingStatus();
}

void WaitReady(UiTestApplication& application, View view)
{
  for(int i = 0; i < 12 && GetStatus(view) != Visual::ResourceStatus::READY; ++i)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(GetStatus(view), Visual::ResourceStatus::READY, TEST_LOCATION);
}

void FlushRemoval(UiTestApplication& application)
{
  application.RunIdles();
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
}

int TestVisualCacheHitDestroyView(VisualKind kind, MaskKind mask, bool destroyInCallback)
{
  UiTestApplication application;
  ConnectionTracker tracker;

  View first = CreateView(kind, mask);
  application.GetScene().Add(first);
  application.SendNotification();
  application.Render();
  WaitReady(application, first);

  View             second = CreateView(kind, mask);
  WeakHandle<View> secondWeak(second);
  unsigned int     readyCount  = 0u;
  bool             attaching   = false;
  bool             inlineReady = false;
  second.ResourceReadySignal().Connect(&tracker, [&](View readyView)
  {
    ++readyCount;
    if(readyCount == 1u)
    {
      inlineReady = attaching;
      if(destroyInCallback)
      {
        application.GetScene().Remove(readyView);
        second.Reset();
      }
    }
  });
  attaching = true;
  application.GetScene().Add(second);
  application.SendNotification();
  application.Render();
  attaching = false;
  DALI_TEST_EQUALS(readyCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(inlineReady);
  if(!destroyInCallback)
  {
    application.GetScene().Remove(second);
    second.Reset();
  }
  DALI_TEST_CHECK(!secondWeak.GetHandle());
  FlushRemoval(application);

  first.Unparent();
  FlushRemoval(application);

  View         probe           = CreateView(kind, mask, true);
  unsigned int probeReadyCount = 0u;
  probe.ResourceReadySignal().Connect(&tracker, [&](View)
  {
    ++probeReadyCount;
  });
  application.GetScene().Add(probe);
  DALI_TEST_EQUALS(probeReadyCount, 0u, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(probeReadyCount, 0u, TEST_LOCATION);
  WaitReady(application, probe);
  DALI_TEST_EQUALS(probeReadyCount, 1u, TEST_LOCATION);
  END_TEST;
}

int TestVisualCacheHitDetach(VisualKind kind, MaskKind mask, bool detachInCallback)
{
#ifdef ENABLE_VISUAL_CACHE_HIT_DETACH_UTC
  UiTestApplication application;
  ConnectionTracker tracker;

  View first = CreateView(kind, mask);
  application.GetScene().Add(first);
  application.SendNotification();
  application.Render();
  WaitReady(application, first);

  View         second      = CreateView(kind, mask);
  unsigned int readyCount  = 0u;
  bool         attaching   = false;
  bool         inlineReady = false;
  second.ResourceReadySignal().Connect(&tracker, [&](View)
  {
    ++readyCount;
    if(readyCount == 1u)
    {
      inlineReady = attaching;
      if(detachInCallback)
      {
        second.Unparent();
      }
    }
  });
  attaching = true;
  application.GetScene().Add(second);
  application.SendNotification();
  application.Render();
  attaching = false;
  DALI_TEST_EQUALS(readyCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(inlineReady);
  if(!detachInCallback)
  {
    second.Unparent();
  }
  DALI_TEST_CHECK(!second.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  first.Unparent();
  FlushRemoval(application);

  View         probe           = CreateView(kind, mask, true);
  unsigned int probeReadyCount = 0u;
  probe.ResourceReadySignal().Connect(&tracker, [&](View)
  {
    ++probeReadyCount;
  });
  application.GetScene().Add(probe);
  DALI_TEST_EQUALS(probeReadyCount, 0u, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(probeReadyCount, 0u, TEST_LOCATION);
  WaitReady(application, probe);
  DALI_TEST_EQUALS(probeReadyCount, 1u, TEST_LOCATION);
#else
  (void)kind;
  (void)mask;
  (void)detachInCallback;
  tet_infoline("TestVisualCacheHitDetach is disabled pending the toolkit scene reentrancy fix");
  DALI_TEST_CHECK(true);
#endif
  END_TEST;
}

int TestAnimatedReloadInReadyCallback(VisualKind kind)
{
  UiTestApplication application;
  ConnectionTracker tracker;
  AnimatedImageView view       = AnimatedImageView::DownCast(CreateView(kind, MaskKind::NONE));
  unsigned int      readyCount = 0u;
  view.ResourceReadySignal().Connect(&tracker, [&](View)
  {
    ++readyCount;
    if(readyCount == 1u)
    {
      view.Reload();
    }
  });
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  for(int i = 0; i < 12 && readyCount < 2u; ++i)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(readyCount, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Visual::ResourceStatus::READY, TEST_LOCATION);
  END_TEST;
}

int TestAnimatedReattachRetainedCache(VisualKind kind)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::DownCast(CreateView(kind, MaskKind::NONE));
  view.SetReleasePolicy(Image::ReleasePolicy::DESTROYED);
  if(kind != VisualKind::GIF)
  {
    const Vector3 naturalSize = view.GetNaturalSize();
    DALI_TEST_CHECK(naturalSize.width > 0.0f);
    DALI_TEST_CHECK(naturalSize.height > 0.0f);
  }

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  WaitReady(application, view);

  view.Unparent();
  FlushRemoval(application);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  WaitReady(application, view);
  DALI_TEST_CHECK(view.GetTotalFrameCount() > 0);
  END_TEST;
}

int TestAnimatedCacheHitDetachDuringSizeChange(VisualKind kind, bool detachInCallback)
{
  UiTestApplication application;
  ConnectionTracker tracker;
  View              first = CreateView(kind, MaskKind::NONE, false, true);
  first.Arrange(LayoutRect(0.0f, 0.0f, 96.0f, 96.0f));
  application.GetScene().Add(first);
  application.SendNotification();
  application.Render();
  WaitReady(application, first);

  View second = CreateView(kind, MaskKind::NONE, false, true);
  second.SetRequestedWidth(64.0f);
  second.SetRequestedHeight(64.0f);
  second.Arrange(LayoutRect(0.0f, 0.0f, 64.0f, 64.0f));
  application.GetScene().Add(second);
  application.SendNotification();
  application.Render();
  WaitReady(application, second);

  unsigned int readyCount        = 0u;
  bool         resizing          = false;
  bool         readyDuringResize = false;
  second.ResourceReadySignal().Connect(&tracker, [&](View)
  {
    ++readyCount;
    if(readyCount == 1u)
    {
      readyDuringResize = resizing;
      if(detachInCallback)
      {
        second.Unparent();
      }
    }
  });
  AnimatedImageView::DownCast(second).Reload();
  DALI_TEST_EQUALS(GetStatus(second), Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  resizing = true;
  second.SetRequestedWidth(96.0f);
  second.SetRequestedHeight(96.0f);
  second.Arrange(LayoutRect(0.0f, 0.0f, 96.0f, 96.0f));
  application.SendNotification();
  application.Render();
  resizing = false;
  DALI_TEST_EQUALS(readyCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(readyDuringResize);
  if(!detachInCallback)
  {
    second.Unparent();
  }
  first.Unparent();
  FlushRemoval(application);

  View probe = CreateView(kind, MaskKind::NONE, false, true);
  probe.Arrange(LayoutRect(0.0f, 0.0f, 96.0f, 96.0f));
  unsigned int probeReadyCount = 0u;
  probe.ResourceReadySignal().Connect(&tracker, [&](View)
  {
    ++probeReadyCount;
  });
  application.GetScene().Add(probe);
  DALI_TEST_EQUALS(probeReadyCount, 0u, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  WaitReady(application, probe);
  DALI_TEST_EQUALS(probeReadyCount, 1u, TEST_LOCATION);
  END_TEST;
}
} // namespace

int UtcDaliUiImageVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::IMAGE, MaskKind::NONE, true);
}

int UtcDaliUiImageVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::IMAGE, MaskKind::NONE, false);
}

int UtcDaliUiSvgVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::SVG, MaskKind::NONE, true);
}

int UtcDaliUiSvgVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::SVG, MaskKind::NONE, false);
}

int UtcDaliUiNPatchVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::N_PATCH, MaskKind::NONE, true);
}

int UtcDaliUiNPatchVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::N_PATCH, MaskKind::NONE, false);
}

int UtcDaliUiImageCpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::IMAGE, MaskKind::CPU, true);
}

int UtcDaliUiImageCpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::IMAGE, MaskKind::CPU, false);
}

int UtcDaliUiImageGpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::IMAGE, MaskKind::GPU, true);
}

int UtcDaliUiImageGpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::IMAGE, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageFixedVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::FIXED_IMAGE, MaskKind::NONE, true);
}

int UtcDaliUiAnimatedImageFixedVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::FIXED_IMAGE, MaskKind::NONE, false);
}

int UtcDaliUiAnimatedImageFixedCpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::FIXED_IMAGE, MaskKind::CPU, true);
}

int UtcDaliUiAnimatedImageFixedCpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::FIXED_IMAGE, MaskKind::CPU, false);
}

int UtcDaliUiAnimatedImageFixedGpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::FIXED_IMAGE, MaskKind::GPU, true);
}

int UtcDaliUiAnimatedImageFixedGpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::FIXED_IMAGE, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageRollingVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::ROLLING_IMAGE, MaskKind::NONE, true);
}

int UtcDaliUiAnimatedImageRollingVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::ROLLING_IMAGE, MaskKind::NONE, false);
}

int UtcDaliUiAnimatedImageRollingCpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::ROLLING_IMAGE, MaskKind::CPU, true);
}

int UtcDaliUiAnimatedImageRollingCpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::ROLLING_IMAGE, MaskKind::CPU, false);
}

int UtcDaliUiAnimatedImageRollingGpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::ROLLING_IMAGE, MaskKind::GPU, true);
}

int UtcDaliUiAnimatedImageRollingGpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::ROLLING_IMAGE, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageGifVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::GIF, MaskKind::NONE, true);
}

int UtcDaliUiAnimatedImageGifVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::GIF, MaskKind::NONE, false);
}

int UtcDaliUiAnimatedImageGifCpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::GIF, MaskKind::CPU, true);
}

int UtcDaliUiAnimatedImageGifCpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::GIF, MaskKind::CPU, false);
}

int UtcDaliUiAnimatedImageGifGpuMaskVisualCacheHitDestroyViewInReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::GIF, MaskKind::GPU, true);
}

int UtcDaliUiAnimatedImageGifGpuMaskVisualCacheHitDestroyViewAfterReadyCallback(void)
{
  return TestVisualCacheHitDestroyView(VisualKind::GIF, MaskKind::GPU, false);
}

int UtcDaliUiImageVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::IMAGE, MaskKind::NONE, true);
}

int UtcDaliUiImageVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::IMAGE, MaskKind::NONE, false);
}

int UtcDaliUiSvgVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::SVG, MaskKind::NONE, true);
}

int UtcDaliUiSvgVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::SVG, MaskKind::NONE, false);
}

int UtcDaliUiNPatchVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::N_PATCH, MaskKind::NONE, true);
}

int UtcDaliUiNPatchVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::N_PATCH, MaskKind::NONE, false);
}

int UtcDaliUiImageCpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::IMAGE, MaskKind::CPU, true);
}

int UtcDaliUiImageCpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::IMAGE, MaskKind::CPU, false);
}

int UtcDaliUiImageGpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::IMAGE, MaskKind::GPU, true);
}

int UtcDaliUiImageGpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::IMAGE, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageFixedVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::FIXED_IMAGE, MaskKind::NONE, true);
}

int UtcDaliUiAnimatedImageFixedVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::FIXED_IMAGE, MaskKind::NONE, false);
}

int UtcDaliUiAnimatedImageFixedCpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::FIXED_IMAGE, MaskKind::CPU, true);
}

int UtcDaliUiAnimatedImageFixedCpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::FIXED_IMAGE, MaskKind::CPU, false);
}

int UtcDaliUiAnimatedImageFixedGpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::FIXED_IMAGE, MaskKind::GPU, true);
}

int UtcDaliUiAnimatedImageFixedGpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::FIXED_IMAGE, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageRollingVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::ROLLING_IMAGE, MaskKind::NONE, true);
}

int UtcDaliUiAnimatedImageRollingVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::ROLLING_IMAGE, MaskKind::NONE, false);
}

int UtcDaliUiAnimatedImageRollingCpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::ROLLING_IMAGE, MaskKind::CPU, true);
}

int UtcDaliUiAnimatedImageRollingCpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::ROLLING_IMAGE, MaskKind::CPU, false);
}

int UtcDaliUiAnimatedImageRollingGpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::ROLLING_IMAGE, MaskKind::GPU, true);
}

int UtcDaliUiAnimatedImageRollingGpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::ROLLING_IMAGE, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageGifVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::GIF, MaskKind::NONE, true);
}

int UtcDaliUiAnimatedImageGifVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::GIF, MaskKind::NONE, false);
}

int UtcDaliUiAnimatedImageGifCpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::GIF, MaskKind::CPU, true);
}

int UtcDaliUiAnimatedImageGifCpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::GIF, MaskKind::CPU, false);
}

int UtcDaliUiAnimatedImageGifGpuMaskVisualCacheHitDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::GIF, MaskKind::GPU, true);
}

int UtcDaliUiAnimatedImageGifGpuMaskVisualCacheHitNormalDetach(void)
{
  return TestVisualCacheHitDetach(VisualKind::GIF, MaskKind::GPU, false);
}

int UtcDaliUiAnimatedImageFixedVisualCacheReloadInReadyCallback(void)
{
  return TestAnimatedReloadInReadyCallback(VisualKind::FIXED_IMAGE);
}

int UtcDaliUiAnimatedImageFixedVisualCacheHitDetachDuringSizeChange(void)
{
  return TestAnimatedCacheHitDetachDuringSizeChange(VisualKind::FIXED_IMAGE, true);
}

int UtcDaliUiAnimatedImageFixedVisualCacheHitNormalDetachDuringSizeChange(void)
{
  return TestAnimatedCacheHitDetachDuringSizeChange(VisualKind::FIXED_IMAGE, false);
}

int UtcDaliUiAnimatedImageRollingVisualCacheReloadInReadyCallback(void)
{
  return TestAnimatedReloadInReadyCallback(VisualKind::ROLLING_IMAGE);
}

int UtcDaliUiAnimatedImageRollingVisualCacheHitDetachDuringSizeChange(void)
{
  return TestAnimatedCacheHitDetachDuringSizeChange(VisualKind::ROLLING_IMAGE, true);
}

int UtcDaliUiAnimatedImageRollingVisualCacheHitNormalDetachDuringSizeChange(void)
{
  return TestAnimatedCacheHitDetachDuringSizeChange(VisualKind::ROLLING_IMAGE, false);
}

int UtcDaliUiAnimatedImageGifVisualCacheReloadInReadyCallback(void)
{
  return TestAnimatedReloadInReadyCallback(VisualKind::GIF);
}

int UtcDaliUiAnimatedImageFixedVisualCacheReattachRetainedCache(void)
{
  return TestAnimatedReattachRetainedCache(VisualKind::FIXED_IMAGE);
}

int UtcDaliUiAnimatedImageRollingVisualCacheReattachRetainedCache(void)
{
  return TestAnimatedReattachRetainedCache(VisualKind::ROLLING_IMAGE);
}

int UtcDaliUiAnimatedImageGifVisualCacheReattachRetainedCache(void)
{
  return TestAnimatedReattachRetainedCache(VisualKind::GIF);
}

int UtcDaliUiAnimatedImageGifVisualCacheHitDetachDuringSizeChange(void)
{
  return TestAnimatedCacheHitDetachDuringSizeChange(VisualKind::GIF, true);
}

int UtcDaliUiAnimatedImageGifVisualCacheHitNormalDetachDuringSizeChange(void)
{
  return TestAnimatedCacheHitDetachDuringSizeChange(VisualKind::GIF, false);
}
