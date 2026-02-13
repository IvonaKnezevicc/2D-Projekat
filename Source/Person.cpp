#include "../Header/Person.h"
#include "../Header/Cinema.h"
#include <cmath>

namespace {
    constexpr float STEP_BOUNCE_AMPLITUDE = 0.05f;
    constexpr float STAIR_FIRST_ROW_AISLE_Z = -1.5f;
    constexpr float STAIR_ROW_SPACING_Z = 1.0f;
    constexpr float STAIR_BASE_Y = 0.25f;
    constexpr float STAIR_STEP_HEIGHT = 0.22f;

    float getStairContactY(float z, float fallbackY) {
        (void)fallbackY;
        if (z > STAIR_FIRST_ROW_AISLE_Z) {
            return STAIR_BASE_Y;
        }

        float progress = (STAIR_FIRST_ROW_AISLE_Z - z) / STAIR_ROW_SPACING_Z;
        if (progress < 0.0f) progress = 0.0f;

        float stepIndex = std::floor(progress);
        float phaseInStep = progress - stepIndex;

        float baseY = STAIR_BASE_Y + stepIndex * STAIR_STEP_HEIGHT;
        if (stepIndex < 1.0f) {
            baseY -= 0.07f;
        }
        float hopY = std::sin(phaseInStep * 3.14159265f) * STEP_BOUNCE_AMPLITUDE;
        return baseY + hopY;
    }
}

Person::Person(float startX, float startY, float startZ, float speed, int delayFrames, int modelIndex)
    : x(startX), y(startY), z(startZ), targetX(0), targetY(0), targetZ(0), intermediateX(0), intermediateY(0), intermediateZ(0), speed(speed),
      isMoving(false), isSeated(false), isExiting(false), reachedIntermediate(false), useRightEntryNudge(false), rightEntryTargetZ(0.0f), assignedSeat(nullptr),
      delayFrames(delayFrames), currentFrame(0), modelIndex(modelIndex) {}

void Person::setTarget(Seat* seat, const Cinema& cinema) {
    if (seat) {
        const float aisleOffsetZ = 0.50f;

        assignedSeat = seat;
        isSeated = false;
        
        targetX = seat->x;
        targetY = seat->y;
        targetZ = seat->z;
        
        bool useLeftStair = seat->x < 0.0f;
        useRightEntryNudge = !useLeftStair;
        rightEntryTargetZ = z - 0.25f;
        intermediateX = cinema.getStairX(useLeftStair);
        intermediateY = seat->y;
        intermediateZ = cinema.getStairZ(seat->row) + aisleOffsetZ;
        
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
            
            if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = intermediateZ;
            } else if (fabs(dx) > 0.01f) {
                if (dx > 0) x += speed;
                else x -= speed;
                if (fabs(dx) < speed * 2) x = intermediateX;
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
            
            if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = targetZ;
                
                float yStep = speed * 0.7f;
                float desiredY = getStairContactY(z, targetY);
                float adjustedDy = desiredY - y;
                if (fabs(adjustedDy) > 0.005f) {
                    if (adjustedDy > 0) y += yStep;
                    else y -= yStep;
                    if (fabs(adjustedDy) < yStep * 2.0f) y = desiredY;
                }
            } else if (fabs(dy) > 0.01f) {
                if (dy > 0) y += speed;
                else y -= speed;
                if (fabs(dy) < speed * 2) y = targetY;
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

            if (useRightEntryNudge && z > rightEntryTargetZ + 0.01f) {
                z -= speed;
                if (z < rightEntryTargetZ) z = rightEntryTargetZ;
            } else if (fabs(dx) > 0.01f) {
                if (dx > 0) x += speed;
                else x -= speed;
                if (fabs(dx) < speed * 2) x = intermediateX;
            } else if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = intermediateZ;
                
                float yStep = speed * 0.7f;
                float desiredY = getStairContactY(z, intermediateY);
                float adjustedDy = desiredY - y;
                if (fabs(adjustedDy) > 0.005f) {
                    if (adjustedDy > 0) y += yStep;
                    else y -= yStep;
                    if (fabs(adjustedDy) < yStep * 2.0f) y = desiredY;
                }
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
            } else if (fabs(dz) > 0.01f) {
                if (dz > 0) z += speed;
                else z -= speed;
                if (fabs(dz) < speed * 2) z = targetZ;
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
        const float aisleOffsetZ = 0.50f;

        intermediateX = exitX;
        intermediateY = assignedSeat->y;
        intermediateZ = assignedSeat->z + aisleOffsetZ;
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
