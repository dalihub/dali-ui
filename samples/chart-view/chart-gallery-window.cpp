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
#include "chart-gallery-window.h"
#include <dali/devel-api/adaptor-framework/application.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <limits>
#include <memory>
#include "chart-case-presenter.h"

using namespace Dali;
using namespace Dali::Ui;

namespace ChartStandard
{
namespace
{
struct Options
{
  std::string caseId;
  std::string dumpPath;
  bool        dark{false};
  bool        freeze{false};
};

void Place(View view, float x, float y, float width, float height)
{
  view.SetLayoutParams(AbsoluteLayoutParams::New().SetX(x).SetY(y).SetWidth(width).SetHeight(height));
}

/**
 * @brief Owns the window, category navigation and card virtualization.
 */
class GalleryWindow : public ConnectionTracker
{
public:
  GalleryWindow(Application application, Options options)
  : mOptions(std::move(options)),
    mCases(BuildReferenceCases()),
    mStates(mCases.size()),
    mCards(mCases.size()),
    mApplication(application)
  {
    for(size_t i = 0; i < mCases.size(); ++i)
    {
      if(mCases[i].id == mOptions.caseId)
      {
        mActive = i;
        mSingle = true;
      }
    }
    mApplication.InitSignal().Connect(this, &GalleryWindow::OnCreate);
    mApplication.TerminateSignal().Connect(this, &GalleryWindow::OnTerminate);
  }

  ~GalleryWindow()
  {
    Shutdown();
  }

  GalleryWindow(const GalleryWindow&)            = delete;
  GalleryWindow& operator=(const GalleryWindow&) = delete;
  GalleryWindow(GalleryWindow&&)                 = delete;
  GalleryWindow& operator=(GalleryWindow&&)      = delete;

private:
  void OnTerminate(Application)
  {
    Shutdown();
  }

  void Shutdown()
  {
    if(mTimer)
    {
      mTimer.Stop();
      mTimer.Reset();
    }
    mCards.clear();
    if(mRoot)
    {
      mRoot.Unparent();
      mRoot.Reset();
    }
    mContent.Reset();
    mScroll.Reset();
  }

  Vector4 TextColor() const
  {
    return mOptions.dark ? Color::WHITE : Vector4(.14f, .14f, .16f, 1);
  }

  void AddLabel(View parent, const std::string& text, float x, float y, float width)
  {
    auto label = Label::New(Dali::String(text.c_str()));
    label.SetFontSize(19);
    label.SetTextColor(UiColor(TextColor()));
    Place(label, x, y, width, 36);
    parent.Add(label);
  }

  void AddButton(View parent, const std::string& text, float x, float y, float width, std::function<void()> callback)
  {
    auto button = InteractiveView::New();
    button.SetFocusable(true);
    button.SetAccessibilityName(Dali::String(text.c_str()));
    button.SetBackgroundColor(UiColor(mOptions.dark ? Vector4(.23f, .25f, .29f, 1) : Vector4(.92f, .94f, .98f, 1)));
    button.SetCornerRadius(18);
    Place(button, x, y, width, 36);
    auto content = AbsoluteLayout::New();
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    button.Add(content);
    auto label = Label::New(Dali::String(text.c_str()));
    label.SetFontSize(13);
    label.SetTextColor(UiColor(TextColor()));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    Place(label, 0, 0, width, 36);
    content.Add(label);
    button.ClickedSignal().Connect(this, [callback](View, InputEvent)
    { callback(); });
    parent.Add(button);
  }

  void OnCreate(Application)
  {
    mApplication.GetWindow().SetPositionSize(PositionSize(0, 0, 1440, 1000));
    mApplication.GetWindow().KeyEventSignal().Connect(this, &GalleryWindow::OnKey);
    BuildWindow();
    mTimer = Timer::New(32);
    mTimer.TickSignal().Connect(this, &GalleryWindow::OnTick);
    mTimer.Start();
    mLastTick = std::chrono::steady_clock::now();
  }

