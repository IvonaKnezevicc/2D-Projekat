#include "../Header/Cinema.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

Cinema::Cinema(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      state(CinemaState::RESERVATION), doorOpen(false), doorAngle(0.0f), showDarkOverlay(false),
      screenColorR(1.0f), screenColorG(1.0f), screenColorB(1.0f),
      frameCount(0), filmDurationFrames(1500),
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
    screenY = hallHeight * 0.7f;
    screenZ = hallMaxZ;
    screenWidth = hallWidth * 0.8f;
    screenHeight = hallHeight * 0.4f;
    screenDepth = 0.1f;
    
    float doorHeight = 2.5f;
    float doorWidth = 1.5f;
    
    doorX = hallMinX + doorWidth / 2.0f;
    doorY = doorHeight / 2.0f;
    doorZ = hallMaxZ;
    
    float personHeight = 1.0f;
    float personStartY = personHeight / 2.0f;
    
    exitX = doorX;
    exitY = personStartY;
    exitZ = doorZ;
    
    initializeSeats();
}

Cinema::~Cinema() {
}

void Cinema::initializeSeats() {
    seats.clear();
    
    int numRows = 10;
    int seatsPerRow[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    
    float seatWidth = 0.6f;
    float seatDepth = 0.6f;
    float seatHeight = 0.4f;
    float seatSpacing = 0.1f;
    float rowSpacing = 1.0f;
    float stepHeight = 0.15f;
    
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
                    people[i].startExiting(exitX, exitY, exitZ);
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
    
    float aspect = (float)windowWidth / (float)windowHeight;
    double glX = ((x / windowWidth) * 2.0 - 1.0) * aspect;
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
    float seatWidth = 0.05f;
    float seatDepth = 0.042f;
    float hitboxWidth = seatWidth * 1.1f;
    float hitboxHeight = seatDepth * 1.1f;
    
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
    
    // Pozicija za ljude koji ulaze - na podu, blizu vrata
    // Ljudi su visoki 1.0f (centar na 0.5f), pa počinju na podu
    float personHeight = 1.0f;
    float personStartY = personHeight / 2.0f;  // Centar osobe na 0.5m od poda
    
    for (int i = 0; i < numPeople && i < availableSeats.size(); i++) {
        Person person(doorX, personStartY, doorZ, 0.03f, i * 8);  // Ljudi počinju na podu (ubrzano kretanje)
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
