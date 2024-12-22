#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include <cstdint>
#include <vector>
class Board;
#include "move.h"

namespace MoveGeneration {
    extern uint64_t knightAttacks[64];

    void precomputeKnightAttacks();

    // Pawn moves
    void generatePawnMoves(const uint64_t& pawns, const uint64_t& emptySquares, const uint64_t& opponentPieces, uint64_t enPassantSquare, uint64_t& moves, uint64_t& captures, bool isWhite);
    void generatePawnCapturesWithEnPassant(const uint64_t& pawns, const uint64_t& opponentPawns, const uint64_t& enPassantSquare, uint64_t& captures);

    // Knight moves
    void generateKnightMoves(const uint64_t& knights, const uint64_t& ownPieces, const uint64_t& opponentPieces, uint64_t& moves, uint64_t& captures);

    // Bishop moves
    void generateBishopMoves(const uint64_t& bishops, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateBishopMovesFromSquare(int square, uint64_t blockers);  // Updated

    // Rook moves
    void generateRookMoves(const uint64_t& rooks, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateRookMovesFromSquare(int square, uint64_t blockers);  // Updated

    // Queen moves
    void generateQueenMoves(const uint64_t& queens, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);

    // King moves
    void generateKingMoves(const uint64_t& king, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures);
    uint64_t generateKingMovesFromSquare(int square, uint64_t blockers);  // Updated

    // Castling moves
    void generateCastlingMoves(const Board& board, std::vector<Move>& moves, bool isWhite);

    // Legal moves
    bool isSquareAttacked(int square, const Board& board, bool byWhite);
    bool isKingSafe(const Board& board, bool isWhite);
    bool isMoveLegal(const Board& board, const Move& move, bool isWhite);
    std::vector<Move> filterLegalMoves(const Board& board, const std::vector<Move>& moves, bool isWhite);

    // Generate all moves for a given board state
    void generateAllMoves(const Board& board);

    // Utility
    void displayMoves(const uint64_t& moves);
    bool outOfBounds(int sourceSquare, int targetSquare, int direction);
}

#endif // MOVE_GENERATION_H