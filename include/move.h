#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <string>

struct Move {
    int sourceSquare;         // Source square (0-63)
    int targetSquare;         // Target square (0-63)
    int promotionPiece;       // Piece to promote to (0 if no promotion, otherwise piece code)
    bool isCapture;           // True if this move captures a piece
    bool isEnPassant;         // True if this is an en passant move
    bool isCastling;          // True if this is a castling move
    bool isPromotion;         // True if this is a promotion move
    bool isDoublePawnPush;    // True if this is a double pawn push
    int previousEnPassantSquare; // To store the en passant square before the move

    std::string toString() const;

    // Constructor
    Move(int source, int target, int promotion = 0, bool capture = false, bool enPassant = false,
         bool castling = false, bool promotionMove = false, bool doublePawnPush = false, int prevEnPassant = -1)
        : sourceSquare(source), targetSquare(target), promotionPiece(promotion), isCapture(capture),
          isEnPassant(enPassant), isCastling(castling), isPromotion(promotionMove),
          isDoublePawnPush(doublePawnPush), previousEnPassantSquare(prevEnPassant) {}
};

enum PieceType {
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6
};

enum PieceColor {
    White = 0,
    Black = 1
};
#endif // MOVE_H
