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
    float uniformSpacing = 0.045f;
    
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
                
                for (auto& person : people) {
                    person.startExiting(exitX, exitY);
                }
            }
            break;
            
        case CinemaState::PEOPLE_EXITING:
            {
                bool allExited = true;
                for (auto& person : people) {
                    person.update();
                    if (person.isMoving) {
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
    
    for (auto& seat : seats) {
        float dx = fabs(x - seat.x);
        float dy = fabs(y - seat.y);
        
        if (dx < seatWidth / 2.0f && dy < seatHeight / 2.0f) {
            return &seat;
        }
    }
    return nullptr;
}

void Cinema::buyTickets(int count) {
    int maxRow = 0;
    for (const auto& seat : seats) {
        if (seat.row > maxRow) maxRow = seat.row;
    }
    
    int maxCol = -1;
    for (const auto& seat : seats) {
        if (seat.row == maxRow && seat.col > maxCol) {
            maxCol = seat.col;
        }
    }
    
    int found = 0;
    for (int col = maxCol; col >= 0 && found < count; col--) {
        for (auto& seat : seats) {
            if (seat.row == maxRow && seat.col == col && seat.isAvailable()) {
                seat.status = SeatStatus::BOUGHT;
                found++;
                break;
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
        Person person(doorX, doorY, 0.015f);
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
