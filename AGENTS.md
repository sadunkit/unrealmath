# AGENTS.md

Instructions for the AI agent when working on this repository.

---

## "Document Transforms" Workflow

When the user says **"Document Transforms"** (or a similar phrase referencing an Unreal Engine type or topic), the agent should perform **two tasks**:

### Task 1 — Generate Documentation (.md)

1. **Research** the Unreal Engine API for the requested type(s) (e.g. `FVector`, `FRotator`, `FQuat`, `FTransform`, etc.).
2. Create a Markdown file under `notes/transforms/<TypeName>.md` (or an appropriate subdirectory under `notes/`).
3. The documentation must include:
   - A short description of what the type is and when to use it.
   - **All code examples wrapped in ` ```cpp ``` ` fenced code blocks** so they render correctly.
   - Sections covering: Construction, Common Operations, Conversion to/from related types, Interpolation, Common Gameplay Patterns, and Gotchas.
   - A "See Also" section linking to related `.md` files in the same folder.

### Task 2 — Write SimpleAutomationTests (.cpp)

1. Create (or append to) a test file under `tests/<TopicName>Tests.cpp`.
2. Use `IMPLEMENT_SIMPLE_AUTOMATION_TEST` — **not** complex tests, not spec-style tests.
3. Tests must have **minimal dependencies**:
   - Only `#include "CoreMinimal.h"` and `#include "Misc/AutomationTest.h"`.
   - No world, no actors, no gameplay classes — pure math tests.
   - Wrap everything in `#if WITH_AUTOMATION_TESTS ... #endif`.
4. Use a consistent test category prefix: `"UnrealMath.<Topic>.<Type>.<TestName>"`.
5. Include a header comment block explaining how to copy the file into a project:
   - Where to place it (`Source/<Module>/Tests/`).
   - What to add to `.Build.cs` (`"Core"` dependency).
   - How to run (Window → Test Automation, filter for the category prefix).
6. Test all the key behaviors documented in Task 1 — construction, arithmetic, normalization, conversion round-trips, interpolation, etc.

---

## Directory Structure

```
unrealmath/
├── AGENTS.md              ← this file
├── README.md
├── notes/
│   └── transforms/
│       ├── FVector.md
│       ├── FRotator.md
│       ├── FQuat.md
│       └── ...            ← future types go here
└── tests/
    ├── TransformTests.cpp
    └── ...                ← future test files go here
```

---

## General Rules

- **Branch naming**: `docs/<topic-slug>` (e.g. `docs/document-transforms`).
- **Commit messages**: Concise, imperative mood. Include `Co-Authored-By: Warp <agent@warp.dev>`.
- **PR body**: Summarize what was documented and tested. Link to Epic's official docs where relevant.
- Keep documentation practical — focus on code snippets over theory.
- Keep tests self-contained — a developer should be able to drop the `.cpp` into any UE project and compile.
