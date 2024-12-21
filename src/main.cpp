#include "move_generation.h"
#include "board.h"
#include "engine.h"
#include <iostream>
#include <cstdint>


int main() {
    // Initialize the board
    Board board;
    board.initializePosition(); // This sets the board to the starting chess position

    /*
     * Idea Behind Debugging int main() function
     * Display the bitboards for each piece type initially
     * Generate possible moves
     * Display the bitboards for each piece type after generating moves
     */

    // Test Pawn Moves
    std::cout << "Testing Pawn Moves:" << std::endl;
    uint64_t pawnMoves = 0ULL;
    board.displayBitboard(board.getWhitePawns());
    MoveGeneration::generatePawnMoves(board.getWhitePawns(), ~board.getOccupiedSquares(), pawnMoves);
    board.displayBitboard(pawnMoves);

    // Test Knight Moves
    std::cout << "Testing Knight Moves:" << std::endl;
    board.displayBitboard(board.getWhiteKnights());
    uint64_t knightMoves = 0ULL;
    MoveGeneration::generateKnightMoves(board.getWhiteKnights(), board.getWhitePieces(), knightMoves);
    board.displayBitboard(knightMoves);

    // Test Bishop Moves
    std::cout << "Testing Bishop Moves:" << std::endl;
    uint64_t bishopMoves = 0ULL;
    board.displayBitboard(board.getWhiteBishops());
    MoveGeneration::generateBishopMoves(board.getWhiteBishops(), board.getWhitePieces(), board.getOccupiedSquares(), bishopMoves);
    board.displayBitboard(bishopMoves);

    // Test Rook Moves
    std::cout << "Testing Rook Moves:" << std::endl;
    uint64_t rookMoves = 0ULL;
    board.displayBitboard(board.getWhiteRooks());
    MoveGeneration::generateRookMoves(board.getWhiteRooks(), board.getOccupiedSquares(), rookMoves);
    board.displayBitboard(rookMoves);

    // Test Queen Moves
    std::cout << "Testing Queen Moves:" << std::endl;
    uint64_t queenMoves = 0ULL;
    board.displayBitboard(board.getWhiteQueens());
    MoveGeneration::generateQueenMoves(board.getWhiteQueens(), board.getWhitePieces(), board.getOccupiedSquares(), queenMoves);
    board.displayBitboard(queenMoves);

    // Test King Moves
    std::cout << "Testing King Moves:" << std::endl;
    uint64_t kingMoves = 0ULL;
    board.displayBitboard(board.getWhiteKing());
    MoveGeneration::generateKingMoves(board.getWhiteKing(), board.getWhitePieces(), kingMoves);
    board.displayBitboard(kingMoves);

    return 0;
}

