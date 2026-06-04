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

/*
 * COLRv1 color glyph sample.
 *
 * This sample is intended to verify COLRv1 format support in DALi text
 * rendering. It displays glyphs from COLRv1-capable fonts and exercises
 * the supported and boundary feature sets of the COLRv1 paint graph.
 *
 * Required local font installation
 * ----------------------------------
 * The following COLRv1 test fonts must be installed on the target device
 * for the glyphs to render correctly. Without them, glyphs will fall back
 * to empty outlines or the system default font.
 *
 *   - Google Noto Color Emoji (COLRv1 build)
 *     https://fonts.google.com/noto/specimen/Noto+Color+Emoji
 *
 *   - Nabla variable font
 *     https://fonts.google.com/specimen/Nabla
 *
 *   - Google color-fonts test suite (COLRv1 Static/Variable Test Glyphs)
 *     https://github.com/googlefonts/color-fonts
 *
 * Reference rendering
 * -------------------
 * Expected rendering results are based on Chrome. See the reference image:
 *   samples/text/res/colrv1-fonts-reference.png
 *
 * Supported COLRv1 features in DALi
 * ----------------------------------
 *   - Linear repeat
 *   - Linear / radial gradient
 *   - Scale
 *   - Rotate
 *   - Skew
 *   - Transform
 *   - Translate
 *   - Composite (SrcIn, SoftLight)
 *   - ClipBox
 *   - Variable alpha
 *
 * TBD features (not yet supported)
 * ---------------------------------
 *   - SweepGradient
 *   - Foreground color
 *   - PaintColrGlyph (cycle / no-cycle)
 *   - Nested PaintGlyph
 *
 * Unsupported features must fail closed (no rendering rather than incorrect
 * rendering).
 *
 * - Add full coverage validation for the glyphs and COLRv1 feature cases
 *   described in the Google color-fonts glyph descriptions:
 *   https://github.com/googlefonts/color-fonts/blob/main/glyph_descriptions.md
 */
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 10.0f;

// Google color-fonts COLRv1 test glyph representatives.
// Feature order:
//   linear repeat, linear/radial gradient, scale, rotate, skew, transform,
//   translate, Composite SrcIn/SoftLight, ClipBox, variable alpha.
constexpr const char* COLRV1_STATIC_TEST_GLYPHS =
  "\U000F0100 \U000F0500 \U000F0503 \U000F0300 \U000F0600 \U000F0700 "
  "\U000F0800 \U000F0900 \U000F0A05 \U000F0A14 \U000F0C00 \U000F1000";

// Boundary representatives: SweepGradient, foreground color, PaintColrGlyph
// cycle/no-cycle, nested PaintGlyph. Unsupported features must fail closed.
constexpr const char* COLRV1_BOUNDARY_TEST_GLYPHS =
  "\U000F0200 \U000F0B00 \U000F1100 \U000F1200 \U000F1400";
} // namespace

class TextController : public ConnectionTracker
{
public:
  explicit TextController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xffffff));

    window.Add(CreateContents());
    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  View CreateContents()
  {
    auto scrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT);

    auto layout = StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("🥰💀✌️🌴🐢🐐🍄⚽🍻👑📸😬👀🚨🏡🕊️🏆😻🌟🧿🍀🎨🍜🔮")
          .SetFontFamily("Noto Color Emoji")
          .SetMultiLine(true)
          .SetFontSize(50),

        Label::New("Whereas disregard and contempt for human rights have resulted")
          .SetFontFamily("Nabla")
          .SetFontVariation("EDPT=100,EHLT=12")
          .SetMultiLine(true)
          .SetFontSize(50),

        Label::New(COLRV1_STATIC_TEST_GLYPHS)
          .SetFontFamily("COLRv1 Static Test Glyphs")
          .SetMultiLine(true)
          .SetFontSize(50),

        Label::New(COLRV1_BOUNDARY_TEST_GLYPHS)
          .SetFontFamily("COLRv1 Static Test Glyphs")
          .SetMultiLine(true)
          .SetFontSize(50),

        Label::New(COLRV1_STATIC_TEST_GLYPHS)
          .SetFontFamily("COLRv1 Variable Test Glyphs")
          .SetMultiLine(true)
          .SetFontSize(50),
      });

    scrollView.SetContent(layout);

    return scrollView;
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
    }
    else if(event.GetKeyName() == "q")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "w")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
    else if(event.GetKeyName() == "e")
    {
      UiScaleManager::Get().SetScale(4.0f);
    }
    else if(event.GetKeyName() == "r")
    {
      UiScaleManager::Get().SetScale(8.0f);
    }
    else if(event.GetKeyName() == "t")
    {
      UiScaleManager::Get().SetScale(12.0f);
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New()
    .SetLabelAsyncRendering(true)
    .Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
