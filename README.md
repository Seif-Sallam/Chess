# Chess
A chess game using SFML.

## Progress
* FEN's format for the chess board (without board state).
* Movement of each piece.
* Castling
* King cannot move into check nor castle while in check or castle while the path is blocked.

## Bugs
* When the king is in check, some moves are generated that will put the king in check next move.
* When the king in check you can still move any piece other than the king ignoring the check.


## NOTE
En passant can be done by saving the en passant available positions and make sure that we are a pawn and then see if we can capture one that is one of the en passant moves.