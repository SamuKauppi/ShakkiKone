#include "transpositionTable.h"
#include "iostream"

// Information saved in hashmap
TTEntry::TTEntry(int key = -1, uint64_t zobristKey = 0, int depth = -1, int evaluation = 0, Move bestMove = Move(0,0,0,0)) 
{
	_key = key;
	_zobristKey = zobristKey;
	_depth = depth;
	_evaluation = evaluation;
	_bestMove = bestMove;
}

TranspositionTable::TranspositionTable()
{
	_positions = new TTEntry[_size];
	_positionRepeats = 0;
	_positionCount = 0;
	init_zobrist();
}

// run this only once at the beginning of the program to generate values that are used to generate keys for positions
void TranspositionTable::init_zobrist()
{

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 12; j++) {
			_zobristBoardValues[i][j] = uint64_prng();
		}
	}

	for (int i = 0; i < 8; i++)
	{
		_zobristEnPassantValues[i] = uint64_prng();
	}

	for (int i = 0; i < 4; i++)
	{
		_zobristCastlingValues[i] = uint64_prng();
	}
	
	_zobristColorValue = uint64_prng();
}



// Generates a hash key using the values generated above using bitwise operations. This part is very fast but,
// function is still somewhat expensive due to having to check every tile on the board for pieces. 
<<<<<<< Updated upstream
uint64_t TranspositionTable::generate_zobrist_key(GameState state)
=======
uint64_t TranspositionTable::generate_zobrist_key(GameState& state) const
>>>>>>> Stashed changes
{
	uint64_t k = 0;
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			int piece = state._board[row][column];
			if (piece == NA) continue;
			k ^= _zobristBoardValues[row * 8 + column][piece];
		}
	}

	if (state.TurnPlayer == BLACK) k ^= _zobristColorValue;
	if (state._w_long_castle) k ^= _zobristCastlingValues[0];
	if (state._w_short_castle) k ^= _zobristCastlingValues[1];
	if (state._b_long_castle) k ^= _zobristCastlingValues[2];
	if (state._b_short_castle) k ^= _zobristCastlingValues[3];
	if (state._doubleStep != -1) k ^= _zobristEnPassantValues[state._doubleStep];
	return k;
}

void TranspositionTable::hash_new_position(uint64_t zobristKey, int depth, int evaluation, Move m) {
	int key = hash_key(zobristKey);
	if (_positions[key]._zobristKey == zobristKey)
	{
		if (_positions[key]._depth >= depth) 
		{ 
		return;
		}
		if (_positions[key]._depth < depth) {
			_positions[key] = TTEntry(key, zobristKey, depth, evaluation, m);
			return;
		}
	}
	_positions[key] = TTEntry(key, zobristKey, depth, evaluation, m);
}


bool TranspositionTable::is_state_hashed(uint64_t zobristKey)
{
	int key = hash_key(zobristKey);
	if (_positions[key]._zobristKey == zobristKey) return true;
	else return false;
}

int TranspositionTable::get_hashed_evaluation(uint64_t zobristKey)
{
	int key = hash_key(zobristKey);
	return _positions[key]._evaluation;
}

int TranspositionTable::hash_key(uint64_t zobristKey)
{
	return zobristKey % _size;
}

bool TranspositionTable::is_state_calculated(uint64_t zobristKey, int depth)
{
	int key = hash_key(zobristKey);
	if (_positions[key]._zobristKey == zobristKey)
	{
		if (_positions[key]._depth >= depth)
		{
			_positionRepeats++;
			return true;
		}
	}
	return false;
}

MinimaxValue TranspositionTable::get_value(uint64_t zobristKey)
{
	int key = hash_key(zobristKey);
	TTEntry entry = _positions[key];
	return MinimaxValue(entry._evaluation, entry._bestMove, entry._depth);
}

// simple pseudo random number generator
uint64_t TranspositionTable::uint64_prng()
{
	mt19937_64 mt(_seed());
	uniform_int_distribution<uint64_t> number(0, UINT64_MAX);
	return number(mt);
}