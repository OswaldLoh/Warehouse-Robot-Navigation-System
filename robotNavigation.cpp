// Task 3 - Robot Navigation & Path Tracking Module
// File: robotNavigation.cpp
//
// Handles all robot movement in the warehouse.
// Uses two data structures:
//   1. Stack (linked list of Steps) - records forward path, reversed on return
//   2. NavRecord linked list - stores completed trip history per robot

#include <iostream>
#include <string>
#include <cctype>
#include "headerFiles/robot.h"
#include "headerFiles/order.h"
#include "headerFiles/warehouse.h"
#include "headerFiles/navigation.h"

using namespace std;

// ===================== Stack Operations =====================
// The navigation stack uses LIFO order so the last step pushed
// is the first step reversed - exactly right for a return journey.

// Push one movement step onto the top of the stack (O(1))
void Robot::pushStep(string fAction, string bAction) {
    Step* newStep  = new Step(fAction, bAction);
    newStep->below = stackTop;
    stackTop       = newStep;
}

// Remove the top step and free its memory (O(1))
// Used by goBack() and by obstacle backtracking
void Robot::popStep() {
    if (stackTop != nullptr) {
        Step* temp = stackTop;
        stackTop   = stackTop->below;
        delete temp;
    }
}

// Return the number of trips this robot has completed
int Robot::getNavHistoryCount() {
    return navHistoryCount;
}

// ===================== Path Display =====================
// Both functions collect stack pointers into a local array first
// so we can print steps in the right order (stack is LIFO).

// Show the forward path in the order the steps were taken
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
    if (count == 0) { cout << "  (No steps recorded)\n"; return; }

    // steps[count-1] = bottom = first step taken
    for (int i = count - 1; i >= 0; i--) {
        cout << "  Step " << (count - i) << ": " << steps[i]->forwardAction << "\n";
    }
}

// Show the reverse path in the order goBack() will execute it
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
    if (count == 0) { cout << "  (No steps to reverse)\n"; return; }

    // steps[0] = top = last step taken, so it is reversed first (LIFO)
    for (int i = 0; i < count; i++) {
        cout << "  Step " << (i + 1) << ": " << steps[i]->backAction << "\n";
    }
}

// Print both paths together as a complete round-trip log
void Robot::printNavigationLog() {
    cout << "\n===== NAVIGATION LOG: Robot [" << ID << "] =====\n";
    printForwardPath();
    printReversePath();
    cout << "===============================================\n";
}

// ===================== Navigation History =====================
// saveNavRecord() must be called BEFORE goBack() because goBack()
// pops and frees every Step node, leaving the stack empty.

void Robot::saveNavRecord(int orderID, const string& itemName) {
    NavRecord* record = new NavRecord(orderID, itemName);

    // Collect stack nodes (index 0 = top = last step taken)
    const int MAX = 50;
    Step* steps[MAX];
    int count = 0;
    Step* temp = stackTop;
    while (temp != nullptr && count < MAX) {
        steps[count++] = temp;
        temp = temp->below;
    }

    record->stepCount = count;

    // Forward: bottom of stack first (chronological order)
    for (int j = 0; j < count; j++)
        record->forwardSteps[j] = steps[count - 1 - j]->forwardAction;

    // Reverse: top of stack first (goBack() execution order)
    for (int j = 0; j < count; j++)
        record->reverseSteps[j] = steps[j]->backAction;

    // Append to tail so trips stay in chronological order
    if (navHistoryHead == nullptr) {
        navHistoryHead = record;
    } else {
        NavRecord* tail = navHistoryHead;
        while (tail->next != nullptr) tail = tail->next;
        tail->next = record;
    }
    navHistoryCount++;
}

// Display all trips in this robot's history list
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
        for (int i = 0; i < curr->stepCount; i++)
            cout << "      Step " << (i + 1) << ": " << curr->forwardSteps[i] << "\n";

        cout << "    Reverse Path:\n";
        for (int i = 0; i < curr->stepCount; i++)
            cout << "      Step " << (i + 1) << ": " << curr->reverseSteps[i] << "\n";

        curr = curr->next;
    }
    cout << "\n================================================\n";
}

// ===================== Return to Base =====================
// Pops every step and executes its backAction.
// LIFO ensures the last step taken is always undone first.
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

// ===================== Path Generation (Order-based) =====================

