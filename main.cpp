#include "src/locatchu/app/application.hpp"
int main(int argc, char** argv) {
    loca::application app;
    try {
        app.run();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
};