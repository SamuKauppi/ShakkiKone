#include "gameState.h"
#include "move.h"
#include "limits"
#include <vector>
#include <iostream>

static float minimax(GameState& state, int depth)
{
	// Generate moves for this state
	vector<Move> moves;
	state.get_moves(moves);

	// If no moves remain, game is over
	if (moves.size() <= 0)
	{
		return state.score_board();
	}

	// Reached max depth
	if (depth <= 0)
	{
		return state.evaluate();
	}

	float best_value = state.TurnPlayer == WHITE ?
		numeric_limits<float>::min() : numeric_limits<float>::max();

	for (Move& m : moves)
	{
		GameState new_state = state;
		new_state.make_move(m);

		float value = minimax(new_state, depth - 1);

		if (state.TurnPlayer == WHITE && value > best_value)
		{
			best_value = value;
		}
		else if (state.TurnPlayer == BLACK && value < best_value)
		{
			best_value = value;
		}
	}

	return best_value;
}

static int get_valid_move_index(vector<Move>& moves, string chosen)
{
	for (int i = 0; i < moves.size(); i++)
	{
		if (chosen == moves[i].get_move_name())
		{
			return i;
		}
	}
	return -1;
}

static void game_loop()
{
	srand((unsigned)time(NULL));
	GameState state;

	float value = minimax(state, 2);
	cout << value << "\n";
	system("pause");


	state.print_board();
	while (true)
	{
		vector<Move> moves;
		state.get_moves(moves);

		if (moves.size() <= 0)
		{
			break;
		}

		for (int i = 0; i < moves.size(); i++)
		{
			cout << moves[i].get_move_name();
			if (i < moves.size() - 1)
			{
				cout << ", ";
			}
		}

		cout << "\nMake a move: ";
		int move_index = -1;

		while (true)
		{
			string chosen = "";
			//cin >> chosen;
			chosen = moves[rand() % moves.size()].get_move_name();
			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}

			cout << "Not a valid move\nMake a move: ";
		}

		state.make_move(moves[move_index]);
		state.print_board();

		system("pause");
	}
}

int main()
{
	game_loop();
	return 0;
}