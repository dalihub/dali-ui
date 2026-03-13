#ifndef DALI_UI_ATLAS_MANAGER_IMPL_H
#define DALI_UI_ATLAS_MANAGER_IMPL_H

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
#include <dali-ui-foundation/internal/text/rendering/atlas/atlas-manager.h>

namespace Dali
{
namespace Ui
{
class AtlasManager;

} // namespace Ui

namespace Ui
{
namespace Internal
{
typedef Dali::Vector<Ui::AtlasManager::AtlasSlot> slotContainer;

class AtlasManager;
typedef IntrusivePtr<AtlasManager> AtlasManagerPtr;

class AtlasManager : public Dali::BaseObject
{
public:
  typedef uint32_t SizeType;
  typedef SizeType AtlasId;
  typedef SizeType ImageId;

  /**
   * @brief Internal storage of atlas attributes and image upload results
   */
  struct AtlasDescriptor
  {
    Dali::Texture               mAtlas;           // atlas image
    Ui::AtlasManager::AtlasSize mSize;            // size of atlas
    Pixel::Format               mPixelFormat;     // pixel format used by atlas
    PixelData                   mHorizontalStrip; // Image used to pad upload
    PixelData                   mVerticalStrip;   // Image used to pad upload
    TextureSet                  mTextureSet;      // Texture set used for atlas texture
    SizeType                    mTotalBlocks;     // total number of blocks in atlas
    SizeType                    mAvailableBlocks; // number of blocks available in atlas
    Dali::Vector<SizeType>      mFreeBlocksList;  // unless there are any previously freed blocks
  };

  struct AtlasSlotDescriptor
  {
    SizeType mCount;       // Reference count for this slot
    SizeType mImageWidth;  // Width of image stored
    SizeType mImageHeight; // Height of image stored
    AtlasId  mAtlasId;     // Image is stored in this Atlas
    SizeType mBlock;       // Block within atlas used for image
  };

  AtlasManager();

  /**
   * Create a new AtlasManager
   */
  static AtlasManagerPtr New();

  virtual ~AtlasManager();

  /**
   * @copydoc: Ui::AtlasManager::CreateAtlas
   */
  AtlasId CreateAtlas(const Ui::AtlasManager::AtlasSize& size, Pixel::Format pixelformat);

  /**
   * @copydoc Ui::AtlasManager::SetAddPolicy
   */
  void SetAddPolicy(Ui::AtlasManager::AddFailPolicy policy);

  /**
   * @copydoc Ui::AtlasManager::Add
   */
  bool Add(const PixelData& image, Ui::AtlasManager::AtlasSlot& slot, Ui::AtlasManager::AtlasId atlas);

  /**
   * @copydoc Ui::AtlasManager::GenerateMeshData
   */
  void GenerateMeshData(ImageId id, const Vector2& position, Ui::AtlasManager::Mesh2D& mesh, bool addReference);

  /**
   * @copydoc Ui::AtlasManager::Remove
   */
  bool Remove(ImageId id);

  /**
   * @copydoc Ui::AtlasManager::GetAtlasContainer
   */
  Dali::Texture GetAtlasContainer(AtlasId atlas) const;

  /**
   * @copydoc Ui::AtlasManager::GetAtlas
   */
  AtlasId GetAtlas(ImageId id) const;

  /**
   * @copydoc Ui::AtlasManager::SetNewAtlasSize
   */
  void SetNewAtlasSize(const Ui::AtlasManager::AtlasSize& size);

  /**
   * @copydoc Ui::AtlasManager::GetAtlasSize
   */
  const Ui::AtlasManager::AtlasSize& GetAtlasSize(AtlasId atlas);

  /**
   * @copydoc Ui::AtlasManager::GetBlockSize
   */
  Vector2 GetBlockSize(AtlasId atlas);

  /**
   * @copydoc Ui::AtlasManager::GetFreeBlocks
   */
  SizeType GetFreeBlocks(AtlasId atlas) const;

  /*
   * @copydoc Ui::AtlasManager::GetAtlasCount
   */
  SizeType GetAtlasCount() const;

  /**
   * @copydoc Ui::AtlasManager::GetPixelFormat
   */
  Pixel::Format GetPixelFormat(AtlasId atlas) const;

  /**
   * @copydoc Ui::AtlasManager::GetMetrics
   */
  void GetMetrics(Ui::AtlasManager::Metrics& metrics);

  /**
   * @copydoc Ui::AtlasManager::GetTextures
   */
  TextureSet GetTextures(AtlasId atlas) const;

  /**
   * @copydoc Ui::AtlasManager::SetTextures
   */
  void SetTextures(AtlasId atlas, TextureSet& textureSet);

private:
  std::vector<AtlasDescriptor>    mAtlasList;     // List of atlases created
  Vector<AtlasSlotDescriptor>     mImageList;     // List of bitmaps stored in atlases
  Ui::AtlasManager::AtlasSize     mNewAtlasSize;  // Atlas size to use in next creation
  Ui::AtlasManager::AddFailPolicy mAddFailPolicy; // Policy for failing to add an Image

  SizeType CheckAtlas(SizeType atlas, SizeType width, SizeType height, Pixel::Format pixelFormat);

  void UploadImage(const PixelData& image, const AtlasSlotDescriptor& desc);
};

} // namespace Internal

inline const Internal::AtlasManager& GetImplementation(const Ui::AtlasManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasManager handle is empty");

  const BaseObject& handle = manager.GetBaseObject();

  return static_cast<const Internal::AtlasManager&>(handle);
}

inline Internal::AtlasManager& GetImplementation(Ui::AtlasManager& manager)
{
  DALI_ASSERT_ALWAYS(manager && "AtlasManager handle is empty");

  BaseObject& handle = manager.GetBaseObject();

  return static_cast<Internal::AtlasManager&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_ATLAS_MANAGER_IMPL_H
