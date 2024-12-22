#include "move_generation.h"
#include "board.h"
#include <iostream>
#include <cstdint>
#include <vector>

// Namespace for clarity
namespace MoveGeneration {
    extern uint64_t knightAttacks[64]; // Declare as external
    void precomputeKnightAttacks();   // Add function declaration

    // Square under attack
    bool isSquareAttacked(int square, const Board& board, bool byWhite) {
        uint64_t attackers = byWhite ? board.getWhitePieces() : board.getBlackPieces();
        uint64_t pawns = byWhite ? board.getWhitePawns() : board.getBlackPawns();
        uint64_t knights = byWhite ? board.getWhiteKnights() : board.getBlackKnights();
        uint64_t bishops = byWhite ? board.getWhiteBishops() : board.getBlackBishops();
        uint64_t rooks = byWhite ? board.getWhiteRooks() : board.getBlackRooks();
        uint64_t queens = byWhite ? board.getWhiteQueens() : board.getBlackQueens();
        uint64_t king = byWhite ? board.getWhiteKing() : board.getBlackKing();

        // Pawn attacks
        uint64_t pawnAttacks = byWhite
            ? ((pawns & 0xFEFEFEFEFEFEFEFE) >> 9) | ((pawns & 0x7F7F7F7F7F7F7F7F) >> 7)
            : ((pawns & 0xFEFEFEFEFEFEFEFE) << 7) | ((pawns & 0x7F7F7F7F7F7F7F7F) << 9);

        if (pawnAttacks & (1ULL << square)) return true;

        // Knight attacks
        uint64_t knightAttacks[64];
        precomputeKnightAttacks();
        if (MoveGeneration::knightAttacks[square] & knights) return true;

        // Bishop/Queen diagonal attacks
        uint64_t bishopMoves = MoveGeneration::generateBishopMovesFromSquare(square, 0ULL, attackers, board.getOccupiedSquares());
        if (bishopMoves & (bishops | queens)) return true;

        // Rook/Queen straight attacks
        uint64_t rookMoves = MoveGeneration::generateRookMovesFromSquare(square, 0ULL, attackers, board.getOccupiedSquares());
        if (rookMoves & (rooks | queens)) return true;

        // King attacks
        uint64_t kingAttacks = MoveGeneration::generateKingMovesFromSquare(square, 0ULL, attackers, board.getOccupiedSquares());
        if (kingAttacks & king) return true;

        return false;
    }

    int firstSetBit(uint64_t x) {
        for (int i = 0; i < 64; i++) {
            if (x & (1ULL << i)) return i;
        }
        return -1; // No bits set
    }

    bool isKingSafe(const Board& board, bool isWhite) {
        int kingSquare = firstSetBit(isWhite ? board.getWhiteKing() : board.getBlackKing()) - 1;
        return !isSquareAttacked(kingSquare, board, !isWhite);
    }

    bool isMoveLegal(const Board& board, const Move& move, bool isWhite) {
        Board tempBoard = board;
        tempBoard.makeMove(move);  // Simulate the move
        bool kingSafe = isKingSafe(tempBoard, isWhite);
        tempBoard.undoMove(move); // Undo the move
        return kingSafe;
    }

    std::vector<Move> filterLegalMoves(const Board& board, const std::vector<Move>& moves, bool isWhite) {
        std::vector<Move> legalMoves;

        for (const Move& move : moves) {
            if (isMoveLegal(board, move, isWhite)) {
                legalMoves.push_back(move);
            }
        }

        return legalMoves;
    }

