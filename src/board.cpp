#include "board.h"
#include "move_generation.h"
#include "move.h"
#include <iostream>
#include <vector>

// Constructor: Initializes bitboards to zero
Board::Board()
    : white_pawns(0ULL), white_knights(0ULL), white_bishops(0ULL), white_rooks(0ULL), white_queens(0ULL), white_king(0ULL),
      black_pawns(0ULL), black_knights(0ULL), black_bishops(0ULL), black_rooks(0ULL), black_queens(0ULL), black_king(0ULL),
      white_pieces(0ULL), black_pieces(0ULL), occupied(0ULL),
      enPassantSquare(0ULL), whiteCanCastleKingSide(true), whiteCanCastleQueenSide(true),
      blackCanCastleKingSide(true), blackCanCastleQueenSide(true) {}

// Sets up the starting position for the board
void Board::initializePosition() {
    white_pieces = black_pieces = occupied = 0ULL;

    // Initialize individual pieces and update aggregate bitboards
    white_pawns = 0x000000000000FF00ULL;
    white_knights = 0x0000000000000042ULL;
    white_bishops = 0x0000000000000024ULL;
    white_rooks = 0x0000000000000081ULL;
    white_queens = 0x0000000000000008ULL;
    white_king = 0x0000000000000010ULL;
    black_pawns = 0x00FF000000000000ULL;
    black_knights = 0x4200000000000000ULL;
    black_bishops = 0x2400000000000000ULL;
    black_rooks = 0x8100000000000000ULL;
    black_queens = 0x0800000000000000ULL;
    black_king = 0x1000000000000000ULL;

    white_pieces = white_pawns | white_knights | white_bishops | white_rooks | white_queens | white_king;
    black_pieces = black_pawns | black_knights | black_bishops | black_rooks | black_queens | black_king;
    occupied = white_pieces | black_pieces;

    // Reset advanced move state
    enPassantSquare = 0ULL;
    whiteCanCastleKingSide = whiteCanCastleQueenSide = true;
    blackCanCastleKingSide = blackCanCastleQueenSide = true;
}

// Sets a piece at a specific square in the given bitboard
void Board::setPiece(int square, uint64_t& bitboard) {
    bitboard |= (1ULL << square);
}

// Clears a piece at a specific square in the given bitboard
void Board::clearPiece(int square, uint64_t& bitboard) {
    bitboard &= ~(1ULL << square);
}

// Checks if a specific square is occupied in the given bitboard
bool Board::isSquareOccupied(int square, const uint64_t& bitboard) const {
    return (bitboard & (1ULL << square)) != 0;
}

