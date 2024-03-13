#pragma once
using namespace std;
# include <stdint.h>

class PositionEntry
{
public:
	PositionEntry(uint64_t key)
	{
		_key = key;
		_count = 1;
	}
	PositionEntry()
	{
		_key = 0;
		_count = 0;
	}

private:
	uint64_t _key;
	int _count;
	friend class RepeatedPositions;
};


class RepeatedPositions
{
public:
	RepeatedPositions()
	{
		_stored_positions = new PositionEntry[1000];
	}

	void update_position(uint64_t key);

	bool is_draw_by_repetition(uint64_t key) const;

	bool is_position_repeated(uint64_t key) const;

	int hash_index(uint64_t key) const;

	void remove_repeat(uint64_t key);

private:
	PositionEntry* _stored_positions;
};