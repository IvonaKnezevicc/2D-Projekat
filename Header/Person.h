#pragma once
#include "Seat.h"

class Cinema;

struct Person {
    float x, y, z;  
    float targetX, targetY, targetZ;  
    float intermediateX, intermediateY, intermediateZ;  
    float speed;
    bool isMoving;
    bool isSeated;
    bool isExiting;
    bool reachedIntermediate;
    Seat* assignedSeat;
    int delayFrames;
    int currentFrame;
    int modelIndex;
    
    Person(float startX, float startY, float startZ = 0.0f, float speed = 0.008f, int delayFrames = 0, int modelIndex = 0);
    
    void setTarget(Seat* seat, const Cinema& cinema);
    void update();
    void startExiting(float exitX, float exitY, float exitZ);
};
