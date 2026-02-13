#include "../Header/Application.h"
#include <iostream>

int main() {
    Application app;
    
    if (!app.initialize()) {
        std::cout << "Greska pri inicijalizaciji aplikacije." << std::endl;
        return -1;
    }
    
    app.run();
    return 0;
}
