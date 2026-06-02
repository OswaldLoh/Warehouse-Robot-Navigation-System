// Task 3 - Robot Navigation & Path Tracking Module
// File: robotNavigation.cpp
//
// Handles all robot movement in the warehouse.
// Uses two data structures:
//   1. Stack (linked list of Steps) - records forward path; LIFO order means
//      the last step pushed is the first one reversed on the return journey.
//   2. NavRecord linked list - one record per completed trip, appended to each
//      robot's history so previous paths are never lost after goBack().

#include <iostream>
#include <string>
#include <cctype>
#include "headerFiles/robot.h"
#include "headerFiles/order.h"
#include "headerFiles/warehouse.h"
#include "headerFiles/navigation.h"

using namespace std;

// ===================== Stack Operations =====================
// The navigation stack uses a singly-linked list of Step nodes.
// Push and pop are both O(1) - efficient for real-time navigation.

// Add a new movement step on top of the stack
void Robot::pushStep(string fAction, string bAction) {
    Step* newStep  = new Step(fAction, bAction);
    newStep->below = stackTop;
    stackTop       = newStep;
}

// Remove the top step and free its memory
void Robot::popStep() {
    if (stackTop != nullptr) {
        Step* temp = stackTop;
        stackTop   = stackTop->below;
        delete temp;
    }
}

int Robot::getNavHistoryCount() { return navHistoryCount; }

// ===================== Path Display =====================
// Stack is LIFO so the top node = last step taken.
// We collect pointers into a local array to print in the right order.

// Show the forward path in chronological order (first step to last)
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

    // steps[count-1] = bottom of stack = first step taken
    for (int i = count - 1; i >= 0; i--)
        cout << "  Step " << (count - i) << ": " << steps[i]->forwardAction << "\n";
}

// Show the reverse path — the order goBack() will execute it
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

    // steps[0] = top of stack = last step taken, so it is reversed first
    for (int i = 0; i < count; i++)
        cout << "  Step " << (i + 1) << ": " << steps[i]->backAction << "\n";
}

// Print both paths together as a complete round-trip log
void Robot::printNavigationLog() {
    cout << "\n============= NAVIGATION LOG: Robot [" << ID << "] ============\n";
    printForwardPath();
    printReversePath();
    cout << "====================================================\n";
}

// ===================== Navigation History (NavRecord List) =====================
// saveNavRecord() snapshots the current stack into a NavRecord node and
// appends it to this robot's singly-linked history list.
// It MUST be called before goBack() because goBack() pops and frees all Steps.

void Robot::saveNavRecord(int orderID, const string& itemName) {
    NavRecord* record = new NavRecord(orderID, itemName);

    // Collect stack pointers (index 0 = top = last step taken)
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

    // Append to tail so trips stay in arrival order
    if (navHistoryHead == nullptr) {
        navHistoryHead = record;
    } else {
        NavRecord* tail = navHistoryHead;
        while (tail->next != nullptr) tail = tail->next;
        tail->next = record;
    }
    navHistoryCount++;
}

// Print all trips stored in this robot's NavRecord linked list
void Robot::displayNavHistory() {
    if (navHistoryHead == nullptr) {
        cout << "  Robot [" << ID << "] has no navigation history yet.\n";
        return;
    }
    cout << "\n========== NAVIGATION HISTORY - Robot [" << ID << "] ==========\n";

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
    cout << "\n====================================================\n";
}

