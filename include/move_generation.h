#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include <cstdint>
#include "board.h"

namespace MoveGeneration {
    void generatePawnMoves(const uint64_t& pawns, const uint64_t& emptySquares, const uint64_t& opponentPieces, uint64_t enPassantSquare, uint64_t& moves, uint64_t& captures);
    void precomputeKnightAttacks();
    void generateKnightMoves(const uint64_t& knights, const uint64_t& ownPieces, const uint64_t& opponentPieces, uint64_t& moves, uint64_t& captures);
    void generateBishopMoves(const uint64_t& bishops, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateBishopMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied);
    bool outOfBounds(int startSquare, int targetSquare, int direction);
    void generateRookMoves(const uint64_t& rooks, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateRookMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied);
    void generateQueenMoves(const uint64_t& queens, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    void generateKingMoves(const uint64_t& king, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateKingMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied);
    void generateAllMoves(const Board& board);
    void displayMoves(const uint64_t& moves);
}

#endif // MOVE_GENERATION_H
