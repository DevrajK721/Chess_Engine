#include "engine.h"
#include "board.h"

int main() {
    startEngine();

    // Create an instance of Board
    Board board;

    // Initialize the board to the starting position
    board.initializePosition();

    // Display each bitboard to verify initialization using getter methods
    std::cout << "White Pawns:" << std::endl;
    board.displayBitboard(board.getWhitePawns());

    std::cout << "Black Pawns:" << std::endl;
    board.displayBitboard(board.getBlackPawns());

    std::cout << "White Knights:" << std::endl;
    board.displayBitboard(board.getWhiteKnights());

    std::cout << "Black Knights:" << std::endl;
    board.displayBitboard(board.getBlackKnights());

    std::cout << "White Bishops:" << std::endl;
    board.displayBitboard(board.getWhiteBishops());

    std::cout << "Black Bishops:" << std::endl;
    board.displayBitboard(board.getBlackBishops());

    std::cout << "White Rooks:" << std::endl;
    board.displayBitboard(board.getWhiteRooks());

    std::cout << "Black Rooks:" << std::endl;
    board.displayBitboard(board.getBlackRooks());

    std::cout << "White Queen:" << std::endl;
    board.displayBitboard(board.getWhiteQueens());

    std::cout << "Black Queen:" << std::endl;
    board.displayBitboard(board.getBlackQueens());

    std::cout << "White King:" << std::endl;
    board.displayBitboard(board.getWhiteKing());

    std::cout << "Black King:" << std::endl;
    board.displayBitboard(board.getBlackKing());

    // Display aggregate bitboards to verify they were set up correctly
    std::cout << "White Pieces (all white pieces combined):" << std::endl;
    board.displayBitboard(board.getWhitePieces());

    std::cout << "Black Pieces (all black pieces combined):" << std::endl;
    board.displayBitboard(board.getBlackPieces());

    std::cout << "Occupied Squares (all pieces combined):" << std::endl;
    board.displayBitboard(board.getOccupiedSquares());

    return 0;
}
