#include "../Header/Cinema.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

Cinema::Cinema(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      state(CinemaState::RESERVATION), doorOpen(false), doorAngle(0.0f), showDarkOverlay(false),
      screenColorR(1.0f), screenColorG(1.0f), screenColorB(1.0f),
      frameCount(0), filmDurationFrames(1500), currentFilmTextureIndex(0),
      rng(std::random_device{}())
{
    hallWidth = 20.0f;
    hallHeight = 8.0f;
    hallDepth = 15.0f;
    
    hallMinX = -hallWidth / 2.0f;
    hallMaxX = hallWidth / 2.0f;
    hallMinY = 0.0f;
    hallMaxY = hallHeight;
    hallMinZ = -hallDepth;
    hallMaxZ = 0.0f;
    
    screenX = 0.0f;
    screenWidth = hallWidth * 0.6f;
    screenHeight = hallHeight * 0.6f;
    screenY = hallHeight - screenHeight / 2.0f - 0.36f;
    screenZ = hallMaxZ;
    screenDepth = 0.1f;
    
    float doorHeight = 2.5f;
    float doorWidth = 1.5f;
    
    doorX = hallMinX + doorWidth / 2.0f;
    doorY = doorHeight / 2.0f;
    doorZ = hallMaxZ;
    
    portalX = doorX + doorWidth * 0.5f;
    portalY = doorY;
    portalZ = doorZ - 0.06f;
    
    float personStartY = 0.25f;
    
    exitX = portalX;
    exitY = personStartY;
    exitZ = portalZ;
    
    initializeSeats();
}

Cinema::~Cinema() {
}

void Cinema::initializeSeats() {
    seats.clear();
    
    int numRows = 10;
    int baseSeatsPerRow[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int seatsPerRow[10];
    for (int i = 0; i < numRows; i++) {
        seatsPerRow[i] = baseSeatsPerRow[i] + 6;
    }
    
    float seatWidth = 0.6f;
    float seatDepth = 0.6f;
    float seatHeight = 0.4f;
    float seatSpacing = 0.2f;
    float rowSpacing = 1.0f;
    float stepHeight = 0.22f;
    
    float startZ = hallMaxZ - 2.0f;
    float startY = seatHeight / 2.0f + 0.05f;
    
    for (int row = 0; row < numRows; row++) {
        int numSeats = seatsPerRow[row];
        
        float y = startY + row * stepHeight;
        float z = startZ - row * rowSpacing;
        
        float totalWidth = numSeats * seatWidth + (numSeats - 1) * seatSpacing;
        float startX = -totalWidth / 2.0f;
        
        for (int col = 0; col < numSeats; col++) {
            float x = startX + col * (seatWidth + seatSpacing) + seatWidth / 2.0f;
            seats.push_back(Seat(x, y, z, row, col));
        }
    }
}

void Cinema::update() {
    if (doorOpen) {
        if (doorAngle < 90.0f) {
            doorAngle += 2.0f;
            if (doorAngle > 90.0f) doorAngle = 90.0f;
        }
    } else {
        if (doorAngle > 0.0f) {
            doorAngle -= 2.0f;
            if (doorAngle < 0.0f) doorAngle = 0.0f;
        }
    }
    
    switch (state) {
        case CinemaState::PEOPLE_ENTERING:
            {
                bool allSeated = true;
                for (auto& person : people) {
                    person.update();
                    if (person.isMoving || !person.isSeated) {
                        allSeated = false;
                    }
                }
                
                if (allSeated && people.size() > 0) {
                    doorOpen = false;
                    state = CinemaState::FILM_PLAYING;
                    frameCount = 0;
                }
            }
            break;
            
        case CinemaState::FILM_PLAYING:
            frameCount++;
            
            if (frameCount % 20 == 0) {
                currentFilmTextureIndex = (currentFilmTextureIndex + 1) % 20;
            }
            
            if (frameCount >= filmDurationFrames) {
                screenColorR = 1.0f;
                screenColorG = 1.0f;
                screenColorB = 1.0f;
                doorOpen = true;
                state = CinemaState::PEOPLE_EXITING;
                
                std::vector<std::pair<int, size_t>> sortedIndices;
                for (size_t i = 0; i < people.size(); i++) {
                    if (people[i].assignedSeat) {
                        int priority = people[i].assignedSeat->row * 10000 + (int)(people[i].assignedSeat->x * 1000);
                        sortedIndices.push_back({priority, i});
                    } else {
                        sortedIndices.push_back({999999, i});
                    }
                }
                std::sort(sortedIndices.begin(), sortedIndices.end());
                
                for (size_t idx = 0; idx < sortedIndices.size(); idx++) {
                    size_t i = sortedIndices[idx].second;
                    people[i].currentFrame = 0;
                    people[i].delayFrames = idx * 8;
                    people[i].startExiting(exitX, exitY, exitZ);
                }
            }
            break;
            
        case CinemaState::PEOPLE_EXITING:
            {
                for (auto& person : people) {
                    person.update();
                }

                people.erase(
                    std::remove_if(people.begin(), people.end(),
                        [&](const Person& person) {
                            return person.isExiting &&
                                   !person.isMoving &&
                                   person.currentFrame >= person.delayFrames;
                        }),
                    people.end()
                );

                if (people.empty()) {
                    doorOpen = false;
                    state = CinemaState::RESETTING;
                }
            }
            break;
            
        case CinemaState::RESETTING:
            resetCinema();
            break;
            
        default:
            break;
    }
}

void Cinema::handleMouseClick(double mouseX, double mouseY, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    if (state != CinemaState::RESERVATION) return;
    
    float centerX = 0.0f;
    float centerY = 0.0f;
    
    glm::vec4 rayClip(centerX, centerY, -1.0f, 1.0f);
    glm::mat4 invProjection = glm::inverse(projection);
    glm::vec4 rayEye = invProjection * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    rayEye = glm::normalize(rayEye);
    
    glm::mat4 invView = glm::inverse(view);
    glm::vec4 rayWorld4 = invView * rayEye;
    glm::vec3 rayWorld = glm::normalize(glm::vec3(rayWorld4));
    
    Seat* clickedSeat = findSeatAtRay(cameraPos, rayWorld);
    if (clickedSeat) {
        clickedSeat->toggleReservation();
    }
}

void Cinema::handleKeyPress(int key) {
    if (state != CinemaState::RESERVATION) return;
    
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
        int count = key - GLFW_KEY_1 + 1;
        buyTickets(count);
    } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
        startProjection();
    }
}

