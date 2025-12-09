#include "../Header/Seat.h"

Seat::Seat(float x, float y, int row, int col) 
    : x(x), y(y), row(row), col(col), status(SeatStatus::AVAILABLE) {}

void Seat::toggleReservation() {
    if (status == SeatStatus::AVAILABLE) {
        status = SeatStatus::RESERVED;
    } else if (status == SeatStatus::RESERVED) {
        status = SeatStatus::AVAILABLE;
    }
}

bool Seat::isAvailable() const {
    return status == SeatStatus::AVAILABLE;
}

bool Seat::isReserved() const {
    return status == SeatStatus::RESERVED;
}

bool Seat::isBought() const {
    return status == SeatStatus::BOUGHT;
}