    // Generate all pawn moves, including captures and en passant
    void generatePawnMoves(const uint64_t& pawns, const uint64_t& emptySquares, const uint64_t& opponentPieces, uint64_t enPassantSquare, uint64_t& moves, uint64_t& captures) {
        moves = 0ULL;
        captures = 0ULL;

        // Single forward moves
        moves |= (pawns << 8) & emptySquares;

        // Double forward moves (from starting position)
        moves |= ((pawns & 0x000000000000FF00ULL) << 16) & (emptySquares << 8);

        // Left diagonal captures
        captures |= (pawns & 0x7F7F7F7F7F7F7F7F) << 7 & opponentPieces;

        // Right diagonal captures
        captures |= (pawns & 0xFEFEFEFEFEFEFEFE) << 9 & opponentPieces;

        // En passant captures
        if (enPassantSquare) {
            captures |= ((pawns & 0x7F7F7F7F7F7F7F7F) << 7 & enPassantSquare);
            captures |= ((pawns & 0xFEFEFEFEFEFEFEFE) << 9 & enPassantSquare);
        }
    }

    void generatePawnCapturesWithEnPassant(
    const uint64_t& pawns, const uint64_t& opponentPawns, const uint64_t& enPassantSquare,
    uint64_t& captures) {

        captures = 0ULL;

        // Left diagonal captures (including en passant)
        uint64_t leftCapture = (pawns & 0x7F7F7F7F7F7F7F7F) << 7;
        captures |= (leftCapture & opponentPawns) | (leftCapture & enPassantSquare);

        // Right diagonal captures (including en passant)
        uint64_t rightCapture = (pawns & 0xFEFEFEFEFEFEFEFE) << 9;
        captures |= (rightCapture & opponentPawns) | (rightCapture & enPassantSquare);
    }

