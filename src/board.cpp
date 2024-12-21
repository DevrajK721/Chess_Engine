#include "board.h"

// Bit Board Layout
/*
56 57 58 59 60 61 62 63
48 49 50 51 52 53 54 55
40 41 42 43 44 45 46 47
32 33 34 35 36 37 38 39
24 25 26 27 28 29 30 31
16 17 18 19 20 21 22 23
8  9 10 11 12 13 14 15
0  1  2  3  4  5  6  7
 */

// Constructor: Initializes bitboards to zero
Board::Board()
    : white_pawns(0ULL), white_knights(0ULL), white_bishops(0ULL), white_rooks(0ULL), white_queens(0ULL), white_king(0ULL),
      black_pawns(0ULL), black_knights(0ULL), black_bishops(0ULL), black_rooks(0ULL), black_queens(0ULL), black_king(0ULL),
      white_pieces(0ULL), black_pieces(0ULL), occupied(0ULL) {} // OULL is a 64-bit unsigned long long integer with all bits set to 0

// Sets up the starting position for the board
void Board::initializePosition() {
    white_pieces = 0ULL; // Clear all white pieces
    black_pieces = 0ULL; // Clear all black pieces
    occupied = 0ULL;     // Clear all occupied squares

    // White Pawns: Second rank (0x000000000000FF00ULL)
    white_pawns = 0x000000000000FF00ULL; // This is in hexadecimal format (converter available online)
    white_pieces |= white_pawns;   // Add white pawns to the white_pieces aggregate
    occupied |= white_pawns;       // Add white pawns to the occupied bitboard

    // Black Pawns: Seventh rank (0x00FF000000000000ULL)
    black_pawns = 0x00FF000000000000ULL; // Represents all black pawns on the seventh rank
    black_pieces |= black_pawns;   // Add black pawns to the black_pieces aggregate
    occupied |= black_pawns;       // Add black pawns to the occupied bitboard

    // White Knights: Initial squares on b1 and g1 (0x0000000000000042ULL)
    white_knights = 0x0000000000000042ULL; // Two bits set for the white knights on b1 and g1
    white_pieces |= white_knights;   // Add white knights to the white_pieces aggregate
    occupied |= white_knights;       // Add white knights to the occupied bitboard

    // Black Knights: Initial squares on b8 and g8 (0x4200000000000000ULL)
    black_knights = 0x4200000000000000ULL; // Two bits set for the black knights on b8 and g8
    black_pieces |= black_knights;   // Add black knights to the black_pieces aggregate
    occupied |= black_knights;       // Add black knights to the occupied bitboard

    // White Bishops: Initial squares on c1 and f1 (0x0000000000000024ULL)
    white_bishops = 0x0000000000000024ULL; // Two bits set for the white bishops on c1 and f1
    white_pieces |= white_bishops;   // Add white bishops to the white_pieces aggregate
    occupied |= white_bishops;       // Add white bishops to the occupied bitboard

    // Black Bishops: Initial squares on c8 and f8 (0x2400000000000000ULL)
    black_bishops = 0x2400000000000000ULL; // Two bits set for the black bishops on c8 and f8
    black_pieces |= black_bishops;   // Add black bishops to the black_pieces aggregate
    occupied |= black_bishops;       // Add black bishops to the occupied bitboard

    // White Rooks: Initial squares on a1 and h1 (0x0000000000000081ULL)
    white_rooks = 0x0000000000000081ULL; // Two bits set for the white rooks on a1 and h1
    white_pieces |= white_rooks;     // Add white rooks to the white_pieces aggregate
    occupied |= white_rooks;         // Add white rooks to the occupied bitboard

    // Black Rooks: Initial squares on a8 and h8 (0x8100000000000000ULL)
    black_rooks = 0x8100000000000000ULL; // Two bits set for the black rooks on a8 and h8
    black_pieces |= black_rooks;     // Add black rooks to the black_pieces aggregate
    occupied |= black_rooks;         // Add black rooks to the occupied bitboard

    // White Queen: Initial square on d1 (0x0000000000000008ULL)
    white_queens = 0x0000000000000008ULL; // Single bit set for the white queen on d1
    white_pieces |= white_queens;    // Add white queen to the white_pieces aggregate
    occupied |= white_queens;        // Add white queen to the occupied bitboard

    // Black Queen: Initial square on d8 (0x0800000000000000ULL)
    black_queens = 0x0800000000000000ULL; // Single bit set for the black queen on d8
    black_pieces |= black_queens;    // Add black queen to the black_pieces aggregate
    occupied |= black_queens;        // Add black queen to the occupied bitboard

    // White King: Initial square on e1 (0x0000000000000010ULL)
    white_king = 0x0000000000000010ULL; // Single bit set for the white king on e1
    white_pieces |= white_king;      // Add white king to the white_pieces aggregate
    occupied |= white_king;          // Add white king to the occupied bitboard

    // Black King: Initial square on e8 (0x1000000000000000ULL)
    black_king = 0x1000000000000000ULL; // Single bit set for the black king on e8
    black_pieces |= black_king;      // Add black king to the black_pieces aggregate
    occupied |= black_king;          // Add black king to the occupied bitboard
}

// Sets a piece at a specific square in the given bitboard
void Board::setPiece(int square, uint64_t& bitboard) {
    bitboard |= (1ULL << square);  // Set the bit at the position 'square' to 1
}

// Clears a piece at a specific square in the given bitboard
void Board::clearPiece(int square, uint64_t& bitboard) {
    bitboard &= ~(1ULL << square); // Clear the bit at the position 'square' to 0
}

// Checks if a specific square is occupied in the given bitboard
bool Board::isSquareOccupied(int square, const uint64_t& bitboard) const {
    return (bitboard & (1ULL << square)) != 0; // Returns true if the bit is set
}

// Displays the bitboard as an 8x8 grid (for debugging purposes)
void Board::displayBitboard(const uint64_t& bitboard) {
    for (int rank = 7; rank >= 0; --rank) { // Display from rank 8 to rank 1
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bitboard & (1ULL << square)) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}