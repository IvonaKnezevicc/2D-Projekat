#pragma once
#include "Seat.h"
#include "Person.h"
#include <vector>
#include <random>

enum class CinemaState {
    RESERVATION,
    PEOPLE_ENTERING,
    FILM_PLAYING,
    PEOPLE_EXITING,
    RESETTING
};

class Cinema {
public:
    Cinema(int windowWidth, int windowHeight);
    ~Cinema();
    
    void update();
    void handleMouseClick(double x, double y);
    void handleKeyPress(int key);
    
    const std::vector<Seat>& getSeats() const { return seats; }
    const std::vector<Person>& getPeople() const { return people; }
    CinemaState getState() const { return state; }
    
    bool isDoorOpen() const { return doorOpen; }
    float getDoorAngle() const { return doorAngle; }
    bool showOverlay() const { return showDarkOverlay; }
    float getScreenColorR() const { return screenColorR; }
    float getScreenColorG() const { return screenColorG; }
    float getScreenColorB() const { return screenColorB; }
    
    float getDoorX() const { return doorX; }
    float getDoorY() const { return doorY; }
    float getScreenX() const { return screenX; }
    float getScreenY() const { return screenY; }
    float getScreenWidth() const { return screenWidth; }
    float getScreenHeight() const { return screenHeight; }
    
private:
    int windowWidth, windowHeight;
    std::vector<Seat> seats;
    std::vector<Person> people;
    CinemaState state;
    
    bool doorOpen;
    float doorAngle;
    bool showDarkOverlay;
    
    float screenColorR, screenColorG, screenColorB;
    int frameCount;
    int filmDurationFrames;
    
    float doorX, doorY;
    float screenX, screenY, screenWidth, screenHeight;
    float exitX, exitY;
    
    std::mt19937 rng;
    
    void initializeSeats();
    void buyTickets(int count);
    void startProjection();
    void createPeople();
    Seat* findSeatAtPosition(double x, double y);
    int countReservedAndBought() const;
    void resetCinema();
};
