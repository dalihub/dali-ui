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

// EXTERNAL INCLUDES
#include <camera.h>
#include <player.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/public-api/text-button.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/views/media/media-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/key.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

/**
 * @brief Demonstrates a MediaView with application-owned player and camera sources.
 *
 * Buttons run on the application thread. Only one producer is active at a time;
 * its borrowed source is disconnected before the application destroys it.
 */
class MediaViewExample final : public ConnectionTracker
{
public:
  MediaViewExample(Application application, bool useCamera, const std::string& videoPath)
  : mApplication(application),
    mUseCamera(useCamera),
    mVideoPath(videoPath)
  {
    mApplication.InitSignal().Connect(this, &MediaViewExample::Initialize);
    mApplication.TerminateSignal().Connect(this, &MediaViewExample::Terminate);
  }

  ~MediaViewExample() override
  {
    CloseProducer();
  }

  bool Failed() const
  {
    return mFailed;
  }

private:
  enum class Action : std::size_t
  {
    PLAYER,
    CAMERA,
    START,
    PAUSE,
    STOP,
    SEEK,
    CROP,
    HORIZONTAL_FLIP,
    VERTICAL_FLIP,
    HALF_SIZE,
    CORNER_RADIUS,
    SCENE,
    SOURCE,
    RESET,
    RESTART,
    QUIT,
    COUNT
  };

  MediaViewExample()                                   = delete;
  MediaViewExample(const MediaViewExample&)            = delete;
  MediaViewExample(MediaViewExample&&)                 = delete;
  MediaViewExample& operator=(const MediaViewExample&) = delete;
  MediaViewExample& operator=(MediaViewExample&&)      = delete;

  bool Check(int result, const char* operation)
  {
    DALI_LOG_RELEASE_INFO("[media-view-sample] %s: 0x%x\n", operation, result);
    mLastOperation = operation;
    if(result != 0)
    {
      mFailed = true;
      std::ostringstream message;
      message << operation << " failed: 0x" << std::hex << result;
      mLastOperation = message.str();
      return false;
    }
    return true;
  }

