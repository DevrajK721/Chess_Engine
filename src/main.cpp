#include "move_generation.h"
#include "board.h"
#include "engine.h"
#include <iostream>
#include <cstdint>
#include <vector>

int main() {
    // Initialize the board
    Board board;
    board.initializePosition();

    // Precompute knight attacks
    MoveGeneration::precomputeKnightAttacks();

    // Generate and display all moves
    std::cout << "Generating all moves for the initial position:\n";
    MoveGeneration::generateAllMoves(board);

    // Generate and display legal moves for White
    std::vector<Move> whiteMoves = board.generateMoves(true);
    std::vector<Move> legalWhiteMoves = MoveGeneration::filterLegalMoves(board, whiteMoves, true);
    std::cout << "\nLegal moves for White:\n";
    for (const Move& move : legalWhiteMoves) {
        std::cout << move.toString() << std::endl;
    }

    // Generate and display legal moves for Black
    std::vector<Move> blackMoves = board.generateMoves(false);
    std::vector<Move> legalBlackMoves = MoveGeneration::filterLegalMoves(board, blackMoves, false);
    std::cout << "\nLegal moves for Black:\n";
    for (const Move& move : legalBlackMoves) {
        std::cout << move.toString() << std::endl;
    }

    return 0;
}
