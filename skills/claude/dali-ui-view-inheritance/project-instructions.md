# Claude Project Instructions: DALi UI View Inheritance

Use these instructions in a Claude Project when developing DALi UI View-derived components.

When the user asks to create a new custom DALi UI View class, use the self-contained template in `references/view-inheritance.md`.

These instructions are primarily for new View-derived class generation. For modifying or reviewing existing components, use them only when the task specifically involves View inheritance structure, handle/impl wiring, or ABI-safe public API patterns.

Rules:

- The handle class must inherit from `Dali::Ui::View`.
- The impl class must inherit from `Dali::Ui::ViewImpl`.
- Template class names are sample names only. Replace them with the user-requested class name.
- Do not add data members to the handle class.
- Put all state and logic in the impl class.
- Keep the handle destructor non-virtual.
- Use `View::DownCast<Handle, Impl>()`.
- Add `GetImpl(Handle&)` helper functions.
- Use `Self()` inside impl code when a handle is needed.
- Add `DALI_TYPE_REGISTRATION_BEGIN(...)`.
- Include `DALI_UI_CHAIN_VIEW_METHODS(Handle)` in the handle class so inherited `View` setters keep fluent chaining with the new handle type.
- Follow public-api / integration-api / internal API level boundaries when the local project uses that layout.
- If the target directory or namespace is not clear from the user's request and the local project structure, ask the user before creating files.
- Preserve ABI compatibility for public API.

Boundary:

- These instructions cover direct inheritance from `Dali::Ui::View`.
- Do not use them to inherit from classes such as `Label` or `ImageView`.
- `ViewImpl` is a special case because its impl class is exposed. Most other impl classes are not public, so external inheritance from those View-derived classes is not generally possible.
- If the user wants to extend an existing class such as `Label` or `ImageView`, recommend data attachment or a dedicated data-attachment skill if available.
