#pragma once
#include "Seat.h"

struct Person {
    float x, y;
    float targetX, targetY;
    float intermediateX, intermediateY;
    float speed;
    bool isMoving;
    bool isSeated;
    bool isExiting;
    bool reachedIntermediate;
    Seat* assignedSeat;
    int delayFrames;
    int currentFrame;
    
    Person(float startX, float startY, float speed = 0.008f, int delayFrames = 0);
    
    void setTarget(Seat* seat);
    void update();
    void startExiting(float exitX, float exitY);
};