Seat* Cinema::findSeatAtRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
    float seatWidth = 0.6f;
    float seatDepth = 0.6f;
    float seatHeight = 0.4f;
    float backHeight = 0.8f;
    
    Seat* closestSeat = nullptr;
    float minDistance = std::numeric_limits<float>::max();
    
    for (auto& seat : seats) {
        float backY = seat.y + seatHeight * 0.5f + backHeight * 0.5f;
        float backZ = seat.z - seatDepth * 0.45f;
        
        float minX = seat.x - seatWidth / 2.0f;
        float maxX = seat.x + seatWidth / 2.0f;
        float minY = seat.y;
        float maxY = backY + backHeight / 2.0f;
        float minZ = backZ - 0.05f;
        float maxZ = seat.z + seatDepth / 2.0f;
        
        float tmin, tmax;
        if (fabs(rayDir.x) < 0.0001f) {
            if (rayOrigin.x < minX || rayOrigin.x > maxX) continue;
            tmin = -std::numeric_limits<float>::max();
            tmax = std::numeric_limits<float>::max();
        } else {
            tmin = (minX - rayOrigin.x) / rayDir.x;
            tmax = (maxX - rayOrigin.x) / rayDir.x;
            if (tmin > tmax) std::swap(tmin, tmax);
        }
        
        float tymin, tymax;
        if (fabs(rayDir.y) < 0.0001f) {
            if (rayOrigin.y < minY || rayOrigin.y > maxY) continue;
            tymin = -std::numeric_limits<float>::max();
            tymax = std::numeric_limits<float>::max();
        } else {
            tymin = (minY - rayOrigin.y) / rayDir.y;
            tymax = (maxY - rayOrigin.y) / rayDir.y;
            if (tymin > tymax) std::swap(tymin, tymax);
        }
        
        if ((tmin > tymax) || (tymin > tmax)) continue;
        
        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;
        
        float tzmin, tzmax;
        if (fabs(rayDir.z) < 0.0001f) {
            if (rayOrigin.z < minZ || rayOrigin.z > maxZ) continue;
            tzmin = -std::numeric_limits<float>::max();
            tzmax = std::numeric_limits<float>::max();
        } else {
            tzmin = (minZ - rayOrigin.z) / rayDir.z;
            tzmax = (maxZ - rayOrigin.z) / rayDir.z;
            if (tzmin > tzmax) std::swap(tzmin, tzmax);
        }
        
        if ((tmin > tzmax) || (tzmin > tmax)) continue;
        
        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;
        
        if (tmin > 0.0f && tmin < 100.0f) {
            glm::vec3 seatCenter(seat.x, seat.y + seatHeight / 2.0f, seat.z);
            glm::vec3 toSeat = seatCenter - rayOrigin;
            
            float t = glm::dot(toSeat, rayDir);
            if (t > 0.0f) {
                glm::vec3 closestPoint = rayOrigin + rayDir * t;
                float distanceToCenter = glm::length(seatCenter - closestPoint);
                
                if (distanceToCenter < minDistance) {
                    minDistance = distanceToCenter;
                    closestSeat = &seat;
                }
            }
        }
    }
    
    if (minDistance > 0.4f) {
        return nullptr;
    }
    
    return closestSeat;
}

