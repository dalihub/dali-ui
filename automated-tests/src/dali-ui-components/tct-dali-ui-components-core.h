#ifndef TCT_DALI_UI_COMPONENTS_CORE_H
#define TCT_DALI_UI_COMPONENTS_CORE_H

#include "testcase.h"

extern void utc_dali_components_startup(void);
extern void utc_dali_components_cleanup(void);
extern void utc_dali_dummy_components_startup(void);
extern void utc_dali_dummy_components_cleanup(void);
extern void utc_dali_text_button_startup(void);
extern void utc_dali_text_button_cleanup(void);
extern void utc_dali_chartview_startup(void);
extern void utc_dali_chartview_cleanup(void);
extern void utc_dali_chartseries_startup(void);
extern void utc_dali_chartseries_cleanup(void);
extern void utc_dali_pieseries_startup(void);
extern void utc_dali_pieseries_cleanup(void);
extern void utc_dali_chartaxis_startup(void);
extern void utc_dali_chartaxis_cleanup(void);
extern void utc_dali_chartsection_startup(void);
extern void utc_dali_chartsection_cleanup(void);

extern int UtcDaliComponentsStyleSheetNewP(void);
extern int UtcDaliDummyComponentConstructorP(void);
extern int UtcDaliDummyComponentNewP(void);
extern int UtcDaliDummyComponentCopyConstructorP(void);
extern int UtcDaliDummyComponentMoveConstructor(void);
extern int UtcDaliDummyComponentAssignmentOperatorP(void);
extern int UtcDaliDummyComponentMoveAssignment(void);
extern int UtcDaliDummyComponentDownCastP(void);
extern int UtcDaliDummyComponentDownCastN(void);
extern int UtcDaliTextButtonConstructorP(void);
extern int UtcDaliTextButtonNewP(void);
extern int UtcDaliTextButtonNewWithTextP(void);
extern int UtcDaliTextButtonNewWithStyleP(void);
extern int UtcDaliTextButtonNewWithTextAndStyleP(void);
extern int UtcDaliTextButtonTextP(void);
extern int UtcDaliTextButtonCopyAndMoveP(void);
extern int UtcDaliTextButtonAssignmentP(void);
extern int UtcDaliTextButtonDownCastP(void);
extern int UtcDaliTextButtonDownCastN(void);
extern int UtcDaliTextButtonAlignmentP(void);
extern int UtcDaliTextButtonStyleP(void);
extern int UtcDaliTextButtonStyleConfigureP(void);
extern int UtcDaliTextButtonTextPropertiesP(void);
extern int UtcDaliTextButtonStyleDefaultKeyP(void);
extern int UtcDaliChartViewConstructorP(void);
extern int UtcDaliChartViewNewLineP(void);
extern int UtcDaliChartViewNewBarP(void);
extern int UtcDaliChartViewNewPieP(void);
extern int UtcDaliChartViewNewGaugeP(void);
extern int UtcDaliChartViewCopyConstructorP(void);
extern int UtcDaliChartViewMoveConstructor(void);
extern int UtcDaliChartViewAssignmentOperatorP(void);
extern int UtcDaliChartViewMoveAssignment(void);
extern int UtcDaliChartViewDownCastP(void);
extern int UtcDaliChartViewDownCastN(void);
extern int UtcDaliChartViewSetTitleP(void);
extern int UtcDaliChartViewAnimationP(void);
extern int UtcDaliChartViewAddRemoveSeriesP(void);
extern int UtcDaliChartViewRemoveSeriesN(void);
extern int UtcDaliChartViewRemoveAllSeriesP(void);
extern int UtcDaliChartViewZoomModeP(void);
extern int UtcDaliChartViewHitTestingP(void);
extern int UtcDaliChartViewPropertyP(void);
extern int UtcDaliChartViewSignalP(void);
extern int UtcDaliChartViewGaugeP(void);
extern int UtcDaliChartViewSectionP(void);
extern int UtcDaliChartViewSettersP(void);
extern int UtcDaliChartSeriesDownCastP(void);
extern int UtcDaliChartSeriesDownCastN(void);
extern int UtcDaliChartSeriesSetNameP(void);
extern int UtcDaliChartSeriesSetValuesP(void);
extern int UtcDaliChartSeriesAppendValueP(void);
extern int UtcDaliChartSeriesSetVisibleP(void);
extern int UtcDaliChartSeriesSetMaxDataPointsP(void);
extern int UtcDaliChartSeriesSetZIndexP(void);
extern int UtcDaliLineSeriesConstructorP(void);
extern int UtcDaliLineSeriesNewP(void);
extern int UtcDaliLineSeriesCopyConstructorP(void);
extern int UtcDaliLineSeriesMoveConstructor(void);
extern int UtcDaliLineSeriesDownCastP(void);
extern int UtcDaliLineSeriesDownCastN(void);
extern int UtcDaliLineSeriesSetLinePropertiesP(void);
extern int UtcDaliLineSeriesSetMarkerPropertiesP(void);
extern int UtcDaliLineSeriesFillP(void);
extern int UtcDaliLineSeriesDataLabelsP(void);
extern int UtcDaliLineSeriesSettersP(void);
extern int UtcDaliBarSeriesNewP(void);
extern int UtcDaliBarSeriesDownCastP(void);
extern int UtcDaliBarSeriesDownCastN(void);
extern int UtcDaliBarSeriesSetPropertiesP(void);
extern int UtcDaliBarSeriesDataLabelsP(void);
extern int UtcDaliPieSeriesConstructorP(void);
extern int UtcDaliPieSeriesNewP(void);
extern int UtcDaliPieSeriesCopyConstructorP(void);
extern int UtcDaliPieSeriesMoveConstructor(void);
extern int UtcDaliPieSeriesDownCastP(void);
extern int UtcDaliPieSeriesDownCastN(void);
extern int UtcDaliPieSeriesAddSliceP(void);
extern int UtcDaliPieSeriesClearSlicesP(void);
extern int UtcDaliPieSeriesInnerRadiusP(void);
extern int UtcDaliPieSeriesSliceGapP(void);
extern int UtcDaliPieSeriesDataLabelsP(void);
extern int UtcDaliPieSeriesSettersP(void);
extern int UtcDaliScatterSeriesNewP(void);
extern int UtcDaliScatterSeriesDownCastP(void);
extern int UtcDaliScatterSeriesDownCastN(void);
extern int UtcDaliScatterSeriesSetPropertiesP(void);
extern int UtcDaliChartAxisConstructorP(void);
extern int UtcDaliChartAxisNewP(void);
extern int UtcDaliChartAxisCopyConstructorP(void);
extern int UtcDaliChartAxisMoveConstructor(void);
extern int UtcDaliChartAxisDownCastP(void);
extern int UtcDaliChartAxisDownCastN(void);
extern int UtcDaliChartAxisSetLabelsP(void);
extern int UtcDaliChartAxisSetTitleP(void);
extern int UtcDaliChartAxisSetRangeLimitsP(void);
extern int UtcDaliChartAxisSetAutoRangeP(void);
extern int UtcDaliChartAxisSetGridLinesP(void);
extern int UtcDaliChartAxisSetAxisLineP(void);
extern int UtcDaliChartAxisSetDataPaddingP(void);
extern int UtcDaliChartAxisSetLabelsRotationP(void);
extern int UtcDaliChartAxisSettersP(void);
extern int UtcDaliChartSectionConstructorP(void);
extern int UtcDaliChartSectionNewP(void);
extern int UtcDaliChartSectionCopyConstructorP(void);
extern int UtcDaliChartSectionAssignmentOperatorP(void);
extern int UtcDaliChartSectionSetBoundsP(void);
extern int UtcDaliChartSectionSetFillColorP(void);
extern int UtcDaliChartSectionSetStrokeP(void);
extern int UtcDaliChartSectionSettersP(void);