// Displays the bitboard as an 8x8 grid (for debugging purposes)
void Board::displayBitboard(const uint64_t& bitboard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::makeMove(const Move& move) {
    // Extract source and target squares from the move
    int sourceSquare = move.sourceSquare;
    int targetSquare = move.targetSquare;
    uint64_t sourceBit = (1ULL << sourceSquare);
    uint64_t targetBit = (1ULL << targetSquare);

    // Determine the piece color based on the source square
    bool isWhite = white_pieces & sourceBit;

    // Remove the piece from the source square
    if (isWhite) {
        white_pieces &= ~sourceBit;
    } else {
        black_pieces &= ~sourceBit;
    }
    occupied &= ~sourceBit;

    // Move the piece to the target square
    if (isWhite) {
        white_pieces |= targetBit;
    } else {
        black_pieces |= targetBit;
    }
    occupied |= targetBit;

    // Handle captures
    if (move.isCapture) {
        uint64_t captureMask = targetBit;
        if (isWhite) {
            black_pieces &= ~captureMask;
        } else {
            white_pieces &= ~captureMask;
        }
    }

    // Handle promotions
    if (move.isPromotion) {
        uint64_t promotionMask = targetBit;
        if (isWhite) {
            white_pawns &= ~promotionMask;
            white_queens |= promotionMask;
        } else {
            black_pawns &= ~promotionMask;
            black_queens |= promotionMask;
        }
    }

    // Handle en passant
    if (move.isEnPassant) {
        int epCapturedSquare = isWhite ? targetSquare - 8 : targetSquare + 8;
        uint64_t epCaptureMask = (1ULL << epCapturedSquare);
        if (isWhite) {
            black_pawns &= ~epCaptureMask;
        } else {
            white_pawns &= ~epCaptureMask;
        }
        occupied &= ~epCaptureMask;
    }

    // Update castling rights
    if (move.isCastling) {
        if (isWhite) {
            if (targetSquare == 6) { // White king-side castling
                white_rooks &= ~(1ULL << 7);
                white_rooks |= (1ULL << 5);
            } else if (targetSquare == 2) { // White queen-side castling
                white_rooks &= ~(1ULL);
                white_rooks |= (1ULL << 3);
            }
        } else {
            if (targetSquare == 62) { // Black king-side castling
                black_rooks &= ~(1ULL << 63);
                black_rooks |= (1ULL << 61);
            } else if (targetSquare == 58) { // Black queen-side castling
                black_rooks &= ~(1ULL << 56);
                black_rooks |= (1ULL << 59);
            }
        }
    }

    // Update en passant square
    enPassantSquare = move.isDoublePawnPush ? (isWhite ? targetSquare - 8 : targetSquare + 8) : 0;
}

void Board::undoMove(const Move& move) {
    // Extract source and target squares from the move
    int sourceSquare = move.sourceSquare;
    int targetSquare = move.targetSquare;
    uint64_t sourceBit = (1ULL << sourceSquare);
    uint64_t targetBit = (1ULL << targetSquare);

    // Determine the piece color based on the target square
    bool isWhite = white_pieces & targetBit;

    // Move the piece back to the source square
    if (isWhite) {
        white_pieces |= sourceBit;
        white_pieces &= ~targetBit;
    } else {
        black_pieces |= sourceBit;
        black_pieces &= ~targetBit;
    }
    occupied &= ~targetBit;
    occupied |= sourceBit;

    // Restore captures
    if (move.isCapture) {
        uint64_t captureMask = targetBit;
        if (isWhite) {
            black_pieces |= captureMask;
        } else {
            white_pieces |= captureMask;
        }
        occupied |= captureMask;
    }

    // Undo promotions
    if (move.isPromotion) {
        uint64_t promotionMask = targetBit;
        if (isWhite) {
            white_queens &= ~promotionMask;
            white_pawns |= promotionMask;
        } else {
            black_queens &= ~promotionMask;
            black_pawns |= promotionMask;
        }
    }

    // Undo en passant
    if (move.isEnPassant) {
        int epCapturedSquare = isWhite ? targetSquare - 8 : targetSquare + 8;
        uint64_t epCaptureMask = (1ULL << epCapturedSquare);
        if (isWhite) {
            black_pawns |= epCaptureMask;
        } else {
            white_pawns |= epCaptureMask;
        }
        occupied |= epCaptureMask;
    }

    // Undo castling
    if (move.isCastling) {
        if (isWhite) {
            if (targetSquare == 6) { // White king-side castling
                white_rooks |= (1ULL << 7);
                white_rooks &= ~(1ULL << 5);
            } else if (targetSquare == 2) { // White queen-side castling
                white_rooks |= (1ULL);
                white_rooks &= ~(1ULL << 3);
            }
        } else {
            if (targetSquare == 62) { // Black king-side castling
                black_rooks |= (1ULL << 63);
                black_rooks &= ~(1ULL << 61);
            } else if (targetSquare == 58) { // Black queen-side castling
                black_rooks |= (1ULL << 56);
                black_rooks &= ~(1ULL << 59);
            }
        }
    }

    // Restore en passant square
    enPassantSquare = move.previousEnPassantSquare;
}

uint64_t Board::getOccupiedSquares() const {
    return white_pawns | black_pawns | white_knights | black_knights |
           white_bishops | black_bishops | white_rooks | black_rooks |
           white_queens | black_queens | white_king | black_king;
}

int firstSetBit(uint64_t x) {
    for (int i = 0; i < 64; i++) {
        if (x & (1ULL << i)) return i;
    }
    return -1; // No bits set
}

void addPawnMovesToVector(uint64_t pawns, uint64_t movesBitboard, int direction, std::vector<Move>& moves) {
    while (movesBitboard) {
        // Extract the target square from the bitboard
        int targetSquare = firstSetBit(movesBitboard);
        movesBitboard &= movesBitboard - 1;

        // Calculate the source square based on direction
        int sourceSquare = targetSquare - direction;

        // Check if the move is a promotion
        bool isPromotion = (targetSquare / 8 == 7 || targetSquare / 8 == 0); // 7th rank for White, 0th rank for Black

        if (isPromotion) {
            // Add promotion moves for each promotion piece type
            moves.emplace_back(Move(sourceSquare, targetSquare, PieceType::Queen, false, false, false, true));
            moves.emplace_back(Move(sourceSquare, targetSquare, PieceType::Rook, false, false, false, true));
            moves.emplace_back(Move(sourceSquare, targetSquare, PieceType::Bishop, false, false, false, true));
            moves.emplace_back(Move(sourceSquare, targetSquare, PieceType::Knight, false, false, false, true));
        } else {
            // Add normal pawn move
            moves.emplace_back(Move(sourceSquare, targetSquare));
        }
    }
}

void addPawnCapturesToVector(uint64_t pawns, uint64_t capturesBitboard, int leftDirection, int rightDirection, std::vector<Move>& moves) {
    while (capturesBitboard) {
        // Extract the target square from the bitboard
        int targetSquare = firstSetBit(capturesBitboard); // Find the least significant bit set to 1
        capturesBitboard &= capturesBitboard - 1;         // Clear the least significant bit

        // Calculate the source square for left and right diagonal captures
        int sourceSquareLeft = targetSquare - leftDirection;
        int sourceSquareRight = targetSquare - rightDirection;

        // Check if the source square is a valid pawn position
        if (pawns & (1ULL << sourceSquareLeft)) {
            moves.emplace_back(Move(sourceSquareLeft, targetSquare, 0, true)); // Add left capture
        }
        if (pawns & (1ULL << sourceSquareRight)) {
            moves.emplace_back(Move(sourceSquareRight, targetSquare, 0, true)); // Add right capture
        }
    }
}

void addKnightMovesToVector(uint64_t knights, uint64_t movesBitboard, std::vector<Move>& moves) {
    while (movesBitboard) {
        int targetSquare = firstSetBit(movesBitboard);
        movesBitboard &= movesBitboard - 1;

        for (int sourceSquare = 0; sourceSquare < 64; ++sourceSquare) {
            if (knights & (1ULL << sourceSquare)) {
                if (MoveGeneration::knightAttacks[sourceSquare] & (1ULL << targetSquare)) {
                    moves.emplace_back(Move(sourceSquare, targetSquare));
                }
            }
        }
    }
}

void addSlidingPieceMovesToVector(uint64_t pieces, uint64_t movesBitboard, std::vector<Move>& moves) {
    while (movesBitboard) {
        int targetSquare = firstSetBit(movesBitboard);
        movesBitboard &= movesBitboard - 1;

        for (int sourceSquare = 0; sourceSquare < 64; ++sourceSquare) {
            if (pieces & (1ULL << sourceSquare)) {
                moves.emplace_back(Move(sourceSquare, targetSquare));
            }
        }
    }
}

void addKingMovesToVector(uint64_t king, uint64_t movesBitboard, std::vector<Move>& moves) {
    while (movesBitboard) {
        int targetSquare = firstSetBit(movesBitboard);
        movesBitboard &= movesBitboard - 1;

        for (int sourceSquare = 0; sourceSquare < 64; ++sourceSquare) {
            if (king & (1ULL << sourceSquare)) {
                moves.emplace_back(Move(sourceSquare, targetSquare));
            }
        }
    }
}



std::vector<Move> Board::generateMoves(bool isWhite) const {
    std::vector<Move> moves;

    uint64_t pawns = isWhite ? getWhitePawns() : getBlackPawns();
    uint64_t knights = isWhite ? getWhiteKnights() : getBlackKnights();
    uint64_t bishops = isWhite ? getWhiteBishops() : getBlackBishops();
    uint64_t rooks = isWhite ? getWhiteRooks() : getBlackRooks();
    uint64_t queens = isWhite ? getWhiteQueens() : getBlackQueens();
    uint64_t king = isWhite ? getWhiteKing() : getBlackKing();
    uint64_t ownPieces = isWhite ? getWhitePieces() : getBlackPieces();
    uint64_t opponentPieces = isWhite ? getBlackPieces() : getWhitePieces();
    uint64_t emptySquares = ~getOccupiedSquares();
    uint64_t enPassantSquare = getEnPassantSquare();

    uint64_t movesBitboard, capturesBitboard;

    // Debugging to confirm pawn bitboard correctness
    if (!isWhite) {
        // std::cout << "Black Pawns Bitboard:\n";
        // displayBitboard(pawns);
    } else {
        // std::cout << "White Pawns Bitboard:\n";
        // displayBitboard(pawns);
    }

    // Generate pawn moves
    int pawnDirection = isWhite ? 8 : -8;
    int pawnLeftCaptureDirection = isWhite ? 7 : -9;
    int pawnRightCaptureDirection = isWhite ? 9 : -7;
    // Generate moves and captures for pawns
    MoveGeneration::generatePawnMoves(pawns, emptySquares, opponentPieces, enPassantSquare, movesBitboard, capturesBitboard, isWhite);
    addPawnMovesToVector(pawns, movesBitboard, pawnDirection, moves);
    addPawnCapturesToVector(pawns, capturesBitboard, pawnLeftCaptureDirection, pawnRightCaptureDirection, moves);

    // Debugging to confirm generated pawn moves
    // std::cout << (isWhite ? "White Pawn Moves:\n" : "Black Pawn Moves:\n");
    // displayBitboard(movesBitboard);
    // std::cout << (isWhite ? "White Pawn Captures:\n" : "Black Pawn Captures:\n");
    // displayBitboard(capturesBitboard);

    // Generate knight moves
    MoveGeneration::generateKnightMoves(knights, ownPieces, opponentPieces, movesBitboard, capturesBitboard);
    addKnightMovesToVector(knights, movesBitboard, moves);

    // Generate bishop moves
    MoveGeneration::generateBishopMoves(bishops, ownPieces, opponentPieces, getOccupiedSquares(), movesBitboard, capturesBitboard);
    addSlidingPieceMovesToVector(bishops, movesBitboard, moves);

    // Generate rook moves
    MoveGeneration::generateRookMoves(rooks, ownPieces, opponentPieces, getOccupiedSquares(), movesBitboard, capturesBitboard);
    addSlidingPieceMovesToVector(rooks, movesBitboard, moves);

    // Generate queen moves
    MoveGeneration::generateQueenMoves(queens, ownPieces, opponentPieces, getOccupiedSquares(), movesBitboard, capturesBitboard);
    addSlidingPieceMovesToVector(queens, movesBitboard, moves);

    // Generate king moves
    MoveGeneration::generateKingMoves(king, ownPieces, opponentPieces, getOccupiedSquares(), movesBitboard, capturesBitboard);
    addKingMovesToVector(king, movesBitboard, moves);

    return moves;
}


uint64_t Board::generateOpponentAttacks(bool isWhite) const {
    uint64_t attacks = 0ULL;

    // Opponent pieces
    uint64_t opponentPawns = isWhite ? black_pawns : white_pawns;
    uint64_t opponentKnights = isWhite ? black_knights : white_knights;
    uint64_t opponentBishops = isWhite ? black_bishops : white_bishops;
    uint64_t opponentRooks = isWhite ? black_rooks : white_rooks;
    uint64_t opponentQueens = isWhite ? black_queens : white_queens;
    uint64_t opponentKing = isWhite ? black_king : white_king;

    // Pawn attacks
    if (isWhite) {
        attacks |= ((opponentPawns & 0xFEFEFEFEFEFEFEFEULL) << 7); // Right capture
        attacks |= ((opponentPawns & 0x7F7F7F7F7F7F7F7FULL) << 9); // Left capture
    } else {
        attacks |= ((opponentPawns & 0xFEFEFEFEFEFEFEFEULL) >> 9); // Right capture
        attacks |= ((opponentPawns & 0x7F7F7F7F7F7F7F7FULL) >> 7); // Left capture
    }

    // Knight attacks
    for (int square = 0; square < 64; ++square) {
        if (opponentKnights & (1ULL << square)) {
            attacks |= MoveGeneration::knightAttacks[square];
        }
    }

    // Bishop and queen diagonal attacks
    for (int square = 0; square < 64; ++square) {
        if ((opponentBishops | opponentQueens) & (1ULL << square)) {
            attacks |= MoveGeneration::generateBishopMovesFromSquare(square, getOccupiedSquares());
        }
    }

    // Rook and queen straight attacks
    for (int square = 0; square < 64; ++square) {
        if ((opponentRooks | opponentQueens) & (1ULL << square)) {
            attacks |= MoveGeneration::generateRookMovesFromSquare(square, getOccupiedSquares());
        }
    }

    // King attacks
    for (int square = 0; square < 64; ++square) {
        if (opponentKing & (1ULL << square)) {
            attacks |= MoveGeneration::generateKingMovesFromSquare(square, getOccupiedSquares());
        }
    }

    return attacks;
}



