
# Board Representation (Bitboard Setup)

## 1. Define the Bitboard Structure

A **bitboard** is a 64-bit integer used to represent the state of a chessboard. Each bit corresponds to a square on the chessboard, where a bit set to `1` indicates the presence of a piece, and a bit set to `0` indicates an empty square.

### 1.1 Bitboard Layout
- Each bit in the 64-bit integer represents a square on the chessboard.
- The least significant bit (bit 0) represents the `a1` square, and the most significant bit (bit 63) represents the `h8` square.
- For simplicity and clarity, the bit positions can be visualized as follows:

| 56 | 57 | 58 | 59 | 60 | 61 | 62 | 63 |
|----|----|----|----|----|----|----|----|
| 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 |
| 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
| 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 |
| 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
| 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
| 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 |
| 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  |

This layout allows easy manipulation and querying of specific squares and ranks.

### 1.2 Separate Bitboards for Different Pieces
- Maintain separate bitboards for each piece type and color (e.g., one for white pawns, another for black knights).
- Example:
  - `uint64_t white_pawns;`
  - `uint64_t black_knights;`

This approach allows efficient representation and manipulation of each piece type on the board.

## 2. Bitboard Operations

To manipulate bitboards effectively, use bitwise operations. Below are some common operations.

### 2.1 Setting and Clearing Bits
- **Set a Bit**: Use the bitwise OR `|` operation to set a specific bit to `1`.
  ```cpp
  bitboard |= (1ULL << square); // Set the bit at 'square' to 1
  ```
- **Clear a Bit**: Use the bitwise AND `&` with the NOT `~` operation to set a specific bit to `0`.
  ```cpp
  bitboard &= ~(1ULL << square); // Clear the bit at 'square' to 0
  ```

### 2.2 Checking Bits
- **Check if a Bit is Set**: Use the bitwise AND `&` to check if a specific bit is set to `1`.
  ```cpp
  bool is_set = (bitboard & (1ULL << square)) != 0;
  ```

## 3. Example Code for Bitboard Initialization and Basic Operations

The following code initializes a bitboard and demonstrates some basic operations.

```cpp
#include <iostream>
#include <cstdint>

// Initialize an empty bitboard for white pawns
uint64_t white_pawns = 0ULL;

// Set a white pawn on e2 (square 12 in bit indexing)
void set_pawn(uint64_t& bitboard, int square) {
    bitboard |= (1ULL << square); // Set the bit at 'square' to 1
}

// Remove a pawn from e2
void clear_square(uint64_t& bitboard, int square) {
    bitboard &= ~(1ULL << square); // Clear the bit at 'square' to 0
}

// Check if there's a pawn on e2
bool is_pawn_on_square(const uint64_t& bitboard, int square) {
    return (bitboard & (1ULL << square)) != 0;
}

int main() {
    int e2_square = 12;

    // Place a pawn on e2
    set_pawn(white_pawns, e2_square);
    std::cout << "Pawn on e2: " << is_pawn_on_square(white_pawns, e2_square) << std::endl;

    // Clear the pawn from e2
    clear_square(white_pawns, e2_square);
    std::cout << "Pawn on e2 after clearing: " << is_pawn_on_square(white_pawns, e2_square) << std::endl;

    return 0;
}
```

## 4. Summary of Bitboard Operations

- **Setting a Square**: Use `|` to set bits to `1`.
- **Clearing a Square**: Use `& ~` to set bits to `0`.
- **Checking a Square**: Use `&` to verify if a bit is `1` or `0`.

## 5. Advanced Bitboard Usage
### 5.1 Efficient Board Queries
- **Row and Column Manipulation**: Use shifting operations to move across ranks or files.
- **Attack Patterns**: Precompute and store common attack patterns for pieces, using bitboards for fast move generation.

By following these guidelines, you can set up a bitboard structure that allows efficient manipulation and querying of board positions, which is essential for a high-performance chess engine.
