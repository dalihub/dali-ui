# DALi UI Framework

DALi UI is a high-performance UI library designed with a **Low Memory Assumption (LMA)** architecture. It provides a lightweight handle-based C++ API for building responsive UI on top of the DALi scene graph.

---

### Core Philosophy
- **Low Memory Assumption**: Minimal object overhead and efficient memory management.
- **Handle-Based API**: Lightweight public handles keep UI code explicit while implementation details stay in internal objects.
- **Native Speed**: Zero-cost abstractions over the underlying DALi scene graph.

---

### Quick Start

Create views, configure them with explicit property calls, and add them to the hierarchy:

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

void Create(Application application)
{
  Window window = application.GetWindow();

  View root = View::New();
  root.SetBackgroundColor(UiColor(0xFFFF00));
  root.SetRequestedWidth(200_spx);
  root.SetRequestedHeight(200_spx);

  View redBox = View::New();
  redBox.SetBackgroundColor(UiColor(0xFF0000));
  redBox.SetRequestedWidth(100_spx);
  redBox.SetRequestedHeight(100_spx);

  View blueBox = View::New();
  blueBox.SetBackgroundColor(UiColor(0x0000FF));
  blueBox.SetRequestedWidth(100_spx);
  blueBox.SetRequestedHeight(100_spx);
  blueBox.SetRequestedPositionX(100_spx);
  blueBox.SetRequestedPositionY(100_spx);

  root.Add(redBox);
  root.Add(blueBox);
  window.Add(root);
}
```

---

### Documentation Reference
- [**Class List**](daliUi/annotated.md)
- [**Namespace Overview**](daliUi/namespaces.md)
- [**File List**](daliUi/files.md)

---
> [!NOTE]
> This developer-centric documentation is version-controlled in `build/doc/source/index.md`.
