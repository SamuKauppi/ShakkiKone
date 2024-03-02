#pragma once
#include <random>
#include "move.h"
#include "chess.h"
#include "gamestate.h"
#include "minimaxValue.h"

class GameState;

class TTEntry
{
public:
	TTEntry(int key, uint64_t zobristKey, int depth, int evaluation, Move bestMove);

private:
	int _key;
	uint64_t _zobristKey;
	int _depth;
	int _evaluation;
	Move _bestMove;

	friend class TranspositionTable;
};

class TranspositionTable
{
public:
	TranspositionTable();
	void init_zobrist();
	uint64_t generate_zobrist_key(GameState state);
	int hash_key(uint64_t zobristKey);
	void hash_new_position(GameState state, int depth, int evaluation, Move m);
	uint64_t uint64_prng();
	bool is_state_hashed(uint64_t zobristKey);
	bool is_state_calculated(uint64_t zobristKey, int depth);
	MinimaxValue get_value(uint64_t zobristKey);

	int get_hashed_evaluation(uint64_t zobristKey);

	// tracking position counts. Not important only used for performance data
	// public for ease of use
	int _positionCount;
	int _positionRepeats;

private:
	// values used to generate keys for positions
	// 64x12 so we have a value for each piece in each position
	// 1 to indicate turn order
	// 8 for possible enpassant and it's column
	// 4 for each castling rights
	uint64_t _zobristBoardValues[64][12];
	uint64_t _zobristColorValue;
	uint64_t _zobristEnPassantValues[8];
	uint64_t _zobristCastlingValues[4];
	random_device _seed;

	// array for positions
	// size fixed 10,000,000 positions for now
	// roughly 500mb
	int _size = 10000000;
	TTEntry* _positions;
};