  void BuildWindow()
  {
    const float previous = mScroll ? mScroll.GetScrollPosition().y : 0.0f;
    for(auto& card : mCards)
    {
      card.reset();
    }
    if(mRoot)
    {
      mRoot.Unparent();
    }
    const auto size          = mApplication.GetWindow().GetPositionSize();
    mWidth                   = static_cast<float>(size.width);
    mHeight                  = static_cast<float>(size.height);
    const float headerHeight = mWidth < 650 ? 116.0f : 72.0f;
    mRoot                    = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(UiColor(mOptions.dark ? Vector4(.115f, .115f, .12f, 1) : Vector4(.965f, .969f, .984f, 1)));
    auto header = AbsoluteLayout::New();
    Place(header, 0, 0, mWidth, headerHeight);
    header.SetBackgroundColor(UiColor(mOptions.dark ? Vector4(.15f, .15f, .15f, 1) : Color::WHITE));
    AddLabel(header, "ST Chart / DALi", 24, 12, std::max(180.0f, mWidth - 390));
    const float x = mWidth < 650 ? 16 : mWidth - 360;
    const float y = mWidth < 650 ? 66.0f : 18.0f;
    AddButton(header, mOptions.dark ? "Light" : "Dark", x, y, 84, [this]
    {mOptions.dark=!mOptions.dark;mRebuild=true; });
    AddButton(header, "Previous", x + 92, y, 100, [this]
    { Jump(-1); });
    AddButton(header, "Next", x + 200, y, 84, [this]
    { Jump(1); });
    mViewportHeight = std::max(1.0f, mHeight - headerHeight);
    mScroll         = ScrollView::New();
    mScroll.SetScrollDirection(ScrollDirection::Vertical);
    Place(mScroll, 0, headerHeight, mWidth, mViewportHeight);
    mContent = AbsoluteLayout::New();
    mContent.SetRequestedWidth(mWidth);
    mScroll.SetContent(mContent);
    mRoot.Add(mScroll);
    mRoot.Add(header);
    mApplication.GetWindow().Add(mRoot);
    mBounds.assign(mCases.size(), LayoutRect());
    mGroups.clear();
    const float margin    = mWidth <= 600 ? 16.0f : 32.0f;
    const float gap       = mWidth <= 600 ? 16.0f : 28.0f;
    const int   columns   = mSingle ? 1 : std::max(1, static_cast<int>((mWidth - 2 * margin + gap) / 528));
    const float cardWidth = mSingle ? std::min(674.0f, mWidth - 2 * margin) : (mWidth - 2 * margin - static_cast<float>(columns - 1) * gap) / static_cast<float>(columns);
    float       top       = 48;
    float       rowHeight = 0;
    int         column    = 0;
    std::string category;
    for(size_t i = 0; i < mCases.size(); ++i)
    {
      if(mSingle && i != mActive)
      {
        continue;
      }
      if(mCases[i].category != category)
      {
        if(column)
        {
          top += rowHeight + gap;
          rowHeight = 0;
        }
        column   = 0;
        category = mCases[i].category;
        mGroups.push_back({i, top - 24});
        AddLabel(mContent, category, margin, top - 26, mWidth - 2 * margin);
        top += 28;
      }
      const float cardHeight = CardHeight(mCases[i]);
      rowHeight              = std::max(rowHeight, cardHeight);
      mBounds[i]             = LayoutRect(mSingle ? (mWidth - cardWidth) * .5f : margin + static_cast<float>(column) * (cardWidth + gap), top, cardWidth, cardHeight);
      ++column;
      if(column == columns)
      {
        column = 0;
        top += rowHeight + gap;
        rowHeight = 0;
      }
    }
    if(column)
    {
      top += rowHeight + gap;
      rowHeight = 0;
    }
    mContent.SetRequestedHeight(top + margin);
    mPendingScroll = mSingle ? 0 : previous;
    mRestoreScroll = true;
    mFrames        = 0;
    mDumped        = false;
    RefreshVisible(mPendingScroll);
  }