// Find an item in the warehouse by name (case-sensitive).
// Orders store the item name, so we need this to get the real location.
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
// Full navigation cycle for one in-progress order:
//   Forward path -> obstacle handling -> pick up item ->
//   save history -> print nav log -> reverse path -> mark completed
void completeOrder() {
    if (inProgressOrders.isEmpty()) {
        cout << "No orders currently in progress.\n";
        return;
    }

    Order* order = inProgressOrders.dequeue();

    // Locate the robot assigned to this order
    Robot* temp          = robotHead;
    Robot* assignedRobot = nullptr;
    do {
        if (temp->getID() == order->assignedRobotID) { assignedRobot = temp; break; }
        temp = temp->nextRobot;
    } while (temp != robotHead);

    if (assignedRobot == nullptr) {
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

    // Save trip to history BEFORE goBack() clears the stack
    assignedRobot->saveNavRecord(order->orderID, order->itemName);

    // Show the forward path log (reverse path is shown by goBack() below).
    // Using printForwardPath() avoids printing the reverse path twice —
    // goBack() already shows each reverse step during live execution.
    assignedRobot->printForwardPath();

    // Reverse navigation — goBack() pops each Step and prints its backAction,
    // showing the reverse path live as the robot returns to base.
    cout << "\n[Reverse Navigation]\n";
    assignedRobot->goBack();



    order->status = "Completed";
    completedOrders.push(order);

    cout << "\nOrder #" << order->orderID << " completed!\n";
    cout << "===================================================\n";
}

// ===================== Manual Navigation Simulation =====================
// Lets the user manually drive a robot step by step using keyboard commands.
// Demonstrates the stack data structure directly - each command pushes
// a Step node (with its reverse), and 'O' triggers obstacle backtracking
// by calling popStep(). At the end the full forward and reverse paths
// are shown, satisfying the "simulate robot movement" requirement.
void manualNavigationSimulation() {
    if (robotHead == nullptr) {
        cout << "No robots in the system.\n";
        return;
    }

    cout << "\n===== MANUAL NAVIGATION SIMULATION =====\n";
    cout << "Drive a robot step by step and see the path stack build up.\n";
    cout << "Use 'O' to simulate an obstacle - the last step is backtracked.\n\n";

    // Show available robots
    Robot* temp = robotHead;
    do {
        cout << "  Robot [" << temp->getID() << "] - " << temp->getStatus() << "\n";
        temp = temp->nextRobot;
    } while (temp != robotHead);

    cout << "\nEnter Robot ID: ";
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

    if (target == nullptr) { cout << "Robot [" << robotID << "] not found.\n"; return; }
    if (target->getStatus() == "Busy") {
        cout << "Robot [" << robotID << "] is currently Busy.\n"; return;
    }
    if (target->getStatus() == "Maintenance") {
        cout << "Robot [" << robotID << "] is under Maintenance.\n"; return;
    }

    // Clear any leftover path from a previous simulation
    while (target->getStack() != nullptr) target->popStep();

    cout << "\nRobot [" << robotID << "] ready.\n";
    cout << "Commands: F(orward)  B(ackward)  L(eft)  R(ight)\n";
    cout << "          O(bstacle - backtrack last step)  D(one)\n\n";

    int stepCount = 0;

    while (true) {
        cout << "  [" << stepCount << " step(s) recorded] > ";
        getline(cin, input);

        if (input.empty()) { cout << "  Use F / B / L / R / O / D\n"; continue; }

        char cmd = (char)toupper((unsigned char)input[0]);

        if (cmd == 'F') {
            target->pushStep("Move Forward", "Move Backward");
            cout << "  -> Moved Forward  (reverse: Move Backward)\n";
            stepCount++;
        } else if (cmd == 'B') {
            target->pushStep("Move Backward", "Move Forward");
            cout << "  -> Moved Backward  (reverse: Move Forward)\n";
            stepCount++;
        } else if (cmd == 'L') {
            target->pushStep("Turn Left", "Turn Right");
            cout << "  -> Turned Left  (reverse: Turn Right)\n";
            stepCount++;
        } else if (cmd == 'R') {
            target->pushStep("Turn Right", "Turn Left");
            cout << "  -> Turned Right  (reverse: Turn Left)\n";
            stepCount++;
        } else if (cmd == 'O') {
            // Obstacle: pop the last step (backtrack)
            if (target->getStack() == nullptr) {
                cout << "  No steps recorded yet - nothing to backtrack.\n";
            } else {
                cout << "  [!] Obstacle! Backtracking: "
                     << target->getStack()->backAction << "\n";
                target->popStep();
                if (stepCount > 0) stepCount--;
                cout << "  -> Robot backtracked. " << stepCount << " step(s) remaining.\n";
            }
        } else if (cmd == 'D') {
            break;
        } else {
            cout << "  Unknown command. Use F / B / L / R / O / D\n";
        }
    }

    if (target->getStack() == nullptr) {
        cout << "\nNo steps recorded. Simulation ended.\n";
        return;
    }

    cout << "\n" << stepCount << " step(s) recorded for Robot [" << robotID << "].\n";

    // Show the complete forward and reverse path
    target->printNavigationLog();

    // Ask whether to execute the return journey
    cout << "\nExecute reverse journey now? (Y/N): ";
    getline(cin, input);
    if (!input.empty() && (char)toupper((unsigned char)input[0]) == 'Y') {
        cout << "\n[Reverse Navigation]\n";
        target->goBack();
    } else {
        // Clear the stack without executing goBack()
        while (target->getStack() != nullptr) target->popStep();
        cout << "Return skipped. Stack cleared.\n";
    }
    cout << "=========================================\n";
}

// ===================== View Navigation Log =====================
// Shows the current stack contents for any robot (non-destructive).
void viewRobotNavigationLog() {
    if (robotHead == nullptr) { cout << "No robots in the system.\n"; return; }

    cout << "\n===== ROBOT STATUS =====\n";
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

    if (target == nullptr) { cout << "Robot [" << robotID << "] not found.\n"; return; }

    // displayNavHistory() reads from the NavRecord linked list which
    // persists across trips - unlike the stack which is cleared by goBack()
    target->displayNavHistory();
}

// ===================== View All Navigation History =====================
// Shows the full NavRecord linked list for every robot that has
// completed at least one order or simulation.
void viewAllNavigationHistory() {
    if (robotHead == nullptr) { cout << "No robots in the system.\n"; return; }

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

    if (!anyHistory) cout << "  No history yet. Complete an order first.\n";

    cout << "====================================================\n";
}
