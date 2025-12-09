#include "../Header/Cinema.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

Cinema::Cinema(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      state(CinemaState::RESERVATION), doorOpen(false), doorAngle(0.0f), showDarkOverlay(true),
      screenColorR(1.0f), screenColorG(1.0f), screenColorB(1.0f),
      frameCount(0), filmDurationFrames(1500),
      rng(std::random_device{}())
{
    float aspect = (float)windowWidth / (float)windowHeight;
    doorX = -0.98f * aspect;
    exitX = doorX;
    exitY = doorY;
    
    screenX = -0.75f * aspect;
    screenY = 0.9f;
    screenWidth = 1.5f * aspect;
    screenHeight = 0.2f;
    
    initializeSeats();
    
    if (!seats.empty()) {
        float firstRowY = seats[0].y;
        float screenBottomY = screenY - screenHeight/2.0f;
        doorY = (screenBottomY + firstRowY) / 2.0f;
        exitY = doorY;
    } else {
        doorY = 0.7f;
        exitY = doorY;
    }
}

Cinema::~Cinema() {
}

void Cinema::initializeSeats() {
    seats.clear();
    
    int numRows = 8;
    int seatsPerRow[] = {8, 10, 12, 14, 16, 18, 20, 22};
    
    float startY = 0.5f;
    float rowSpacing = 0.13f;
    float seatWidth = 0.06f;
    float uniformSpacing = 0.06f;
    
    for (int row = 0; row < numRows; row++) {
        int numSeats = seatsPerRow[row];
        
        float totalWidth = numSeats * seatWidth + (numSeats - 1) * uniformSpacing;
        float startX = -totalWidth / 2.0f;
        
        float y = startY - row * rowSpacing;
        
        for (int col = 0; col < numSeats; col++) {
            float x = startX + col * (seatWidth + uniformSpacing) + seatWidth / 2.0f;
            seats.push_back(Seat(x, y, row, col));
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
                std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
                screenColorR = colorDist(rng);
                screenColorG = colorDist(rng);
                screenColorB = colorDist(rng);
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
                    people[i].startExiting(exitX, exitY);
                }
            }
            break;
            
        case CinemaState::PEOPLE_EXITING:
            {
                bool allExited = true;
                for (auto& person : people) {
                    person.update();
                    if (person.isMoving || !person.isExiting || person.currentFrame < person.delayFrames) {
                        allExited = false;
                    }
                }
                
                if (allExited) {
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

void Cinema::handleMouseClick(double x, double y) {
    if (state != CinemaState::RESERVATION) return;
    
    double glX = (x / windowWidth) * 2.0 - 1.0;
    double glY = 1.0 - (y / windowHeight) * 2.0;
    
    Seat* seat = findSeatAtPosition(glX, glY);
    if (seat) {
        seat->toggleReservation();
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

Seat* Cinema::findSeatAtPosition(double x, double y) {
    float seatWidth = 0.06f;
    float seatHeight = 0.07f;
    float hitboxWidth = seatWidth * 0.7f;
    float hitboxHeight = seatHeight * 0.7f;
    
    Seat* closestSeat = nullptr;
    float minDistance = 999.0f;
    
    for (auto& seat : seats) {
        float dx = fabs(x - seat.x);
        float dy = fabs(y - seat.y);
        float distance = sqrt(dx * dx + dy * dy);
        
        if (dx < hitboxWidth / 2.0f && dy < hitboxHeight / 2.0f) {
            if (distance < minDistance) {
                minDistance = distance;
                closestSeat = &seat;
            }
        }
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
    
    std::uniform_int_distribution<int> peopleDist(1, totalSeats);
    int numPeople = peopleDist(rng);
    
    std::vector<Seat*> availableSeats;
    for (auto& seat : seats) {
        if (seat.isReserved() || seat.isBought()) {
            availableSeats.push_back(&seat);
        }
    }
    
    std::shuffle(availableSeats.begin(), availableSeats.end(), rng);
    
    for (int i = 0; i < numPeople && i < availableSeats.size(); i++) {
        Person person(doorX, doorY, 0.008f, i * 8);
        person.setTarget(availableSeats[i]);
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
}
