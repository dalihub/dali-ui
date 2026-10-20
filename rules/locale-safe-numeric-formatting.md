# Locale-Safe Numeric Formatting and Parsing

## Rule: Keep Numeric Conversion Independent of the Global Locale

- Status: required
- Scope: public-api | extension-api | integration-api | internal
- Applies To: Production/library code that formats or parses numeric values

### Intent

Numeric formatting and parsing must not depend on a process-wide or thread-wide
locale that may be changed by the application or another library. DALi library
code must not change the global locale itself. In particular,
`Dali::LocaleNumericGuard` uses `setlocale`, which is not thread-safe and can
temporarily affect unrelated code.

### Preferred

- Prefer an existing DALi locale-independent conversion utility when one
  provides the required behavior.
- For numeric text formatting, use a stream imbued with the classic locale:

  ```cpp
  std::ostringstream output;
  output.imbue(std::locale::classic());
  output << std::fixed << std::setprecision(2) << value;
  ```

- For numeric text parsing, use an `std::istringstream` imbued with the classic
  locale, or a supported locale-independent parser such as `std::from_chars`
  when its available overloads and required parsing semantics are suitable.
- Preserve the existing output/input contract when replacing an API: precision,
  fixed versus significant digits, rounding, accepted syntax, consumed input,
  error handling, and truncation behavior.
- If a locale-sensitive API cannot yet be replaced, keep
  `Dali::LocaleNumericGuard` to the smallest possible scope and add a comment
  explaining why replacement is not currently possible and what should change.
- When a guarded function may be called concurrently from multiple threads,
  add an explicit warning comment at the guard explaining that
  `LocaleNumericGuard` changes the global C locale and is not thread-safe.
  Prefer redesigning the API rather than retaining a guard on such a path.

### Avoid

- Locale-sensitive floating-point conversions such as `std::to_string(float)`
  or `std::to_string(double)`, `std::stof`, `std::stod`, `std::atof`,
  `std::strtof`, and `std::strtod` when their locale-dependent behavior is not
  explicitly intended.
- Floating-point `printf`-family formatting such as `snprintf`, `sprintf`,
  `fprintf`, or their `v*` variants for machine-stable numeric text.
- Formatted streams that rely on their default locale. Set
  `std::locale::classic()` explicitly on each stream used for stable numeric
  text.
- Adding or expanding `Dali::LocaleNumericGuard` as a broad workaround,
  especially around an entire function or a path reachable from multiple
  threads.
- Calling `setlocale` from DALi library code.

### Exceptions

- Numeric text exchanged with an external API or format may need to follow
  that interface's numeric conventions rather than an internal, locale-neutral
  representation. Do not force the classic locale when doing so would make the
  text incompatible with values produced or consumed by the external side.
  Document the required formatting contract and ensure the corresponding
  parser uses compatible rules. Do not rely on the ambient locale by default;
  use it only when the external contract explicitly requires it and the
  resulting thread-safety implications are acceptable. Whenever locale
  conversion is intentionally omitted for this reason, add a code comment at
  the conversion explaining why the classic locale must not be applied and
  identifying the external format or contract that requires this behavior.
- A printf-style variadic API may not be convertible to streams without changing
  its call contract. Keep its guard local, document the API limitation, and
  leave a clear follow-up note to replace or redesign the variadic interface.
- Locale-sensitive formatting is acceptable when localized user-facing output
  is an intentional requirement and the locale source and thread-safety
  guarantees are explicit.
- Tests, samples, and manual-test code may use locale-sensitive APIs to exercise
  or demonstrate behavior; do not change them solely to satisfy this rule.

### Validation

- Search production sources for `LocaleNumericGuard`, `setlocale`,
  `std::to_string` with floating-point arguments, `stof`/`stod`/`atof`,
  `strtof`/`strtod`, and printf-family numeric formatting.
- For each remaining `LocaleNumericGuard`, verify that it is narrowly scoped,
  its reason is documented, and any potentially concurrent call path has the
  required thread-safety warning.
- Build affected targets and run relevant tests. Where practical, validate
  stable output and parsing with a non-classic global locale.
