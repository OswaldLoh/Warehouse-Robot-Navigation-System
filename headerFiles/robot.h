#ifndef ROBOT_H
#define ROBOT_H

// Shared header used by Task 2 (robotManagement.cpp) and Task 3 (robotNavigation.cpp)
// Declares the Step struct, NavRecord struct, and Robot class.

#include <iostream>
#include <string>

// One node in the navigation stack.
// Stores both the forward action and its reverse so the robot
// can retrace its path without any extra calculation.
struct Step {
    std::string forwardAction;
    std::string backAction;
    Step*       below;          // next node down in the stack

    Step(std::string fAction, std::string bAction) {
        forwardAction = fAction;
        backAction    = bAction;
        below         = nullptr;
    }
};

// One node in the navigation history linked list.
// After each trip, a NavRecord is created from a snapshot of the
// navigation stack and appended to the robot's history list.
struct NavRecord {
    int         orderID;
    std::string itemName;
    std::string forwardSteps[50];   // forward path in chronological order
    std::string reverseSteps[50];   // reverse path in execution order
    int         stepCount;
    NavRecord*  next;

    NavRecord(int oID, const std::string& iName) {
        orderID   = oID;
        itemName  = iName;
        stepCount = 0;
        next      = nullptr;
    }
};

class Robot {
    private:
        int         ID;
        std::string status;             // "Available", "Busy", or "Maintenance"
        Step*       stackTop;           // top of the navigation stack
        NavRecord*  navHistoryHead;     // head of the navigation history list
        int         navHistoryCount;    // number of completed trips recorded

    public:
        Robot* nextRobot;               // circular list link (Task 2)

        Robot(int robotID);

        // Getters
        int         getID();
        std::string getStatus();
        Step*       getStack();
        int         getNavHistoryCount();

        // Setters
        void setStatus(std::string newStatus);
        void setStack(Step* newStack);

        // Stack operations - Task 3 (robotNavigation.cpp)
        void pushStep(std::string fAction, std::string bAction);
        void popStep();
        void goBack();
        void printForwardPath();
        void printReversePath();
        void printNavigationLog();

        // Navigation history - Task 3 (robotNavigation.cpp)
        void saveNavRecord(int orderID, const std::string& itemName);
        void displayNavHistory();
};

// Global robot state (defined in robotManagement.cpp)
extern Robot* robotHead;
extern Robot* currentRobot;
extern int    robotCount;

// Robot management functions (defined in robotManagement.cpp)
void   initRobots(int n);
Robot* findAvailableRobot();
void   displayRobotStatus();
void   robotMaintenance();

#endif
