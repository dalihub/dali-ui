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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <cstdlib>
#include <ctime>
#include <string>

namespace
{
using namespace Dali;
using namespace Dali::Ui;

constexpr uint32_t NUMBER_OF_IMAGES = 1000u;
constexpr float    TOP_OFFSET        = 72.0f;

void SetStandaloneGeometry(View view, float x, float y, float width, float height)
{
  view.SetLayoutMode(LayoutMode::STANDALONE);
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
}

void SetRequestedGeometry(View view, float x, float y, float width, float height)
{
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
}

class PerfLikeCsfs : public ConnectionTracker
{
public:
  explicit PerfLikeCsfs(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &PerfLikeCsfs::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    const PositionSize windowPositionSize = window.GetPositionSize();
    const Vector2 windowSize(static_cast<float>(windowPositionSize.width),
                             static_cast<float>(windowPositionSize.height));
    const float   sectionWidth = windowSize.x / 3.0f;
    const float   contentHeight = windowSize.y - TOP_OFFSET;

    uint32_t nRow     = 23u;
    uint32_t nColumn  = 24u;
    Vector2  cellSize = Vector2(sectionWidth / nColumn, contentHeight / nRow);

    // Create Color section. Keep one parent View per colored View, matching
    // the original PerfTestLikeCSFS hierarchy and construction order.
    View colorSection = View::New();
    SetStandaloneGeometry(colorSection, 0.0f, TOP_OFFSET, sectionWidth, contentHeight);

    for(uint32_t i = 0u; i < nColumn; ++i)
    {
      for(uint32_t j = 0u; j < nRow; ++j)
      {
        View colorCell = View::New();
        colorCell.SetRequestedWidth(cellSize.x);
        colorCell.SetRequestedHeight(cellSize.y);
        colorCell.SetBackgroundColor(UiColor(rand() % 256 / 256.0f,
                                             rand() % 256 / 256.0f,
                                             rand() % 256 / 256.0f,
                                             1.0f));

        View parent = View::New();
        SetRequestedGeometry(parent, i * cellSize.x, j * cellSize.y, cellSize.x, cellSize.y);
        parent.Add(colorCell);
        colorSection.Add(parent);
      }
    }
    window.Add(colorSection);

    nRow     = 23u;
    nColumn  = 8u;
    cellSize = Vector2(sectionWidth / nColumn, contentHeight / nRow);

    // Preserve the original max-dimension square calculation, even though the
    // resulting ImageView can be taller than its parent cell.
    const float   imageSide = cellSize.x < cellSize.y ? cellSize.y : cellSize.x;
    const Vector2 imageSize(imageSide, imageSide);
    const std::string imagePrefix = RESOURCES_DIR "images/gallery-medium-";
    const std::string imageSuffix = ".jpg";
    uint32_t          imageIndex  = 0u;

    View imageSection = View::New();
    SetStandaloneGeometry(imageSection, sectionWidth, TOP_OFFSET, sectionWidth, contentHeight);

    for(uint32_t i = 0u; i < nColumn; ++i)
    {
      for(uint32_t j = 0u; j < nRow; ++j)
      {
        const std::string imagePath = imagePrefix + std::to_string((imageIndex++) % NUMBER_OF_IMAGES + 1u) + imageSuffix;
        ImageView         imageView = ImageView::New(imagePath.c_str());
        SetRequestedGeometry(imageView, 0.0f, 0.0f, imageSize.x, imageSize.y);
        imageView.SetDesiredWidth(static_cast<int>(imageSize.x));
        imageView.SetDesiredHeight(static_cast<int>(imageSize.y));

        View parent = View::New();
        SetRequestedGeometry(parent, i * cellSize.x, j * cellSize.y, cellSize.x, cellSize.y);
        parent.Add(imageView);
        imageSection.Add(parent);
      }
    }
    window.Add(imageSection);

    nRow     = 23u;
    nColumn  = 16u;
    cellSize = Vector2(sectionWidth / nColumn, contentHeight / nRow);
    uint32_t textIndex = 0u;

    View textSection = View::New();
    SetStandaloneGeometry(textSection, sectionWidth * 2.0f, TOP_OFFSET, sectionWidth, contentHeight);

    for(uint32_t i = 0u; i < nColumn; ++i)
    {
      for(uint32_t j = 0u; j < nRow; ++j)
      {
        const std::string text = std::to_string(textIndex++);
        Label             label = Label::New(text.c_str());
        SetRequestedGeometry(label, 0.0f, 0.0f, cellSize.x, cellSize.y);

        View parent = View::New();
        SetRequestedGeometry(parent, i * cellSize.x, j * cellSize.y, cellSize.x, cellSize.y);
        parent.Add(label);
        textSection.Add(parent);
      }
    }
    window.Add(textSection);

    window.KeyEventSignal().Connect(this, &PerfLikeCsfs::OnKeyEvent);
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK) || IsKey(event, DALI_KEY_BACKSPACE)))
    {
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
};
} // unnamed namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  Dali::Application application = Dali::Application::New(&argc, &argv);
  PerfLikeCsfs     sample(application);
  application.MainLoop();
  return 0;
}
