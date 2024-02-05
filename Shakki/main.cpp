#include "gameState.h"
#include "move.h"
#include <vector>
#include <iostream>


static void test()
{
	GameState state;

	Move move(6, 4, 4, 4);
	state.make_move(move);

	Move stringMove("h2h6");
	state.make_move(stringMove);

	Move rookMove("h1e5");
	state.make_move(rookMove);

	Move pawnMove("a7a3");
	state.make_move(pawnMove);

	Move promotion("a2a7");
	state.make_move(promotion);

	Move pawnMove2("a3a2");
	state.make_move(pawnMove2);

	state.print_board();
	vector<Move> rook_moves;
	vector<Move> bishop_moves;
	vector<Move> queen_moves;
	vector<Move> knight_moves;
	vector<Move> king_moves;
	vector<Move> pawn_moves;
	vector<Move> pro_moves;

	state.get_pawn_moves(1, 0, WHITE, pro_moves);

	state.get_rook_moves(3, 4, WHITE, rook_moves);
	state.get_bishop_moves(3, 4, WHITE, bishop_moves);

	state.get_rook_moves(3, 4, WHITE, queen_moves);
	state.get_bishop_moves(3, 4, WHITE, queen_moves);

	state.get_knight_moves(3, 4, WHITE, knight_moves);

	state.get_king_moves(3, 4, WHITE, king_moves);

	state.get_pawn_moves(6, 1, WHITE, pawn_moves);


	state.get_pawn_moves(6, 0, BLACK, pro_moves);

	cout << rook_moves.size() << "\n";
	cout << bishop_moves.size() << "\n";
	cout << knight_moves.size() << "\n";
	cout << king_moves.size() << "\n";
	cout << pawn_moves.size() << "\n";
	cout << pro_moves.size() << "\n";
	cout << queen_moves.size();
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
	GameState state;
	state.print_board();
	while (true)
	{
		vector<Move> moves;
		state.get_all_moves(state.TurnPlayer, moves);

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
			cin >> chosen;

			move_index = get_valid_move_index(moves, chosen);
			if (move_index != -1)
			{
				break;
			}

			cout << "Not a valid move\nMake a move: ";
		}

		state.make_move(moves[move_index]);
		state.print_board();
	}
}

int main()
{
	// Test function
	//test();

	game_loop();

	return 0;
}