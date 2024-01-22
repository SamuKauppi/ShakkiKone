#include "gameState.h"
#include "move.h"
#include <vector>
#include <iostream>

int main()
{
	GameState state;

	Move move(6, 4, 4, 4);
	state.make_move(move);

	Move stringMove("h2h6");
	state.make_move(stringMove);

	Move rookMove("h1h4");
	state.make_move(rookMove);

	state.print_board();
	vector<Move> moves;
	state.get_rook_moves(4, 7, WHITE, 4, moves);
	cout << moves.size();

	return 0;
}