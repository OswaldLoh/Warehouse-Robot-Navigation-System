# Warehouse Robot Navigation System

## Overview

The Warehouse Robot Navigation System is a C++ console application that simulates autonomous robot operations inside a large e-commerce warehouse. The system coordinates multiple robots to locate items on shelves, pick them, and deliver them to packing stations — all managed through five integrated modules.

The end-to-end workflow operates as follows:

1. A new customer order is received and queued for processing.
2. An available robot is assigned to the order using a fair rotation mechanism.
3. The system identifies the item's location within the warehouse hierarchy (Zone → Aisle → Shelf).
4. A shortest-path route is generated from the robot's base to the target shelf.
5. The robot navigates step-by-step to the item, with every movement recorded.
6. After completing the task, the robot retraces its exact path in reverse to return to base.

All five modules work together as one cohesive system to ensure efficient, fair, and accurate warehouse operations.

---

## Techniques & Data Structures Used

| Data Structure | Purpose | Module |
|---|---|---|
| **Queue (Linked List)** | Manages pending and in-progress orders in FIFO sequence to ensure fair processing | Order Management |
| **Stack (Linked List)** | Records completed/failed orders in LIFO order; also records each robot's movement steps for reverse path retracing | Order Management, Robot Navigation |
| **Circular Queue (Linked List)** | Maintains a circular roster of robots so task assignments rotate evenly without restarting the cycle | Robot Assignment |
| **Tree (General Tree)** | Models the warehouse layout hierarchy — Zones contain Aisles, which contain Shelves, which contain Items | Warehouse Layout |
| **Binary Search Tree (BST)** | Enables fast O(log n) item lookup, insertion, and deletion by Item ID | Item Search & Management |
| **Graph (Adjacency Matrix) + Dijkstra's Algorithm** | Represents physical connections between warehouse locations and computes shortest paths for robot navigation | Warehouse Layout & Navigation |
| **Linked List** | Used throughout for dynamic storage of orders, items on shelves, and navigation history records | Multiple modules |

---

## Task Breakdown

### Task 1 — Order Management Module

**Responsible Member:** `[INSERT MEMBER NAME HERE]`

**Source Files:**
- `orderManagement.cpp` — Implementation of all order management functions
- `headerFiles/order.h` — Declares `Order` struct, `OrderQueue` class (FIFO), and `CompletedStack` class (LIFO)

This module handles the lifecycle of customer orders from arrival to completion. New orders are enqueued into a pending queue and processed sequentially in FIFO order. Once an order is assigned to a robot and fulfilled, it is pushed onto a completed stack. The module supports adding new orders, viewing pending/in-progress/completed orders, viewing all order statuses, and handling edge cases such as empty order lists or system overload (queue capacity limit).

**Key Data Structures:** Queue (pending & in-progress orders), Stack (completed orders)

---

### Task 2 — Robot Assignment Module

**Responsible Member:** `[INSERT MEMBER NAME HERE]`

**Source Files:**
- `robotManagement.cpp` — Implementation of robot initialisation, assignment, and maintenance functions
- `headerFiles/robot.h` — Declares `Robot` class with circular linked list pointer (`nextRobot`)

This module manages a pool of robots using a circular linked list. Robots are initialised at startup and connected in a loop so that task assignments rotate continuously through the roster. When assigning orders, the system finds the next available robot (skipping those marked as "Busy" or "Maintenance"), assigns the order, and advances the pointer. The module also provides robot status display and a maintenance toggle to take robots in and out of service.

**Key Data Structures:** Circular Queue (circular linked list of robots)

---

### Task 3 — Robot Navigation & Path Tracking Module

**Responsible Member:** `[INSERT MEMBER NAME HERE]`

**Source Files:**
- `robotNavigation.cpp` — Implementation of navigation, path tracking, and history functions
- `headerFiles/navigation.h` — Declares public navigation function prototypes
- `headerFiles/robot.h` — Declares `Step` struct (stack node) and `NavRecord` struct (history node)

This module controls robot movement within the warehouse and enables path retracing. Each movement step (Forward, Backward, Left, Right) is pushed onto a navigation stack along with its reverse action. When a task is completed, the robot pops steps off the stack to retrace the exact reverse path back to base. The module also integrates with the graph module (Task 5) to compute Dijkstra shortest paths for automated navigation. A manual simulation mode allows step-by-step navigation with obstacle handling. All completed trips are saved as navigation history records.

**Key Data Structures:** Stack (navigation path), Linked List (navigation history), Graph + Dijkstra (shortest path)

---

### Task 4 — Item Search & Management Module

**Responsible Member:** `[INSERT MEMBER NAME HERE]`

**Source Files:**
- `itemManagement.cpp` — Implementation of item CRUD operations and display functions
- `tree.cpp` — Implementation of BST operations (insert, search, delete, traversal)
- `storage.cpp` — Implementation of warehouse structure initialisation and item storage into shelves
- `fileHandling.cpp` — CSV file parser to load items from `database/items.csv`
- `headerFiles/warehouse.h` — Declares `Item`, `Shelf`, `Aisle`, `Zone`, and `TreeNode` structs

This module manages the item inventory within the warehouse. Items are stored in a Binary Search Tree (BST) keyed by Item ID for efficient search, and simultaneously placed onto their designated shelf in the warehouse tree structure. The module supports displaying all items (sorted by ID or name using bubble sort), searching for items by ID, inserting new items, updating item details (name or location), and deleting items. Item data is initially loaded from a CSV file at startup.

**Key Data Structures:** Binary Search Tree (item lookup), General Tree (warehouse hierarchy), Linked List (items on shelves)

---

### Task 5 — Warehouse Layout & Navigation Module

**Responsible Member:** `[INSERT MEMBER NAME HERE]`

**Source Files:**
- `graph.cpp` — Implementation of the warehouse graph, Dijkstra's algorithm, and graph display
- `headerFiles/graph.h` — Declares graph constants, node-conversion functions, and Dijkstra prototype

This module represents the physical layout of the warehouse as an undirected weighted graph with 19 nodes (1 base node + 18 shelf locations across 3 zones × 2 aisles × 3 shelves). Edges are weighted by physical proximity — adjacent shelves cost 1, cross-aisle and inter-zone connections cost 2, and base connections cost 2–3. The module implements Dijkstra's shortest-path algorithm to compute optimal routes from the robot base to any target shelf. It also provides a display function to visualise all graph connections and their edge costs.

**Key Data Structures:** Graph (adjacency matrix), Dijkstra's Algorithm (shortest path computation)

---

## Shared Files

The following file is shared across all modules:

- `main.cpp` — System entry point. Initialises all modules (robots, warehouse, items), displays the main menu, and routes user input to each task's sub-menu.
- `database/items.csv` — Pre-loaded item data (Item ID, name, zone, aisle, shelf).

---

## Installation & How to Run

### Prerequisites

- A C++ compiler that supports C++11 or later (e.g., `g++` from [MinGW](https://www.mingw-w64.org/) on Windows)
- Command-line terminal (Command Prompt, PowerShell, or similar)

### Steps

**1. Extract the zip file**

Extract the submitted zip file to a folder of your choice.

**2. Open a terminal and navigate to the project folder**

```bash
cd "Warehouse Robot Navigation System"
```

**3. Compile the program**

```bash
g++ -o main.exe *.cpp
```

**4. Run the program**

```bash
./main.exe
```

The system will initialise with 3 robots and load the item database from `database/items.csv`. You will be presented with the main menu to access all five modules.