  static void Place(View view, float x, float y, float width, float height)
  {
    view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, width, height)));
  }

  void Initialize(Application application)
  {
    Components::UiConfig::New().Apply();
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Vector4(0.06f, 0.08f, 0.12f, 1.0f));
    mWindow.KeyEventSignal().Connect(this, &MediaViewExample::OnKeyEvent);

    const auto  bounds       = mWindow.GetPositionSize();
    const float panelX       = static_cast<float>(bounds.width) * 0.62f;
    const float buttonWidth  = (static_cast<float>(bounds.width) - panelX - 40.0f) * 0.5f;
    const float buttonHeight = std::min(48.0f, (static_cast<float>(bounds.height) - 84.0f) / 8.0f - 8.0f);
    mPreviewSize             = Vector2(panelX - 32.0f, static_cast<float>(bounds.height) - 164.0f);

    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mWindow.Add(mRoot);

    Label title = Label::New("MediaView | Player + Camera");
    title.SetFontSize(22.0f);
    title.SetTextColor(Color::WHITE);
    Place(title, 16.0f, 12.0f, static_cast<float>(bounds.width) - 32.0f, 36.0f);
    mRoot.Add(title);

    mPreview = AbsoluteLayout::New();
    mPreview.SetBackgroundColor(Vector4(0.8f, 0.1f, 0.6f, 1.0f));
    Place(mPreview, 16.0f, 56.0f, mPreviewSize.x, mPreviewSize.y);
    mRoot.Add(mPreview);
    mView = MediaView::New();
    mPreview.Add(mView);

    mStatus = Label::New();
    mStatus.SetFontSize(14.0f);
    mStatus.SetTextColor(Color::WHITE);
    mStatus.SetMultiLine(true);
    Place(mStatus, 16.0f, static_cast<float>(bounds.height) - 96.0f, panelX - 32.0f, 88.0f);
    mRoot.Add(mStatus);

    const std::array<const char*, static_cast<std::size_t>(Action::COUNT)> labels = {
      "Player", "Camera", "Start / Resume", "Pause", "Stop", "Seek +5s",
      "Crop: off", "H flip: off", "V flip: off", "Half size: off",
      "Radius: 0", "Scene: in", "Source: on", "Reset view", "Restart source", "Quit"};
    const TextButtonStyle style = TextButtonStyle::Default().Configure().SetMinimumSize(Vector2::ZERO).SetPadding(Insets(4.0f, 4.0f, 4.0f, 4.0f)).SetFontSize(15.0f).Build();
    for(std::size_t index = 0; index < labels.size(); ++index)
    {
      const Action action = static_cast<Action>(index);
      TextButton   button = TextButton::New(labels[index], style);
      Place(button, panelX + static_cast<float>(index % 2) * (buttonWidth + 8.0f),
            56.0f + static_cast<float>(index / 2) * (buttonHeight + 8.0f), buttonWidth, buttonHeight);
      button.ClickedSignal().Connect(this, [this, action](View /*view*/, InputEvent /*event*/)
      {
        OnAction(action);
      });
      mButtons[index] = button;
      mRoot.Add(button);
    }
    OpenProducer(mUseCamera);
    UpdateControls();
  }

  void Terminate(Application /*application*/)
  {
    CloseProducer();
  }

  bool OpenProducer(bool useCamera)
  {
    CloseProducer();
    mUseCamera = useCamera;
    ResetView();

    if(mUseCamera)
    {
      if(!Check(camera_create(CAMERA_DEVICE_CAMERA0, &mCamera), "camera create") ||
         !Check(camera_set_display(mCamera, CAMERA_DISPLAY_TYPE_NONE, nullptr), "camera display none") ||
         !Check(camera_get_media_frame_source(mCamera, &mSource), "camera get source"))
      {
        return false;
      }
    }
    else
    {
      if(!Check(player_create(&mPlayer), "player create") ||
         !Check(player_set_uri(mPlayer, mVideoPath.c_str()), "player set uri") ||
         !Check(player_set_looping(mPlayer, true), "player loop") ||
         !Check(player_get_media_frame_source(mPlayer, &mSource), "player get source"))
      {
        return false;
      }
    }

    // Attach the exact media_frame_source_h type before preparing/starting.
    if(!Check(mView.SetSource(Any(mSource)) ? 0 : -1, "MediaView SetSource"))
    {
      return false;
    }
    if(mPlayer && !Check(player_prepare(mPlayer), "player prepare"))
    {
      return false;
    }
    return StartPlayback();
  }

  bool StartPlayback()
  {
    if(!mSource || mView.GetSource().Empty())
    {
      mLastOperation = "Connect Source before Start";
      return false;
    }
    if(mPlayer)
    {
      player_state_e state = PLAYER_STATE_NONE;
      if(!Check(player_get_state(mPlayer, &state), "player get state"))
      {
        return false;
      }
      if(state != PLAYER_STATE_PLAYING)
      {
        return Check(player_start(mPlayer), "player start / resume");
      }
    }
    if(mCamera)
    {
      camera_state_e state = CAMERA_STATE_NONE;
      if(!Check(camera_get_state(mCamera, &state), "camera get state"))
      {
        return false;
      }
      if(state != CAMERA_STATE_PREVIEW)
      {
        return Check(camera_start_preview(mCamera), "camera start preview");
      }
    }
    mLastOperation = "Already playing";
    return true;
  }

  void StopPlayback()
  {
    if(mPlayer)
    {
      player_state_e state = PLAYER_STATE_NONE;
      if(Check(player_get_state(mPlayer, &state), "player get state") &&
         (state == PLAYER_STATE_PLAYING || state == PLAYER_STATE_PAUSED))
      {
        Check(player_stop(mPlayer), "player stop");
      }
    }
    if(mCamera)
    {
      camera_state_e state = CAMERA_STATE_NONE;
      if(Check(camera_get_state(mCamera, &state), "camera get state") && state == CAMERA_STATE_PREVIEW)
      {
        Check(camera_stop_preview(mCamera), "camera stop preview");
      }
    }
  }

  void CloseProducer()
  {
    if(!mPlayer && !mCamera)
    {
      return;
    }
    StopPlayback();
    if(mView)
    {
      mView.ClearSource();
    }
    mSource = nullptr;
    if(mPlayer)
    {
      player_state_e state = PLAYER_STATE_NONE;
      if(Check(player_get_state(mPlayer, &state), "player get state") && state == PLAYER_STATE_READY)
      {
        Check(player_unprepare(mPlayer), "player unprepare");
      }
      Check(player_destroy(mPlayer), "player destroy");
      mPlayer = nullptr;
    }
    if(mCamera)
    {
      Check(camera_destroy(mCamera), "camera destroy");
      mCamera = nullptr;
    }
  }

  void SetViewSize()
  {
    const Vector2 size = mPreviewSize * (mHalfSize ? 0.5f : 1.0f);
    Place(mView, (mPreviewSize.x - size.x) * 0.5f, (mPreviewSize.y - size.y) * 0.5f, size.x, size.y);
  }

  void ResetView()
  {
    mCrop        = false;
    mHalfSize    = false;
    mRadiusIndex = 0;
    Check(mView.SetSourceCrop(Rect<float>(0.0f, 0.0f, 1.0f, 1.0f)) ? 0 : -1, "reset crop");
    mView.SetHorizontalFlipEnabled(false);
    mView.SetVerticalFlipEnabled(false);
    mView.SetCornerRadius(Vector4::ZERO);
    mView.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
    if(!mView.GetParent())
    {
      mPreview.Add(mView);
    }
    SetViewSize();
  }

  void OnAction(Action action)
  {
    DALI_LOG_RELEASE_INFO("[media-view-sample] button: %zu\n", static_cast<std::size_t>(action));
    mLastOperation = "View updated";
    switch(action)
    {
      case Action::PLAYER:
      {
        OpenProducer(false);
        break;
      }
      case Action::CAMERA:
      {
        OpenProducer(true);
        break;
      }
      case Action::START:
      {
        StartPlayback();
        break;
      }
      case Action::PAUSE:
      {
        player_state_e state = PLAYER_STATE_NONE;
        if(mPlayer && Check(player_get_state(mPlayer, &state), "player get state") && state == PLAYER_STATE_PLAYING)
        {
          Check(player_pause(mPlayer), "player pause");
        }
        break;
      }
      case Action::STOP:
      {
        StopPlayback();
        break;
      }
      case Action::SEEK:
      {
        int position = 0;
        int duration = 0;
        if(mPlayer && Check(player_get_play_position(mPlayer, &position), "player get position") &&
           Check(player_get_duration(mPlayer, &duration), "player get duration") && duration > 0)
        {
          const int seekPosition = static_cast<int>((static_cast<int64_t>(position) + 5000) % duration);
          Check(player_set_play_position(mPlayer, seekPosition, true, nullptr, nullptr), "player seek +5s");
        }
        break;
      }
      case Action::CROP:
      {
        const Rect<float> crop = mCrop ? Rect<float>(0.0f, 0.0f, 1.0f, 1.0f) : Rect<float>(0.125f, 0.125f, 0.75f, 0.75f);
        if(Check(mView.SetSourceCrop(crop) ? 0 : -1, "set crop"))
        {
          mCrop = !mCrop;
        }
        break;
      }
      case Action::HORIZONTAL_FLIP:
      {
        mView.SetHorizontalFlipEnabled(!mView.IsHorizontalFlipEnabled());
        break;
      }
      case Action::VERTICAL_FLIP:
      {
        mView.SetVerticalFlipEnabled(!mView.IsVerticalFlipEnabled());
        break;
      }
      case Action::HALF_SIZE:
      {
        mHalfSize = !mHalfSize;
        SetViewSize();
        break;
      }
      case Action::CORNER_RADIUS:
      {
        mRadiusIndex                       = (mRadiusIndex + 1) % 4;
        const std::array<Vector4, 4> radii = {Vector4::ZERO, Vector4(48.0f, 48.0f, 48.0f, 48.0f),
                                              Vector4(96.0f, 96.0f, 96.0f, 96.0f), Vector4(12.0f, 36.0f, 72.0f, 96.0f)};
        mView.SetCornerRadius(radii[mRadiusIndex]);
        break;
      }
      case Action::SCENE:
      {
        if(mView.GetParent())
        {
          mView.Unparent();
        }
        else
        {
          mPreview.Add(mView);
        }
        break;
      }
      case Action::SOURCE:
      {
        if(!mView.GetSource().Empty())
        {
          StopPlayback();
          mView.ClearSource();
          if(mPlayer && !Check(player_unprepare(mPlayer), "player unprepare for source disconnect"))
          {
            break;
          }
          mLastOperation = "Source disconnected";
        }
        else if(mSource && Check(mView.SetSource(Any(mSource)) ? 0 : -1, "reconnect source"))
        {
          // MMPlayer accepts decoded callback registration only while IDLE.
          if(mPlayer && !Check(player_prepare(mPlayer), "player prepare after source reconnect"))
          {
            break;
          }
          StartPlayback();
        }
        break;
      }
      case Action::RESET:
      {
        ResetView();
        break;
      }
      case Action::RESTART:
      {
        OpenProducer(mUseCamera);
        break;
      }
      case Action::QUIT:
      {
        mApplication.Quit();
        return;
      }
      case Action::COUNT:
      {
        return;
      }
    }
    UpdateControls();
  }

  TextButton& GetButton(Action action)
  {
    return mButtons[static_cast<std::size_t>(action)];
  }

  void UpdateControls()
  {
    const bool connected = !mView.GetSource().Empty();
    GetButton(Action::PLAYER).SetText(mUseCamera ? "Player" : "Player: selected");
    GetButton(Action::CAMERA).SetText(mUseCamera ? "Camera: selected" : "Camera");
    GetButton(Action::START).SetEnabled(connected);
    GetButton(Action::PAUSE).SetEnabled(connected && !mUseCamera);
    GetButton(Action::STOP).SetEnabled(connected);
    GetButton(Action::SEEK).SetEnabled(connected && !mUseCamera);
    GetButton(Action::SOURCE).SetEnabled(mSource != nullptr);
    GetButton(Action::SOURCE).SetText(connected ? "Source: on" : "Source: off");
    GetButton(Action::CROP).SetText(mCrop ? "Crop: on" : "Crop: off");
    GetButton(Action::HORIZONTAL_FLIP).SetText(mView.IsHorizontalFlipEnabled() ? "H flip: on" : "H flip: off");
    GetButton(Action::VERTICAL_FLIP).SetText(mView.IsVerticalFlipEnabled() ? "V flip: on" : "V flip: off");
    GetButton(Action::HALF_SIZE).SetText(mHalfSize ? "Half size: on" : "Half size: off");
    GetButton(Action::SCENE).SetText(mView.GetParent() ? "Scene: in" : "Scene: out");
    const std::array<const char*, 4> radiusLabels = {"Radius: 0", "Radius: 48", "Radius: 96", "Radius: mixed"};
    GetButton(Action::CORNER_RADIUS).SetText(radiusLabels[mRadiusIndex]);

    const char* stateName = "unavailable";
    if(mPlayer)
    {
      player_state_e state = PLAYER_STATE_NONE;
      if(player_get_state(mPlayer, &state) == PLAYER_ERROR_NONE)
      {
        stateName = state == PLAYER_STATE_PLAYING ? "playing (loop)" : state == PLAYER_STATE_PAUSED ? "paused"
                                                                                                    : "stopped";
      }
    }
    if(mCamera)
    {
      camera_state_e state = CAMERA_STATE_NONE;
      if(camera_get_state(mCamera, &state) == CAMERA_ERROR_NONE)
      {
        stateName = state == CAMERA_STATE_PREVIEW ? "preview" : "stopped";
      }
    }
    const std::string status = std::string(mUseCamera ? "Camera 0 | " : "Player | ") + stateName +
                               "\n" + (mUseCamera ? "Target camera / virtual test pattern" : mVideoPath) +
                               "\n" + mLastOperation;
    mStatus.SetText(status.c_str());
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

  Application                                                     mApplication;
  bool                                                            mUseCamera;
  std::string                                                     mVideoPath;
  Window                                                          mWindow;
  AbsoluteLayout                                                  mRoot;
  AbsoluteLayout                                                  mPreview;
  MediaView                                                       mView;
  Label                                                           mStatus;
  std::array<TextButton, static_cast<std::size_t>(Action::COUNT)> mButtons;
  Vector2                                                         mPreviewSize;
  player_h                                                        mPlayer{nullptr};
  camera_h                                                        mCamera{nullptr};
  media_frame_source_h                                            mSource{nullptr}; ///< Borrowed from the active producer.
  std::string                                                     mLastOperation;
  bool                                                            mCrop{false};
  bool                                                            mHalfSize{false};
  std::size_t                                                     mRadiusIndex{0};
  bool                                                            mFailed{false};
};

} // namespace

int main(int argc, char** argv)
{
  const bool        useCamera   = argc > 1 && std::strcmp(argv[1], "camera") == 0;
  const std::string videoPath   = argc > 2 ? argv[2] : "/home/video-01.mp4";
  Application       application = Application::New(&argc, &argv);
  MediaViewExample  example(application, useCamera, videoPath);
  application.MainLoop();
  return example.Failed() ? 1 : 0;
}
