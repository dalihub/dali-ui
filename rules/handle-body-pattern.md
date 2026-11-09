# Handle-Body Pattern Rules

## Rule: Public Handles Stay Lightweight

- Status: required
- Scope: public-api, extension-api, integration-api, internal
- Applies To: Public handle classes and implementation classes

### Intent

Public API classes should remain lightweight handles. State and behavior live in
implementation objects.

`extension-api` is not limited to storing the body type for a public handle. It
also contains ABI-stable interfaces, implementation base classes, and helper
APIs that module providers and `dali-ui-components` may depend on. Those
extension contracts must remain usable without depending on `integration-api`
or `internal` headers.

### Preferred

Typical layout:

```text
public-api/foo.h
extension-api/foo-impl.h
internal/foo-impl.cpp
```

or, when the implementation is integration-facing:

```text
public-api/foo.h
integration-api/foo-impl.h
internal/foo-impl.cpp
```

An `extension-api` header may instead define a provider-facing interface,
implementation base, or helper API with no corresponding public handle. Keep
such declarations focused on the stable extension contract and keep private
implementation details under `internal`.

### Avoid

- Adding data members to public handles.
- Adding logic-heavy public API methods that bypass the implementation layer.
- Duplicating state between handle and impl.
- Treating every `extension-api` declaration as if it were only an internal
  public-handle body.
- Exposing internal implementation details through an extension interface,
  implementation base, or helper API.

- A `BaseObject` implementation may provide `NewForBaseHandle()` solely for
  immediate construction of a `BaseHandle`-derived public handle. Return a
  freshly allocated raw object with reference count zero; the handle constructor
  must acquire its first reference. Keep ordinary `New()` returning IntrusivePtr
  for native owners. Do not obtain the raw result by detaching an owning pointer.
  Document this restriction on every raw factory. Mark declarations in devel-api
  or integration-api with `DALI_INTERNAL`; internal declarations may use the
  same visibility restriction.
- If initialization can create handles or invoke callbacks, perform it only
  after the handle has acquired the first reference. Keep the initialization
  sequence equivalent in the owning `New()` path. Raw factories must not expose
  an unreferenced object to callbacks before that ownership transfer.

- Existing bindings that transfer a refcount-0 implementation through a fixed
  external ABI may use a dedicated `CreateForBinding()` compatibility factory.
  Keep its constructors and destructors protected and offer an owning typed
  `New()` for native use. Preserve the external ownership protocol; detaching
  an IntrusivePtr would transfer a refcount-1 object and change that protocol.

### Validation

Future validation can inspect public handle headers for data members, check
that setters/getters forward into an implementation class, and review exported
`extension-api` interfaces, implementation bases, and helpers for accidental
dependencies on `integration-api` or `internal` details.

## Rule: RefObject Implementations Own Their Lifetime

- Status: required
- Scope: public-api, extension-api, integration-api, devel-api, internal
- Applies To: Classes that directly or indirectly inherit `Dali::RefObject`

### Intent

A reference-counted implementation has object identity and must be released
through `Unreference()`. Public construction or destruction makes it possible
to bypass that lifetime, and implicit copying can duplicate implementation
state without a valid ownership contract. Public handle classes are not subject
to this restriction: copying a handle shares its implementation.

### Preferred

- Declare implementation constructors and destructors `protected`. Existing
  `private` lifetime restrictions may remain when subclass construction is not
  supported. Do not leave an implicit public default constructor or destructor.
- Create concrete implementations through a static factory returning
  `Dali::IntrusivePtr<T>`. Hold that owner throughout initialization and transfer
  ownership to public handles before releasing it. An abstract base does not
  need an instantiating factory.
- Disable copy construction and copy assignment unless a documented copy
  contract is required. Direct `RefObject` subclasses must explicitly declare
  their copy constructor and copy assignment operator `= delete`; classes whose
  noncopyable base already deletes those operations remain noncopyable.
  `RefObject`'s protected copy operations do not make derived classes noncopyable.