  void Mount(size_t i)
  {
    const auto bounds    = mBounds[i];
    mCards[i]            = std::make_unique<CasePresenter>(mCases[i], mStates[i], bounds.width, bounds.height, mOptions.dark, mOptions.freeze);
    mCards[i]->activated = [this, i]
    {mActive=i;mDumped=false; };
    mCards[i]->recreate = [this, i]
    { mRecreate = i; };
    auto view = mCards[i]->GetView();
    Place(view, bounds.x, bounds.y, bounds.width, bounds.height);
    mContent.Add(view);
  }

  void RefreshVisible(float scroll)
  {
    for(size_t i = 0; i < mCases.size(); ++i)
    {
      if(mSingle && i != mActive)
      {
        continue;
      }
      const auto& bounds  = mBounds[i];
      const bool  visible = bounds.y + bounds.height >= scroll - 500 && bounds.y <= scroll + mViewportHeight + 500;
      if(visible && !mCards[i])
      {
        Mount(i);
      }
      else if(!visible && mCards[i])
      {
        mCards[i].reset();
      }
    }
  }

  void Jump(int direction)
  {
    if(mSingle)
    {
      const auto count = static_cast<int64_t>(mCases.size());
      mActive          = static_cast<size_t>((static_cast<int64_t>(mActive) + direction + count) % count);
      mRebuild         = true;
      return;
    }
    const auto it      = std::find_if(mGroups.begin(), mGroups.end(), [this](const auto& group)
         { return group.first > mActive; });
    int        current = static_cast<int>(std::distance(mGroups.begin(), it)) - 1;
    current            = std::clamp(current + direction, 0, static_cast<int>(mGroups.size()) - 1);
    mActive            = mGroups[static_cast<size_t>(current)].first;
    mPendingScroll     = mGroups[static_cast<size_t>(current)].second;
    mRestoreScroll     = true;
    mDumped            = false;
  }

  bool OnTick()
  {
    const auto  now     = std::chrono::steady_clock::now();
    const float seconds = std::chrono::duration<float>(now - mLastTick).count();
    mLastTick           = now;
    const auto size     = mApplication.GetWindow().GetPositionSize();
    if(mRebuild || static_cast<float>(size.width) != mWidth || static_cast<float>(size.height) != mHeight)
    {
      mRebuild = false;
      BuildWindow();
    }
    if(mRecreate < mCards.size())
    {
      const size_t index = mRecreate;
      mRecreate          = mCards.size();
      mCards[index].reset();
      Mount(index);
      mDumped = false;
    }
    if(mRestoreScroll)
    {
      mScroll.ScrollTo(Vector2(0, mPendingScroll), false);
      mRestoreScroll = false;
    }
    RefreshVisible(mScroll.GetScrollPosition().y);
    for(auto& card : mCards)
    {
      if(card)
      {
        card->Advance(seconds);
      }
    }
    ++mFrames;
    if(!mOptions.dumpPath.empty() && mFrames > 4 && mCards[mActive] && mCards[mActive]->IsReady())
    {
      const auto snapshot = mCards[mActive]->Snapshot();
      if(mDumped && snapshot == mLastSnapshot)
      {
        return true;
      }
      mLastSnapshot           = snapshot;
      const auto    temporary = mOptions.dumpPath + ".tmp";
      std::ofstream out(temporary);
      out << snapshot << "\n";
      out.close();
      std::rename(temporary.c_str(), mOptions.dumpPath.c_str());
      mDumped = true;
      std::fprintf(stdout, "CASE_READY %s %s\n", mCases[mActive].id.c_str(), mOptions.dark ? "dark" : "light");
      std::fflush(stdout);
    }
    return true;
  }

  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }
    const auto key = event.GetKeyName();
    if(key == "Escape")
    {
      mApplication.Quit();
      return;
    }
    auto focus = FocusManager::Get().GetCurrentFocusView(mApplication.GetWindow());
    if(InputField::DownCast(focus))
    {
      return;
    }
    if(key == "t" || key == "T")
    {
      mOptions.dark = !mOptions.dark;
      mRebuild      = true;
      return;
    }
    if(key == "n" || key == "N" || key == "F6")
    {
      Jump(1);
      return;
    }
    if(key == "p" || key == "P")
    {
      Jump(-1);
      return;
    }
    auto& card = mCards[mActive];
    if(!card)
    {
      return;
    }
    if(key == "Right" || key == "Left")
    {
      card->Select(key == "Right" ? 1 : -1);
    }
    else if(key == "Next" || key == "Prior")
    {
      card->Navigate(key == "Next" ? 1 : -1);
    }
    else if(key == "space")
    {
      card->Update();
    }
    mDumped = false;
  }

