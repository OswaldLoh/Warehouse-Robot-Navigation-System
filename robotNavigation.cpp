// Task 3 - Robot Navigation & Path Tracking Module
// TP085239 Ng Ern Chi
//
// Handles all robot movement in the warehouse.
// Uses two data structures:
//   1. Stack (linked list of Steps) - records forward path, reversed on return
//   2. NavRecord linked list - stores completed trip history per robot

#include <iostream>
#include <string>
#include <cctype>
#include "robot.h"
#include "order.h"
#include "warehouse.h"
#include "graph.h"       // Warehouse graph + Dijkstra
#include "navigation.h"

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
    cout << "\n============= NAVIGATION LOG: Robot [" << ID << "] ============\n";
    printForwardPath();
    printReversePath();
    cout << "====================================================\n";
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
// Pops every step and executes its backAction.
// LIFO ensures the last step taken is always undone first.
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

// Build and push navigation steps using Dijkstra's shortest path.
//
// Obstacle simulation: for paths with 4+ nodes (3+ hops), an obstacle
// is detected at the 2nd hop. The robot backtracks (popStep) then
// re-routes via alternate corridor, demonstrating stack backtracking
// on a real computed path.
//
// Returns false if the item is not in the database (order marked Failed).
static bool navigateRobot(Robot* robot, const string& itemName) {
    Item* item = findItemByName(getWarehouseHead(), itemName);

    if (item == nullptr) {
        cout << "[!] Item \"" << itemName << "\" not found in the warehouse database.\n";
        cout << "    Robot stays at base. No movement recorded.\n";
        return false;
    }

    // Convert item's physical location to its graph node ID
    int targetNode = locationToNode(item->zoneID, item->aisleID, item->shelfID);

    // Run Dijkstra from Base (node 0) to the target shelf node
    int path[GRAPH_NODES];
    int pathLen = dijkstra(BASE_NODE, targetNode, path);

    if (pathLen == 0) {
        cout << "[!] Dijkstra: no path found to " << nodeLabel(targetNode) << ".\n";
        return false;
    }

    cout << " Target : " << nodeLabel(targetNode) << "\n";

    // Obstacle simulation: trigger for paths with 4+ nodes (3+ hops).
    // At the 2nd hop, an obstacle is detected — robot backtracks 1 step
    // via popStep() then re-routes via alternate corridor to the same node.
    bool obstacleSimulated = (pathLen >= 4);
    bool obstacleHandled   = false;
    int  stepNum           = 1;

    for (int i = 1; i < pathLen; i++) {
        string fwd = "Move to "     + nodeLabel(path[i]);
        string bwd = "Return from " + nodeLabel(path[i]);

        if (obstacleSimulated && !obstacleHandled && i == 2) {
            // Obstacle at path[2] — demonstrate backtracking
            cout << "  Step " << stepNum << ": Attempting " << fwd << "...\n";
            cout << "  [!] OBSTACLE detected at " << nodeLabel(path[i]) << "!\n";
            cout << "  Robot holds position at " << nodeLabel(path[i - 1]) << ".\n";

            // Re-route: take alternate corridor to reach the same node
            string altFwd = "Move to " + nodeLabel(path[i]) + " (alt route)";
            string altBwd = "Return from " + nodeLabel(path[i]) + " (alt route)";
            cout << "  Re-routing via alternate corridor\n";
            cout << "  Step " << stepNum++ << ": " << altFwd << "\n";
            robot->pushStep(altFwd, altBwd);
            obstacleHandled = true;
        } else {
            cout << "  Step " << stepNum++ << ": " << fwd << "\n";
            robot->pushStep(fwd, bwd);
        }
    }

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
    cout << "====================================================\n";

    // Forward navigation
    cout << "\n[Forward Navigation]\n";
    bool itemFound = navigateRobot(assignedRobot, order->itemName);

    if (!itemFound) {
        // Item does not exist - robot never moved, free it immediately
        assignedRobot->setStatus("Available");
        order->status = "Failed";
        completedOrders.push(order);
        cout << "\nOrder #" << order->orderID
             << " marked as Failed (item not in database). \nRobot ["
             << assignedRobot->getID() << "] is now Available.\n";
        cout << "====================================================\n";
        return;
    }

    cout << "\n[!] Robot [" << assignedRobot->getID() << "] reached item: \"" << order->itemName << "\""
            << " - Picking up item...\n";

    // Save trip to history BEFORE goBack() clears the stack
    assignedRobot->saveNavRecord(order->orderID, order->itemName);

    // Reverse navigation — goBack() pops each Step and executes its backAction
    cout << "\n[Reverse Navigation]\n";
    assignedRobot->goBack();



    order->status = "Completed";
    completedOrders.push(order);

    cout << "\nOrder #" << order->orderID << " completed!\n";
    cout << "====================================================\n";
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

    cout << "\n=========== MANUAL NAVIGATION SIMULATION ===========\n";
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

    // Show the stored forward path only — goBack() shows the reverse during execution.
    // printForwardPath() is used here (not printNavigationLog) to avoid showing
    // the reverse path twice: once as a preview and again during goBack() execution.
    target->printForwardPath();

    // Save this simulation trip to NavRecord history BEFORE goBack() clears the stack.
    // orderID = 0, itemName = "Manual Simulation" since there is no real order.
    target->saveNavRecord(0, "Manual Simulation");

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
    cout << "====================================================\n";
}

// ===================== View Navigation Log =====================
// Shows the current stack contents for any robot (non-destructive).
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

    // displayNavHistory() reads from the NavRecord linked list which
    // persists across trips - unlike the stack which is cleared by goBack()
    target->displayNavHistory();
}

// ===================== View All Navigation History =====================
// Shows the full NavRecord linked list for every robot that has
// completed at least one order or simulation.
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

}