// ===================== Return to Base =====================
// Pops every Step node and executes its backAction.
// Because the stack is LIFO, the last step taken is always undone first —
// this is exactly the correct order for retracing the path in reverse.
void Robot::goBack() {
    cout << "\nRobot [" << ID << "] returning to base...\n";

    if (stackTop == nullptr) {
        cout << "  (No path recorded - robot is already at base)\n";
        status = "Available";
        cout << "\nRobot [" << ID << "] is now Available.\n";
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

// Find an item across the warehouse hierarchy by name.
// Orders store the item name, so we need this to look up Zone/Aisle/Shelf.
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

// Build and push navigation steps using the item's real warehouse location.
//
// Obstacle simulation: if the item is in Aisle 2, an obstacle is detected.
//   The robot backtracks (popStep) and reroutes via Aisle 1, demonstrating
//   how the stack supports backtracking without losing previous steps.
//
// Returns false if the item is not in the database (order marked Failed).
static bool navigateRobot(Robot* robot, const string& itemName) {
    Item* item = findItemByName(getWarehouseHead(), itemName);

    if (item == nullptr) {
        cout << "[!] Item \"" << itemName << "\" not found in warehouse database.\n";
        cout << "    Robot stays at base. No movement recorded.\n";
        return false;
    }

    string zoneName  = "Zone ";  zoneName += item->zoneID;
    string aisleName = "Aisle " + to_string(item->aisleID);
    string shelfName = "Shelf " + to_string(item->shelfID);

    // Step 1: navigate to the correct zone
    cout << "  Step 1: Navigate to " << zoneName << "\n";
    robot->pushStep("Navigate to " + zoneName, "Return from " + zoneName);

    if (item->aisleID == 2) {
        // Obstacle in Aisle 2 — pop the last step and reroute via Aisle 1
        cout << "  Step 2: Attempting to enter " << aisleName << "...\n";
        cout << "  [!] OBSTACLE detected in " << aisleName << "! Backtracking...\n";
        cout << "  Backtrack: " << robot->getStack()->backAction << "\n";
        robot->popStep();

        cout << "  Re-entering " << zoneName << " via alternate corridor\n";
        robot->pushStep("Navigate to " + zoneName + " (alt corridor)",
                        "Return from "  + zoneName + " (alt corridor)");

        cout << "  Step 2 (alt): Navigate to Aisle 1 (detour)\n";
        robot->pushStep("Navigate to Aisle 1 (detour)", "Return from Aisle 1 (detour)");

        cout << "  Step 3: Cross to " << aisleName << " via Aisle 1\n";
        robot->pushStep("Cross to " + aisleName + " via Aisle 1",
                        "Return via Aisle 1 to " + zoneName);
    } else {
        // Direct path — no obstacle
        cout << "  Step 2: Navigate to " << aisleName << "\n";
        robot->pushStep("Navigate to " + aisleName, "Return from " + aisleName);
    }

    // Count actual steps pushed so the printed step number is always correct
    int nextStep = 0;
    Step* temp = robot->getStack();
    while (temp != nullptr) { nextStep++; temp = temp->below; }
    nextStep++;

    cout << "  Step " << nextStep << ": Navigate to " << shelfName << "\n";
    robot->pushStep("Navigate to " + shelfName, "Return from " + shelfName);

    return true;
}

// ===================== Complete Order =====================
// Full navigation cycle for one in-progress order:
//   Forward path -> obstacle handling -> pick up item ->
//   save history -> show forward log -> reverse path -> mark Completed
void completeOrder() {
    if (inProgressOrders.isEmpty()) {
        cout << "No orders currently in progress.\n";
        return;
    }

    Order* order = inProgressOrders.dequeue();

    // Find the robot assigned to this order in the circular linked list
    Robot* temp          = robotHead;
    Robot* assignedRobot = nullptr;
    do {
        if (temp->getID() == order->assignedRobotID) { assignedRobot = temp; break; }
        temp = temp->nextRobot;
    } while (temp != robotHead);

    if (assignedRobot == nullptr) {
        // Safety check — robot was removed from system somehow
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
    cout << "====================================================\n";

    // Forward navigation — build the path on the stack
    cout << "\n[Forward Navigation]\n";
    bool itemFound = navigateRobot(assignedRobot, order->itemName);

    if (!itemFound) {
        // Item not in database — robot never moved, free it immediately
        assignedRobot->setStatus("Available");
        order->status = "Failed";
        completedOrders.push(order);
        cout << "\nOrder #" << order->orderID
             << " marked as Failed (item not in database).\n"
             << "Robot [" << assignedRobot->getID() << "] is now Available.\n";
        cout << "====================================================\n";
        return;
    }

    cout << "\n[!] Robot [" << assignedRobot->getID()
         << "] reached item: \"" << order->itemName << "\" - Picking up...\n";

    // Save trip to history BEFORE goBack() clears the stack
    assignedRobot->saveNavRecord(order->orderID, order->itemName);

    // Display the stored forward path (non-destructive read of stack)
    assignedRobot->printForwardPath();

    // Reverse navigation — LIFO pops each Step and executes its backAction
    cout << "\n[Reverse Navigation]\n";
    assignedRobot->goBack();

    order->status = "Completed";
    completedOrders.push(order);

    cout << "\nOrder #" << order->orderID << " completed!\n";
    cout << "====================================================\n";
}

// ===================== Manual Navigation Simulation =====================
// Lets the user manually drive a robot step by step using keyboard commands.
// Each command pushes a Step node onto the stack with its matching reverse.
// 'O' simulates an obstacle by calling popStep() to backtrack one step.
// At the end, the full forward and reverse path are shown from the stack,
// and the user can choose to execute the return journey with goBack().
void manualNavigationSimulation() {
    if (robotHead == nullptr) {
        cout << "No robots in the system.\n";
        return;
    }

    cout << "\n=========== MANUAL NAVIGATION SIMULATION ===========\n";
    cout << "Drive a robot step by step and watch the path stack build up.\n";
    cout << "Use 'O' to simulate an obstacle - the last step is backtracked.\n\n";

    // Show all robots and their current status
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

    // Clear any leftover steps from a previous simulation
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
            // Obstacle: pop the top step (backtrack) and show what was undone
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

    // Show the recorded forward path (non-destructive stack read)
    // Use printForwardPath() only — goBack() will show the reverse during execution
    target->printForwardPath();

    // Save this simulation trip to NavRecord history BEFORE goBack() clears the stack.
    // orderID = 0 and itemName = "Manual Simulation" since there is no real order.
    target->saveNavRecord(0, "Manual Simulation");

    // Let the user choose whether to execute the return journey
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
    cout << "====================================================\n";
}

// ===================== View Navigation Log =====================
// Shows the NavRecord history for a chosen robot (non-destructive).
void viewRobotNavigationLog() {
    if (robotHead == nullptr) { cout << "No robots in the system.\n"; return; }

    cout << "=================== ROBOT STATUS ===================\n";
    Robot* temp = robotHead;
    do {
        cout << "Robot [" << temp->getID() << "] - "
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

    // displayNavHistory() reads the NavRecord linked list, which persists
    // across trips unlike the stack (which is cleared by goBack())
    target->displayNavHistory();
}

// ===================== View All Navigation History =====================
// Shows the full NavRecord list for every robot that has completed a trip.
// This satisfies the "Complete Navigation Log" expected output in the spec.
void viewAllNavigationHistory() {
    if (robotHead == nullptr) { cout << "No robots in the system.\n"; return; }

    cout << "\n=========== COMPLETE NAVIGATION HISTORY  ===========\n";

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