  Options                                     mOptions;
  std::string                                 mLastSnapshot;
  std::vector<ReferenceCase>                  mCases;
  std::vector<CaseState>                      mStates;
  std::vector<std::unique_ptr<CasePresenter>> mCards;
  std::vector<LayoutRect>                     mBounds;
  std::vector<std::pair<size_t, float>>       mGroups;
  Application                                 mApplication;
  AbsoluteLayout                              mRoot;
  AbsoluteLayout                              mContent;
  ScrollView                                  mScroll;
  Timer                                       mTimer;
  std::chrono::steady_clock::time_point       mLastTick;
  size_t                                      mActive{0};
  size_t                                      mRecreate{std::numeric_limits<size_t>::max()};
  float                                       mWidth{0};
  float                                       mHeight{0};
  float                                       mViewportHeight{0};
  float                                       mPendingScroll{0};
  int                                         mFrames{0};
  bool                                        mSingle{false};
  bool                                        mRebuild{false};
  bool                                        mRestoreScroll{false};
  bool                                        mDumped{false};
};
} //namespace

int RunGallery(int argc, char** argv)
{
  Options options;
  for(int i = 1; i < argc; ++i)
  {
    const std::string argument = argv[i];
    if(argument == "--list-cases" || argument == "--list-reference-cases")
    {
      for(const auto& spec : BuildReferenceCases())
      {
        if(argument == "--list-reference-cases" && !spec.webReference)
        {
          continue;
        }
        std::printf("%s\t%s\t%s\n", spec.id.c_str(), spec.category.c_str(), spec.title.c_str());
      }
      return 0;
    }
    if(argument.rfind("--case=", 0) == 0)
    {
      options.caseId = argument.substr(7);
    }
    else if(argument.rfind("--dump-state=", 0) == 0)
    {
      options.dumpPath = argument.substr(13);
    }
    else if(argument == "--theme=dark")
    {
      options.dark = true;
    }
    else if(argument == "--freeze-animations")
    {
      options.freeze = true;
    }
  }
  if(!options.caseId.empty())
  {
    const auto cases = BuildReferenceCases();
    if(std::none_of(cases.begin(), cases.end(), [&options](const auto& spec)
    { return spec.id == options.caseId; }))
    {
      std::fprintf(stderr, "Unknown case: %s\n", options.caseId.c_str());
      return 2;
    }
  }
  try
  {
    int  daliArgumentCount = 1;
    auto application       = Application::New(&daliArgumentCount, &argv);
    Components::UiConfig::New().Apply();
    GalleryWindow gallery(application, options);
    application.MainLoop();
    return 0;
  }
  catch(const Dali::DaliException& exception)
  {
    std::fprintf(stderr, "Chart gallery assertion: %s at %s\n", exception.condition, exception.location);
    return 1;
  }
}
} //namespace ChartStandard
