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

	Move rookMove("h1e5");
	state.make_move(rookMove);

	state.print_board();
	vector<Move> rook_moves;
	vector<Move> bishop_moves;
	vector<Move> queen_moves;
	state.get_officer_moves(3, 4, WHITE, rook_moves);
	state.get_bishop_moves(3, 4, WHITE, bishop_moves);
	
	state.get_officer_moves(3, 4, WHITE, queen_moves);
	state.get_bishop_moves(3, 4, WHITE, queen_moves);

	cout << rook_moves.size() << "\n";
	cout << bishop_moves.size() << "\n";
	cout << queen_moves.size();

	return 0;
}