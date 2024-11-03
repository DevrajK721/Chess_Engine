#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <iostream>

class Board {
public:
    // Constructor to initialize the board with the starting position
    Board();

    // Method to initialize the board position to the standard chess setup
    void initializePosition();

    // Basic bitboard operations for setting, clearing, and checking squares
    void setPiece(int square, uint64_t& bitboard);
    void clearPiece(int square, uint64_t& bitboard);
    bool isSquareOccupied(int square, const uint64_t& bitboard) const;

    // Display function for debugging
    void displayBitboard(const uint64_t& bitboard) const;

    // Getters for each bitboard (add these)
    uint64_t getWhitePawns() const { return white_pawns; }
    uint64_t getBlackPawns() const { return black_pawns; }
    uint64_t getWhiteKnights() const { return white_knights; }
    uint64_t getBlackKnights() const { return black_knights; }
    uint64_t getWhiteBishops() const { return white_bishops; }
    uint64_t getBlackBishops() const { return black_bishops; }
    uint64_t getWhiteRooks() const { return white_rooks; }
    uint64_t getBlackRooks() const { return black_rooks; }
    uint64_t getWhiteQueens() const { return white_queens; }
    uint64_t getBlackQueens() const { return black_queens; }
    uint64_t getWhiteKing() const { return white_king; }
    uint64_t getBlackKing() const { return black_king; }

    // Getters for the aggregate bitboards
    uint64_t getWhitePieces() const { return white_pieces; }
    uint64_t getBlackPieces() const { return black_pieces; }
    uint64_t getOccupiedSquares() const { return occupied; }

private:
    // Individual bitboards for each piece type and color
    uint64_t white_pawns;
    uint64_t white_knights;
    uint64_t white_bishops;
    uint64_t white_rooks;
    uint64_t white_queens;
    uint64_t white_king;

    uint64_t black_pawns;
    uint64_t black_knights;
    uint64_t black_bishops;
    uint64_t black_rooks;
    uint64_t black_queens;
    uint64_t black_king;

    // Aggregate bitboards for convenience in move generation
    uint64_t white_pieces;  // All white pieces combined
    uint64_t black_pieces;  // All black pieces combined
    uint64_t occupied;      // All occupied squares combined
};

#endif // BOARD_H