#pragma once
#include "Seat.h"
#include <cmath>

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
    
    Person(float startX, float startY, float speed = 0.008f, int delayFrames = 0)
        : x(startX), y(startY), targetX(0), targetY(0), intermediateX(0), intermediateY(0), speed(speed),
          isMoving(false), isSeated(false), isExiting(false), reachedIntermediate(false), assignedSeat(nullptr),
          delayFrames(delayFrames), currentFrame(0) {}
    
    void setTarget(Seat* seat) {
        if (seat) {
            targetX = seat->x;
            targetY = seat->y;
            assignedSeat = seat;
            isSeated = false;
        }
    }
    
    void update() {
        currentFrame++;
        if (currentFrame < delayFrames) {
            return;
        }
        
        if (isExiting) {
            if (!isMoving && currentFrame >= delayFrames) {
                isMoving = true;
            }
            if (!isMoving) return;
            
            if (!reachedIntermediate) {
                float dx = intermediateX - x;
                float dy = intermediateY - y;
                
                if (fabs(dx) > 0.01f) {
                    if (dx > 0) x += speed;
                    else x -= speed;
                    if (fabs(dx) < speed * 2) x = intermediateX;
                } else {
                    x = intermediateX;
                    y = intermediateY;
                    reachedIntermediate = true;
                }
            } else {
                float dx = targetX - x;
                float dy = targetY - y;
                
                if (fabs(dy) > 0.01f) {
                    if (dy > 0) y += speed;
                    else y -= speed;
                    if (fabs(dy) < speed * 2) y = targetY;
                } else {
                    y = targetY;
                    x = targetX;
                    isMoving = false;
                }
            }
        } else {
            if (!isMoving && assignedSeat) {
                isMoving = true;
            }
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
    }
    
    void startExiting(float exitX, float exitY) {
        if (assignedSeat) {
            intermediateX = exitX;
            intermediateY = assignedSeat->y;
            targetX = exitX;
            targetY = exitY;
        } else {
            intermediateX = exitX;
            intermediateY = y;
            targetX = exitX;
            targetY = exitY;
        }
        isSeated = false;
        isExiting = true;
        reachedIntermediate = false;
    }
};
