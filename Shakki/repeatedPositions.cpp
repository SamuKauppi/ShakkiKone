#include "repeatedPositions.h"
#include <iostream>

void RepeatedPositions::update_position(uint64_t key) {
	int index = hash_index(key);

	// if position is in array update count
	if (_stored_positions[index]._key == key)
	{
		_stored_positions[index]._count++;
	}
	else
	{
		// else add position to array
		_stored_positions[index] = PositionEntry(key);
	}

}

bool RepeatedPositions::is_draw_by_repetition(uint64_t key) const {
	int index = hash_index(key);

	// check if position has been played three times
	if (_stored_positions[index]._count >= 3) return true;
	else return false;
}

bool RepeatedPositions::is_position_repeated(uint64_t key) const {
	int index = hash_index(key);
	if (_stored_positions[index]._key == key && _stored_positions[index]._count > 1) {
		return true;
	}
	else return false;
}

int RepeatedPositions::hash_index(uint64_t key) const {
	int index = key % 1000;
	while (_stored_positions[index]._key != key && _stored_positions[index]._key != 0)
	{
		index++;
		if (index >= 1000) index = 0;
	}
	return index;
}

void RepeatedPositions::remove_repeat(uint64_t key) {
	int index = hash_index(key);
	if (_stored_positions[index]._count > 1)
	{
		_stored_positions[index]._count--;
		return;
	}
	else
	{
		_stored_positions[index] = PositionEntry();
		return;
	}
	return;
}