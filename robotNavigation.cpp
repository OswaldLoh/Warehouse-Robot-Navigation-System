// Task 3 - Robot Navigation & Path Tracking Module
//
// Handles all robot movement in the warehouse.
// Uses two data structures:
//   1. Stack (linked list of Steps) - records forward path, used to reverse it
//   2. NavRecord linked list - stores completed trip history for each robot

#include <iostream>
#include <string>
#include "headerFiles/robot.h"
#include "headerFiles/order.h"
#include "headerFiles/warehouse.h"
#include "headerFiles/navigation.h"

using namespace std;

// ===================== Stack Operations =====================
// The navigation stack stores each movement as a Step node.
// LIFO order means the last step pushed is the first reversed
// - exactly what we need for the robot's return journey.

// Add a step to the top of the stack (O(1))
void Robot::pushStep(string fAction, string bAction) {
    Step* newStep  = new Step(fAction, bAction);
    newStep->below = stackTop;
    stackTop       = newStep;
}

// Remove the top step and free its memory (O(1))
// Called by goBack() and during obstacle backtracking
void Robot::popStep() {
    if (stackTop != nullptr) {
        Step* temp = stackTop;
        stackTop   = stackTop->below;
        delete temp;
    }
}

// Return how many navigation trips this robot has completed
int Robot::getNavHistoryCount() {
    return navHistoryCount;
}

// ===================== Path Display =====================
// Both functions read the stack without modifying it.
// Steps are collected into a local array first so we can
// print them in the correct order (stack is stored LIFO).

// Show the forward path from first step to last
void Robot::printForwardPath() {
    const int MAX_STEPS = 50;
    Step* steps[MAX_STEPS];
    int count = 0;

    Step* temp = stackTop;
    while (temp != nullptr && count < MAX_STEPS) {
        steps[count++] = temp;
        temp = temp->below;
    }

    cout << "\n  -- Forward Path (Start --> Destination) --\n";
    if (count == 0) {
        cout << "  (No steps recorded)\n";
        return;
    }
    // steps[count-1] is the bottom (first step taken)
    for (int i = count - 1; i >= 0; i--) {
        cout << "  Step " << (count - i) << ": " << steps[i]->forwardAction << "\n";
    }
}

// Show the reverse path (what goBack() will execute)
void Robot::printReversePath() {
    const int MAX_STEPS = 50;
    Step* steps[MAX_STEPS];
    int count = 0;

    Step* temp = stackTop;
    while (temp != nullptr && count < MAX_STEPS) {
        steps[count++] = temp;
        temp = temp->below;
    }

    cout << "\n  -- Reverse Path (Destination --> Base) --\n";
    if (count == 0) {
        cout << "  (No steps to reverse)\n";
        return;
    }
    // steps[0] is the top (last step taken), so it's reversed first
    for (int i = 0; i < count; i++) {
        cout << "  Step " << (i + 1) << ": " << steps[i]->backAction << "\n";
    }
}

// Print the full round-trip log for this robot
void Robot::printNavigationLog() {
    cout << "\n===== NAVIGATION LOG: Robot [" << ID << "] =====\n";
    printForwardPath();
    printReversePath();
    cout << "===============================================\n";
}

// ===================== Navigation History =====================
// saveNavRecord() snapshots the current stack into a NavRecord node
// and appends it to this robot's history linked list.
// Must be called BEFORE goBack() because goBack() clears the stack.

void Robot::saveNavRecord(int orderID, const string& itemName) {
    NavRecord* record = new NavRecord(orderID, itemName);

    // Collect stack nodes into an array (index 0 = top = last step taken)
    const int MAX = 50;
    Step* steps[MAX];
    int count = 0;
    Step* temp = stackTop;
    while (temp != nullptr && count < MAX) {
        steps[count++] = temp;
        temp = temp->below;
    }

    record->stepCount = count;

    // Store forward steps in chronological order (bottom of stack first)
    for (int j = 0; j < count; j++) {
        record->forwardSteps[j] = steps[count - 1 - j]->forwardAction;
    }

    // Store reverse steps in execution order (top of stack first)
    for (int j = 0; j < count; j++) {
        record->reverseSteps[j] = steps[j]->backAction;
    }

    // Append to the end of the history list to keep trips in order
    if (navHistoryHead == nullptr) {
        navHistoryHead = record;
    } else {
        NavRecord* tail = navHistoryHead;
        while (tail->next != nullptr) {
            tail = tail->next;
        }
        tail->next = record;
    }
    navHistoryCount++;
}