testcase tc_array[] = {
    {"UtcDaliComponentsStyleSheetNewP", UtcDaliComponentsStyleSheetNewP, utc_dali_components_startup, utc_dali_components_cleanup},
    {"UtcDaliDummyComponentConstructorP", UtcDaliDummyComponentConstructorP, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentNewP", UtcDaliDummyComponentNewP, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentCopyConstructorP", UtcDaliDummyComponentCopyConstructorP, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentMoveConstructor", UtcDaliDummyComponentMoveConstructor, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentAssignmentOperatorP", UtcDaliDummyComponentAssignmentOperatorP, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentMoveAssignment", UtcDaliDummyComponentMoveAssignment, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentDownCastP", UtcDaliDummyComponentDownCastP, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliDummyComponentDownCastN", UtcDaliDummyComponentDownCastN, utc_dali_dummy_components_startup, utc_dali_dummy_components_cleanup},
    {"UtcDaliTextButtonConstructorP", UtcDaliTextButtonConstructorP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonNewP", UtcDaliTextButtonNewP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonNewWithTextP", UtcDaliTextButtonNewWithTextP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonNewWithStyleP", UtcDaliTextButtonNewWithStyleP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonNewWithTextAndStyleP", UtcDaliTextButtonNewWithTextAndStyleP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonTextP", UtcDaliTextButtonTextP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonCopyAndMoveP", UtcDaliTextButtonCopyAndMoveP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonAssignmentP", UtcDaliTextButtonAssignmentP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonDownCastP", UtcDaliTextButtonDownCastP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonDownCastN", UtcDaliTextButtonDownCastN, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonAlignmentP", UtcDaliTextButtonAlignmentP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonStyleP", UtcDaliTextButtonStyleP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonStyleConfigureP", UtcDaliTextButtonStyleConfigureP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonTextPropertiesP", UtcDaliTextButtonTextPropertiesP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliTextButtonStyleDefaultKeyP", UtcDaliTextButtonStyleDefaultKeyP, utc_dali_text_button_startup, utc_dali_text_button_cleanup},
    {"UtcDaliChartViewConstructorP", UtcDaliChartViewConstructorP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewNewLineP", UtcDaliChartViewNewLineP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewNewBarP", UtcDaliChartViewNewBarP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewNewPieP", UtcDaliChartViewNewPieP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewNewGaugeP", UtcDaliChartViewNewGaugeP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewCopyConstructorP", UtcDaliChartViewCopyConstructorP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewMoveConstructor", UtcDaliChartViewMoveConstructor, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewAssignmentOperatorP", UtcDaliChartViewAssignmentOperatorP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewMoveAssignment", UtcDaliChartViewMoveAssignment, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewDownCastP", UtcDaliChartViewDownCastP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewDownCastN", UtcDaliChartViewDownCastN, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewSetTitleP", UtcDaliChartViewSetTitleP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewAnimationP", UtcDaliChartViewAnimationP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewAddRemoveSeriesP", UtcDaliChartViewAddRemoveSeriesP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewRemoveSeriesN", UtcDaliChartViewRemoveSeriesN, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewRemoveAllSeriesP", UtcDaliChartViewRemoveAllSeriesP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewZoomModeP", UtcDaliChartViewZoomModeP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewHitTestingP", UtcDaliChartViewHitTestingP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewPropertyP", UtcDaliChartViewPropertyP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewSignalP", UtcDaliChartViewSignalP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewGaugeP", UtcDaliChartViewGaugeP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewSectionP", UtcDaliChartViewSectionP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartViewSettersP", UtcDaliChartViewSettersP, utc_dali_chartview_startup, utc_dali_chartview_cleanup},
    {"UtcDaliChartSeriesDownCastP", UtcDaliChartSeriesDownCastP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesDownCastN", UtcDaliChartSeriesDownCastN, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesSetNameP", UtcDaliChartSeriesSetNameP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesSetValuesP", UtcDaliChartSeriesSetValuesP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesAppendValueP", UtcDaliChartSeriesAppendValueP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesSetVisibleP", UtcDaliChartSeriesSetVisibleP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesSetMaxDataPointsP", UtcDaliChartSeriesSetMaxDataPointsP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliChartSeriesSetZIndexP", UtcDaliChartSeriesSetZIndexP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesConstructorP", UtcDaliLineSeriesConstructorP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesNewP", UtcDaliLineSeriesNewP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesCopyConstructorP", UtcDaliLineSeriesCopyConstructorP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesMoveConstructor", UtcDaliLineSeriesMoveConstructor, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesDownCastP", UtcDaliLineSeriesDownCastP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesDownCastN", UtcDaliLineSeriesDownCastN, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesSetLinePropertiesP", UtcDaliLineSeriesSetLinePropertiesP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesSetMarkerPropertiesP", UtcDaliLineSeriesSetMarkerPropertiesP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesFillP", UtcDaliLineSeriesFillP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesDataLabelsP", UtcDaliLineSeriesDataLabelsP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliLineSeriesSettersP", UtcDaliLineSeriesSettersP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliBarSeriesNewP", UtcDaliBarSeriesNewP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliBarSeriesDownCastP", UtcDaliBarSeriesDownCastP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliBarSeriesDownCastN", UtcDaliBarSeriesDownCastN, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliBarSeriesSetPropertiesP", UtcDaliBarSeriesSetPropertiesP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliBarSeriesDataLabelsP", UtcDaliBarSeriesDataLabelsP, utc_dali_chartseries_startup, utc_dali_chartseries_cleanup},
    {"UtcDaliPieSeriesConstructorP", UtcDaliPieSeriesConstructorP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesNewP", UtcDaliPieSeriesNewP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesCopyConstructorP", UtcDaliPieSeriesCopyConstructorP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesMoveConstructor", UtcDaliPieSeriesMoveConstructor, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesDownCastP", UtcDaliPieSeriesDownCastP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesDownCastN", UtcDaliPieSeriesDownCastN, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesAddSliceP", UtcDaliPieSeriesAddSliceP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesClearSlicesP", UtcDaliPieSeriesClearSlicesP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesInnerRadiusP", UtcDaliPieSeriesInnerRadiusP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesSliceGapP", UtcDaliPieSeriesSliceGapP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesDataLabelsP", UtcDaliPieSeriesDataLabelsP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliPieSeriesSettersP", UtcDaliPieSeriesSettersP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliScatterSeriesNewP", UtcDaliScatterSeriesNewP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliScatterSeriesDownCastP", UtcDaliScatterSeriesDownCastP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliScatterSeriesDownCastN", UtcDaliScatterSeriesDownCastN, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliScatterSeriesSetPropertiesP", UtcDaliScatterSeriesSetPropertiesP, utc_dali_pieseries_startup, utc_dali_pieseries_cleanup},
    {"UtcDaliChartAxisConstructorP", UtcDaliChartAxisConstructorP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisNewP", UtcDaliChartAxisNewP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisCopyConstructorP", UtcDaliChartAxisCopyConstructorP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisMoveConstructor", UtcDaliChartAxisMoveConstructor, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisDownCastP", UtcDaliChartAxisDownCastP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisDownCastN", UtcDaliChartAxisDownCastN, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetLabelsP", UtcDaliChartAxisSetLabelsP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetTitleP", UtcDaliChartAxisSetTitleP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetRangeLimitsP", UtcDaliChartAxisSetRangeLimitsP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetAutoRangeP", UtcDaliChartAxisSetAutoRangeP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetGridLinesP", UtcDaliChartAxisSetGridLinesP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetAxisLineP", UtcDaliChartAxisSetAxisLineP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetDataPaddingP", UtcDaliChartAxisSetDataPaddingP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSetLabelsRotationP", UtcDaliChartAxisSetLabelsRotationP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartAxisSettersP", UtcDaliChartAxisSettersP, utc_dali_chartaxis_startup, utc_dali_chartaxis_cleanup},
    {"UtcDaliChartSectionConstructorP", UtcDaliChartSectionConstructorP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionNewP", UtcDaliChartSectionNewP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionCopyConstructorP", UtcDaliChartSectionCopyConstructorP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionAssignmentOperatorP", UtcDaliChartSectionAssignmentOperatorP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionSetBoundsP", UtcDaliChartSectionSetBoundsP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionSetFillColorP", UtcDaliChartSectionSetFillColorP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionSetStrokeP", UtcDaliChartSectionSetStrokeP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {"UtcDaliChartSectionSettersP", UtcDaliChartSectionSettersP, utc_dali_chartsection_startup, utc_dali_chartsection_cleanup},
    {NULL, NULL}
};

#endif // TCT_DALI_UI_COMPONENTS_CORE_H
