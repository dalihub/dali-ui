#ifndef DALI_UI_ATLAS_GLYPH_MANAGER_IMPL_H
#define DALI_UI_ATLAS_GLYPH_MANAGER_IMPL_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-glyph-manager.h>

namespace Dali
{
namespace Ui
{
class AtlasGlyphManager;

} // namespace Ui

namespace Ui
{
namespace Internal
{
class AtlasGlyphManager;
typedef IntrusivePtr<AtlasGlyphManager> AtlasGlyphManagerPtr;

class AtlasGlyphManager : public Dali::BaseObject
{
public:
  struct GlyphRecordEntry
  {
    Text::GlyphIndex mIndex;
    uint32_t         mImageId;
    int32_t          mCount;
    uint16_t         mOutlineWidth;
    bool             isItalic : 1;
    bool             isBold : 1;
  };

  struct FontGlyphRecord
  {
    Text::FontId             mFontId;
    Vector<GlyphRecordEntry> mGlyphRecords;
  };

  /**
   * @brief Constructor
   */
  AtlasGlyphManager();

  /**
   * @copydoc Ui::AtlasGlyphManager::Add
   */
  void Add(const Text::GlyphInfo& glyph, const Ui::AtlasGlyphManager::GlyphStyle& style, const PixelData& bitmap,
           Dali::Ui::AtlasManager::AtlasSlot& slot);

  /**
   * @copydoc Ui::AtlasGlyphManager::GenerateMeshData
   */
  void GenerateMeshData(uint32_t imageId, const Vector2& position, Ui::AtlasManager::Mesh2D& mesh);

  /**
   * @copydoc Ui::AtlasGlyphManager::IsCached
   */
  bool IsCached(Text::FontId fontId, Text::GlyphIndex index, const Ui::AtlasGlyphManager::GlyphStyle& style,
                Dali::Ui::AtlasManager::AtlasSlot& slot);

  /**
   * @copydoc Ui::AtlasGlyphManager::GetAtlasSize
   */
  Vector2 GetAtlasSize(uint32_t atlasId);

  /**
   * @copydoc Ui::AtlasGlyphManager::SetNewAtlasSize
   */
  void SetNewAtlasSize(uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight);

  /**
   * @copydoc Ui::AtlasGlyphManager::GetPixelFormat
   */
  Pixel::Format GetPixelFormat(uint32_t atlasId);

  /**
   * @copydoc Ui::AtlasGlyphManager::AdjustReferenceCount
   */
  void AdjustReferenceCount(Text::FontId fontId, Text::GlyphIndex index, const Ui::AtlasGlyphManager::GlyphStyle& style,
                            int32_t delta);

  /**
   * @copydoc Ui::AtlasGlyphManager::GetTextures
   */
  TextureSet GetTextures(uint32_t atlasId) const;

  /**
   * @copydoc Ui::AtlasGlyphManager::GetMetrics
   */
  const Ui::AtlasGlyphManager::Metrics& GetMetrics();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~AtlasGlyphManager();

private:
  Dali::Ui::AtlasManager         mAtlasManager; ///> Atlas Manager created by GlyphManager
  std::vector<FontGlyphRecord>   mFontGlyphRecords;
  Ui::AtlasGlyphManager::Metrics mMetrics; ///> Metrics to pass back on GlyphManager status
  Sampler                        mSampler;
};

} // namespace Internal

inline const Internal::AtlasGlyphManager& GetImplementation(const Ui::AtlasGlyphManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasGlyphManager handle is empty");

  const BaseObject& handle = manager.GetBaseObject();

  return static_cast<const Internal::AtlasGlyphManager&>(handle);
}

inline Internal::AtlasGlyphManager& GetImplementation(Ui::AtlasGlyphManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasGlyphManager handle is empty");

  BaseObject& handle = manager.GetBaseObject();

  return static_cast<Internal::AtlasGlyphManager&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_ATLAS_GLYPH_MANAGER_IMPL_H