    // Precompute all possible knight attack masks
    uint64_t knightAttacks[64];
    void precomputeKnightAttacks() {
        for (int square = 0; square < 64; ++square) {
            uint64_t attack = 0ULL;
            int rank = square / 8;
            int file = square % 8;

            // Knight move offsets
            int knightOffsets[8][2] = {
                {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
                {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
            };

            for (int i = 0; i < 8; ++i) {
                int targetRank = rank + knightOffsets[i][0];
                int targetFile = file + knightOffsets[i][1];

                // Ensure the target position is on the board
                if (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
                    int targetSquare = targetRank * 8 + targetFile;
                    attack |= (1ULL << targetSquare); // Set the bit
                }
            }
            knightAttacks[square] = attack;
        }
    }

    // Generate knight moves
    void generateKnightMoves(const uint64_t& knights, const uint64_t& ownPieces, const uint64_t& opponentPieces, uint64_t& moves, uint64_t& captures) {
        moves = 0ULL;
        captures = 0ULL;

        for (int square = 0; square < 64; ++square) {
            if (knights & (1ULL << square)) {
                uint64_t potentialMoves = knightAttacks[square];
                moves |= potentialMoves & ~ownPieces; // Exclude friendly pieces
                captures |= potentialMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    // Generate bishop moves
    void generateBishopMoves(const uint64_t& bishops, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures) {
        moves = 0ULL;
        captures = 0ULL;

        for (int square = 0; square < 64; ++square) {
            if (bishops & (1ULL << square)) {
                uint64_t bishopMoves = generateBishopMovesFromSquare(square, ownPieces, opponentPieces, occupied);
                moves |= bishopMoves & ~occupied; // Exclude occupied squares
                captures |= bishopMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    uint64_t MoveGeneration::generateBishopMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied) {
        uint64_t moves = 0ULL;
        const int directions[4] = {9, 7, -9, -7};
        for (int dir : directions) {
            int currentSquare = square;
            while (true) {
                currentSquare += dir;
                if (currentSquare < 0 || currentSquare >= 64 || outOfBounds(square, currentSquare, dir)) {
                    break;
                }
                uint64_t targetBit = (1ULL << currentSquare);
                if (targetBit & ownPieces) break; // Stop at own piece
                moves |= targetBit;
                if (targetBit & (opponentPieces | occupied)) break; // Stop after capturing or blocked
            }
        }
        return moves;
    }


    // Generate rook moves
    void generateRookMoves(const uint64_t& rooks, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures) {
        moves = 0ULL;
        captures = 0ULL;

        for (int square = 0; square < 64; ++square) {
            if (rooks & (1ULL << square)) {
                uint64_t rookMoves = generateRookMovesFromSquare(square, ownPieces, opponentPieces, occupied);
                moves |= rookMoves & ~occupied; // Exclude occupied squares
                captures |= rookMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    uint64_t MoveGeneration::generateRookMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied) {
        uint64_t moves = 0ULL;
        const int directions[4] = {8, -8, 1, -1};
        for (int dir : directions) {
            int currentSquare = square;
            while (true) {
                currentSquare += dir;
                if (currentSquare < 0 || currentSquare >= 64 || outOfBounds(square, currentSquare, dir)) {
                    break;
                }
                uint64_t targetBit = (1ULL << currentSquare);
                if (targetBit & ownPieces) break; // Stop at own piece
                moves |= targetBit;
                if (targetBit & (opponentPieces | occupied)) break; // Stop after capturing or blocked
            }
        }
        return moves;
    }


    // Generate queen moves
    void generateQueenMoves(const uint64_t& queens, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures) {
        uint64_t bishopMoves, rookMoves, bishopCaptures, rookCaptures;
        generateBishopMoves(queens, ownPieces, opponentPieces, occupied, bishopMoves, bishopCaptures);
        generateRookMoves(queens, ownPieces, opponentPieces, occupied, rookMoves, rookCaptures);
        moves = bishopMoves | rookMoves;
        captures = bishopCaptures | rookCaptures;
    }

    // Generate king moves
    void generateKingMoves(const uint64_t& king, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied, uint64_t& moves, uint64_t& captures) {
        moves = 0ULL;
        captures = 0ULL;

        for (int square = 0; square < 64; ++square) {
            if (king & (1ULL << square)) {
                uint64_t potentialMoves = generateKingMovesFromSquare(square, ownPieces, opponentPieces, occupied);
                moves |= potentialMoves & ~ownPieces; // Exclude own pieces
                captures |= potentialMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    uint64_t MoveGeneration::generateKingMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& opponentPieces, const uint64_t& occupied) {
        uint64_t moves = 0ULL;
        const int directions[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
        for (int dir : directions) {
            int targetSquare = square + dir;
            if (targetSquare < 0 || targetSquare >= 64 || outOfBounds(square, targetSquare, dir)) {
                continue;
            }
            uint64_t targetBit = (1ULL << targetSquare);
            if (targetBit & ownPieces) continue; // Skip friendly pieces
            moves |= targetBit;
        }
        return moves;
    }

    void generateCastlingMoves(const Board& board, uint64_t& castlingMoves, bool isWhite) {
        castlingMoves = 0ULL;

        // Define castling conditions for white and black
        bool kingSide, queenSide;
        uint64_t kingPosition, rookKingSidePosition, rookQueenSidePosition;
        uint64_t kingSidePath, queenSidePath;
        uint64_t opponentAttacks = 0ULL; // Replace with a function to generate opponent attacks.

        if (isWhite) {
            kingSide = board.canWhiteCastleKingSide();
            queenSide = board.canWhiteCastleQueenSide();
            kingPosition = 0x0000000000000010ULL; // e1
            rookKingSidePosition = 0x0000000000000080ULL; // h1
            rookQueenSidePosition = 0x0000000000000001ULL; // a1
            kingSidePath = 0x0000000000000060ULL; // f1, g1
            queenSidePath = 0x000000000000000EULL; // d1, c1, b1
        } else {
            kingSide = board.canBlackCastleKingSide();
            queenSide = board.canBlackCastleQueenSide();
            kingPosition = 0x1000000000000000ULL; // e8
            rookKingSidePosition = 0x8000000000000000ULL; // h8
            rookQueenSidePosition = 0x0100000000000000ULL; // a8
            kingSidePath = 0x6000000000000000ULL; // f8, g8
            queenSidePath = 0x0E00000000000000ULL; // d8, c8, b8
        }

        // Check for king-side castling
        if (kingSide && !(board.getOccupiedSquares() & kingSidePath) && !(opponentAttacks & (kingPosition | kingSidePath))) {
            castlingMoves |= (isWhite ? 0x0000000000000040ULL : 0x4000000000000000ULL); // g1 or g8
        }

        // Check for queen-side castling
        if (queenSide && !(board.getOccupiedSquares() & queenSidePath) && !(opponentAttacks & (kingPosition | queenSidePath))) {
            castlingMoves |= (isWhite ? 0x0000000000000004ULL : 0x0400000000000000ULL); // c1 or c8
        }
    }

    bool outOfBounds(int startSquare, int targetSquare, int direction) {
        int startRank = startSquare / 8;
        int targetRank = targetSquare / 8;
        return abs(startRank - targetRank) > 1; // Ensure move is valid within bounds
    }

    // Generate all legal moves for the current board
    void generateAllMoves(const Board& board) {
        uint64_t moves, captures;

        std::cout << "Generating moves for WHITE pieces" << std::endl;

        // Pawn moves
        MoveGeneration::generatePawnMoves(board.getWhitePawns(), ~board.getOccupiedSquares(), board.getBlackPieces(), board.getEnPassantSquare(), moves, captures);
        std::cout << "White Pawn moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Pawn captures: " << std::endl;
        board.displayBitboard(captures);

        // Castling
        uint64_t castlingMoves;
        MoveGeneration::generateCastlingMoves(board, castlingMoves, true);
        std::cout << "White Castling moves: " << std::endl;
        board.displayBitboard(castlingMoves);

        // Generate knight moves and captures
        generateKnightMoves(board.getWhiteKnights(), board.getWhitePieces(), board.getBlackPieces(), moves, captures);
        std::cout << "White Knight moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Knight captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate bishop moves and captures
        generateBishopMoves(board.getWhiteBishops(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White Bishop moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Bishop captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate rook moves and captures
        generateRookMoves(board.getWhiteRooks(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White Rook moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Rook captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate queen moves and captures
        generateQueenMoves(board.getWhiteQueens(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White Queen moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Queen captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate king moves and captures
        generateKingMoves(board.getWhiteKing(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White King moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White King captures: " << std::endl;
        board.displayBitboard(captures);

        std::cout << "Generating moves for BLACK pieces" << std::endl;

        // Pawn moves
        MoveGeneration::generatePawnMoves(board.getBlackPawns(), ~board.getOccupiedSquares(), board.getWhitePieces(), board.getEnPassantSquare(), moves, captures);
        std::cout << "Black Pawn moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Pawn captures: " << std::endl;
        board.displayBitboard(captures);

        // Castling
        MoveGeneration::generateCastlingMoves(board, castlingMoves, false);
        std::cout << "Black Castling moves: " << std::endl;
        board.displayBitboard(castlingMoves);

        // Generate knight moves and captures
        generateKnightMoves(board.getBlackKnights(), board.getBlackPieces(), board.getWhitePieces(), moves, captures);
        std::cout << "Black Knight moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Knight captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate bishop moves and captures
        generateBishopMoves(board.getBlackBishops(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black Bishop moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Bishop captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate rook moves and captures
        generateRookMoves(board.getBlackRooks(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black Rook moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Rook captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate queen moves and captures
        generateQueenMoves(board.getBlackQueens(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black Queen moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Queen captures: " << std::endl;
        board.displayBitboard(captures);

        // Generate king moves and captures
        generateKingMoves(board.getBlackKing(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black King moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black King captures: " << std::endl;
        board.displayBitboard(captures);
    }

    // Utility function to display moves and captures
    void displayMoves(const uint64_t& moves) {
        for (int rank = 7; rank >= 0; --rank) {
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                std::cout << ((moves & (1ULL << square)) ? "1 " : ". ");
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