void Cinema::buyTickets(int count) {
    int maxRow = 0;
    for (const auto& seat : seats) {
        if (seat.row > maxRow) maxRow = seat.row;
    }
    
    for (int row = maxRow; row >= 0; row--) {
        int maxCol = -1;
        for (const auto& seat : seats) {
            if (seat.row == row && seat.col > maxCol) {
                maxCol = seat.col;
            }
        }
        
        for (int startCol = maxCol; startCol >= count - 1; startCol--) {
            bool allAvailable = true;
            for (int i = 0; i < count; i++) {
                int col = startCol - i;
                bool found = false;
                for (const auto& seat : seats) {
                    if (seat.row == row && seat.col == col) {
                        if (!seat.isAvailable()) {
                            allAvailable = false;
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    allAvailable = false;
                    break;
                }
            }
            
            if (allAvailable) {
                for (int i = 0; i < count; i++) {
                    int col = startCol - i;
                    for (auto& seat : seats) {
                        if (seat.row == row && seat.col == col) {
                            seat.status = SeatStatus::BOUGHT;
                            break;
                        }
                    }
                }
                return;
            }
        }
    }
}

void Cinema::startProjection() {
    doorOpen = true;
    showDarkOverlay = false;
    state = CinemaState::PEOPLE_ENTERING;
    createPeople();
}

void Cinema::createPeople() {
    people.clear();
    
    int totalSeats = countReservedAndBought();
    if (totalSeats == 0) return;
    
    int numPeople = totalSeats;
    
    std::vector<Seat*> availableSeats;
    for (auto& seat : seats) {
        if (seat.isReserved() || seat.isBought()) {
            availableSeats.push_back(&seat);
        }
    }
    
    std::shuffle(availableSeats.begin(), availableSeats.end(), rng);
    
    float personStartY = 0.25f;
    
    for (int i = 0; i < numPeople && i < availableSeats.size(); i++) {
        int modelIndex = i % 15;
        Person person(exitX, personStartY, exitZ, 0.03f, i * 8, modelIndex);
        person.setTarget(availableSeats[i], *this);
        people.push_back(person);
    }
}

int Cinema::countReservedAndBought() const {
    int count = 0;
    for (const auto& seat : seats) {
        if (seat.isReserved() || seat.isBought()) {
            count++;
        }
    }
    return count;
}

void Cinema::resetCinema() {
    for (auto& seat : seats) {
        if (seat.isReserved() || seat.isBought()) {
            seat.status = SeatStatus::AVAILABLE;
        }
    }
    
    people.clear();
    doorOpen = false;
    doorAngle = 0.0f;
    showDarkOverlay = true;
    state = CinemaState::RESERVATION;
    screenColorR = 1.0f;
    screenColorG = 1.0f;
    screenColorB = 1.0f;
    frameCount = 0;
    currentFilmTextureIndex = 0;
}

float Cinema::getStairX(bool isLeft) const {
    if (isLeft) {
        return hallMinX + 1.0f;
    } else {
        return hallMaxX - 1.0f;
    }
}

float Cinema::getStairZ(int row) const {
    float startZ = hallMaxZ - 2.0f;
    float rowSpacing = 1.0f;
    return startZ - row * rowSpacing;
}
