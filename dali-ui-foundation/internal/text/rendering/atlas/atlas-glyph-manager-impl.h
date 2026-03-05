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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-glyph-manager.h>

namespace Dali
{
namespace UI
{
class AtlasGlyphManager;

} // namespace UI

namespace UI
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
    uint32_t mImageId;
    int32_t mCount;
    uint16_t mOutlineWidth;
    bool isItalic : 1;
    bool isBold : 1;
  };

  struct FontGlyphRecord
  {
    Text::FontId mFontId;
    Vector<GlyphRecordEntry> mGlyphRecords;
  };

  /**
   * @brief Constructor
   */
  AtlasGlyphManager();

  /**
   * @copydoc UI::AtlasGlyphManager::Add
   */
  void Add(const Text::GlyphInfo& glyph, const UI::AtlasGlyphManager::GlyphStyle& style, const PixelData& bitmap,
           Dali::UI::AtlasManager::AtlasSlot& slot);

  /**
   * @copydoc UI::AtlasGlyphManager::GenerateMeshData
   */
  void GenerateMeshData(uint32_t imageId, const Vector2& position, UI::AtlasManager::Mesh2D& mesh);

  /**
   * @copydoc UI::AtlasGlyphManager::IsCached
   */
  bool IsCached(Text::FontId fontId, Text::GlyphIndex index, const UI::AtlasGlyphManager::GlyphStyle& style,
                Dali::UI::AtlasManager::AtlasSlot& slot);

  /**
   * @copydoc UI::AtlasGlyphManager::GetAtlasSize
   */
  Vector2 GetAtlasSize(uint32_t atlasId);

  /**
   * @copydoc UI::AtlasGlyphManager::SetNewAtlasSize
   */
  void SetNewAtlasSize(uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight);

  /**
   * @copydoc UI::AtlasGlyphManager::GetPixelFormat
   */
  Pixel::Format GetPixelFormat(uint32_t atlasId);

  /**
   * @copydoc toolkit::AtlasGlyphManager::AdjustReferenceCount
   */
  void AdjustReferenceCount(Text::FontId fontId, Text::GlyphIndex index, const UI::AtlasGlyphManager::GlyphStyle& style,
                            int32_t delta);

  /**
   * @copydoc UI::AtlasGlyphManager::GetTextures
   */
  TextureSet GetTextures(uint32_t atlasId) const;

  /**
   * @copydoc UI::AtlasGlyphManager::GetMetrics
   */
  const UI::AtlasGlyphManager::Metrics& GetMetrics();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~AtlasGlyphManager();

private:
  Dali::UI::AtlasManager mAtlasManager; ///> Atlas Manager created by GlyphManager
  std::vector<FontGlyphRecord> mFontGlyphRecords;
  UI::AtlasGlyphManager::Metrics mMetrics; ///> Metrics to pass back on GlyphManager status
  Sampler mSampler;
};

} // namespace Internal

inline const Internal::AtlasGlyphManager& GetImplementation(const UI::AtlasGlyphManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasGlyphManager handle is empty");

  const BaseObject& handle = manager.GetBaseObject();

  return static_cast<const Internal::AtlasGlyphManager&>(handle);
}

inline Internal::AtlasGlyphManager& GetImplementation(UI::AtlasGlyphManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasGlyphManager handle is empty");

  BaseObject& handle = manager.GetBaseObject();

  return static_cast<Internal::AtlasGlyphManager&>(handle);
}

} // namespace UI

} // namespace Dali

#endif // DALI_UI_ATLAS_GLYPH_MANAGER_IMPL_H
