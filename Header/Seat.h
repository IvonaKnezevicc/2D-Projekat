#pragma once

enum class SeatStatus {
    AVAILABLE,
    RESERVED,
    BOUGHT
};

struct Seat {
    float x, y;
    SeatStatus status;
    int row, col;
    
    Seat(float x = 0, float y = 0, int row = 0, int col = 0) 
        : x(x), y(y), row(row), col(col), status(SeatStatus::AVAILABLE) {}
    
    void toggleReservation() {
        if (status == SeatStatus::AVAILABLE) {
            status = SeatStatus::RESERVED;
        } else if (status == SeatStatus::RESERVED) {
            status = SeatStatus::AVAILABLE;
        }
    }
    
    bool isAvailable() const { return status == SeatStatus::AVAILABLE; }
    bool isReserved() const { return status == SeatStatus::RESERVED; }
    bool isBought() const { return status == SeatStatus::BOUGHT; }
};
