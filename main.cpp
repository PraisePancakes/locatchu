#include "src/locatchu/game.hpp"

int main(int argc, char** argv) {
    loca::Game game;
    try {
        game.run();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    };
    return EXIT_SUCCESS;
};