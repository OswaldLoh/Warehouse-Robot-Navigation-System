#ifndef ROBOT_H
#define ROBOT_H
#include <iostream>
#include <string>

using namespace std;

struct Step {
    string forwardAction;   
    string backAction;
    Step* below;

    Step(string fAction, string bAction) {
        forwardAction = fAction;
        backAction = bAction;
        below = nullptr;
    }
};

class Robot {
    private:
        int ID;
        string status;
        Step* stackTop;
    public:
        Robot* nextRobot;
        Robot(int robotID);
        int getID();
        string getStatus();
        Step* getStack();
        void setStatus(string newStatus);
        void setStack(Step* newStack);
        void pushStep(string fAction, string bAction);
        void popStep();
        void goBack();
};

// Global robot state (defined in robot.cpp)
extern Robot* robotHead;
extern Robot* currentRobot;
extern int robotCount;

// Robot management functions (defined in robot.cpp)
void initRobots(int n);
Robot* findAvailableRobot();
void displayRobotStatus();
void robotMaintenance();

#endif
