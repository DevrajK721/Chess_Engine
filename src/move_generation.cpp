#include "move_generation.h"
#include "board.h"
#include <iostream>
#include <cstdint>

// Namespace for clarity
namespace MoveGeneration {

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


    bool outOfBounds(int startSquare, int targetSquare, int direction) {
        int startRank = startSquare / 8;
        int targetRank = targetSquare / 8;
        return abs(startRank - targetRank) > 1; // Ensure move is valid within bounds
    }

    // Generate all legal moves for the current board
    void generateAllMoves(const Board& board) {
        uint64_t moves, captures;

        std::cout << "Generating moves for WHITE pieces" << std::endl;

        // Generate pawn moves and captures
        generatePawnMoves(board.getWhitePawns(), ~board.getOccupiedSquares(), board.getBlackPieces(), 0ULL, moves, captures);
        std::cout << "White Pawn moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Pawn captures: " << std::endl;
        board.displayBitboard(captures);

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

        // Generate pawn moves and captures
        generatePawnMoves(board.getBlackPawns(), ~board.getOccupiedSquares(), board.getWhitePieces(), 0ULL, moves, captures);
        std::cout << "Black Pawn moves: " << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Pawn captures: " << std::endl;
        board.displayBitboard(captures);

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
