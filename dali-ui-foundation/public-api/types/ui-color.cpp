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

// CLASS HEADER
#include <dali-ui-foundation/public-api/types/ui-color.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/math/math-utils.h>
#include <algorithm>
#include <cstring>
#include <deque>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/ui-color-cache.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>

namespace Dali
{
namespace Ui
{

const UiColor UiColor::PRIMARY("Primary");
const UiColor UiColor::ON_PRIMARY("OnPrimary");
const UiColor UiColor::BACKGROUND("Background");
const UiColor UiColor::SURFACE("Surface");
const UiColor UiColor::ON_SURFACE("OnSurface");
const UiColor UiColor::OUTLINE("Outline");
const UiColor UiColor::SHADOW("Shadow");
const UiColor UiColor::BLUR_SURFACE("BlurSurface");

namespace
{

using TokenId = uint32_t;

/**
 * @brief Registry for mapping between string color names and compact token IDs.
 *
 * Maintains bidirectional mapping:
 *   - string color name  -> tokenId  (nameToId)
 *   - tokenId            -> string*  (idToName, deque for pointer stability)
 *
 * tokenId 0 is reserved as "invalid / not set".
 * UiColor is only used on the UI thread, so no synchronization is needed.
 */
struct TokenRegistry
{
  std::unordered_map<String, TokenId> nameToId;
  std::deque<String>                  idToName;

  TokenId GetOrCreateId(const String& name)
  {
    auto it = nameToId.find(name);
    if(it != nameToId.end())
    {
      return it->second;
    }
    TokenId id = static_cast<TokenId>(idToName.size() + 1u);
    idToName.push_back(name);
    nameToId.emplace(name, id);

    return id;
  }

  TokenId GetOrCreateId(String&& name)
  {
    auto it = nameToId.find(name);
    if(it != nameToId.end())
    {
      return it->second;
    }
    TokenId id = static_cast<TokenId>(idToName.size() + 1u);
    nameToId.emplace(name, id);
    idToName.push_back(std::move(name));

    return id;
  }

