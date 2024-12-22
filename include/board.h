#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <iostream>
#include <vector>
#include "move_generation.h"
#include "move.h"

class Board {
public:
    Board();

    void initializePosition();
    void setPiece(int square, uint64_t& bitboard);
    void clearPiece(int square, uint64_t& bitboard);
    void makeMove(const Move& move);
    void undoMove(const Move& move);
    bool isSquareOccupied(int square, const uint64_t& bitboard) const;
    std::vector<Move> generateMoves(bool isWhite) const;
    static void displayBitboard(const uint64_t& bitboard);

    uint64_t generateOpponentAttacks(bool isWhite) const;
    uint64_t getOccupiedSquares() const;
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

    uint64_t getWhitePieces() const { return white_pieces; }
    uint64_t getBlackPieces() const { return black_pieces; }
    uint64_t getEnPassantSquare() const { return enPassantSquare; }
    bool canWhiteCastleKingSide() const { return whiteCanCastleKingSide; }
    bool canWhiteCastleQueenSide() const { return whiteCanCastleQueenSide; }
    bool canBlackCastleKingSide() const { return blackCanCastleKingSide; }
    bool canBlackCastleQueenSide() const { return blackCanCastleQueenSide; }

private:
    uint64_t white_pawns, white_knights, white_bishops, white_rooks, white_queens, white_king;
    uint64_t black_pawns, black_knights, black_bishops, black_rooks, black_queens, black_king;
    uint64_t white_pieces, black_pieces, occupied;
    uint64_t enPassantSquare;
    bool whiteCanCastleKingSide, whiteCanCastleQueenSide;
    bool blackCanCastleKingSide, blackCanCastleQueenSide;
};

#endif // BOARD_H
