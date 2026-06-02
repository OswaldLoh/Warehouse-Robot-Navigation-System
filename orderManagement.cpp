#include <iostream>
#include <string>
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
        cout << "  Order #" << temp->orderID
             << " | Item: " << temp->itemName;
        if (temp->assignedRobotID != -1)
            cout << " | Robot [" << temp->assignedRobotID << "]";
        cout << " | Status: " << temp->status << "\n";
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
        cout << "  Order #" << temp->orderID
             << " | Item: " << temp->itemName
             << " | Robot [" << temp->assignedRobotID << "]"
             << " | Status: " << temp->status << "\n";
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

// Complete the next in-progress order (FIFO)
void completeOrder() {
    if (inProgressOrders.isEmpty()) {
        cout << "No orders currently in progress.\n";
        return;
    }

    Order* order = inProgressOrders.dequeue();

    // Find the assigned robot in the circular list
    Robot* temp = robotHead;
    Robot* assignedRobot = nullptr;
    do {
        if (temp->getID() == order->assignedRobotID) {
            assignedRobot = temp;
            break;
        }
        temp = temp->nextRobot;
    } while (temp != robotHead);

    if (assignedRobot != nullptr) {
        // Simulate robot navigation (push steps onto the stack)
        cout << "\n--- Navigating Robot [" << assignedRobot->getID()
             << "] for Order #" << order->orderID << " ---\n";

        assignedRobot->pushStep("Move Forward", "Move Backward");
        cout << "Step: Move Forward\n";
        assignedRobot->pushStep("Turn Left", "Turn Right");
        cout << "Step: Turn Left\n";
        assignedRobot->pushStep("Move Forward", "Move Backward");
        cout << "Step: Move Forward\n";

        cout << "Robot [" << assignedRobot->getID()
             << "] reached item: \"" << order->itemName << "\"\n";
        cout << "Picking up item...\n\n";

        // Robot returns using reverse path (stack pop)
        assignedRobot->goBack();
    }

    // Move order to completed stack
    order->status = "Completed";
    completedOrders.push(order);

    cout << "Order #" << order->orderID << " has been completed!\n";
}

// Display all order statuses across the 3 data structures
void viewOrderStatus() {
    cout << "\n=============== ORDER STATUS ===============\n";

    cout << "\n[PENDING] (" << pendingOrders.getCount() << " orders)\n";
    if (pendingOrders.isEmpty()) cout << "  (none)\n";
    else pendingOrders.display();

    cout << "\n[IN PROGRESS] (" << inProgressOrders.getCount() << " orders)\n";
    if (inProgressOrders.isEmpty()) cout << "  (none)\n";
    else inProgressOrders.display();

    cout << "\n[COMPLETED] (" << completedOrders.getCount() << " orders)\n";
    if (completedOrders.isEmpty()) cout << "  (none)\n";
    else completedOrders.display();

    cout << "\n============================================\n";
}
