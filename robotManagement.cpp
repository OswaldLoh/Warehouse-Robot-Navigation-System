#include <iostream>
#include <string>
#include "headerFiles/robot.h"

using namespace std;

// Constructor
Robot::Robot(int robotID) {
    this->ID = robotID;
    this->status = "Available";
    this->stackTop = nullptr;
    this->nextRobot = nullptr;
};

// Getters
int Robot::getID() { return ID; }
string Robot::getStatus() { return status; }
Step* Robot::getStack() { return stackTop; }

// Setters
void Robot:: setStatus(string newStatus) { status = newStatus; }
void Robot:: setStack(Step* newStack) { stackTop = newStack; }

// Methods
void Robot::pushStep(string fAction, string bAction) {  // Add a traversal step onto the stack
    Step* newStep = new Step(fAction, bAction);
    newStep->below = stackTop;       // When adding a new step, the below of new step is the current top
    stackTop = newStep;             // So now the top of stack is the new step
}

void Robot::popStep() {             // Delete the top traversal step on the stack
    if (stackTop != nullptr) {
        Step* temp = stackTop;
        stackTop = stackTop->below;
        delete temp;
    }
}

void Robot::goBack() {              // Robot execute return path by retracing the stack
    cout << "Robot [" << ID << "] is returning to base...\n";
    while (stackTop != nullptr) {
        std::cout << "Executing: " << stackTop->backAction << "\n";
        popStep();
    }
    status = "Available";
    cout << "Robot [" << ID << "] is now Available.\n";
}

// ==================== Circular Robot Queue ====================
Robot* robotHead = nullptr;         // First robot in the circular list
Robot* currentRobot = nullptr;      // Pointer for round-robin rotation
int robotCount = 0;

// Initialize robots in a circular linked list
void initRobots(int n) {
    Robot* first = nullptr;
    Robot* prev = nullptr;

    for (int i = 1; i <= n; i++) {
        Robot* newRobot = new Robot(i);
        if (first == nullptr) {
            first = newRobot;
        } else {
            prev->nextRobot = newRobot;
        }
        prev = newRobot;
        robotCount++;
    }
    prev->nextRobot = first;           // Close the circular link
    robotHead = first;
    currentRobot = first;
}

// Find the next available robot using circular rotation
Robot* findAvailableRobot() {
    if (robotHead == nullptr) return nullptr;

    Robot* start = currentRobot;
    do {
        if (currentRobot->getStatus() == "Available") {
            Robot* assigned = currentRobot;
            currentRobot = currentRobot->nextRobot;    // Advance for next assignment
            return assigned;
        }
        currentRobot = currentRobot->nextRobot;
    } while (currentRobot != start);

    return nullptr;                     // All robots are busy
}

// Display all robot statuses (circular traversal)
void displayRobotStatus() {
    if (robotHead == nullptr) {
        cout << "No robots in the system.\n";
        return;
    }

    cout << "\n===== ROBOT STATUS =====\n";
    Robot* temp = robotHead;
    do {
        cout << "  Robot [" << temp->getID() << "] - " << temp->getStatus() << "\n";
        temp = temp->nextRobot;
    } while (temp != robotHead);
    cout << "========================\n";
}

// Robot Maintenance sub-menu
void robotMaintenance() {
    if (robotHead == nullptr) {
        cout << "No robots in the system.\n";
        return;
    }

    string input;
    int subChoice;

    do {
        cout << "\n===== ROBOT MAINTENANCE =====\n";
        cout << "1. Put Robot to Maintenance\n";
        cout << "2. Back from Maintenance\n";
        cout << "3. Back to Main Menu\n";
        cout << "=============================\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input.length() != 1 || input[0] < '1' || input[0] > '3') {
            cout << "\nInvalid input. Please enter 1, 2, or 3.\n";
            continue;
        }

        subChoice = input[0] - '0';

        if (subChoice == 3) break;

        // Show current robot statuses
        displayRobotStatus();

        cout << "Enter Robot ID: ";
        string idInput;
        getline(cin, idInput);

        // Validate robot ID input
        bool validID = true;
        for (int i = 0; i < (int)idInput.length(); i++) {
            if (idInput[i] < '0' || idInput[i] > '9') {
                validID = false;
                break;
            }
        }
        if (idInput.empty() || !validID) {
            cout << "Invalid Robot ID.\n";
            continue;
        }

        int robotID = stoi(idInput);

        // Find the robot in the circular list
        Robot* temp = robotHead;
        Robot* target = nullptr;
        do {
            if (temp->getID() == robotID) {
                target = temp;
                break;
            }
            temp = temp->nextRobot;
        } while (temp != robotHead);

        if (target == nullptr) {
            cout << "Robot [" << robotID << "] not found.\n";
            continue;
        }

        if (subChoice == 1) {
            // Put robot to maintenance
            if (target->getStatus() == "Maintenance") {
                cout << "Robot [" << robotID << "] is already under Maintenance.\n";
            } else if (target->getStatus() == "Busy") {
                cout << "Robot [" << robotID << "] is currently Busy. Complete its task first.\n";
            } else {
                target->setStatus("Maintenance");
                cout << "Robot [" << robotID << "] has been put under Maintenance.\n";
            }
        } else if (subChoice == 2) {
            // Back from maintenance
            if (target->getStatus() != "Maintenance") {
                cout << "Robot [" << robotID << "] is not under Maintenance (current status: "
                     << target->getStatus() << ").\n";
            } else {
                target->setStatus("Available");
                cout << "Robot [" << robotID << "] is now back to Available.\n";
            }
        }
    } while (true);
}
