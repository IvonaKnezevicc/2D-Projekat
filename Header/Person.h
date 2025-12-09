#pragma once
#include "Seat.h"
#include <cmath>

struct Person {
    float x, y;
    float targetX, targetY;
    float speed;
    bool isMoving;
    bool isSeated;
    bool isExiting;
    Seat* assignedSeat;
    
    Person(float startX, float startY, float speed = 0.01f)
        : x(startX), y(startY), targetX(0), targetY(0), speed(speed),
          isMoving(false), isSeated(false), isExiting(false), assignedSeat(nullptr) {}
    
    void setTarget(Seat* seat) {
        if (seat) {
            targetX = seat->x;
            targetY = seat->y;
            assignedSeat = seat;
            isMoving = true;
            isSeated = false;
        }
    }
    
    void update() {
        if (!isMoving) return;
        
        float dx = targetX - x;
        float dy = targetY - y;
        
        if (fabs(dy) > 0.01f) {
            if (dy > 0) y += speed;
            else y -= speed;
            if (fabs(dy) < speed * 2) y = targetY;
        } else if (fabs(dx) > 0.01f) {
            if (dx > 0) x += speed;
            else x -= speed;
            if (fabs(dx) < speed * 2) x = targetX;
        } else {
            x = targetX;
            y = targetY;
            isMoving = false;
            isSeated = true;
        }
    }
    
    void startExiting(float exitX, float exitY) {
        targetX = exitX;
        targetY = exitY;
        isMoving = true;
        isSeated = false;
        isExiting = true;
    }
};