// Display all past trips stored in this robot's NavRecord list
void Robot::displayNavHistory() {
    if (navHistoryHead == nullptr) {
        cout << "  Robot [" << ID << "] has no navigation history yet.\n";
        return;
    }

    cout << "\n===== NAVIGATION HISTORY: Robot [" << ID << "] ("
         << navHistoryCount << " trip(s)) =====\n";

    NavRecord* curr = navHistoryHead;
    int tripNum = 1;
    while (curr != nullptr) {
        cout << "\n  Trip #" << tripNum++
             << " | Order #" << curr->orderID
             << " | Item: \"" << curr->itemName << "\"\n";

        cout << "    Forward Path (" << curr->stepCount << " steps):\n";
        for (int i = 0; i < curr->stepCount; i++) {
            cout << "      Step " << (i + 1) << ": " << curr->forwardSteps[i] << "\n";
        }

        cout << "    Reverse Path:\n";
        for (int i = 0; i < curr->stepCount; i++) {
            cout << "      Step " << (i + 1) << ": " << curr->reverseSteps[i] << "\n";
        }

        curr = curr->next;
    }
    cout << "\n================================================\n";
}

// ===================== Return to Base =====================
// Pops every step off the stack and executes its backAction.
// Because the stack is LIFO, the last step taken is always
// undone first - the correct order for returning home.
void Robot::goBack() {
    cout << "\nRobot [" << ID << "] returning to base...\n";

    if (stackTop == nullptr) {
        cout << "  (No path recorded - robot is already at base)\n";
        status = "Available";
        cout << "Robot [" << ID << "] is now Available.\n";
        return;
    }

    int stepNum = 1;
    while (stackTop != nullptr) {
        cout << "  Reverse Step " << stepNum++ << ": " << stackTop->backAction << "\n";
        popStep();
    }

    status = "Available";
    cout << "Robot [" << ID << "] has returned to base and is now Available.\n";
}

// ===================== Path Generation =====================

// Search warehouse by item name (case-sensitive).
// Orders store the item name, not the ID, so we need this
// to look up the real Zone/Aisle/Shelf location.
static Item* findItemByName(Zone* head, const string& name) {
    Zone* z = head;
    while (z != nullptr) {
        Aisle* a = z->headAisle;
        while (a != nullptr) {
            Shelf* s = a->headShelf;
            while (s != nullptr) {
                Item* item = s->headItem;
                while (item != nullptr) {
                    if (item->name == name) return item;
                    item = item->next;
                }
                s = s->nextShelf;
            }
            a = a->nextAisle;
        }
        z = z->nextZone;
    }
    return nullptr;
}

// Build and push navigation steps based on the item's real location.
// Returns true if the item was found and steps were pushed.
// Returns false if the item does not exist in the database - no steps
// are pushed and the robot stays put.
//
// Obstacle handling: if the item is in Aisle 2, an obstacle is
// simulated. The robot backtracks (pops the last step) then takes
// an alternate route through Aisle 1 to reach Aisle 2.
static bool navigateRobot(Robot* robot, const string& itemName) {
    Item* item = findItemByName(getWarehouseHead(), itemName);

    if (item == nullptr) {
        cout << "  [!] Item \"" << itemName << "\" not found in the warehouse database.\n";
        cout << "  Robot stays at base. No movement recorded.\n";
        return false;
    }

    string zoneName  = "Zone ";  zoneName += item->zoneID;
    string aisleName = "Aisle " + to_string(item->aisleID);
    string shelfName = "Shelf " + to_string(item->shelfID);

    // Step 1 - go to the correct zone
    cout << "  Step 1: Navigate to " << zoneName << "\n";
    robot->pushStep("Navigate to " + zoneName,
                    "Return from "  + zoneName);

    if (item->aisleID == 2) {
        // Obstacle in Aisle 2 - backtrack and reroute via Aisle 1
        cout << "  Step 2: Attempting to enter " << aisleName << "...\n";
        cout << "  [!] OBSTACLE detected in " << aisleName << "! Backtracking...\n";

        if (robot->getStack() != nullptr) {
            cout << "  Backtrack: " << robot->getStack()->backAction << "\n";
            robot->popStep();
        }

        cout << "  Alternate: Re-entering " << zoneName << " via alternate corridor\n";
        robot->pushStep("Navigate to " + zoneName + " (alt corridor)",
                        "Return from " + zoneName + " (alt corridor)");

        cout << "  Step 2 (alt): Navigate to Aisle 1 (detour)\n";
        robot->pushStep("Navigate to Aisle 1 (detour)",
                        "Return from Aisle 1 (detour)");

        cout << "  Step 3: Cross to " << aisleName << " via Aisle 1\n";
        robot->pushStep("Cross to " + aisleName + " via Aisle 1",
                        "Return via Aisle 1 to " + zoneName);
    } else {
        // Direct path - no obstacle
        cout << "  Step 2: Navigate to " << aisleName << "\n";
        robot->pushStep("Navigate to " + aisleName,
                        "Return from "  + aisleName);
    }

    // Count actual steps pushed so the step number is always accurate
    int nextStep = 0;
    Step* temp = robot->getStack();
    while (temp != nullptr) { nextStep++; temp = temp->below; }
    nextStep++;  // next step number = current count + 1

    cout << "  Step " << nextStep << ": Navigate to " << shelfName << "\n";
    robot->pushStep("Navigate to " + shelfName,
                    "Return from "  + shelfName);

    return true;
}