```cpp
class Implementation : public Dali::RefObject
{
public:
  static Dali::IntrusivePtr<Implementation> New()
  {
    return Dali::IntrusivePtr<Implementation>(new Implementation());
  }

protected:
  Implementation() = default;
  ~Implementation() override = default;

private:
  Implementation(const Implementation&)            = delete;
  Implementation& operator=(const Implementation&) = delete;
};
```

### Avoid

- Stack allocation, public destructors, and direct `delete` of implementations.
- Owning a `RefObject` implementation with `std::unique_ptr` or `Dali::UniquePtr`;
  use `IntrusivePtr` so the last reference also runs `OnDestroy()`.
- Returning a newly allocated implementation as a raw pointer outside the
  restricted `NewForBaseHandle()` transfer described below.
- Calling `Implementation::New().Get()` to hand off a factory result. Pass or
  move the `IntrusivePtr` directly when the receiver owns a reference. Do not
  extract a raw pointer just to construct another `IntrusivePtr`; that adds
  avoidable reference-count operations. Report existing raw-only API boundaries
  for review instead of adding public constructor overloads implicitly.
- Defaulting copy operations simply because the reference counter itself can
  be copied. Sharing should copy a handle, not the implementation object.

### Exceptions

- `Internal::ThreadLocalStorage` is owned explicitly by `Core`, which calls
  `Reference()` after direct construction and `Unreference()` during teardown.
  Its constructor and destructor must remain public. Preserve that manual
  ownership protocol and do not require an intrusive factory for this class.
- `NoneStateEffectImpl` in `public-api/views/effects/state-effect.cpp` is a
  sentinel constructed directly by `StateEffect::None()`. Its default
  constructor must stay public. Its destructor remains protected, and this
  exception does not authorize copying the object.
- Immutable style builders and `OverlayEffectImpl` deliberately copy authored
  settings into a fresh implementation. Keep these explicit copy constructors
  protected, construct the reference-counted base afresh, and expose the copy
  through an owning factory. Do not copy reference counts or attached runtime
  state. Copy assignment remains disabled through their noncopyable bases.
- Existing static factories returning public handles retain that contract.
  Keep the implementation owned while constructing and initializing its handle.

- Existing bindings that transfer a refcount-0 implementation through a fixed
  external ABI may use a dedicated `CreateForBinding()` compatibility factory.
  Keep its constructors and destructors protected and offer an owning typed
  `New()` for native use. Preserve the external ownership protocol; detaching
  an IntrusivePtr would transfer a refcount-1 object and change that protocol.

### Validation

Inspect constructor and destructor access, including compiler-generated
members, for each direct and indirect `RefObject` subclass. Verify explicit
`= delete` declarations for direct subclasses and document intentional copy
constructors. Inspect factory results, raw variables, and handle construction
for temporary-owner lifetime errors. Check declarations in their namespace so
that similarly named handle classes are not mistaken for implementations.

### Declaration Order

For new implementation classes, group declarations in this order: public static
APIs, public APIs, protected static APIs, protected APIs, private static APIs,
private APIs, then member variables. Keep constructors and destructors together
in the protected API section.

For existing classes, limit ownership changes to the affected declarations.
Preserve the positions of unrelated APIs, nested types, aliases and forward
declarations. In particular, keep private enums and implementation declarations
near their original private APIs or member variables; do not hoist them merely
to satisfy the grouping above. Move a type only when an API actually requires
its prior declaration. Preserve existing Doxygen group boundaries, conditional
platform declarations, blank lines, alignment and formatting. Preserve the
relative order of virtual functions and data members to retain the ABI.
Preserve repeated access specifiers that separate API groups or member variables,
even when they do not change the effective access level.

Factory results should keep their owning type:

```cpp
IntrusivePtr<Implementation> implementation = Implementation::New();
owner = Implementation::New();
```

C++ keeps a temporary owner alive until the end of the full-expression.
This lifetime guarantee does not remove the extra reference-count operations
caused by extracting its raw pointer and constructing a new owner from it.

The raw factory is restricted to the public handle boundary:

```cpp
// HandleImplementation derives from BaseObject and returns refcount zero here.
PublicHandle handle(HandleImplementation::NewForBaseHandle());
IntrusivePtr<HandleImplementation> nativeOwner = HandleImplementation::New();
```
