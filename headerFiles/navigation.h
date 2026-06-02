#ifndef NAVIGATION_H
#define NAVIGATION_H

// Task 3 - Robot Navigation & Path Tracking Module
// Declares public navigation functions implemented in robotNavigation.cpp.

// Run the full navigation cycle for the next in-progress order
void completeOrder();

// Manually drive a robot step by step (F/B/L/R) and simulate obstacles (O)
void manualNavigationSimulation();

// Show the current navigation stack (active path) for a chosen robot
void viewRobotNavigationLog();

// Show the full navigation history (all completed trips) for all robots
void viewAllNavigationHistory();


#endif
