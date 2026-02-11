#pragma once

enum class SeatStatus {
    AVAILABLE,
    RESERVED,
    BOUGHT
};

struct Seat {
    float x, y, z;  // 3D pozicija
    SeatStatus status;
    int row, col;
    
    Seat(float x = 0, float y = 0, float z = 0, int row = 0, int col = 0);
    
    void toggleReservation();
    bool isAvailable() const;
    bool isReserved() const;
    bool isBought() const;
};