// ===================== Complete Order =====================
// Main Task 3 function. Runs the full navigation flow:
//   1. Forward path  - navigate to item location
//   2. Obstacle check - backtrack and reroute if needed
//   3. Pick up item
//   4. Save trip to history (before stack is cleared)
//   5. Print navigation log
//   6. Reverse path  - robot returns to base step by step
//   7. Mark order as Completed
void completeOrder() {
    if (inProgressOrders.isEmpty()) {
        cout << "No orders currently in progress.\n";
        return;
    }

    Order* order = inProgressOrders.dequeue();

    // Find the assigned robot in the circular linked list
    Robot* temp          = robotHead;
    Robot* assignedRobot = nullptr;
    do {
        if (temp->getID() == order->assignedRobotID) {
            assignedRobot = temp;
            break;
        }
        temp = temp->nextRobot;
    } while (temp != robotHead);

    if (assignedRobot == nullptr) {
        // Robot not found (safety check) - complete order without navigation
        cout << "[!] Warning: Robot [" << order->assignedRobotID
             << "] not found. Order completed without navigation.\n";
        order->status = "Completed";
        completedOrders.push(order);
        cout << "Order #" << order->orderID << " marked as Completed.\n";
        return;
    }

    cout << "\n====================================================\n";
    cout << " Robot [" << assignedRobot->getID()
         << "] | Order #" << order->orderID
         << " | Item: \"" << order->itemName << "\"\n";
    cout << "===================================================\n";

    // Forward navigation
    cout << "\n[Forward Navigation]\n";
    bool itemFound = navigateRobot(assignedRobot, order->itemName);

    if (!itemFound) {
        // Item does not exist - robot never moved, free it immediately
        assignedRobot->setStatus("Available");
        order->status = "Failed";
        completedOrders.push(order);
        cout << "\nOrder #" << order->orderID
             << " marked as Failed (item not in database). Robot ["
             << assignedRobot->getID() << "] is now Available.\n";
        cout << "===================================================\n";
        return;
    }

    cout << "\nRobot [" << assignedRobot->getID()
         << "] reached item: \"" << order->itemName << "\"\n";
    cout << "Picking up item...\n";

    // Save trip record before goBack() clears the stack
    assignedRobot->saveNavRecord(order->orderID, order->itemName);

    // Reverse navigation - robot returns to base
    cout << "\n[Reverse Navigation]\n";
    assignedRobot->goBack();

    order->status = "Completed";
    completedOrders.push(order);

    cout << "\nOrder #" << order->orderID << " completed!\n";
    cout << "===================================================\n";
}

// ===================== View Robot Navigation Log =====================
// Prompts the user to choose a robot by ID, then displays its full
// NavRecord history (all completed trips, forward + reverse paths).
// Uses displayNavHistory() instead of the stack because goBack()
// clears the stack when a trip finishes - only NavRecord persists.
void viewRobotNavigationLog() {
    if (robotHead == nullptr) {
        cout << "No robots in the system.\n";
        return;
    }

    // Show all robots and their trip counts so the user knows who has history
    cout << "\n================== ROBOT OVERVIEW ==================\n";
    Robot* temp = robotHead;
    do {
        cout << "  Robot [" << temp->getID() << "] - "
             << temp->getStatus()
             << " | Trips completed: " << temp->getNavHistoryCount() << "\n";
        temp = temp->nextRobot;
    } while (temp != robotHead);
    cout << "====================================================\n";

    cout << "Enter Robot ID: ";
    string input;
    getline(cin, input);

    bool valid = !input.empty();
    for (int i = 0; i < (int)input.length() && valid; i++) {
        if (input[i] < '0' || input[i] > '9') valid = false;
    }
    if (!valid) { cout << "Invalid Robot ID.\n"; return; }

    int robotID = stoi(input);

    Robot* target = nullptr;
    temp = robotHead;
    do {
        if (temp->getID() == robotID) { target = temp; break; }
        temp = temp->nextRobot;
    } while (temp != robotHead);

    if (target == nullptr) {
        cout << "Robot [" << robotID << "] not found.\n";
        return;
    }

    // displayNavHistory() reads from the NavRecord linked list which
    // persists across trips - unlike the stack which is cleared by goBack()
    target->displayNavHistory();
}

// ===================== View All Navigation History =====================
// Shows the full NavRecord history for every robot that has
// completed at least one trip. Covers the "Complete Navigation Log"
// expected output from the assignment.
void viewAllNavigationHistory() {
    if (robotHead == nullptr) {
        cout << "No robots in the system.\n";
        return;
    }

    cout << "\n===== COMPLETE NAVIGATION HISTORY (ALL ROBOTS) =====\n";

    bool anyHistory = false;
    Robot* temp = robotHead;
    do {
        if (temp->getNavHistoryCount() > 0) {
            temp->displayNavHistory();
            anyHistory = true;
        }
        temp = temp->nextRobot;
    } while (temp != robotHead);

    if (!anyHistory) {
        cout << "  No history yet. Complete an order first.\n";
    }

    cout << "====================================================\n";
}
