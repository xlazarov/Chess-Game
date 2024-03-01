# Chess-Game
This repository contains a C++ implementation of the game of chess. The header file chess.hpp defines the structure and functions necessary to represent and manipulate a chess game, while the corresponding implementation file chess.cpp provides the actual implementation of these functionalities.

## Overview
The implementation provides functionality for playing chess according to standard rules. It includes definitions for positions, piece types, players, and possible outcomes of moves.

## Functionality

### Constructors
- `chess()`: Constructs a new game of chess in its default starting position.

### Methods
- `play()`: Moves a piece from one position to another on the chessboard, considering various rules such as pawn promotion and castling. Returns the result of the move.
- `at()`: Retrieves the occupant of a given position on the chessboard.

## Usage

To use this chess implementation, include the `chess.hpp` header file in your C++ project and ensure that the `chess.cpp` implementation file is compiled and linked properly.

You can try playing the game in the terminal. By calling the `play()` method, you can make the moves and see the updated board with moved pieces drawn in the terminal after each move.

