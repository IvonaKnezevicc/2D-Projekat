#pragma once
#include "Seat.h"
#include "Person.h"
#include <vector>
#include <random>
#include <glm/glm.hpp>

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
    void handleMouseClick(double x, double y, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
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
    int getCurrentFilmTextureIndex() const { return currentFilmTextureIndex; }
    bool isFilmPlaying() const { return state == CinemaState::FILM_PLAYING; }
    
    float getDoorX() const { return doorX; }
    float getDoorY() const { return doorY; }
    float getDoorZ() const { return doorZ; }
    float getPortalX() const { return portalX; }
    float getPortalY() const { return portalY; }
    float getPortalZ() const { return portalZ; }
    float getScreenX() const { return screenX; }
    float getScreenY() const { return screenY; }
    float getScreenZ() const { return screenZ; }
    float getScreenWidth() const { return screenWidth; }
    float getScreenHeight() const { return screenHeight; }
    float getScreenDepth() const { return screenDepth; }
    
    
    float getHallMinX() const { return hallMinX; }
    float getHallMaxX() const { return hallMaxX; }
    float getHallMinY() const { return hallMinY; }
    float getHallMaxY() const { return hallMaxY; }
    float getHallMinZ() const { return hallMinZ; }
    float getHallMaxZ() const { return hallMaxZ; }
    
    float getStairX(bool isLeft) const;
    float getStairZ(int row) const;
    
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
    int currentFilmTextureIndex;
    
    float doorX, doorY, doorZ;
    float portalX, portalY, portalZ;
    float screenX, screenY, screenZ, screenWidth, screenHeight, screenDepth;
    float exitX, exitY, exitZ;
    
    
    float hallWidth, hallHeight, hallDepth;
    float hallMinX, hallMaxX, hallMinY, hallMaxY, hallMinZ, hallMaxZ;
    
    std::mt19937 rng;
    
    void initializeSeats();
    void buyTickets(int count);
    void startProjection();
    void createPeople();
    Seat* findSeatAtRay(const glm::vec3& rayOrigin, const glm::vec3& rayDir);
    int countReservedAndBought() const;
    void resetCinema();
};
