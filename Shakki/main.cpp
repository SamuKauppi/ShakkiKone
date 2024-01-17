#include "gameState.h"
#include "move.h"
#include <vector>


int main()
{
	GameState state;

	Move move(6, 4, 4, 4);
	state.make_move(move);

	Move stringMove("h2h4");
	state.make_move(stringMove);
	state.print_board();


	return 0;
}