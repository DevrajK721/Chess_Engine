#include "move_generation.h"
#include "board.h"
#include <iostream>
#include <cstdint>

// Namespace for clarity
namespace MoveGeneration {

    // Generate all pawn moves
    void generatePawnMoves(const uint64_t& pawns, const uint64_t& emptySquares, uint64_t& moves) {
        uint64_t singleMoves = (pawns << 8) & emptySquares; // One square forward
        uint64_t doubleMoves = ((singleMoves & 0x000000000000FF00ULL) << 8) & emptySquares; // Two squares forward

        // Combine single and double moves
        moves = singleMoves | doubleMoves;
    }

    // Precompute all possible knight attack masks from a particular position
    uint64_t knightAttacks[64];
    void precomputeKnightAttacks() {
        for (int square = 0; square < 64; ++square) {
            uint64_t attack = 0ULL;
            int rank = square / 8; // Calculate rank
            int file = square % 8; // Calculate file

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
    void generateKnightMoves(const uint64_t& knights, const uint64_t& occupiedSquares, uint64_t& moves) {
        moves = 0ULL;
        precomputeKnightAttacks();
        for (int square = 0; square < 64; ++square) {
            if (knights & (1ULL << square)) { // Knight detected

                // Fetch precomputed moves
                uint64_t potentialMoves = knightAttacks[square];

                // Prune moves blocked by own pieces
                uint64_t legalMoves = potentialMoves & ~occupiedSquares;

                moves |= legalMoves;
            }
        }
    }


    // Generate bishop moves
    void generateBishopMoves(const uint64_t& bishops, const uint64_t& ownPieces, const uint64_t& occupied, uint64_t& moves) {
        moves = 0ULL;
        for (int square = 0; square < 64; ++square) {
            if (bishops & (1ULL << square)) {
                moves |= generateBishopMovesFromSquare(square, ownPieces, occupied);
            }
        }
    }

    uint64_t generateBishopMovesFromSquare(int square, const uint64_t& ownPieces, const uint64_t& occupied) {
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
                if (targetBit & occupied) break; // Stop after capturing
            }
        }
        return moves;
    }

    bool outOfBounds(int startSquare, int targetSquare, int direction) {
        int startRank = startSquare / 8;
        int targetRank = targetSquare / 8;
        if (direction == 1 || direction == -1) {
            return startRank != targetRank; // Ensure continuity for horizontal moves
        }
        return false;
    }

    // Generate rook moves
    void generateRookMoves(const uint64_t& rooks, const uint64_t& occupiedSquares, uint64_t& moves) {
        moves = 0ULL;
        for (int square = 0; square < 64; ++square) {
            if (rooks & (1ULL << square)) {
                moves |= generateRookMovesFromSquare(square, occupiedSquares);
            }
        }
    }

    uint64_t generateRookMovesFromSquare(int square, const uint64_t& occupiedSquares) {
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
                if (targetBit & occupiedSquares) break;
                moves |= targetBit;
                if (targetBit & occupiedSquares) break;
            }
        }
        return moves;
    }

    // Generate queen moves
    void generateQueenMoves(const uint64_t& queens, const uint64_t& ownPieces, const uint64_t& occupied, uint64_t& moves) {
        uint64_t bishopMoves, rookMoves;
        generateBishopMoves(queens, ownPieces, occupied, bishopMoves);
        generateRookMoves(queens, occupied, rookMoves);
        moves = bishopMoves | rookMoves;
    }

    // Generate king moves
    void generateKingMoves(const uint64_t& king, const uint64_t& occupiedSquares, uint64_t& moves) {
        moves = 0ULL;
        for (int square = 0; square < 64; ++square) {
            if (king & (1ULL << square)) {
                moves |= generateKingMovesFromSquare(square, occupiedSquares);
            }
        }
    }

    uint64_t generateKingMovesFromSquare(int square, const uint64_t& occupiedSquares) {
        uint64_t moves = 0ULL;
        const int directions[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
        for (int dir : directions) {
            int targetSquare = square + dir;
            if (targetSquare < 0 || targetSquare >= 64 || outOfBounds(square, targetSquare, dir)) {
                continue;
            }
            uint64_t targetBit = (1ULL << targetSquare);
            if (targetBit & occupiedSquares) continue;
            moves |= targetBit;
        }
        return moves;
    }

    // Generate all legal moves for the current board
    void generateAllMoves(const Board& board) {
        uint64_t moves;

        std::cout << "Generating moves for WHITE pieces" << std::endl;

        // Generate pawn moves
        generatePawnMoves(board.getWhitePawns(), ~board.getOccupiedSquares(), moves);
        std::cout << "White Pawn moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate knight moves
        generateKnightMoves(board.getWhiteKnights(), board.getWhitePieces(), moves);
        std::cout << "White Knight moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate bishop moves
        generateBishopMoves(board.getWhiteBishops(), board.getWhitePieces(), board.getOccupiedSquares(), moves);
        std::cout << "White Bishop moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate rook moves
        generateRookMoves(board.getWhiteRooks(), board.getOccupiedSquares(), moves);
        std::cout << "White Rook moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate queen moves
        generateQueenMoves(board.getWhiteQueens(), board.getWhitePieces(), board.getOccupiedSquares(), moves);
        std::cout << "White Queen moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate king moves
        generateKingMoves(board.getWhiteKing(), board.getWhitePieces(), moves);
        std::cout << "White King moves: " << std::endl;
        board.displayBitboard(moves);

        // BLACK Pieces
        std::cout << "Generating moves for BLACK pieces" << std::endl;

        // Generate pawn moves
        generatePawnMoves(board.getBlackPawns(), ~board.getOccupiedSquares(), moves);
        std::cout << "Black Pawn moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate knight moves
        generateKnightMoves(board.getBlackKnights(), board.getBlackPieces(), moves);
        std::cout << "Black Knight moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate bishop moves
        generateBishopMoves(board.getBlackBishops(), board.getBlackPieces(), board.getOccupiedSquares(), moves);
        std::cout << "Black Bishop moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate rook moves
        generateRookMoves(board.getBlackRooks(), board.getOccupiedSquares(), moves);
        std::cout << "Black Rook moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate queen moves
        generateQueenMoves(board.getBlackQueens(), board.getBlackPieces(), board.getOccupiedSquares(), moves);
        std::cout << "Black Queen moves: " << std::endl;
        board.displayBitboard(moves);

        // Generate king moves
        generateKingMoves(board.getBlackKing(), board.getBlackPieces(), moves);
        std::cout << "Black King moves: " << std::endl;
        board.displayBitboard(moves);
    }

    // Utility function to display moves
    void displayMoves(const uint64_t& moves) {
        for (int rank = 7; rank >= 0; --rank) {
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                std::cout << ((moves & (1ULL << square)) ? "1 " : ". ");
            }
            std::cout << std::endl;
        }
    }
}
