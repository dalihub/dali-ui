---
name: dali-ui-view-inheritance
description: Use this skill when creating a new DALi UI class derived from Dali::Ui::View, including generating the handle/impl pair, ViewImpl subclass, New(), DownCast, GetImpl, Self(), type registration, and ABI-safe public API structure.
x-updated: "2026-06-04T05:02:19Z"
---

# DALi UI View Inheritance

Use this skill when the user wants to create a new custom DALi UI class derived from `Dali::Ui::View`.

This skill is primarily for new View-derived class generation. For modifying or reviewing existing components, use it only when the task specifically involves View inheritance structure, handle/impl wiring, or ABI-safe public API patterns.

## Required References

Read these references before generating or changing code:

- `references/view-inheritance.md`: handle/impl workflow and naming rules.
- `references/abi-rules.md`: public API and ABI constraints.
- `references/review-checklist.md`: final validation checklist.

## Core Rules

- Create a public handle class derived from `Dali::Ui::View`.
- Create an impl class derived from `Dali::Ui::ViewImpl`.
- Treat template class names in the reference as sample names only. Replace them with the class requested by the user.
- Do not add data members to the handle class.
- Put all state and logic in the impl class.
- Keep the handle destructor non-virtual.
- Use `View::DownCast<Handle, Impl>()`.
- Provide `GetImpl(Handle&)` helpers in the handle source file.
- Use `Self()` from the impl when a handle is needed.
- Add DALi type registration with `DALI_TYPE_REGISTRATION_BEGIN(...)`.
- Include `DALI_UI_CHAIN_VIEW_METHODS(Handle)` in the handle class so inherited `View` setters keep fluent chaining with the new handle type.
- Follow the existing directory and naming patterns in the local project.
- If the target directory or namespace is not clear from the user's request and the local project structure, ask the user before creating files.

## Boundary

This skill covers direct inheritance from `Dali::Ui::View`. Do not use it to inherit from classes such as `Label` or `ImageView`.

`ViewImpl` is a special case: its impl class is exposed so framework developers can derive from it. Most other impl classes are not public, so external inheritance from those View-derived classes is not generally possible. If the user wants to extend an existing class such as `Label` or `ImageView`, recommend data attachment or a dedicated data-attachment skill if available.

## Default Workflow

1. Read `references/view-inheritance.md`.
2. Derive the requested class name, impl name, and file names.
3. Confirm the target directory and namespace from the user request or local project structure. If unclear, ask the user before creating files.
4. Create or update handle and impl files following the reference template.
5. Keep public handle ABI-safe.
6. Add type registration and DownCast support.
7. Run formatting or tests only when requested or locally appropriate.
8. Report changed files and any remaining API assumptions.

## Naming

If the user requests `ProductCardView`:

- Handle class: `ProductCardView`
- Impl class: `ProductCardViewImpl`
- Public handle files: `product-card-view.h`, `product-card-view.cpp`
- Impl files: `product-card-view-impl.h`, `product-card-view-impl.cpp`

Confirm exact target directories from the local project before editing.
