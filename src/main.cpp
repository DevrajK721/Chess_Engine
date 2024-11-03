#include "engine.h"
#include "board.h"

int main() {
    startEngine();

    // Create an instance of Board
    Board board;

    // Initialize the board to the starting position
    board.initializePosition();

    std::cout << "Occupied Squares (all pieces combined):" << std::endl;
    board.displayBitboard(board.getOccupiedSquares());

    return 0;
}
