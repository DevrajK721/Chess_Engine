#include "move_generation.h"
#include "board.h"
#include "engine.h"
#include <iostream>
#include <cstdint>

int main() {
    // Initialize the board
    Board board;
    board.initializePosition(); // This sets the board to the starting chess position

    // Precompute attack patterns
    MoveGeneration::precomputeKnightAttacks();

    // Generate all moves
    std::cout << "Generating all moves for the initial position:\n";
    MoveGeneration::generateAllMoves(board);

    return 0;
}
