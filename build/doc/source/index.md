# DALi UI Framework

DALi UI is a high-performance UI library designed with a **Low Memory Assumption (LMA)** architecture. It provides a **Modern C++ Fluent API** with method chaining, enabling declarative and intuitive UI development for resource-constrained systems.

---

### Core Philosophy
- **Low Memory Assumption**: Minimal object overhead and efficient memory management.
- **Fluent Interface**: Method chaining for readable, declarative UI composition.
- **Native Speed**: Zero-cost abstractions over the underlying DALi scene graph.

---

### Quick Start (Fluent API)

Construct complex UI hierarchies with ease using our declarative method chaining:

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::UI;

void Create(Application application) {
  application.GetWindow().Add(
    View::New()
      .SetBackgroundColor(Color::YELLOW)
      .SetRequestedWidth(200_spx)
      .SetRequestedHeight(200_spx)
      .Children({
        View::New()
          .SetBackgroundColor(Color::RED)
          .SetRequestedWidth(100_spx)
          .SetRequestedHeight(100_spx)
          .With([](View& v) {
            v.TouchedSignal().Connect([](Actor a, TouchEvent t) {
              return true;
            });
          }),
        View::New()
          .SetBackgroundColor(Color::BLUE)
          .SetPositionX(100_spx)
          .SetPositionY(100_spx)
      })
  );
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
