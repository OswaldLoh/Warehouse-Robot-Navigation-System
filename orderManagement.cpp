// Task 1 - Order Management Module
//
// Manages customer orders using two custom data structures:
//   - OrderQueue  (FIFO linked list) - holds pending and in-progress orders
//   - CompletedStack (LIFO linked list) - holds completed orders, newest on top
#include <iostream>
#include <string>
#include <iomanip>
#include "headerFiles/order.h"
#include "headerFiles/robot.h"

using namespace std;

// ==================== OrderQueue (FIFO) ====================

// Constructor
OrderQueue::OrderQueue(int max) {
    this->front = nullptr;
    this->rear = nullptr;
    this->count = 0;
    this->maxCapacity = max;
}

bool OrderQueue::enqueue(Order* order) {        // Add order to the rear of the queue
    if (maxCapacity > 0 && count >= maxCapacity) {
        return false;                           // Queue is full
    }
    order->next = nullptr;
    if (rear == nullptr) {                      // Queue is empty
        front = order;
        rear = order;
    } else {
        rear->next = order;
        rear = order;
    }
    count++;
    return true;
}

Order* OrderQueue::dequeue() {                  // Remove and return order from the front
    if (front == nullptr) return nullptr;

    Order* temp = front;
    front = front->next;
    if (front == nullptr) rear = nullptr;       // Queue became empty
    temp->next = nullptr;
    count--;
    return temp;
}

void OrderQueue::display() {
    Order* temp = front;
    while (temp != nullptr) {
        string robotCol = (temp->assignedRobotID != -1)
                          ? "Robot [" + to_string(temp->assignedRobotID) + "]"
                          : "(unassigned)";
        cout << "  "
             << left
             << setw(10) << ("#" + to_string(temp->orderID))
             << setw(22) << temp->itemName
             << setw(16) << robotCol
             << temp->status << "\n";
        temp = temp->next;
    }
}

bool OrderQueue::isEmpty() { return front == nullptr; }
bool OrderQueue::isFull() { return maxCapacity > 0 && count >= maxCapacity; }
int OrderQueue::getCount() { return count; }

// ==================== CompletedStack (LIFO) ====================

// Constructor
CompletedStack::CompletedStack() {
    this->top = nullptr;
    this->count = 0;
}

void CompletedStack::push(Order* order) {       // Push completed order onto the stack
    order->next = top;
    top = order;
    count++;
}

void CompletedStack::display() {
    Order* temp = top;
    while (temp != nullptr) {
        string robotCol = (temp->assignedRobotID != -1)
                          ? "Robot [" + to_string(temp->assignedRobotID) + "]"
                          : "(unassigned)";
        cout << "  "
             << left
             << setw(10) << ("#" + to_string(temp->orderID))
             << setw(22) << temp->itemName
             << setw(16) << robotCol
             << temp->status << "\n";
        temp = temp->next;
    }
}

bool CompletedStack::isEmpty() { return top == nullptr; }
int CompletedStack::getCount() { return count; }

// ==================== Order State ====================

OrderQueue pendingOrders(5);         // Queue for PENDING ORDERS
OrderQueue inProgressOrders;        // Queue for PROCESSING ORDERS
CompletedStack completedOrders;     // Stack for COMPLETED ORDERS (LIFO)

int orderCounter = 0;               // Auto-incrementing order ID

// ==================== Order Functions ====================

// Add a new order to the pending queue
void addOrder() {
    if (pendingOrders.isFull()) {
        cout << "System overload! Pending order queue is full (max "
             << pendingOrders.getCount() << "). Please process existing orders first.\n";
        return;
    }

    string itemName;
    cout << "Enter item name: ";
    getline(cin, itemName);

    orderCounter++;
    Order* newOrder = new Order(orderCounter, itemName);
    if (!pendingOrders.enqueue(newOrder)) {
        cout << "Failed to add order. Queue is full.\n";
        delete newOrder;
        orderCounter--;
        return;
    }

    cout << "Order #" << orderCounter << " for \"" << itemName
         << "\" added to pending queue. (" << pendingOrders.getCount() << "/5)\n";
}

// Assign all pending orders to available robots
void assignOrders() {
    if (pendingOrders.isEmpty()) {
        cout << "No pending orders to assign.\n";
        return;
    }

    int assigned = 0;
    while (!pendingOrders.isEmpty()) {
        Robot* robot = findAvailableRobot();
        if (robot == nullptr) {
            cout << "No more available robots. Remaining orders stay in pending queue.\n";
            break;
        }

        Order* order = pendingOrders.dequeue();
        order->assignedRobotID = robot->getID();
        order->status = "In Progress";
        robot->setStatus("Busy");

        inProgressOrders.enqueue(order);
        assigned++;

        cout << "Order #" << order->orderID << " assigned to Robot [" << robot->getID() << "]\n";
    }

    if (assigned == 0)
        cout << "Could not assign any orders (all robots busy).\n";
}


// Shared table header printed above every order list
static void printTableHeader() {
    cout << "  "
         << left
         << setw(10) << "Order"
         << setw(22) << "Item"
         << setw(16) << "Robot"
         << "Status" << "\n";
    cout << "  " << string(60, '-') << "\n";
}

// Display all order statuses across the 3 data structures
void viewOrderStatus() {
    cout << "\n=================== ORDER STATUS ===================\n";

    cout << "\n[PENDING] (" << pendingOrders.getCount() << " orders)\n";
    if (pendingOrders.isEmpty()) cout << "  (none)\n";
    else { printTableHeader(); pendingOrders.display(); }

    cout << "\n[IN PROGRESS] (" << inProgressOrders.getCount() << " orders)\n";
    if (inProgressOrders.isEmpty()) cout << "  (none)\n";
    else { printTableHeader(); inProgressOrders.display(); }

    cout << "\n[COMPLETED / FAILED] (" << completedOrders.getCount() << " orders)\n";
    if (completedOrders.isEmpty()) cout << "  (none)\n";
    else { printTableHeader(); completedOrders.display(); }

    cout << "\n====================================================\n";
}

// Display only pending orders
void viewPendingOrders() {
    if (pendingOrders.isEmpty()) {
        cout << "No pending orders.\n";
        return;
    }
    cout << "================== PENDING ORDERS ==================\n";
    printTableHeader();
    pendingOrders.display();
    cout << "====================================================\n";
}

// Display only in-progress orders
void viewInProgressOrders() {
    if (inProgressOrders.isEmpty()) {
        cout << "No orders currently in progress.\n";
        return;
    }
    cout << "================ IN PROGRESS ORDERS ================\n";
    printTableHeader();
    inProgressOrders.display();
    cout << "====================================================\n";
}

// Display only completed / failed orders
void viewCompletedOrders() {
    if (completedOrders.isEmpty()) {
        cout << "No completed or failed orders yet.\n";
        return;
    }
    cout << "=========== COMPLETED / FAILED ORDERS  ===========\n";
    printTableHeader();
    completedOrders.display();
    cout << "====================================================\n";
}
