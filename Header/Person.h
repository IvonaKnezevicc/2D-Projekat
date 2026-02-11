#pragma once
#include "Seat.h"

class Cinema;

struct Person {
    float x, y, z;  // Dodata z koordinata za 3D
    float targetX, targetY, targetZ;  // Dodata targetZ za 3D
    float intermediateX, intermediateY, intermediateZ;  // Dodata intermediateZ za 3D
    float speed;
    bool isMoving;
    bool isSeated;
    bool isExiting;
    bool reachedIntermediate;
    Seat* assignedSeat;
    int delayFrames;
    int currentFrame;
    
    Person(float startX, float startY, float startZ = 0.0f, float speed = 0.008f, int delayFrames = 0);
    
    void setTarget(Seat* seat, const Cinema& cinema);
    void update();
    void startExiting(float exitX, float exitY, float exitZ);
};
