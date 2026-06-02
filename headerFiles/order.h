#ifndef ORDER_H
#define ORDER_H
#include <iostream>
#include <string>

using namespace std;

struct Order {
    int orderID;
    string itemName;
    int assignedRobotID;
    string status;
    Order* next;

    Order(int id, string item) {
        orderID = id;
        itemName = item;
        assignedRobotID = -1;
        status = "Pending";
        next = nullptr;
    }
};

// Queue for Pending and In-Progress orders (FIFO)
class OrderQueue {
    private:
        Order* front;
        Order* rear;
        int count;
        int maxCapacity;    // 0 = unlimited
    public:
        OrderQueue(int max = 0);
        bool enqueue(Order* order);
        Order* dequeue();
        void display();
        bool isEmpty();
        bool isFull();
        int getCount();
};

// Stack for Completed orders (LIFO - most recent completion on top)
class CompletedStack {
    private:
        Order* top;
        int count;
    public:
        CompletedStack();
        void push(Order* order);
        void display();
        bool isEmpty();
        int getCount();
};

// Global order state (defined in orderManagement.cpp)
extern OrderQueue pendingOrders;
extern OrderQueue inProgressOrders;
extern CompletedStack completedOrders;
extern int orderCounter;

// Order management functions (defined in orderManagement.cpp)
void addOrder();
void assignOrders();
void completeOrder();
void viewOrderStatus();

#endif
