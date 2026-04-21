#ifndef DALI_UI_TEXT_ENUMERATION_IMPL_H
#define DALI_UI_TEXT_ENUMERATION_IMPL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-value.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Get the alignment from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] alignment The resulting alignment from the given source
 * @return true if the resulting alignment has been updated
 */
bool GetHorizontalAlignmentEnumeration(const Property::Value& propertyValue,
                                       Alignment&             alignment);

/**
 * @brief Get the alignment from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] alignment The resulting alignment from the given source
 * @return true if the resulting alignment has been updated
 */
bool GetVerticalAlignmentEnumeration(const Property::Value& propertyValue,
                                     Alignment&             alignment);

/**
 * @brief Get the overflow mode from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] overflowMode The resulting OverflowMode from the given source
 * @return true if the resulting overflowMode has been updated
 */
bool GetOverflowModeEnumeration(const Property::Value& propertyValue,
                                OverflowMode&          overflowMode);

/**
 * @brief Get the line-wrap-mode from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] alignment The resulting lineWrapMode from the given source
 * @return true if the resulting lineWrapMode has been updated
 */
bool GetLineWrapModeEnumeration(const Property::Value& propertyValue, LineWrapMode& lineWrapMode);

/**
 * @brief Get the alignment string from the provided alignment string.
 * @param[in] alignment the Text::HORIZONTAL enum source
 * @return the string equivalent
 */
const char* GetHorizontalAlignmentString(const Alignment& alignment);

/**
 * @brief Get the alignment string from the provided alignment string.
 * @param[in] alignment the Text::VerticalAlignment enum source
 * @return the string equivalent
 */
const char* GetVerticalAlignmentString(const Alignment& alignment);

/**
 * @brief Get the ellipsis-location-type from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] ellipsisPositionType The resulting ellipsisPositionType from the given source
 * @return true if the resulting ellipsisPositionType has been updated
 */
bool GetEllipsisPositionTypeEnumeration(const Property::Value&  propertyValue,
                                        EllipsisPosition::Type& ellipsisPositionType);

/**
 * @brief Get the line height mode from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] lineHeightMode The resulting LineHeightMode from the given source
 * @return true if the resulting lineHeightMode has been updated
 */
bool GetLineHeightModeEnumeration(const Property::Value& propertyValue,
                                  LineHeightMode&        lineHeightMode);

/**
 * @brief Get the layout direction mode from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] layoutDirectionMode The resulting LayoutDirectionMode from the given source
 * @return true if the resulting layoutDirectionMode has been updated
 */
bool GetLayoutDirectionModeEnumeration(const Property::Value& propertyValue,
                                       LayoutDirectionMode&   layoutDirectionMode);

/**
 * @brief Get the marquee trigger policy from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] marqueeTriggerPolicy The resulting MarqueeTriggerPolicy from the given source
 * @return true if the resulting marqueeTriggerPolicy has been updated
 */
bool GetMarqueeTriggerPolicyEnumeration(const Property::Value& propertyValue,
                                        MarqueeTriggerPolicy&  marqueeTriggerPolicy);

/**
 * @brief Get the marquee stop mode from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] marqueeStopMode The resulting MarqueeStopMode from the given source
 * @return true if the resulting marqueeStopMode has been updated
 */
bool GetMarqueeStopModeEnumeration(const Property::Value& propertyValue,
                                   MarqueeStopMode&       marqueeStopMode);

/**
 * @brief Get the marquee orientation from the provided property value.
 * @param[in] propertyValue The source value (which can be a Property::INTEGER or Property::STRING type)
 * @param[out] marqueeOrientation The resulting MarqueeOrientation from the given source
 * @return true if the resulting marqueeOrientation has been updated
 */
bool GetMarqueeOrientationEnumeration(const Property::Value& propertyValue,
                                      MarqueeOrientation&    marqueeOrientation);

/**
 * @brief Converts the underline type to string format.
 * @param[in] type the Ui::Text::Underline::Type enum source
 * @return the string equivalent
 */
const char* GetUnderlineTypeToString(const Underline::Type& type);

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_ENUMERATION_IMPL_H
