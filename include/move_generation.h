#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include <cstdint>
#include "board.h"

namespace MoveGeneration {
    void generatePawnMoves(const uint64_t& pawns, const uint64_t& emptySquares, const uint64_t& opponentPieces, uint64_t enPassantSquare, uint64_t& moves, uint64_t& captures);
    void generatePawnCapturesWithEnPassant(const uint64_t& pawns, const uint64_t& opponentPawns, const uint64_t& enPassantSquare, uint64_t& captures);

    void precomputeKnightAttacks();
    void generateKnightMoves(const uint64_t& knights, const uint64_t& ownPieces, const uint64_t& opponentPieces, uint64_t& moves, uint64_t& captures);

    void generateBishopMoves(const uint64_t& bishops, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateBishopMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied);

    void generateRookMoves(const uint64_t& rooks, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateRookMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied);

    void generateQueenMoves(const uint64_t& queens, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);

    void generateKingMoves(const uint64_t& king, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateKingMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied);

    void generateCastlingMoves(const Board& board, uint64_t& castlingMoves, bool isWhite);

    void generateAllMoves(const Board& board);

    void displayMoves(const uint64_t& moves);

    bool outOfBounds(int sourceSquare, int targetSquare, int direction);
}

#endif // MOVE_GENERATION_H
