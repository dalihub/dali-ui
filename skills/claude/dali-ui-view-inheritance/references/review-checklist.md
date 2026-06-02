# Review Checklist

Use this checklist before finishing a View inheritance task.

- The handle class inherits from `Dali::Ui::View`.
- The impl class inherits from `Dali::Ui::ViewImpl`.
- The handle class has no data members.
- The handle destructor is non-virtual.
- State and logic are stored in the impl class.
- `New()` creates impl, creates handle, calls `Initialize()`, and returns the handle.
- `DownCast()` uses `View::DownCast<Handle, Impl>()`.
- `GetImpl()` helpers exist where needed.
- Impl code uses `Self()` when it needs a handle.
- DALi type registration is present.
- `OnInitialize()` calls the base implementation, such as `ViewImpl::OnInitialize()`, before subclass-specific initialization.
- The handle class includes `DALI_UI_CHAIN_VIEW_METHODS(Handle)`.
- File names match the local project naming style.
- Public API changes follow ABI rules.
- Includes and namespaces match local patterns.
- The implementation follows the self-contained View inheritance reference.
- The task is direct `View` inheritance. If the user asks to inherit from `Label`, `ImageView`, or another View-derived class, recommend data attachment instead.
