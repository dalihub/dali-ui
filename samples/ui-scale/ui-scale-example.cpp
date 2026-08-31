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

/**
 * UiScale system test sample
 *
 * Verifies all UiScalePolicy requirements in one scrollable screen:
 *
 *  [Control Panel]
 *    - Current scale display
 *    - Preset buttons: x0.8 / x1.0 / x1.2 / x1.5 / x2.0
 *    - Custom scale InputField + Apply button
 *
 *  [Zone A] INHERIT (default)
 *    - Three colored 60×60 boxes, all INHERIT
 *    - All grow/shrink proportionally with system scale
 *    - Nested 2×2 GridLayout also scales
 *
 *  [Zone B] DISABLED container + ENABLED children  ← KEY TEST
 *    - The outer StackLayout is DISABLED (effectiveScale = 1.0 always)
 *    - Red box:   INHERIT  → inherits DISABLED parent → does NOT scale
 *    - Green box: ENABLED  → bypasses DISABLED parent → DOES scale
 *    - Blue box:  ENABLED  → same as green
 *    Observe: Red stays 60×60. Green/Blue grow as scale increases.
 *
 *  [Zone C] DISABLED subtree inside normal INHERIT parent
 *    - Orange box: INHERIT inside INHERIT zone → scales
 *    - Purple sub-container: DISABLED → isolates its children
 *      - Purple child: INHERIT under DISABLED → does NOT scale
 *      - Orange-red child: ENABLED → scales despite DISABLED parent
 *
 *  [Zone G] Reparent: DISABLED → INHERIT
 *    - A box (INHERIT) starts under a DISABLED parent → effectiveScale = 1.0
 *    - "→ INHERIT 부모로" button moves it to an INHERIT parent
 *    - Box immediately picks up system scale on reparent
 *    - "← DISABLED 부모로" button moves it back → effectiveScale = 1.0 again
 *
 *  [Zone H] Unparent + scale change + Re-add
 *    - "Unparent" removes the box from its parent → excluded from scale tracking
 *    - Change system scale while the box is unparented (no overhead)
 *    - "Re-add" puts it back → new scale is applied correctly
 *
 * Press Escape / Back to quit.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

// ─────────────────────────── Color palette ───────────────────────────────────
namespace
{
constexpr uint32_t C_CTRL_BG    = 0x1A1A2E; // Control panel dark bg
constexpr uint32_t C_BTN        = 0x0F3460; // Preset button bg
constexpr uint32_t C_BTN_APPLY  = 0x00695C; // Apply button (teal)
constexpr uint32_t C_WHITE      = 0xFFFFFF;
constexpr uint32_t C_LIGHT_TEXT = 0xF5F5F5;
constexpr uint32_t C_YELLOW_HI  = 0xFFEB3B; // Scale display highlight
constexpr uint32_t C_DARK_TEXT  = 0x212121;
constexpr uint32_t C_GRAY_TEXT  = 0x757575;
constexpr uint32_t C_INPUT_BG   = 0x2D2D4E;

// Zone backgrounds
constexpr uint32_t C_ZONE_A_BG  = 0xE3F2FD; // light blue
constexpr uint32_t C_ZONE_B_BG  = 0xFFF9C4; // light yellow
constexpr uint32_t C_ZONE_C_BG  = 0xE8F5E9; // light green
constexpr uint32_t C_ZONE_TITLE = 0x37474F;

// Box colors
constexpr uint32_t C_RED        = 0xE53935;
constexpr uint32_t C_GREEN      = 0x43A047;
constexpr uint32_t C_BLUE       = 0x1E88E5;
constexpr uint32_t C_ORANGE     = 0xFB8C00;
constexpr uint32_t C_PURPLE     = 0x8E24AA;
constexpr uint32_t C_DEEP_ORG   = 0xE64A19; // "ENABLED in DISABLED subtree"

// Grid cell colors
constexpr uint32_t C_GRID[4]    = {0xEF9A9A, 0x90CAF9, 0xA5D6A7, 0xFFCC80};

// Misc
constexpr float    BOX_SIZE     = 60.0f;  // natural size of each demo box
constexpr float    ZONE_RADIUS  = 10.0f;
constexpr float    BOX_RADIUS   = 6.0f;
constexpr float    BTN_RADIUS   = 8.0f;
constexpr float    BTN_HEIGHT   = 44.0f;
} // namespace

// ──────────────────── float → "1.23" helper ──────────────────────────────────
static std::string Fmt(float v)
{
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << v;
  return oss.str();
}

// ─────────────────────────── Controller ──────────────────────────────────────
class UiScaleController : public ConnectionTracker
{
public:
  explicit UiScaleController(Application& app)
  : mApplication(app)
  {
    app.InitSignal().Connect(this, &UiScaleController::OnInit);
  }

private:
  // ─────────── App lifecycle ───────────────────────────────────────────────

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root vertical stack — INHERIT (default), fills window
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(0.0f);

    root.Add(BuildControlPanel());
    root.Add(BuildScrollZones());

    window.Add(root);
    window.KeyEventSignal().Connect(this, &UiScaleController::OnKeyEvent);

    UpdateScaleLabel();
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  // ─────────── Scale control ───────────────────────────────────────────────

  void ApplyScale(float s)
  {
    UiScaleManager::Get().SetScale(s);
    UpdateScaleLabel();
  }

  void OnApplyCustomScale()
  {
    Dali::String text = mScaleInput.GetText();
    if(text.Size() == 0)
      return;
    float s = 1.0f;
    try
    {
      s = std::stof(std::string(text.CStr()));
    }
    catch(...)
    {
      return; // ignore invalid input
    }
    if(s > 0.1f && s < 5.0f)
    {
      ApplyScale(s);
    }
  }

  void UpdateScaleLabel()
  {
    std::string text = "System Scale: " + Fmt(UiScaleManager::Get().GetScale());
    text += UiScaleManager::Get().IsScalable() ? "  (scaling ON)" : "  (scaling OFF)";
    mScaleLabel.SetText(text.c_str());
  }

  // Global master switch handlers

  void OnToggleScalable()
  {
    UiScaleManager::Get().SetScalable(!UiScaleManager::Get().IsScalable());
    UpdateScalableButton();
    UpdateScaleLabel();
  }

  void UpdateScalableButton()
  {
    const bool enabled = UiScaleManager::Get().IsScalable();
    mScalableBtnLabel.SetText(enabled ? "Scaling: ON (tap to disable)" : "Scaling: OFF (tap to enable)");
    mScalableBtn.SetBackgroundColor(UiColor(enabled ? 0x2E7D32 : 0xC62828));
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Build: Control Panel
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildControlPanel()
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetRequestedWidth(MATCH_PARENT);
    panel.SetRequestedHeight(WRAP_CONTENT);
    panel.SetBackgroundColor(UiColor(C_CTRL_BG));
    panel.SetPadding(Insets(16.0f, 16.0f, 16.0f, 16.0f));
    panel.SetSpacing(10.0f);

    // Title
    Label title = Label::New("UiScale System Test");
    title.SetRequestedWidth(MATCH_PARENT);
    title.SetRequestedHeight(WRAP_CONTENT);
    title.SetFontSize(22.0f);
    title.SetTextColor(UiColor(C_LIGHT_TEXT));
    panel.Add(title);

    // Current scale display (updated dynamically)
    mScaleLabel = Label::New("System Scale: 1.00");
    mScaleLabel.SetRequestedWidth(MATCH_PARENT);
    mScaleLabel.SetRequestedHeight(WRAP_CONTENT);
    mScaleLabel.SetFontSize(16.0f);
    mScaleLabel.SetTextColor(UiColor(C_YELLOW_HI));
    panel.Add(mScaleLabel);

    // Preset buttons row
    StackLayout btnRow = StackLayout::New(StackOrientation::HORIZONTAL);
    btnRow.SetRequestedWidth(MATCH_PARENT);
    btnRow.SetRequestedHeight(WRAP_CONTENT);
    btnRow.SetSpacing(6.0f);

    static constexpr float kPresets[] = {0.8f, 1.0f, 1.2f, 1.5f, 2.0f};
    for(float preset : kPresets)
    {
      std::string label = "x" + Fmt(preset);
      InteractiveView btn = MakeButton(label, UiColor(C_BTN));
      btn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

      const float capturedScale = preset;
      btn.ConnectClickedSignal(this, [this, capturedScale](View, const InputEvent&) {
        ApplyScale(capturedScale);
      });
      btnRow.Add(btn);
    }
    panel.Add(btnRow);

    // Custom scale: InputField + Apply button
    StackLayout inputRow = StackLayout::New(StackOrientation::HORIZONTAL);
    inputRow.SetRequestedWidth(MATCH_PARENT);
    inputRow.SetRequestedHeight(WRAP_CONTENT);
    inputRow.SetSpacing(6.0f);

    mScaleInput = InputField::New();
    mScaleInput.SetPlaceholder("Custom scale (e.g. 1.3)");
    mScaleInput.SetFontSize(15.0f);
    mScaleInput.SetCursorWidth(2);
    mScaleInput.SetRequestedHeight(BTN_HEIGHT);
    mScaleInput.SetBackgroundColor(UiColor(C_INPUT_BG));
    mScaleInput.SetTextColor(UiColor(C_LIGHT_TEXT));
    mScaleInput.SetCursorColor(UiColor(C_LIGHT_TEXT));
    mScaleInput.SetPlaceholderColor(UiColor(C_GRAY_TEXT));
    mScaleInput.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mScaleInput.SetPadding(Insets(10.0f, 10.0f, 4.0f, 4.0f));
    mScaleInput.SetCornerRadius(Vector4(BTN_RADIUS, BTN_RADIUS, BTN_RADIUS, BTN_RADIUS));
    mScaleInput.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    InteractiveView applyBtn = MakeButton("Apply", UiColor(C_BTN_APPLY));
    applyBtn.SetRequestedWidth(70.0f);
    applyBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      OnApplyCustomScale();
    });

    inputRow.Add(mScaleInput);
    inputRow.Add(applyBtn);
    panel.Add(inputRow);

    // Global master switch: enable/disable UI scaling process-wide. When OFF,
    // every view renders at 1.0 regardless of its UiScalePolicy; the stored scale
    // is preserved and re-applied when scaling is turned back ON.
    mScalableBtn = InteractiveView::New();
    mScalableBtn.SetRequestedWidth(MATCH_PARENT);
    mScalableBtn.SetRequestedHeight(BTN_HEIGHT);
    mScalableBtn.SetCornerRadius(Vector4(BTN_RADIUS, BTN_RADIUS, BTN_RADIUS, BTN_RADIUS));
    mScalableBtn.SetPadding(Insets(12.0f, 12.0f, 6.0f, 6.0f));

    mScalableBtnLabel = Label::New();
    mScalableBtnLabel.SetRequestedWidth(MATCH_PARENT);
    mScalableBtnLabel.SetRequestedHeight(MATCH_PARENT);
    mScalableBtnLabel.SetFontSize(15.0f);
    mScalableBtnLabel.SetTextColor(UiColor(C_WHITE));
    mScalableBtnLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mScalableBtnLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mScalableBtn.Add(mScalableBtnLabel);

    mScalableBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      OnToggleScalable();
    });
    panel.Add(mScalableBtn);
    UpdateScalableButton();

    return panel;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Build: Scrollable test zones
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildScrollZones()
  {
    // Vertical content stack for all three zones
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetSpacing(16.0f);
    content.SetPadding(Insets(14.0f, 14.0f, 16.0f, 24.0f));

    content.Add(BuildZoneA());
    content.Add(BuildZoneB());
    content.Add(BuildZoneC());
    content.Add(BuildZoneD());
    content.Add(BuildZoneE());
    content.Add(BuildZoneF());
    content.Add(BuildZoneG());
    content.Add(BuildZoneH());

    // ScrollView fills remaining height in the root StackLayout
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(WRAP_CONTENT);
    scrollView.SetContent(content);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    return scrollView;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone A: All INHERIT — every view scales with the system scale
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneA()
  {
    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_A_BG));
    // No SetUiScalePolicy call — INHERIT is the default

    zone.Add(MakeZoneTitle("[Zone A] INHERIT (default)"));
    zone.Add(MakeDesc(
      "All views use the default INHERIT policy. "
      "The three boxes and the nested grid all scale proportionally "
      "as you change the system scale."));

    // Three boxes — all INHERIT
    StackLayout row = MakeHStack();
    row.Add(MakePolicyBox("INHERIT", UiColor(C_RED),   UiScalePolicy::INHERIT, BOX_SIZE));
    row.Add(MakePolicyBox("INHERIT", UiColor(C_GREEN), UiScalePolicy::INHERIT, BOX_SIZE));
    row.Add(MakePolicyBox("INHERIT", UiColor(C_BLUE),  UiScalePolicy::INHERIT, BOX_SIZE));
    zone.Add(row);

    // Nested 2×2 GridLayout (also INHERIT)
    zone.Add(MakeDesc("Nested 2×2 GridLayout — also INHERIT:"));
    zone.Add(BuildDemoGrid());

    return zone;
  }

  View BuildDemoGrid()
  {
    GridLayout grid = GridLayout::New();
    grid.AddColumnDefinition(GridLength::Star(1.0f));
    grid.AddColumnDefinition(GridLength::Star(1.0f));
    grid.AddRowDefinition(GridLength::Absolute(50.0f));
    grid.AddRowDefinition(GridLength::Absolute(50.0f));
    grid.SetRequestedWidth(MATCH_PARENT);
    grid.SetRequestedHeight(WRAP_CONTENT);
    grid.SetRowSpacing(4.0f);
    grid.SetColumnSpacing(4.0f);
    // INHERIT (default) — inherits system scale

    for(int i = 0; i < 4; ++i)
    {
      View cell = View::New();
      cell.SetBackgroundColor(UiColor(C_GRID[i]));
      cell.SetLayoutParams(GridLayoutParams::New());
      grid.Add(cell);
    }
    return grid;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone B: DISABLED container + ENABLED children  ← KEY TEST CASE
  //
  // This demonstrates the requirement: "자신은 scale을 적용받지 않되
  // child는 scale을 적용받는 뷰"
  //
  // The zone container (StackLayout) is DISABLED → effectiveScale = 1.0.
  // Children with INHERIT also don't scale (they inherit the 1.0 from parent).
  // Children with ENABLED bypass the DISABLED parent and use the system scale
  // directly.
  //
  // Visual proof: increase scale to 1.5 or 2.0 and observe:
  //   - Red (INHERIT)  stays 60×60
  //   - Green (ENABLED) grows to 90×90 / 120×120
  //   - Blue  (ENABLED) grows to 90×90 / 120×120
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneB()
  {
    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_B_BG));

    // ★★★ Key: DISABLED — this container and its INHERIT descendants don't scale
    zone.SetUiScalePolicy(UiScalePolicy::DISABLED);

    zone.Add(MakeZoneTitle("[Zone B] DISABLED container + ENABLED children"));
    zone.Add(MakeDesc(
      "This container is DISABLED (effectiveScale = 1.0 always). "
      "Its INHERIT children also don't scale. "
      "But children with ENABLED policy bypass DISABLED and use the system scale directly."));

    // The three comparison boxes
    StackLayout row = MakeHStack();

    // Box 1: INHERIT → inherits DISABLED parent → effectiveScale = 1.0 → no scale
    row.Add(MakePolicyBox("INHERIT\n(no scale)", UiColor(C_RED), UiScalePolicy::INHERIT, BOX_SIZE));

    // Box 2: ENABLED → directly reads UiScaleManager → effectiveScale = system scale
    row.Add(MakePolicyBox("ENABLED\n(scales!)", UiColor(C_GREEN), UiScalePolicy::ENABLED, BOX_SIZE));

    // Box 3: ENABLED → same as Box 2
    row.Add(MakePolicyBox("ENABLED\n(scales!)", UiColor(C_BLUE), UiScalePolicy::ENABLED, BOX_SIZE));

    zone.Add(row);
    zone.Add(MakeDesc(
      "Red stays 60×60. Green/Blue grow when scale > 1.0. "
      "The container (yellow background) expands to accommodate them but its own "
      "padding/spacing remain at 1.0 scale."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone C: DISABLED subtree nested inside a normal INHERIT parent
  //
  // Shows how a DISABLED sub-container creates a "scale-free island" inside
  // an otherwise scaled layout, and how ENABLED can break out of it.
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneC()
  {
    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_C_BG));
    // INHERIT (default)

    zone.Add(MakeZoneTitle("[Zone C] DISABLED sub-container inside INHERIT zone"));
    zone.Add(MakeDesc(
      "The outer zone is INHERIT (scales normally). "
      "The purple sub-container is DISABLED, creating a scale-free island. "
      "Inside it: one INHERIT child (fixed) and one ENABLED child (still scales)."));

    // Outer row: INHERIT zone content
    StackLayout outerRow = MakeHStack();

    // Left: normal INHERIT box — scales with the zone
    outerRow.Add(MakePolicyBox("INHERIT\n(scales)", UiColor(C_ORANGE), UiScalePolicy::INHERIT, BOX_SIZE));

    // Right: DISABLED sub-container (purple tint) — creates isolated island
    StackLayout disabledSub = StackLayout::New(StackOrientation::VERTICAL);
    disabledSub.SetRequestedWidth(WRAP_CONTENT);
    disabledSub.SetRequestedHeight(WRAP_CONTENT);
    disabledSub.SetBackgroundColor(UiColor(0xE1BEE7)); // light purple
    disabledSub.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));
    disabledSub.SetSpacing(6.0f);
    disabledSub.SetCornerRadius(Vector4(ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS));
    disabledSub.SetUiScalePolicy(UiScalePolicy::DISABLED); // ★ isolated island

    Label disabledSubLabel = Label::New("DISABLED sub-container");
    disabledSubLabel.SetRequestedWidth(WRAP_CONTENT);
    disabledSubLabel.SetRequestedHeight(WRAP_CONTENT);
    disabledSubLabel.SetFontSize(12.0f);
    disabledSubLabel.SetTextColor(UiColor(0x6A1B9A));
    disabledSub.Add(disabledSubLabel);

    StackLayout subRow = MakeHStack();
    // INHERIT under DISABLED → fixed size (1.0)
    subRow.Add(MakePolicyBox("INHERIT\n→ 1.0", UiColor(C_PURPLE), UiScalePolicy::INHERIT, BOX_SIZE - 10.0f));
    // ENABLED inside DISABLED → still scales
    subRow.Add(MakePolicyBox("ENABLED\n→ sys", UiColor(C_DEEP_ORG), UiScalePolicy::ENABLED, BOX_SIZE - 10.0f));
    disabledSub.Add(subRow);

    outerRow.Add(disabledSub);
    zone.Add(outerRow);
    zone.Add(MakeDesc(
      "Orange: INHERIT — scales. "
      "Purple/Orange-red (inside purple box): "
      "Purple=INHERIT (fixed at 1.0), Orange-red=ENABLED (scales despite DISABLED parent)."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone D: Corner radius (ABSOLUTE & RELATIVE) + Borderline width
  //
  // Verifies that pixel-unit decoration properties are scaled correctly:
  //   - ABSOLUTE corner radius: visual radius = natural × effectiveScale
  //     → the rounded corner stays physically the same size
  //   - RELATIVE corner radius: fraction of view size → unaffected by scale
  //     (visual radius already grows because the view itself grows)
  //   - Borderline width: visual width = natural × effectiveScale
  //     → the border stays physically the same thickness
  //
  // To confirm correctness: increase scale and observe that:
  //   - ABSOLUTE radius boxes keep the same proportional rounding
  //   - RELATIVE radius box keeps the same fraction of each side
  //   - Borderline box keeps the same physical border thickness
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneD()
  {
    constexpr uint32_t C_ZONE_D_BG   = 0xFCE4EC; // light pink
    constexpr float    DECO_BOX_SIZE = 80.0f;     // natural box size
    constexpr float    ABS_RADIUS    = 20.0f;     // natural corner radius (ABSOLUTE)
    constexpr float    REL_RADIUS    = 0.3f;      // corner radius fraction (RELATIVE)
    constexpr float    BORDER_WIDTH  = 5.0f;      // natural borderline width (px)

    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_D_BG));

    zone.Add(MakeZoneTitle("[Zone D] Corner Radius & Borderline vs. UiScale"));
    zone.Add(MakeDesc(
      "Decoration properties (corner radius, borderline) are stored in natural pixels. "
      "The renderer uses naturalValue × effectiveScale so the decoration stays "
      "proportionally consistent as the view grows."));

    // ── Row 1: Corner radius comparison ──────────────────────────────────────
    zone.Add(MakeDesc("Corner radius: ABSOLUTE (20 px) vs. RELATIVE (0.3):"));

    StackLayout rowRadius = MakeHStack();

    // Box A: ABSOLUTE corner radius 20 px
    {
      View box = MakeDecoBox(UiColor(C_BLUE), DECO_BOX_SIZE);
      box.SetCornerRadius(ABS_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      rowRadius.Add(MakeLabeled(box, "ABSOLUTE\n20 px"));
    }

    // Box B: RELATIVE corner radius 0.3
    {
      View box = MakeDecoBox(UiColor(C_GREEN), DECO_BOX_SIZE);
      box.SetCornerRadius(REL_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      rowRadius.Add(MakeLabeled(box, "RELATIVE\n0.30"));
    }

    // Box C: individual ABSOLUTE radii (top-left only = large)
    {
      View box = MakeDecoBox(UiColor(C_ORANGE), DECO_BOX_SIZE);
      box.SetCornerRadius(30.0f, 5.0f, 30.0f, 5.0f);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      rowRadius.Add(MakeLabeled(box, "ABSOLUTE\nTL/BR=30\nTR/BL=5"));
    }

    zone.Add(rowRadius);

    // ── Row 2: Borderline width ───────────────────────────────────────────────
    zone.Add(MakeDesc("Borderline (5 px natural) with and without corner radius:"));

    StackLayout rowBorder = MakeHStack();

    // Box D: borderline only
    {
      View box = MakeDecoBox(UiColor(C_RED), DECO_BOX_SIZE);
      box.SetBorderlineWidth(BORDER_WIDTH);
      box.SetBorderlineColor(UiColor(C_DARK_TEXT));
      rowBorder.Add(MakeLabeled(box, "Border\n5 px\nno radius"));
    }

    // Box E: borderline + ABSOLUTE corner radius
    {
      View box = MakeDecoBox(UiColor(C_PURPLE), DECO_BOX_SIZE);
      box.SetCornerRadius(16.0f);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      box.SetBorderlineWidth(BORDER_WIDTH);
      box.SetBorderlineColor(UiColor(C_DARK_TEXT));
      rowBorder.Add(MakeLabeled(box, "Border 5 px\n+ABSOLUTE\nradius 16"));
    }

    // Box F: borderline + RELATIVE corner radius
    {
      View box = MakeDecoBox(UiColor(C_DEEP_ORG), DECO_BOX_SIZE);
      box.SetCornerRadius(0.25f);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      box.SetBorderlineWidth(BORDER_WIDTH);
      box.SetBorderlineColor(UiColor(C_DARK_TEXT));
      rowBorder.Add(MakeLabeled(box, "Border 5 px\n+RELATIVE\nradius 0.25"));
    }

    zone.Add(rowBorder);

    zone.Add(MakeDesc(
      "Change scale and verify: boxes grow/shrink, but corner roundness and "
      "border thickness remain visually proportional (not double-scaled)."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone E: VisualBase properties (Offset / Size / ExtraSize / CornerRadius / BordrlineWidth / BlurRadius)
  //
  // VisualBase transform properties which are not proportions are scaled correctly:
  //   - Offset / Size with Visual::Transform::ProportionFlags off : result = natural × effectiveScale
  //   - Offset / Size with Visual::Transform::ProportionFlags on → unaffected by scale
  //     (visual transform already grows because the view itself grows)
  //   - ExtraSize : extra size = natural × effectiveScale
  //   - ColorVisual::BlurRadius : blur radius = natural × effectiveScale
  //   - BorderVisual::BorderSize : border size = natural × effectiveScale
  //
  // Verifies that pixel-unit decoration properties are scaled correctly:
  //   - ABSOLUTE corner radius: visual radius = natural × effectiveScale
  //     → the rounded corner stays physically the same size
  //   - RELATIVE corner radius: fraction of view size → unaffected by scale
  //     (visual radius already grows because the view itself grows)
  //   - Borderline width: visual width = natural × effectiveScale
  //     → the border stays physically the same thickness
  //
  // To confirm correctness: increase scale and observe that:
  //   - Visual::Transform::ProportionFlags off Offset / Size keep the same proportional variables
  //   - Visual::Transform::ProportionFlags on Offset / Size keep the same fraction of each variables
  //   - ExtraSize keep the same fraction of each variables
  //   - For special, ColorVisual with BlurRadius and BorderVisual with BorderSize used keep the same fraction of each variables
  //   - ABSOLUTE radius boxes keep the same proportional rounding
  //   - RELATIVE radius box keeps the same fraction of each side
  //   - Borderline box keeps the same physical border thickness
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneE()
  {
    constexpr uint32_t C_ZONE_E_BG     = 0xDCD4FC; // light purple
    constexpr float    VISUAL_BOX_SIZE = 80.0f;    // natural box size for visuals
    constexpr float    ABS_OFFSET      = 10.0f;    // natural offset (ABSOLUTE)
    constexpr float    ABS_SIZE        = 60.0f;    // natural size (ABSOLUTE)
    constexpr float    REL_SIZE        = 0.5f;     // size proportion (RELATIVE)
    constexpr float    EXTRA_SIZE      = 10.0f;    // extra size
    constexpr float    BLUR_RADIUS     = 15.0f;    // blur radius for ColorVisual
    constexpr float    CORNER_RADIUS   = 12.0f;    // corner radius
    constexpr float    BORDER_WIDTH    = 4.0f;     // borderline width

    constexpr float CONTAINER_CORNER_RADIUS   = 20.0f;    // corner radius

    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_E_BG));

    zone.Add(MakeZoneTitle("[Zone E] VisualBase Transform & Decoration Properties"));
    zone.Add(MakeDesc(
      "VisualBase properties (Offset, Size, ExtraSize, CornerRadius, BorderlineWidth, BlurRadius) "
      "are stored in natural pixels and scaled by effectiveScale at render time."));

    // ── Row 1: Offset & Size with ProportionFlags ───────────────────────────────
    zone.Add(MakeDesc("Offset/Size with ProportionFlags (NONE vs ALL):"));

    StackLayout row1 = MakeHStack();

    // Box A: ProportionFlags::NONE - all values are absolute pixels
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE + 20.0f);
      container.SetRequestedHeight(VISUAL_BOX_SIZE + 20.0f);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_BLUE));
      visual.SetOffsetX(ABS_OFFSET);
      visual.SetOffsetY(ABS_OFFSET);
      visual.SetWidth(ABS_SIZE);
      visual.SetHeight(ABS_SIZE);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::NONE);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row1.Add(MakeLabeled(container, "NONE\nOffset:10px\nSize:60px"));
    }

    // Box B: ProportionFlags::ALL - all values are proportions
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE + 20.0f);
      container.SetRequestedHeight(VISUAL_BOX_SIZE + 20.0f);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_GREEN));
      visual.SetOffsetX(0.1f);
      visual.SetOffsetY(0.1f);
      visual.SetWidth(REL_SIZE);
      visual.SetHeight(REL_SIZE);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::ALL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row1.Add(MakeLabeled(container, "ALL\nOffset:0.1\nSize:0.5"));
    }

    // Box C: Mixed - X/Y proportional, Width/Height absolute
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE + 20.0f);
      container.SetRequestedHeight(VISUAL_BOX_SIZE + 20.0f);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_ORANGE));
      visual.SetOffsetX(0.1f);
      visual.SetOffsetY(0.1f);
      visual.SetWidth(ABS_SIZE);
      visual.SetHeight(ABS_SIZE);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::OFFSET_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row1.Add(MakeLabeled(container, "OFFSET\nOffset:0.1\nSize:60px"));
    }

    zone.Add(row1);

    // ── Row 2: ExtraSize ────────────────────────────────────────────────────────
    zone.Add(MakeDesc("ExtraSize (adds absolute pixels to size):"));

    StackLayout row2 = MakeHStack();

    // Box D: No extra size
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_PURPLE));
      visual.SetWidth(0.8f);
      visual.SetHeight(0.8f);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row2.Add(MakeLabeled(container, "No Extra\nSize:0.8"));
    }

    // Box E: With extra size
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_DEEP_ORG));
      visual.SetWidth(0.6f);
      visual.SetHeight(0.6f);
      visual.SetExtraWidth(EXTRA_SIZE);
      visual.SetExtraHeight(EXTRA_SIZE);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row2.Add(MakeLabeled(container, "Extra +10\nSize:0.6+10"));
    }

    zone.Add(row2);

    // ── Row 3: ColorVisual BlurRadius ────────────────────────────────────────────
    zone.Add(MakeDesc("ColorVisual BlurRadius (scaled by effectiveScale):"));

    StackLayout row3 = MakeHStack();

    // Box F: BlurRadius with ABSOLUTE corner radius
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_RED));
      visual.SetBlurRadius(BLUR_RADIUS);
      visual.SetCornerRadius(CORNER_RADIUS);
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      visual.SetWidth(0.7f);
      visual.SetHeight(0.7f);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row3.Add(MakeLabeled(container, "Blur 15px\nABS radius"));
    }

    // Box G: BlurRadius with RELATIVE corner radius
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_GREEN));
      visual.SetBlurRadius(BLUR_RADIUS);
      visual.SetCornerRadius(0.2f);
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      visual.SetWidth(0.7f);
      visual.SetHeight(0.7f);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row3.Add(MakeLabeled(container, "Blur 15px\nREL radius"));
    }

    // Box H: BorderlineWidth
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_BLUE));
      visual.SetBorderlineWidth(BORDER_WIDTH);
      visual.SetBorderlineColor(UiColor(C_DARK_TEXT));
      visual.SetCornerRadius(CORNER_RADIUS);
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      visual.SetWidth(0.7f);
      visual.SetHeight(0.7f);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row3.Add(MakeLabeled(container, "Border 4px\nABS radius"));
    }

    zone.Add(row3);

    // ── Row 4: ColorVisual BlurRadius with CutoutPolicy ────────────────────────────────────────────
    zone.Add(MakeDesc("ColorVisual BlurRadius with CutoutPolicy (scaled by effectiveScale):"));

    StackLayout row4 = MakeHStack();

    // Box I: ABSOLUTE corner radius container + BlurRadius with ABSOLUTE corner radius
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);
      container.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_RED));
      visual.SetBlurRadius(BLUR_RADIUS);
      visual.SetCornerRadius(CORNER_RADIUS);
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      visual.SetCutoutPolicy(Ui::CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
      visual.SetWidth(1.2f);
      visual.SetHeight(1.2f);
      visual.SetOrigin(Ui::Align::CENTER);
      visual.SetPivot(Ui::Align::CENTER);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row4.Add(MakeLabeled(container, "Blur 15px\nABS contaier\nABS radius"));
    }

    // Box J: ABSOLUTE corner radius container + BlurRadius with RELATIVE corner radius
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS);
      container.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_GREEN));
      visual.SetBlurRadius(BLUR_RADIUS);
      visual.SetCornerRadius(CORNER_RADIUS / (1.2f * VISUAL_BOX_SIZE));
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      visual.SetCutoutPolicy(Ui::CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
      visual.SetWidth(1.2f);
      visual.SetHeight(1.2f);
      visual.SetOrigin(Ui::Align::CENTER);
      visual.SetPivot(Ui::Align::CENTER);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row4.Add(MakeLabeled(container, "Blur 15px\nABS contaier\nABS relative"));
    }

    // Box K: RELATIVE corner radius container + BlurRadius with ABSOLUTE corner radius
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS / VISUAL_BOX_SIZE);
      container.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_BLUE));
      visual.SetBlurRadius(BLUR_RADIUS);
      visual.SetCornerRadius(CORNER_RADIUS);
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      visual.SetCutoutPolicy(Ui::CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
      visual.SetWidth(1.2f);
      visual.SetHeight(1.2f);
      visual.SetOrigin(Ui::Align::CENTER);
      visual.SetPivot(Ui::Align::CENTER);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row4.Add(MakeLabeled(container, "Blur 15px\nREL contaier\nABS radius"));
    }

    // Box L: RELATIVE corner radius container + BlurRadius with RELATIVE corner radius
    {
      View container = View::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetCornerRadius(CONTAINER_CORNER_RADIUS / VISUAL_BOX_SIZE);
      container.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);

      ColorVisual visual = ColorVisual::New();
      visual.SetColor(UiColor(C_PURPLE));
      visual.SetBlurRadius(BLUR_RADIUS);
      visual.SetCornerRadius(CORNER_RADIUS / (1.2f * VISUAL_BOX_SIZE));
      visual.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      visual.SetCutoutPolicy(Ui::CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
      visual.SetWidth(1.2f);
      visual.SetHeight(1.2f);
      visual.SetOrigin(Ui::Align::CENTER);
      visual.SetPivot(Ui::Align::CENTER);
      visual.SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
      container.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row4.Add(MakeLabeled(container, "Blur 15px\nREL contaier\nABS relative"));
    }

    zone.Add(row4);

    // ── Row 5: BorderVisual BorderSize ────────────────────────────────────────────
    zone.Add(MakeDesc("BorderVisual BorderSize (scaled by effectiveScale):"));

    StackLayout row5 = MakeHStack();

    // Box M: BorderlineWidth of Label
    {
      Label container = Label::New("BORDER and TEXT");
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));
      container.SetBorderlineWidth(BORDER_WIDTH);
      container.SetBorderlineColor(UiColor(C_DARK_TEXT));
      container.SetBorderlineOffset(-1.0f);

      container.SetFontSize(22.0f);
      container.SetTextColor(UiColor(C_DARK_TEXT));
      container.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      container.SetMultiLine(true);
      container.SetLineWrapMode(Text::LineWrapMode::WORD);

      row5.Add(MakeLabeled(container, "BorderlineWidth 4px without BorderVisual\n"));
    }

    // Box M: Same result with BorderVisual and TextVisual
    {
      Label container = Label::New();
      container.SetRequestedWidth(VISUAL_BOX_SIZE);
      container.SetRequestedHeight(VISUAL_BOX_SIZE);
      container.SetBackgroundColor(UiColor(0xF5F5F5));

      BorderVisual borderVisual = BorderVisual::New();
      borderVisual.SetColor(UiColor(C_DARK_TEXT));
      borderVisual.SetBorderSize(BORDER_WIDTH);
      container.AddVisual(borderVisual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      TextVisual textVisual = TextVisual::New();
      textVisual.SetText("BORDER and TEXT");
      textVisual.SetFontSize(22.0f);
      textVisual.SetTextColor(UiColor(C_DARK_TEXT));
      textVisual.SetHorizontalAlignment(Text::Alignment::CENTER);
      textVisual.SetMultiLine(true);
      textVisual.SetLineWrapMode(Text::LineWrapMode::WORD);
      container.AddVisual(textVisual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

      row5.Add(MakeLabeled(container, "BorderVisual 4px\n"));
    }

    zone.Add(row5);

    zone.Add(MakeDesc(
      "Change scale and verify: ABSOLUTE values scale proportionally, "
      "RELATIVE values stay as fractions. ExtraSize and BlurRadius scale with effectiveScale."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone F: RenderEffect (GaussianBlurEffect / BackgroundBlurEffect / MaskEffect / OffscreenRendering)
  //
  // Test for the render effect result follow the applied view's corner radius with ui scale.
  //
  // Make 2x4 layout, upside has ABSOLUTE corner radius, downside has RELATIVE corner radius.
  // Each column has the render effects.
  // Check whether upside result is same as downside result.
  //
  // To check background blur effect works well, root layout should have ImageVisual.
  //
  // ═══════════════════════════════════════════════════════════════════════════

  Label MakeSmallLabel(const char* text, uint32_t color)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetFontSize(10.0f);
    label.SetTextColor(UiColor(color));
    return label;
  }

  View MakeEffectChild(float size, float x, float y, UiColor color)
  {
    View child = View::New();
    child.SetRequestedWidth(size);
    child.SetRequestedHeight(size);
    child.SetRequestedX(x);
    child.SetRequestedY(y);
    child.SetBackgroundColor(color);
    return child;
  }

  View BuildZoneF()
  {
    constexpr uint32_t C_ZONE_F_BG       = 0xDCF4DC; // light green
    constexpr float    EFFECT_BOX_SIZE   = 80.0f;    // box size for effect tests
    constexpr float    ABS_CORNER_RADIUS = 20.0f;    // ABSOLUTE corner radius
    constexpr float    REL_CORNER_RADIUS = 0.25f;     // RELATIVE corner radius
    constexpr uint32_t BLUR_RADIUS       = 60u;      // blur radius for effects

    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_F_BG));

    zone.Add(MakeZoneTitle("[Zone F] RenderEffect with Corner Radius & UiScale"));
    zone.Add(MakeDesc(
      "RenderEffect results should follow the applied view's corner radius with ui scale. "
      "Top row: ABSOLUTE corner radius. Bottom row: RELATIVE corner radius. "
      "Each column shows different render effects. Results should match visually."));

    // ── Row 1: ABSOLUTE corner radius ──────────────────────────────────────────
    zone.Add(MakeDesc("ABSOLUTE corner radius (20 px):"));

    StackLayout rowAbs = MakeHStack();

    // Box 1: GaussianBlurEffect with ABSOLUTE corner radius
    {
      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      box.SetBackgroundColor(UiColor(C_BLUE));
      box.SetCornerRadius(ABS_CORNER_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, 0.0f, UiColor(C_RED)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, 0.0f, UiColor(C_GREEN)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_BLUE)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_ORANGE)));

      GaussianBlurEffect blurEffect = GaussianBlurEffect::New(BLUR_RADIUS);
      box.SetRenderEffect(blurEffect);

      rowAbs.Add(MakeLabeled(box, "GaussianBlur\nABS radius"));
    }

    // Box 2: BackgroundBlurEffect with ABSOLUTE corner radius
    {
      View bgBlurBox;
      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      bgBlurBox = MakeEffectChild(EFFECT_BOX_SIZE, 0.0f, 0.0f, UiColor(C_WHITE).WithAlpha(0.3f));
      bgBlurBox.SetCornerRadius(ABS_CORNER_RADIUS);
      bgBlurBox.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, 0.0f, UiColor(C_RED)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, 0.0f, UiColor(C_GREEN)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_BLUE)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_ORANGE)));
    box.Add(bgBlurBox);

      BackgroundBlurEffect bgBlurEffect = BackgroundBlurEffect::New(BLUR_RADIUS);
      bgBlurBox.SetRenderEffect(bgBlurEffect);

      rowAbs.Add(MakeLabeled(box, "BackgroundBlur\nABS radius"));
    }

    // Box 3: MaskEffect with ABSOLUTE corner radius
    {
      // Create mask source view (a simple colored shape)
      View maskSource = View::New();
      maskSource.SetRequestedWidth(EFFECT_BOX_SIZE);
      maskSource.SetRequestedHeight(EFFECT_BOX_SIZE);
      maskSource.SetRequestedX(EFFECT_BOX_SIZE * 0.5f);
      maskSource.SetRequestedY(EFFECT_BOX_SIZE * 0.5f);
      maskSource.SetBackgroundColor(UiColor(C_WHITE));
      maskSource.SetCornerRadius(ABS_CORNER_RADIUS);
      maskSource.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);

      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      box.SetBackgroundColor(UiColor(C_ORANGE));
      box.SetCornerRadius(ABS_CORNER_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);

      MaskEffect maskEffect = MaskEffect::New(maskSource);
      box.SetRenderEffect(maskEffect);
      box.Add(maskSource); // MaskEffect needs the source as child

      rowAbs.Add(MakeLabeled(box, "MaskEffect\nABS radius"));
    }

    // Box 4: OffscreenRendering with ABSOLUTE corner radius
    {
      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      box.SetBackgroundColor(UiColor(C_PURPLE));
      box.SetCornerRadius(ABS_CORNER_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      box.SetProperty(Ui::View::Property::OFFSCREEN_RENDERING, Ui::View::OffScreenRenderingType::REFRESH_ALWAYS);
      box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, 0.0f, UiColor(C_RED)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, 0.0f, UiColor(C_GREEN)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_BLUE)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_ORANGE)));

      rowAbs.Add(MakeLabeled(box, "OffscreenRendering\nABS radius"));
    }

    zone.Add(rowAbs);

    // ── Row 2: RELATIVE corner radius ──────────────────────────────────────────
    zone.Add(MakeDesc("RELATIVE corner radius (0.25):"));

    StackLayout rowRel = MakeHStack();

    // Box 5: GaussianBlurEffect with RELATIVE corner radius
    {
      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      box.SetBackgroundColor(UiColor(C_BLUE));
      box.SetCornerRadius(REL_CORNER_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, 0.0f, UiColor(C_RED)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, 0.0f, UiColor(C_GREEN)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_BLUE)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_ORANGE)));

      GaussianBlurEffect blurEffect = GaussianBlurEffect::New(BLUR_RADIUS);
      box.SetRenderEffect(blurEffect);

      rowRel.Add(MakeLabeled(box, "GaussianBlur\nREL radius"));
    }

    // Box 6: BackgroundBlurEffect with RELATIVE corner radius
    {
      View bgBlurBox;
      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      bgBlurBox = MakeEffectChild(EFFECT_BOX_SIZE, 0.0f, 0.0f, UiColor(C_WHITE).WithAlpha(0.3f));
      bgBlurBox.SetCornerRadius(REL_CORNER_RADIUS);
      bgBlurBox.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, 0.0f, UiColor(C_RED)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, 0.0f, UiColor(C_GREEN)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_BLUE)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_ORANGE)));
    box.Add(bgBlurBox);

      BackgroundBlurEffect bgBlurEffect = BackgroundBlurEffect::New(BLUR_RADIUS);
      bgBlurBox.SetRenderEffect(bgBlurEffect);

      rowRel.Add(MakeLabeled(box, "BackgroundBlur\nREL radius"));
    }

    // Box 7: MaskEffect with RELATIVE corner radius
    {
      // Create mask source view (a simple colored shape)
      View maskSource = View::New();
      maskSource.SetRequestedWidth(EFFECT_BOX_SIZE);
      maskSource.SetRequestedHeight(EFFECT_BOX_SIZE);
      maskSource.SetRequestedX(EFFECT_BOX_SIZE * 0.5f);
      maskSource.SetRequestedY(EFFECT_BOX_SIZE * 0.5f);
      maskSource.SetBackgroundColor(UiColor(C_WHITE));
      maskSource.SetCornerRadius(ABS_CORNER_RADIUS);
      maskSource.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);

      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      box.SetBackgroundColor(UiColor(C_ORANGE));
      box.SetCornerRadius(REL_CORNER_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);

      MaskEffect maskEffect = MaskEffect::New(maskSource);
      box.SetRenderEffect(maskEffect);
      box.Add(maskSource); // MaskEffect needs the source as child

      rowRel.Add(MakeLabeled(box, "MaskEffect\nREL radius"));
    }

    // Box 8: OffscreenRendering with RELATIVE corner radius
    {
      View box = View::New();
      box.SetRequestedWidth(EFFECT_BOX_SIZE);
      box.SetRequestedHeight(EFFECT_BOX_SIZE);
      box.SetBackgroundColor(UiColor(C_PURPLE));
      box.SetCornerRadius(REL_CORNER_RADIUS);
      box.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
      box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, 0.0f, UiColor(C_RED)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, 0.0f, UiColor(C_GREEN)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, 0.0f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_BLUE)));
    box.Add(MakeEffectChild(EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, EFFECT_BOX_SIZE * 0.5f, UiColor(C_ORANGE)));

      box.SetProperty(Ui::View::Property::OFFSCREEN_RENDERING, Ui::View::OffScreenRenderingType::REFRESH_ALWAYS);

      rowRel.Add(MakeLabeled(box, "OffscreenRendering\nREL radius"));
    }

    zone.Add(rowRel);

    zone.Add(MakeDesc(
      "Change scale and verify: ABSOLUTE and RELATIVE corner radius results should match proportionally. "
      "RenderEffect output should respect the view's corner radius at any scale."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone G: Reparent from DISABLED parent to INHERIT parent
  //
  // A box (INHERIT policy) starts under a DISABLED container → effectiveScale = 1.0.
  // Pressing "→ INHERIT 부모로" reparents it to an INHERIT container so it
  // immediately picks up the system scale.
  // Pressing "← DISABLED 부모로" moves it back → effectiveScale = 1.0 again.
  //
  // Observe: set scale to 1.5+ first, then press the move buttons.
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneG()
  {
    StackLayout zone = BuildZoneContainer(UiColor(0xFFF3E0)); // light amber

    zone.Add(MakeZoneTitle("[Zone G] Reparent: DISABLED → INHERIT 시 Scale 재적용"));
    zone.Add(MakeDesc(
      "박스(INHERIT)가 처음에 DISABLED 부모 아래에 있어 effectiveScale = 1.0으로 고정됩니다. "
      "버튼으로 INHERIT 부모로 이동하면 OnChildAdd → 재레이아웃이 트리거되어 "
      "시스템 스케일이 즉시 적용됩니다."));

    // Two slots side by side
    StackLayout slotsRow = MakeHStack();

    // Left slot: DISABLED parent (red tint)
    mDisabledSlot = StackLayout::New(StackOrientation::VERTICAL);
    mDisabledSlot.SetRequestedWidth(130.0f);
    mDisabledSlot.SetRequestedHeight(WRAP_CONTENT);
    mDisabledSlot.SetBackgroundColor(UiColor(0xFFCDD2));
    mDisabledSlot.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));
    mDisabledSlot.SetSpacing(6.0f);
    mDisabledSlot.SetCornerRadius(Vector4(8.0f, 8.0f, 8.0f, 8.0f));
    mDisabledSlot.SetUiScalePolicy(UiScalePolicy::DISABLED); // ★ blocks scale
    mDisabledSlot.Add(MakeSmallLabel("DISABLED parent", 0xB71C1C));

    // Right slot: INHERIT parent (teal tint)
    mInheritSlot = StackLayout::New(StackOrientation::VERTICAL);
    mInheritSlot.SetRequestedWidth(130.0f);
    mInheritSlot.SetRequestedHeight(WRAP_CONTENT);
    mInheritSlot.SetBackgroundColor(UiColor(0xE0F2F1));
    mInheritSlot.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));
    mInheritSlot.SetSpacing(6.0f);
    mInheritSlot.SetCornerRadius(Vector4(8.0f, 8.0f, 8.0f, 8.0f));
    // INHERIT (default)
    mInheritSlot.Add(MakeSmallLabel("INHERIT parent", 0x1B5E20));

    // The box that moves between parents — INHERIT policy, so its effective scale
    // depends entirely on which parent it is attached to.
    mReparentBox   = MakePolicyBox("INHERIT", UiColor(0x00897B), UiScalePolicy::INHERIT, BOX_SIZE);
    mBoxInDisabled = true;
    mDisabledSlot.Add(mReparentBox); // starts in the DISABLED parent

    slotsRow.Add(mDisabledSlot);
    slotsRow.Add(mInheritSlot);
    zone.Add(slotsRow);

    // Action buttons
    StackLayout btnRow = MakeHStack();

    InteractiveView toInheritBtn = MakeButton("→ INHERIT 부모로", UiColor(0x00695C));
    toInheritBtn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    toInheritBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      if(mBoxInDisabled)
      {
        mDisabledSlot.Remove(mReparentBox);
        mInheritSlot.Add(mReparentBox);
        mBoxInDisabled = false;
      }
    });

    InteractiveView toDisabledBtn = MakeButton("← DISABLED 부모로", UiColor(0xC62828));
    toDisabledBtn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    toDisabledBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      if(!mBoxInDisabled)
      {
        mInheritSlot.Remove(mReparentBox);
        mDisabledSlot.Add(mReparentBox);
        mBoxInDisabled = true;
      }
    });

    btnRow.Add(toInheritBtn);
    btnRow.Add(toDisabledBtn);
    zone.Add(btnRow);

    zone.Add(MakeDesc(
      "확인 방법: 위 컨트롤 패널에서 Scale을 1.5 이상으로 설정한 뒤 "
      "→ INHERIT 부모로 버튼을 누르면 박스가 즉시 커집니다. "
      "← DISABLED 부모로 복귀하면 다시 1.0 크기로 돌아옵니다."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone H: Unparent → scale change while detached → Re-add applies new scale
  //
  // Shows that:
  //   (a) An unparented view is excluded from UiScaleManager tracking — no
  //       overhead when scale changes while the view is off-scene.
  //   (b) When the view is re-added, it picks up the current system scale
  //       correctly via OnSceneConnection → InvalidateMeasure → re-layout.
  //
  // Steps: ① Unparent → ② change scale in control panel → ③ Re-add.
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneH()
  {
    StackLayout zone = BuildZoneContainer(UiColor(0xE8EAF6)); // light indigo

    zone.Add(MakeZoneTitle("[Zone H] Unparent 후 Scale 변경 → Re-add 시 정확한 Scale 적용"));
    zone.Add(MakeDesc(
      "박스를 Unparent 하면 OnSceneDisconnection이 발생하고 UiScaleManager 추적에서 제외됩니다. "
      "unparent 상태에서 시스템 스케일이 바뀌어도 박스에는 연산이 발생하지 않습니다. "
      "Re-add 시 OnSceneConnection → InvalidateMeasure → 재레이아웃으로 새 scale이 정확히 적용됩니다."));

    // Status label (updated dynamically)
    mHStatusLabel = Label::New("상태: 부모에 연결됨");
    mHStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mHStatusLabel.SetRequestedHeight(WRAP_CONTENT);
    mHStatusLabel.SetFontSize(13.0f);
    mHStatusLabel.SetTextColor(UiColor(0x1A237E));
    zone.Add(mHStatusLabel);

    // INHERIT container where the box lives
    mHContainer = StackLayout::New(StackOrientation::VERTICAL);
    mHContainer.SetRequestedWidth(MATCH_PARENT);
    mHContainer.SetRequestedHeight(WRAP_CONTENT);
    mHContainer.SetBackgroundColor(UiColor(0xC5CAE9));
    mHContainer.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));
    mHContainer.SetSpacing(6.0f);
    mHContainer.SetCornerRadius(Vector4(8.0f, 8.0f, 8.0f, 8.0f));
    // INHERIT (default) — normal scale propagation
    mHContainer.Add(MakeSmallLabel("INHERIT parent container", 0x1A237E));

    mScaleChangeBox = MakePolicyBox("INHERIT", UiColor(0x3949AB), UiScalePolicy::INHERIT, BOX_SIZE);
    mHContainer.Add(mScaleChangeBox);
    mIsParented = true;

    zone.Add(mHContainer);

    // Action buttons
    StackLayout btnRow = MakeHStack();

    InteractiveView unparentBtn = MakeButton("① Unparent", UiColor(0xE53935));
    unparentBtn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    unparentBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      if(mIsParented)
      {
        mHContainer.Remove(mScaleChangeBox);
        mIsParented = false;
        mHStatusLabel.SetText("상태: Unparented — ② 위 패널에서 Scale 변경 후 ③ Re-add 해보세요");
      }
    });

    InteractiveView readdBtn = MakeButton("③ Re-add", UiColor(0x2E7D32));
    readdBtn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    readdBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      if(!mIsParented)
      {
        mHContainer.Add(mScaleChangeBox);
        mIsParented = true;
        std::string s = "상태: 부모에 연결됨 — Scale " + Fmt(UiScaleManager::Get().GetScale()) + " 적용됨";
        mHStatusLabel.SetText(s.c_str());
      }
    });

    btnRow.Add(unparentBtn);
    btnRow.Add(readdBtn);
    zone.Add(btnRow);

    zone.Add(MakeDesc(
      "② 단계에서 Scale을 바꿔도 박스는 화면에 없으므로 재연산이 일어나지 않습니다. "
      "③ Re-add 후에 변경된 Scale이 올바르게 반영되는지 확인하세요."));

    return zone;
  }

  // ─── Decoration demo helpers ──────────────────────────────────────────────

  // Plain colored box, no decorations set
  View MakeDecoBox(const UiColor& color, float size)
  {
    View box = View::New();
    box.SetRequestedWidth(size);
    box.SetRequestedHeight(size);
    box.SetBackgroundColor(color);
    return box;
  }

  // Wraps a box with a small centered label below it in a vertical stack
  View MakeLabeled(View box, const char* text)
  {
    Label lbl = Label::New(text);
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetRequestedHeight(WRAP_CONTENT);
    lbl.SetFontSize(11.0f);
    lbl.SetTextColor(UiColor(C_DARK_TEXT));
    lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    lbl.SetMultiLine(true);
    lbl.SetLineWrapMode(Text::LineWrapMode::WORD);

    StackLayout col = StackLayout::New(StackOrientation::VERTICAL);
    col.SetRequestedWidth(WRAP_CONTENT);
    col.SetRequestedHeight(WRAP_CONTENT);
    col.SetSpacing(4.0f);
    col.Add(box);
    col.Add(lbl);
    return col;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Shared UI builder helpers
  // ═══════════════════════════════════════════════════════════════════════════

  // Wrapper zone container (vertical stack with background)
  StackLayout BuildZoneContainer(const UiColor& bgColor)
  {
    StackLayout zone = StackLayout::New(StackOrientation::VERTICAL);
    zone.SetRequestedWidth(MATCH_PARENT);
    zone.SetRequestedHeight(WRAP_CONTENT);
    zone.SetBackgroundColor(bgColor);
    zone.SetPadding(Insets(12.0f, 12.0f, 12.0f, 12.0f));
    zone.SetSpacing(10.0f);
    zone.SetCornerRadius(Vector4(ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS));
    return zone;
  }

  // Horizontal stack for a row of boxes
  StackLayout MakeHStack()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(WRAP_CONTENT);
    row.SetSpacing(8.0f);
    return row;
  }

  // Colored box with centered policy label and explicit UiScalePolicy
  View MakePolicyBox(const char* text, const UiColor& color, UiScalePolicy policy, float size)
  {
    Label lbl = Label::New(text);
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetRequestedHeight(MATCH_PARENT);
    lbl.SetFontSize(11.0f);
    lbl.SetTextColor(UiColor(C_WHITE));
    lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    lbl.SetVerticalTextAlignment(Text::Alignment::CENTER);
    lbl.SetMultiLine(true);
    lbl.SetLineWrapMode(Text::LineWrapMode::WORD);

    View box = View::New();
    box.SetRequestedWidth(size);
    box.SetRequestedHeight(size);
    box.SetBackgroundColor(color);
    box.SetCornerRadius(Vector4(BOX_RADIUS, BOX_RADIUS, BOX_RADIUS, BOX_RADIUS));
    box.SetUiScalePolicy(policy); // ← apply the policy
    box.Add(lbl);
    return box;
  }

  // Clickable button with centered text label
  InteractiveView MakeButton(const std::string& text, const UiColor& color)
  {
    Label lbl = Label::New(text.c_str());
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetRequestedHeight(MATCH_PARENT);
    lbl.SetFontSize(15.0f);
    lbl.SetTextColor(UiColor(C_WHITE));
    lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    lbl.SetVerticalTextAlignment(Text::Alignment::CENTER);

    InteractiveView btn = InteractiveView::New();
    btn.SetRequestedWidth(WRAP_CONTENT);
    btn.SetRequestedHeight(BTN_HEIGHT);
    btn.SetBackgroundColor(color);
    btn.SetCornerRadius(Vector4(BTN_RADIUS, BTN_RADIUS, BTN_RADIUS, BTN_RADIUS));
    btn.SetPadding(Insets(12.0f, 12.0f, 6.0f, 6.0f));
    btn.Add(lbl);
    return btn;
  }

  // Zone section title label
  Label MakeZoneTitle(const char* text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetFontSize(15.0f);
    label.SetTextColor(UiColor(C_ZONE_TITLE));
    return label;
  }

  // Small description label (multi-line, gray)
  Label MakeDesc(const char* text)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetFontSize(12.0f);
    label.SetTextColor(UiColor(C_GRAY_TEXT));
    label.SetMultiLine(true);
    label.SetLineWrapMode(Text::LineWrapMode::WORD);
    return label;
  }

private:
  Application&    mApplication;
  Label           mScaleLabel;  // updated on every ApplyScale()
  InputField      mScaleInput;
  InteractiveView mScalableBtn;      // global master switch toggle
  Label           mScalableBtnLabel; // toggle button caption (ON/OFF)

  // Zone G: reparent between DISABLED / INHERIT parents
  StackLayout mDisabledSlot;
  StackLayout mInheritSlot;
  View        mReparentBox;
  bool        mBoxInDisabled{true};

  // Zone H: unparent → scale change → re-add
  StackLayout mHContainer;
  View        mScaleChangeBox;
  Label       mHStatusLabel;
  bool        mIsParented{true};
};

// ─────────────────────────── Entry point ─────────────────────────────────────
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  UiScaleController controller(application);
  application.MainLoop();
  return 0;
}
