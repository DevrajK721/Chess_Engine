# Chess_Engine

## Using the Chess Engine

## Theory
### 1. **Bitboard Representation**
- Implemented efficient bitboard-based representation for the chessboard.
- Each piece and side (white/black) is represented using a 64-bit integer, where each bit corresponds to a square on the chessboard.

### 2. **Move Representation**
- Developed a `Move` structure to encapsulate information about individual moves:
    - Source square.
    - Target square.
    - Flags for captures, promotions, en passant, and castling.

### 3. **Move Generation**
- **Pawn Moves**:
    - Single and double forward moves.
    - Captures (including en passant).
    - Promotions.
- **Knight Moves**:
    - Precomputed attack masks for all possible knight moves.
- **Bishop, Rook, and Queen Moves**:
    - Implemented sliding piece attack generation using ray-tracing techniques.
- **King Moves**:
    - Normal moves.
    - Castling moves with proper validation.

### 4. **Move Validation**
- Added functionality to validate moves:
    - Ensured that the king does not move into check.
    - Handled pinned pieces (pieces that cannot move because they are shielding the king from a check).

### 5. **Board Manipulation**
- Developed functions to make and undo moves:
    - Adjusted bitboards to reflect piece movements.
    - Handled special cases like en passant, castling, and promotions.
    - Maintained state consistency for captures and occupied squares.

### 6. **Move Legality Checks**
- Implemented methods to:
    - Check if a square is attacked by a given side.
    - Determine whether the king is in check.
    - Filter legal moves from generated pseudo-legal moves.

### 7. **Testing and Debugging**
- Debugged move generation and board manipulation functions using various test scenarios.
- Addressed edge cases such as pawn promotions and special moves.

### 8. **Code Refinement**
- Modularized the codebase by splitting responsibilities into headers and implementation files.
- Ensured clarity and maintainability by using descriptive variable names and consistent formatting.