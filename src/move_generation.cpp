#include "move_generation.h"
#include "board.h"
#include "move.h"
#include <iostream>
#include <cstdint>
#include <vector>

// Namespace for clarity
namespace MoveGeneration {
    uint64_t knightAttacks[64];  // Precomputed knight attacks
    void precomputeKnightAttacks();   // Add function declaration
    uint64_t generateBishopMovesFromSquare(int square, uint64_t blockers);
    uint64_t generateRookMovesFromSquare(int square, uint64_t blockers);
    uint64_t generateKingMovesFromSquare(int square, uint64_t blockers);

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
        precomputeKnightAttacks();
        if (knightAttacks[square] & knights) return true;

        // Bishop/Queen diagonal attacks
        uint64_t bishopMoves = generateBishopMovesFromSquare(square, board.getOccupiedSquares());
        if (bishopMoves & (bishops | queens)) return true;

        // Rook/Queen straight attacks
        uint64_t rookMoves = generateRookMovesFromSquare(square, board.getOccupiedSquares());
        if (rookMoves & (rooks | queens)) return true;

        // King attacks
        uint64_t kingAttacks = generateKingMovesFromSquare(square, board.getOccupiedSquares());
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
        // Create a temporary board to simulate the move
        Board tempBoard = board;
        tempBoard.makeMove(move);  // Simulate the move

        // Check if the king is in check after the move
        bool kingSafe = isKingSafe(tempBoard, isWhite);

