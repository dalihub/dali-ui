# Bit-field Enum Rules

## Rule: Declare Bit-field Enum Representation Explicitly

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: C++ enum values stored in bit-fields

### Intent

The signedness, packing, and layout details of enum bit-fields are implementation-defined. In particular, an unscoped enum stored in a signed two-bit field cannot represent the non-negative value `2`: the bit pattern `0b10` is read as `-2` when interpreted as signed two's-complement.

This caused the Windows/MSVC image visual squircle shader selection to fall back to the normal image shader. `SQUIRCLE_CORNER` has value `2`, while the field storing it had width `2` and no explicitly unsigned representation.

### Preferred

- For non-negative state-selection enums stored in bit-fields, declare an explicitly unsigned fixed-width underlying type.

  ```cpp
  enum Type : uint32_t
  {
    DISABLED = 0,
    ROUNDED_CORNER,
    SQUIRCLE_CORNER,
  };

  Type mRoundedCorner : 2;
  ```

- For bitwise flags, use an explicitly typed scoped enum and unsigned flag constants.

  ```cpp
  enum class Flag : uint32_t
  {
    NONE   = 0u,
    OPTION = 1u << 0,
  };
  ```

- When adding or changing an enum value, verify that its value fits the bit-field width. For an `N`-bit field, unsigned values range from `0` to `2^N - 1`; signed two's-complement values range from `-2^(N-1)` to `2^(N-1) - 1`.

- Include `<cstdint>` directly when using fixed-width integer types.

### Avoid

- Do not store a non-negative enum value that uses the top bit of a signed bit-field. For example, do not store enum value `2` in a signed `: 2` field.
- Do not use unscoped, implicitly typed enums as bitwise flags.
- Do not use bit-fields for file formats, IPC, network payloads, or data that crosses platform/compiler ABI boundaries.

### Exceptions

- A signed bit-field is allowed only when negative values are intentional and both the signed underlying type and complete value range are documented.
- Existing ABI-sensitive types require an explicit layout review before changing a bit-field's declared type or underlying enum type.

### Validation

- Search for enum-typed bit-fields and compare each enum's largest stored value with its field width.
- Build affected code with every supported compiler, including MSVC and GCC.
- For ABI-sensitive classes, compare the relevant layout/size as part of the change review.
