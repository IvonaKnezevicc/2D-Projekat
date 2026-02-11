#include "../Header/Person.h"
#include "../Header/Cinema.h"
#include <cmath>

Person::Person(float startX, float startY, float startZ, float speed, int delayFrames)
    : x(startX), y(startY), z(startZ), targetX(0), targetY(0), targetZ(0), intermediateX(0), intermediateY(0), intermediateZ(0), speed(speed),
      isMoving(false), isSeated(false), isExiting(false), reachedIntermediate(false), assignedSeat(nullptr),
      delayFrames(delayFrames), currentFrame(0) {}

void Person::setTarget(Seat* seat, const Cinema& cinema) {
    if (seat) {
        assignedSeat = seat;
        isSeated = false;
        
        targetX = seat->x;
        targetY = seat->y;
        targetZ = seat->z;
        
        bool useLeftStair = seat->x < 0.0f;
        intermediateX = cinema.getStairX(useLeftStair);
        intermediateY = seat->y;
        intermediateZ = cinema.getStairZ(seat->row);
        
        reachedIntermediate = false;
    }
}

void Person::update() {
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
            float dz = intermediateZ - z;
            
            if (fabs(dx) > 0.01f) {
                if (dx > 0) x += speed;
                else x -= speed;
                if (fabs(dx) < speed * 2) x = intermediateX;
            } else if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = intermediateZ;
            } else {
                x = intermediateX;
                y = intermediateY;
                z = intermediateZ;
                reachedIntermediate = true;
            }
        } else {
            float dx = targetX - x;
            float dy = targetY - y;
            float dz = targetZ - z;
            
            if (fabs(dy) > 0.01f) {
                if (dy > 0) y += speed;
                else y -= speed;
                if (fabs(dy) < speed * 2) y = targetY;
            } else if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = targetZ;
            } else {
                y = targetY;
                x = targetX;
                z = targetZ;
                isMoving = false;
            }
        }
    } else {
        if (!isMoving && assignedSeat) {
            isMoving = true;
        }
        if (!isMoving) return;
        
        if (!reachedIntermediate) {
            float dx = intermediateX - x;
            float dy = intermediateY - y;
            float dz = intermediateZ - z;
            
            if (fabs(dx) > 0.01f) {
                if (dx > 0) x += speed;
                else x -= speed;
                if (fabs(dx) < speed * 2) x = intermediateX;
            } else if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = intermediateZ;
            } else if (fabs(dy) > 0.01f) {
                if (dy > 0) y += speed;
                else y -= speed;
                if (fabs(dy) < speed * 2) y = intermediateY;
            } else {
                x = intermediateX;
                y = intermediateY;
                z = intermediateZ;
                reachedIntermediate = true;
            }
        } else {
            float dx = targetX - x;
            float dy = targetY - y;
            float dz = targetZ - z;
            
            if (fabs(dx) > 0.01f) {
                if (dx > 0) x += speed;
                else x -= speed;
                if (fabs(dx) < speed * 2) x = targetX;
            } else {
                x = targetX;
                y = targetY;
                z = targetZ;
                isMoving = false;
                isSeated = true;
            }
        }
    }
}

void Person::startExiting(float exitX, float exitY, float exitZ) {
    if (assignedSeat) {
        intermediateX = exitX;
        intermediateY = assignedSeat->y;
        intermediateZ = assignedSeat->z;
        targetX = exitX;
        targetY = exitY;
        targetZ = exitZ;
    } else {
        intermediateX = exitX;
        intermediateY = y;
        intermediateZ = z;
        targetX = exitX;
        targetY = exitY;
        targetZ = exitZ;
    }
    isSeated = false;
    isExiting = true;
    reachedIntermediate = false;
}