  const String* TryGetName(TokenId id) const
  {
    if(id == 0u)
    {
      return nullptr;
    }
    const size_t idx = static_cast<size_t>(id - 1u);
    if(idx >= idToName.size())
    {
      return nullptr;
    }
    return &idToName[idx];
  }
};

TokenRegistry& GetTokenRegistry()
{
  static TokenRegistry registry;
  return registry;
}

/**
 * mData[24] layout (alignas(8))
 * ┌──────────┬───────────┬──────────┬─────────────────────────────────┐
 * │ [0]      │ [1]       │ [2-7]    │ [8 .. 23]  (payload)            │
 * │ Type     │ AlphaMode │ reserved │  Rgba : r g b a  (4 floats)     │
 * │          │           │          │  Token: tokenId(u32) alpha(f32) │
 * └──────────┴───────────┴──────────┴─────────────────────────────────┘
 */
constexpr size_t OFFSET_TYPE       = 0;
constexpr size_t OFFSET_ALPHA_MODE = 1;
constexpr size_t OFFSET_PAYLOAD    = 8;

// Rgba payload offsets (from mData base)
constexpr size_t OFFSET_RGBA_R = OFFSET_PAYLOAD;
constexpr size_t OFFSET_RGBA_G = OFFSET_PAYLOAD + sizeof(float);
constexpr size_t OFFSET_RGBA_B = OFFSET_PAYLOAD + sizeof(float) * 2;
constexpr size_t OFFSET_RGBA_A = OFFSET_PAYLOAD + sizeof(float) * 3;

// Token payload offsets (from mData base)
constexpr size_t OFFSET_TOKEN_ID    = OFFSET_PAYLOAD;
constexpr size_t OFFSET_TOKEN_ALPHA = OFFSET_PAYLOAD + sizeof(uint32_t);

static_assert(sizeof(float) == 4, "UiColor layout assumes 32-bit IEEE 754 float");
static_assert(sizeof(UiColor) == 24, "UiColor must be exactly 24 bytes for ABI stability");

inline float ReadFloat(const uint8_t* data, size_t offset)
{
  float value;
  std::memcpy(&value, data + offset, sizeof(float));
  return value;
}

inline void WriteFloat(uint8_t* data, size_t offset, float value)
{
  std::memcpy(data + offset, &value, sizeof(float));
}

inline uint32_t ReadU32(const uint8_t* data, size_t offset)
{
  uint32_t v;
  std::memcpy(&v, data + offset, sizeof(uint32_t));
  return v;
}

inline void WriteU32(uint8_t* data, size_t offset, uint32_t v)
{
  std::memcpy(data + offset, &v, sizeof(uint32_t));
}

} // namespace

UiColor::UiColor()
{
  std::memset(mData, 0, sizeof(mData));
}

UiColor::UiColor(float r, float g, float b, float a)
{
  std::memset(mData, 0, sizeof(mData));
  SetType(Type::Rgba);
  SetAlphaMode(AlphaMode::Multiply);
  SetRgba(Vector4(r, g, b, a));
}

UiColor::UiColor(const Vector4& color)
{
  std::memset(mData, 0, sizeof(mData));
  SetType(Type::Rgba);
  SetAlphaMode(AlphaMode::Multiply);
  SetRgba(color);
}

UiColor::UiColor(const String& colorId)
{
  std::memset(mData, 0, sizeof(mData));
  SetType(Type::Token);
  SetAlphaMode(AlphaMode::With);

  TokenRegistry& registry = GetTokenRegistry();
  SetTokenId(registry.GetOrCreateId(colorId));
  SetTokenAlpha(1.0f);
}

UiColor::UiColor(String&& colorId)
{
  std::memset(mData, 0, sizeof(mData));
  SetType(Type::Token);
  SetAlphaMode(AlphaMode::With);

  TokenRegistry& registry = GetTokenRegistry();
  SetTokenId(registry.GetOrCreateId(std::move(colorId)));
  SetTokenAlpha(1.0f);
}

UiColor::UiColor(uint32_t rgb, float a)
{
  std::memset(mData, 0, sizeof(mData));
  SetType(Type::Rgba);
  SetAlphaMode(AlphaMode::Multiply);
  SetRgba(Vector4(((rgb >> 16) & 0xFF) / 255.0f,
                  ((rgb >> 8) & 0xFF) / 255.0f,
                  (rgb & 0xFF) / 255.0f,
                  a));
}

bool UiColor::operator==(const UiColor& rhs) const
{
  if(GetType() != rhs.GetType())
  {
    return false;
  }

  if(GetType() == Type::Rgba)
  {
    const Vector4 lhsColor = ReadRgbaPayload();
    const Vector4 rhsColor = rhs.ReadRgbaPayload();

    return Dali::Equals(lhsColor.r, rhsColor.r) &&
           Dali::Equals(lhsColor.g, rhsColor.g) &&
           Dali::Equals(lhsColor.b, rhsColor.b) &&
           Dali::Equals(lhsColor.a, rhsColor.a);
  }

  return GetTokenId() == rhs.GetTokenId() &&
         GetAlphaMode() == rhs.GetAlphaMode() &&
         Dali::Equals(GetTokenAlpha(), rhs.GetTokenAlpha());
}

bool UiColor::operator!=(const UiColor& rhs) const
{
  return !(*this == rhs);
}

bool UiColor::HasColorId() const
{
  return GetType() == Type::Token;
}

String UiColor::GetColorId() const
{
  if(GetType() != Type::Token)
  {
    return {};
  }

  TokenRegistry& registry = GetTokenRegistry();
  const String*  name     = registry.TryGetName(static_cast<TokenId>(GetTokenId()));
  return name ? *name : String{};
}

Vector4 UiColor::GetRgba() const
{
  if(GetType() == Type::Token)
  {
    const uint32_t          tokenId = GetTokenId();
    Internal::UiColorCache& cache   = Internal::UiColorCache::Get();

    // Cache hit — return cached base color with alpha applied
    Vector4 resolved;
    if(cache.TryGet(tokenId, resolved))
    {
      const float tokenAlpha = std::clamp(GetTokenAlpha(), 0.0f, 1.0f);
      resolved.a             = std::clamp((GetAlphaMode() == AlphaMode::Multiply) ? resolved.a * tokenAlpha : tokenAlpha, 0.0f, 1.0f);
      return resolved;
    }

    // Cache miss — full lookup via UiColorManager
    const String* namePtr = GetTokenRegistry().TryGetName(tokenId);
    if(namePtr)
    {
      if(UiColorManager::Get().GetColor(*namePtr, resolved))
      {
        cache.Store(tokenId, resolved);

        const float tokenAlpha = std::clamp(GetTokenAlpha(), 0.0f, 1.0f);
        resolved.a             = std::clamp((GetAlphaMode() == AlphaMode::Multiply) ? resolved.a * tokenAlpha : tokenAlpha, 0.0f, 1.0f);
        return resolved;
      }
    }
    return Vector4::ZERO;
  }
  return ReadRgbaPayload();
}

UiColor::operator Vector4() const
{
  return GetRgba();
}

UiColor UiColor::ScaleAlpha(float factor) const
{
  UiColor out(*this);
  if(GetType() == Type::Rgba)
  {
    Vector4 c = ReadRgbaPayload();
    c.a *= factor;
    c.a = std::clamp(c.a, 0.0f, 1.0f);
    out.SetRgba(c);
  }
  else
  {
    if(GetAlphaMode() == AlphaMode::With)
    {
      out.SetTokenAlpha(std::clamp(GetTokenAlpha() * factor, 0.0f, 1.0f));
    }
    else
    {
      out.SetAlphaMode(AlphaMode::Multiply);
      out.SetTokenAlpha(std::clamp(GetTokenAlpha() * factor, 0.0f, 1.0f));
    }
  }
  return out;
}

UiColor UiColor::WithAlpha(float alpha) const
{
  UiColor out(*this);
  alpha = std::clamp(alpha, 0.0f, 1.0f);
  if(GetType() == Type::Rgba)
  {
    Vector4 c = ReadRgbaPayload();
    c.a       = alpha;
    out.SetRgba(c);
  }
  else
  {
    out.SetAlphaMode(AlphaMode::With);
    out.SetTokenAlpha(alpha);
  }
  return out;
}

UiColor::Type UiColor::GetType() const
{
  return static_cast<Type>(mData[OFFSET_TYPE]);
}

void UiColor::SetType(Type type)
{
  mData[OFFSET_TYPE] = static_cast<uint8_t>(type);
}

UiColor::AlphaMode UiColor::GetAlphaMode() const
{
  return static_cast<AlphaMode>(mData[OFFSET_ALPHA_MODE]);
}

void UiColor::SetAlphaMode(AlphaMode mode)
{
  mData[OFFSET_ALPHA_MODE] = static_cast<uint8_t>(mode);
}

Vector4 UiColor::ReadRgbaPayload() const
{
  return Vector4(ReadFloat(mData, OFFSET_RGBA_R),
                 ReadFloat(mData, OFFSET_RGBA_G),
                 ReadFloat(mData, OFFSET_RGBA_B),
                 ReadFloat(mData, OFFSET_RGBA_A));
}

void UiColor::SetRgba(const Vector4& rgba)
{
  WriteFloat(mData, OFFSET_RGBA_R, rgba.r);
  WriteFloat(mData, OFFSET_RGBA_G, rgba.g);
  WriteFloat(mData, OFFSET_RGBA_B, rgba.b);
  WriteFloat(mData, OFFSET_RGBA_A, rgba.a);
}

uint32_t UiColor::GetTokenId() const
{
  return ReadU32(mData, OFFSET_TOKEN_ID);
}

void UiColor::SetTokenId(uint32_t tokenId)
{
  WriteU32(mData, OFFSET_TOKEN_ID, tokenId);
}

float UiColor::GetTokenAlpha() const
{
  return ReadFloat(mData, OFFSET_TOKEN_ALPHA);
}

void UiColor::SetTokenAlpha(float alpha)
{
  WriteFloat(mData, OFFSET_TOKEN_ALPHA, alpha);
}

} // namespace Ui
} // namespace Dali
