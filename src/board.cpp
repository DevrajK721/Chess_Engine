#include "board.h"

// Constructor: Initializes bitboards to zero
Board::Board()
    : white_pawns(0ULL), white_knights(0ULL), white_bishops(0ULL), white_rooks(0ULL), white_queens(0ULL), white_king(0ULL),
      black_pawns(0ULL), black_knights(0ULL), black_bishops(0ULL), black_rooks(0ULL), black_queens(0ULL), black_king(0ULL),
      white_pieces(0ULL), black_pieces(0ULL), occupied(0ULL),
      enPassantSquare(0ULL), whiteCanCastleKingSide(true), whitecanCastleQueenSide(true),
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
    whiteCanCastleKingSide = whitecanCastleQueenSide = true;
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
