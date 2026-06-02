# Warehouse Robot Navigation System — Unimplemented Features

> [!NOTE]
> Based on requirements in [WAREHOUSE ROBOT NAVIGATION SYSTEM.txt](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/WAREHOUSE%20ROBOT%20NAVIGATION%20SYSTEM.txt) vs. the current codebase as of 2 June 2026.

---

## Quick Overview

| Module | Missing Count |
|--------|:-------------:|
| Task 1 – Order Management | 1 |
| Task 2 – Robot Assignment | 2 |
| Task 3 – Robot Navigation & Path Tracking | 3 |
| Task 4 – Item Search & Management | 2 |
| Task 5 – Warehouse Layout & Navigation | 4 |
| **Total** | **12** |

---

## Task 1 — Order Management Module

**Relevant files:** [order.h](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/headerFiles/order.h), [orderManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/orderManagement.cpp)

| # | Missing Feature | Details |
|---|----------------|---------|
| 1 | **Current order being processed** | The requirements specify displaying the "current order being processed" as a distinct output. `viewOrderStatus()` lists all in-progress orders together but does not specifically highlight or isolate the single order currently being actively fulfilled. |

---

## Task 2 — Robot Assignment Module

**Relevant files:** [robot.h](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/headerFiles/robot.h), [robotManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/robotManagement.cpp)

| # | Missing Feature | Details |
|---|----------------|---------|
| 1 | **Per-robot task assignment tracking** | The `Robot` class in [robot.h](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/headerFiles/robot.h) only stores a `status` string. There is no member variable or data structure to record which orders have been assigned to a given robot. Task history is lost the moment a robot's status returns to "Available". |
| 2 | **Robot assignment list display** | No dedicated menu function exists to display a list mapping each robot to its current or past task assignments. The only way to partially infer this is by reading the in-progress order queue in Module 1. |

---

## Task 3 — Robot Navigation & Path Tracking Module

**Relevant files:** [robot.h](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/headerFiles/robot.h), [robotManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/robotManagement.cpp), [orderManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/orderManagement.cpp)

| # | Missing Feature | Details |
|---|----------------|---------|
| 1 | **Dynamic path generation based on item location** | In `completeOrder()` in [orderManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/orderManagement.cpp#L179-L184), the robot's movement steps are hardcoded as 3 fixed actions (`Move Forward`, `Turn Left`, `Move Forward`). The path is not derived from the actual Zone/Aisle/Shelf location of the requested item. |
| 2 | **Obstacle and navigation error handling** | No logic exists to detect obstacles, handle incorrect paths, or allow the robot to backtrack around a blocked route. The requirements specifically call for "basic obstacle handling through backtracking". |
| 3 | **Persistent navigation log** | There is no log structure that survives after a task is completed. Movement steps exist only in the robot's stack and are deleted one-by-one during `goBack()`. Once the task is done, the complete forward and return path is gone. The requirements call for a "complete navigation log". |

> [!IMPORTANT]
> The warehouse layout (Task 5) and the navigation module (Task 3) are completely disconnected. Navigation steps must be generated from the warehouse tree structure — this is the most significant missing integration in the entire system.

---

## Task 4 — Item Search & Management Module

**Relevant files:** [itemManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/itemManagement.cpp), [fileHandling.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/fileHandling.cpp)

| # | Missing Feature | Details |
|---|----------------|---------|
| 1 | **Search items by name** | `searchItem()` in [itemManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/itemManagement.cpp) only accepts an Item ID as input. The requirements state items should be searchable by "specific criteria (e.g., ID **or name**)". Name-based search is entirely absent. |
| 2 | **Save/persist item changes to file** | [fileHandling.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/fileHandling.cpp) implements `loadItemsFromCSV()` for reading data at startup, but there is no corresponding save function. Any items inserted, updated, or deleted at runtime exist only in memory and are permanently lost when the program exits. |

---

## Task 5 — Warehouse Layout & Navigation Module

**Relevant files:** [warehouse.h](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/headerFiles/warehouse.h), [storage.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/storage.cpp)

| # | Missing Feature | Details |
|---|----------------|---------|
| 1 | **Physical connections between locations** | The `Zone → Aisle → Shelf` hierarchy in [warehouse.h](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/headerFiles/warehouse.h) models containment only (a shelf is inside an aisle). There are no edges or adjacency definitions between neighbouring physical locations (e.g., Aisle 1 is adjacent to Aisle 2), which are required for any pathfinding. |
| 2 | **Route/path generation between two points** | No algorithm (BFS, DFS, Dijkstra, etc.) exists to compute a route from a robot's starting position to a target shelf. This is a core requirement: "Provide navigation routes from one point to another". |
| 3 | **Warehouse section traversal** | There are no tree/graph traversal functions. `displayWarehouse()` uses nested `while` loops to linearly print inventory — it is not a proper traversal (pre-order, level-order, etc.) as required. |
| 4 | **Integration with Robot Navigation Module** | The warehouse layout is entirely passive (display only). It does not feed movement instructions to the robot navigation stack in [robotManagement.cpp](file:///c:/Users/oswal/Desktop/Warehouse%20Robot%20Navigation%20System/robotManagement.cpp). The requirements explicitly state this module must "integrate with the robot navigation module for path planning". |

> [!WARNING]
> Tasks 3 and 5 are tightly coupled — Task 5 must provide routes, and Task 3 must consume them. Neither end of this integration is implemented.

---

## Full System Workflow — Missing Steps

The system workflow defined in the `.txt` requirements has 7 steps. Steps 4 and 5 are entirely missing:

| Step | Description | Status |
|------|-------------|--------|
| 1 | New order is received | ✅ Working |
| 2 | Order is stored and prepared for processing | ✅ Working |
| 3 | A robot is assigned to the task | ✅ Working |
| **4** | **System identifies the item location** | ❌ **Not implemented** — no link between order item name and its warehouse location |
| **5** | **A route is generated for the robot** | ❌ **Not implemented** — route is hardcoded, not derived from warehouse structure |
| 6 | Robot moves step-by-step to the item | ⚠️ Partial — steps are simulated (hardcoded), not real |
| 7 | Robot returns using the reverse path | ✅ Working |

---

## Priority Order for Implementation

| Priority | Feature | Module |
|----------|---------|--------|
| 🔴 High | Dynamic route generation from item location | Task 3 + 5 |
| 🔴 High | Physical location connections in warehouse layout | Task 5 |
| 🟡 Medium | Persistent navigation log | Task 3 |
| 🟡 Medium | Obstacle/error handling in navigation | Task 3 |
| 🟡 Medium | Search items by name | Task 4 |
| 🟡 Medium | Save/persist item changes to CSV | Task 4 |
| 🟢 Low | Per-robot task assignment history | Task 2 |
| 🟢 Low | Robot assignment list display | Task 2 |
| 🟢 Low | Current order being processed indicator | Task 1 |
