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

#include "video-view-validation.h"

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/video-source-descriptor.h>
#include <player.h>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <utility>

namespace
{
using namespace Dali;

class VideoViewValidation : public ConnectionTracker
{
public:
  VideoViewValidation(Application& application, std::string uri, bool nativeImage, bool sync);
  ~VideoViewValidation() override;
  VideoViewValidation(const VideoViewValidation&)            = delete;
  VideoViewValidation& operator=(const VideoViewValidation&) = delete;
  VideoViewValidation(VideoViewValidation&&)                 = delete;
  VideoViewValidation& operator=(VideoViewValidation&&)      = delete;

  unsigned int GetFailures() const;

private:
  void           Initialize(Application application);
  bool           Check(bool condition, const char* message);
  bool           CheckApi(int error, const char* operation);
  player_state_e GetState();
  bool           CreateView();
  void           SetGeometry(float x, float y, float width, float height);
  void           Reconnect(unsigned int windowIndex, player_state_e expectedState);
  void           Snapshot(const char* stage);
  void           ReleaseView();
  void           Cleanup();
  void           Finish();
  void           OnKeyEvent(Window window, KeyEvent event);
  bool           Tick();

  Application&              mApplication;
  std::string               mUri;
  Window                    mWindows[2];
  Ui::AbsoluteLayout        mRoots[2];
  Ui::VideoView             mView;
  Timer                     mTimer;
  player_h                  mPlayer{nullptr};
  std::atomic<unsigned int> mPlayerErrors{0u};
  unsigned int              mTick{0u};
  unsigned int              mFailures{0u};
  int                       mResetPosition{0};
  bool                      mNativeImage;
  bool                      mSync;
  bool                      mQuitRequested{false};
};

VideoViewValidation::VideoViewValidation(Application& application, std::string uri, bool nativeImage, bool sync)
: mApplication(application),
  mUri(std::move(uri)),
  mNativeImage(nativeImage),
  mSync(sync)
{
  mApplication.InitSignal().Connect(this, &VideoViewValidation::Initialize);
}

VideoViewValidation::~VideoViewValidation()
{
  Cleanup();
}

unsigned int VideoViewValidation::GetFailures() const
{
  return mFailures;
}

bool VideoViewValidation::Check(bool condition, const char* message)
{
  printf("CHECK %s %s\n", condition ? "PASS" : "FAIL", message);
  if(!condition)
  {
    ++mFailures;
  }
  return condition;
}

bool VideoViewValidation::CheckApi(int error, const char* operation)
{
  printf("API %s result=%d\n", operation, error);
  return Check(error == PLAYER_ERROR_NONE, operation);
}

player_state_e VideoViewValidation::GetState()
{
  player_state_e state = PLAYER_STATE_NONE;
  CheckApi(player_get_state(mPlayer, &state), "player_get_state");
  return state;
}

void VideoViewValidation::Initialize(Application application)
{
  mWindows[0] = application.GetWindow();
  mWindows[0].KeyEventSignal().Connect(this, &VideoViewValidation::OnKeyEvent);
  if(!mNativeImage)
  {
    auto bounds = mWindows[0].GetPositionSize();
    mWindows[1] = Window::New(bounds, "VideoView validation second Window");
    mWindows[1].Hide();
  }
  for(unsigned int index = 0u; index < (mNativeImage ? 1u : 2u); ++index)
  {
    mWindows[index].SetBackgroundColor(Color::BLACK);
    mRoots[index] = Ui::AbsoluteLayout::New();
    mRoots[index].SetRequestedWidth(Ui::MATCH_PARENT);
    mRoots[index].SetRequestedHeight(Ui::MATCH_PARENT);
    mWindows[index].Add(mRoots[index]);
  }

  if(!CheckApi(player_create(&mPlayer), "player_create"))
  {
    Finish();
    return;
  }
  if(!CheckApi(player_set_uri(mPlayer, mUri.c_str()), "player_set_uri") ||
     !CheckApi(player_set_looping(mPlayer, true), "player_set_looping") ||
     !CheckApi(player_set_mute(mPlayer, true), "player_set_mute"))
  {
    Finish();
    return;
  }
  CheckApi(player_set_error_cb(mPlayer, [](int error, void* data)
  {
    auto* validation = static_cast<VideoViewValidation*>(data);
    ++validation->mPlayerErrors;
    printf("PLAYER_ERROR code=%d\n", error);
  }, this),
           "player_set_error_cb");

  if(!CreateView() || !CheckApi(player_prepare(mPlayer), "player_prepare"))
  {
    Finish();
    return;
  }
  mView.Play();
  mTimer = Timer::New(1000u);
  mTimer.TickSignal().Connect(this, &VideoViewValidation::Tick);
  mTimer.Start();
}

bool VideoViewValidation::CreateView()
{
  VideoSourceDescriptor descriptor;
  descriptor.SetProviderId("tizen.mmplayer");
  descriptor.SetNativeSession(Any(static_cast<void*>(mPlayer)));
  descriptor.SetOwnership(VideoSourceOwnership::EXTERNAL);
  descriptor.SetRenderingMode(mNativeImage ? VideoRenderingMode::NATIVE_IMAGE : VideoRenderingMode::UNDERLAY);

  mView = Ui::VideoView::New();
  mView.SetSyncMode(mSync ? Ui::VideoSyncMode::ENABLED : Ui::VideoSyncMode::DISABLED);
  if(!Check(mView.SetSource(Ui::CreateVideoSource(descriptor)), "source accepted"))
  {
    return false;
  }
  mView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  mView.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  SetGeometry(80.0f, 100.0f, 640.0f, 360.0f);
  mRoots[0].Add(mView);
  return Check(GetState() == PLAYER_STATE_IDLE, "attach before prepare keeps IDLE");
}

void VideoViewValidation::SetGeometry(float x, float y, float width, float height)
{
  mView.SetRequestedWidth(width);
  mView.SetRequestedHeight(height);
  mView.SetLayoutParams(Ui::AbsoluteLayoutParams::New().SetX(x).SetY(y).SetWidth(width).SetHeight(height));
}

void VideoViewValidation::Reconnect(unsigned int windowIndex, player_state_e expectedState)
{
  mView.Unparent();
  mRoots[windowIndex].Add(mView);
  Check(GetState() == expectedState, "reconnect preserves player state");
}

void VideoViewValidation::Snapshot(const char* stage)
{
  Vector3 size         = mView.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  Vector2 position     = mView.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();
  int     playPosition = 0;
  CheckApi(player_get_play_position(mPlayer, &playPosition), "player_get_play_position");
  printf("STAGE %s state=%d position=%d bounds=%gx%g+%g+%g\n",
         stage, GetState(), playPosition, size.width, size.height, position.x, position.y);
}

void VideoViewValidation::ReleaseView()
{
  if(mView)
  {
    mView.ClearSource();
    mView.Unparent();
    mView.Reset();
  }
}

void VideoViewValidation::Cleanup()
{
  if(mPlayer)
  {
    auto state = GetState();
    if(state == PLAYER_STATE_PLAYING || state == PLAYER_STATE_PAUSED)
    {
      CheckApi(player_stop(mPlayer), "player_stop before releasing view");
    }
    ReleaseView();
    if(GetState() == PLAYER_STATE_READY)
    {
      CheckApi(player_unprepare(mPlayer), "player_unprepare after releasing view");
    }
    CheckApi(player_destroy(mPlayer), "player_destroy");
    mPlayer = nullptr;
  }
  // Extra Windows must be released while the application's event loop is alive.
  mWindows[1].Reset();
}

void VideoViewValidation::Finish()
{
  Check(mPlayerErrors == 0u, "no asynchronous player errors");
  Cleanup();
  printf("RESULT %s failures=%u\n", mFailures ? "FAIL" : "PASS", mFailures);
  mApplication.Quit();
}

void VideoViewValidation::OnKeyEvent(Window /* window */, KeyEvent event)
{
  if(event.GetState() == KeyEvent::DOWN &&
     (event.GetKeyName() == "Escape" || event.GetKeyName() == "Back" || event.GetKeyName() == "XF86Back"))
  {
    mQuitRequested = true;
  }
}

bool VideoViewValidation::Tick()
{
  if(mQuitRequested)
  {
    Check(false, "validation interrupted before completion");
    Finish();
    return false;
  }
  switch(++mTick)
  {
    case 1u:
    {
      Check(GetState() == PLAYER_STATE_PLAYING, "initial PLAYING");
      Snapshot("baseline");
      break;
    }
    case 2u:
    {
      auto mode   = mView.GetSyncMode();
      auto source = mView.GetSource();
      mView.SetSyncMode(mode == Ui::VideoSyncMode::ENABLED ? Ui::VideoSyncMode::DISABLED : Ui::VideoSyncMode::ENABLED);
      Check(mView.GetSyncMode() == mode && mView.GetSource() == source, "SetSyncMode preserves mode and source");
      Reconnect(0u, PLAYER_STATE_PLAYING);
      break;
    }
    case 3u:
    {
      SetGeometry(240.0f, 140.0f, 640.0f, 360.0f);
      break;
    }
    case 4u:
    {
      Snapshot("moved");
      break;
    }
    case 5u:
    {
      SetGeometry(240.0f, 140.0f, 480.0f, 270.0f);
      break;
    }
    case 6u:
    {
      Snapshot("resized");
      break;
    }
    case 7u:
    {
      mRoots[0].SetProperty(Actor::Property::POSITION, Vector3(40.0f, 30.0f, 0.0f));
      break;
    }
    case 8u:
    {
      Snapshot("parent-moved");
      mView.Pause();
      Check(GetState() == PLAYER_STATE_PAUSED, "explicit pause");
      break;
    }
    case 9u:
    {
      Reconnect(0u, PLAYER_STATE_PAUSED);
      Snapshot("paused-reconnect");
      break;
    }
    case 10u:
    {
      mView.Play();
      Check(GetState() == PLAYER_STATE_PLAYING, "explicit resume");
      break;
    }
    case 11u:
    {
      Snapshot("resumed");
      if(!mNativeImage)
      {
        mWindows[1].Show();
        Reconnect(1u, PLAYER_STATE_PLAYING);
      }
      break;
    }
    case 12u:
    {
      Check(GetState() == PLAYER_STATE_PLAYING, mNativeImage ? "same Window keeps PLAYING" : "Window change attempt keeps PLAYING");
      if(!mNativeImage)
      {
        Reconnect(0u, PLAYER_STATE_PLAYING);
        mWindows[1].Hide();
      }
      break;
    }
    case 13u:
    {
      Snapshot("returned");
      break;
    }
    case 14u:
    {
      CheckApi(player_stop(mPlayer), "player_stop for source reset");
      Check(GetState() == PLAYER_STATE_READY, "app stopped before releasing view");
      ReleaseView();
      CheckApi(player_unprepare(mPlayer), "player_unprepare after source reset");
      CheckApi(player_set_uri(mPlayer, mUri.c_str()), "player_set_uri after reset");
      break;
    }
    case 15u:
    {
      if(!CreateView())
      {
        Finish();
        return false;
      }
      break;
    }
    case 16u:
    {
      if(!CheckApi(player_prepare(mPlayer), "player_prepare after reset"))
      {
        Finish();
        return false;
      }
      mView.Play();
      CheckApi(player_get_play_position(mPlayer, &mResetPosition), "initial position after reset");
      break;
    }
    case 17u:
    {
      int position = 0;
      CheckApi(player_get_play_position(mPlayer, &position), "recovered position");
      Check(GetState() == PLAYER_STATE_PLAYING && position > mResetPosition + 300, "recovered playback advances");
      Snapshot("recovered");
      break;
    }
    default:
    {
      Finish();
      return false;
    }
  }
  return true;
}

} // namespace

int RunVideoViewValidation(int argc, char** argv, std::string uri)
{
  const bool nativeImage = std::strcmp(argv[1], "test-native") == 0;
  const bool sync        = std::strcmp(argv[1], "test-sync") == 0;
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  auto application = Dali::Application::New(&argc, &argv);
  auto config      = Dali::Ui::UiConfig::New();
  config.Apply();
  VideoViewValidation validation(application, std::move(uri), nativeImage, sync);
  application.MainLoop();
  return validation.GetFailures() == 0u ? 0 : 1;
}