        // If castling, ensure the king does not pass through or land in check
        if (move.isCastling) {
            int kingSource = isWhite ? 4 : 60; // e1 or e8
            int kingTarget = move.targetSquare;
            int midSquare = (kingSource + kingTarget) / 2;

            uint64_t opponentAttacks = board.generateOpponentAttacks(!isWhite);
            if (opponentAttacks & ((1ULL << kingSource) | (1ULL << midSquare) | (1ULL << kingTarget))) {
                kingSafe = false;
            }
        }

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
    void generatePawnMoves(const uint64_t& pawns, const uint64_t& emptySquares, const uint64_t& opponentPieces, uint64_t enPassantSquare, uint64_t& moves, uint64_t& captures, bool isWhite) {
        moves = 0ULL;
        captures = 0ULL;

        if (isWhite) {
            // Single forward moves for White
            moves |= (pawns << 8) & emptySquares;

            // Double forward moves for White (from starting rank)
            moves |= ((pawns & 0x000000000000FF00ULL) << 16) & (emptySquares << 8);

            // Left diagonal captures for White
            captures |= (pawns & 0x7F7F7F7F7F7F7F7F) << 7 & opponentPieces;

            // Right diagonal captures for White
            captures |= (pawns & 0xFEFEFEFEFEFEFEFE) << 9 & opponentPieces;

            // En passant captures for White
            if (enPassantSquare) {
                captures |= ((pawns & 0x7F7F7F7F7F7F7F7F) << 7 & enPassantSquare);
                captures |= ((pawns & 0xFEFEFEFEFEFEFEFE) << 9 & enPassantSquare);
            }
        } else {
            // Single forward moves for Black
            moves |= (pawns >> 8) & emptySquares;

            // Double forward moves for Black (from starting rank)
            moves |= ((pawns & 0x00FF000000000000ULL) >> 16) & (emptySquares >> 8);

            // Left diagonal captures for Black
            captures |= (pawns & 0xFEFEFEFEFEFEFEFE) >> 9 & opponentPieces;

            // Right diagonal captures for Black
            captures |= (pawns & 0x7F7F7F7F7F7F7F7F) >> 7 & opponentPieces;

            // En passant captures for Black
            if (enPassantSquare) {
                captures |= ((pawns & 0xFEFEFEFEFEFEFEFE) >> 9 & enPassantSquare);
                captures |= ((pawns & 0x7F7F7F7F7F7F7F7F) >> 7 & enPassantSquare);
            }
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
                uint64_t bishopMoves = generateBishopMovesFromSquare(square, occupied);
                moves |= bishopMoves & ~occupied; // Exclude occupied squares
                captures |= bishopMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    uint64_t generateBishopMovesFromSquare(int square, uint64_t blockers) {
        uint64_t moves = 0ULL;
        const int directions[4] = {7, 9, -7, -9};  // Diagonal directions

        for (int direction : directions) {
            int currentSquare = square;
            while (true) {
                int targetSquare = currentSquare + direction;
                if (targetSquare < 0 || targetSquare >= 64 ||
                    outOfBounds(currentSquare, targetSquare, direction)) {
                    break;
                    }

                uint64_t targetBit = 1ULL << targetSquare;
                moves |= targetBit;

                if (blockers & targetBit) {
                    break;  // Stop if we hit a piece
                }

                currentSquare = targetSquare;
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
                uint64_t rookMoves = generateRookMovesFromSquare(square, occupied);
                moves |= rookMoves & ~occupied; // Exclude occupied squares
                captures |= rookMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    uint64_t generateRookMovesFromSquare(int square, uint64_t blockers) {
        uint64_t moves = 0ULL;
        const int directions[4] = {8, -8, 1, -1};  // Horizontal and vertical directions

        for (int direction : directions) {
            int currentSquare = square;
            while (true) {
                int targetSquare = currentSquare + direction;
                if (targetSquare < 0 || targetSquare >= 64 ||
                    outOfBounds(currentSquare, targetSquare, direction)) {
                    break;
                    }

                uint64_t targetBit = 1ULL << targetSquare;
                moves |= targetBit;

                if (blockers & targetBit) {
                    break;  // Stop if we hit a piece
                }

                currentSquare = targetSquare;
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
                uint64_t potentialMoves = generateKingMovesFromSquare(square, occupied);
                moves |= potentialMoves & ~ownPieces; // Exclude own pieces
                captures |= potentialMoves & opponentPieces; // Include opponent pieces
            }
        }
    }

    // Update the king move generator
    uint64_t generateKingMovesFromSquare(int square, uint64_t blockers) {
        uint64_t moves = 0ULL;
        const int directions[8] = {-9, -8, -7, -1, 1, 7, 8, 9};

        for (int direction : directions) {
            int targetSquare = square + direction;
            if (targetSquare < 0 || targetSquare >= 64 ||
                outOfBounds(square, targetSquare, direction)) {
                continue;
                }

            uint64_t targetBit = 1ULL << targetSquare;
            if (!(blockers & targetBit)) {
                moves |= targetBit;
            }
        }
        return moves;
    }

    // Generate castling moves
    void generateCastlingMoves(const Board& board, std::vector<Move>& moves, bool isWhite) {
        uint64_t opponentAttacks = board.generateOpponentAttacks(!isWhite);
        uint64_t occupied = board.getOccupiedSquares();

        if (isWhite) {
            if (board.canWhiteCastleKingSide() && !(occupied & 0x60ULL) && !(opponentAttacks & 0x70ULL)) {
                moves.emplace_back(Move(4, 6, PieceType::King, false, false, true));
            }
            if (board.canWhiteCastleQueenSide() && !(occupied & 0xEULL) && !(opponentAttacks & 0x1CULL)) {
                moves.emplace_back(Move(4, 2, PieceType::King, false, false, true));
            }
        } else {
            if (board.canBlackCastleKingSide() && !(occupied & 0x6000000000000000ULL) && !(opponentAttacks & 0x7000000000000000ULL)) {
                moves.emplace_back(Move(60, 62, PieceType::King, false, false, true));
            }
            if (board.canBlackCastleQueenSide() && !(occupied & 0x0E00000000000000ULL) && !(opponentAttacks & 0x1C00000000000000ULL)) {
                moves.emplace_back(Move(60, 58, PieceType::King, false, false, true));
            }
        }
    }


    // Helper function to check if a move would cross board boundaries
    bool outOfBounds(int startSquare, int targetSquare, int direction) {
        int startRank = startSquare / 8;
        int startFile = startSquare % 8;
        int targetRank = targetSquare / 8;
        int targetFile = targetSquare % 8;

        // Check for wrapping around the board
        if (direction == 1 || direction == -1) {  // Horizontal moves
            return startRank != targetRank;
        }
        return false;  // Vertical and diagonal moves don't need this check
    }

    // Generate all legal moves for the current board
    void generateAllMoves(const Board& board) {
        std::vector<Move> allMoves; // Vector to store all generated moves

        std::cout << "Generating moves for WHITE pieces" << std::endl;

        // Generate White pawn moves and captures
        uint64_t whitePawnMoves = 0, whitePawnCaptures = 0;
        generatePawnMoves(
            board.getWhitePawns(),
            ~board.getOccupiedSquares(),
            board.getBlackPieces(),
            board.getEnPassantSquare(),
            whitePawnMoves,
            whitePawnCaptures,
            true
        );
        std::cout << "White Pawn Moves:" << std::endl;
        board.displayBitboard(whitePawnMoves);
        std::cout << "White Pawn Captures:" << std::endl;
        board.displayBitboard(whitePawnCaptures);

        // Generate White castling moves
        generateCastlingMoves(board, allMoves, true);
        std::cout << "White Castling Moves:" << std::endl;
        for (const Move& move : allMoves) {
            std::cout << move.toString() << std::endl;
        }

        // Generate White knight moves and captures
        uint64_t whiteKnightMoves = 0, whiteKnightCaptures = 0;
        generateKnightMoves(
            board.getWhiteKnights(),
            board.getWhitePieces(),
            board.getBlackPieces(),
            whiteKnightMoves,
            whiteKnightCaptures
        );
        std::cout << "White Knight Moves:" << std::endl;
        board.displayBitboard(whiteKnightMoves);
        std::cout << "White Knight Captures:" << std::endl;
        board.displayBitboard(whiteKnightCaptures);

        // Generate moves for other White pieces (bishops, rooks, queens, king)
        uint64_t moves = 0, captures = 0;
        generateBishopMoves(board.getWhiteBishops(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White Bishop Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Bishop Captures:" << std::endl;
        board.displayBitboard(captures);

        generateRookMoves(board.getWhiteRooks(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White Rook Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Rook Captures:" << std::endl;
        board.displayBitboard(captures);

        generateQueenMoves(board.getWhiteQueens(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White Queen Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "White Queen Captures:" << std::endl;
        board.displayBitboard(captures);

        generateKingMoves(board.getWhiteKing(), board.getWhitePieces(), board.getBlackPieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "White King Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "White King Captures:" << std::endl;
        board.displayBitboard(captures);

        std::cout << "Generating moves for BLACK pieces" << std::endl;

        // Generate Black pawn moves and captures
        uint64_t blackPawnMoves = 0, blackPawnCaptures = 0;
        generatePawnMoves(
            board.getBlackPawns(),
            ~board.getOccupiedSquares(),
            board.getWhitePieces(),
            board.getEnPassantSquare(),
            blackPawnMoves,
            blackPawnCaptures,
            false
        );
        std::cout << "Black Pawn Moves:" << std::endl;
        board.displayBitboard(blackPawnMoves);
        std::cout << "Black Pawn Captures:" << std::endl;
        board.displayBitboard(blackPawnCaptures);

        // Generate Black castling moves
        generateCastlingMoves(board, allMoves, false);
        std::cout << "Black Castling Moves:" << std::endl;
        for (const Move& move : allMoves) {
            std::cout << move.toString() << std::endl;
        }

        // Generate Black knight moves and captures
        uint64_t blackKnightMoves = 0, blackKnightCaptures = 0;
        generateKnightMoves(
            board.getBlackKnights(),
            board.getBlackPieces(),
            board.getWhitePieces(),
            blackKnightMoves,
            blackKnightCaptures
        );
        std::cout << "Black Knight Moves:" << std::endl;
        board.displayBitboard(blackKnightMoves);
        std::cout << "Black Knight Captures:" << std::endl;
        board.displayBitboard(blackKnightCaptures);

        // Generate moves for other Black pieces (bishops, rooks, queens, king)
        generateBishopMoves(board.getBlackBishops(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black Bishop Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Bishop Captures:" << std::endl;
        board.displayBitboard(captures);

        generateRookMoves(board.getBlackRooks(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black Rook Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Rook Captures:" << std::endl;
        board.displayBitboard(captures);

        generateQueenMoves(board.getBlackQueens(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black Queen Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black Queen Captures:" << std::endl;
        board.displayBitboard(captures);

        generateKingMoves(board.getBlackKing(), board.getBlackPieces(), board.getWhitePieces(), board.getOccupiedSquares(), moves, captures);
        std::cout << "Black King Moves:" << std::endl;
        board.displayBitboard(moves);
        std::cout << "Black King Captures:" << std::endl;
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
