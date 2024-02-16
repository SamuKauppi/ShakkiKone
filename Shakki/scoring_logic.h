#pragma once
#include "math.h"
/// <summary>
/// Returns score based on how close is the axis to other_axis side
/// min = 0
/// max = 3.5
/// </summary>
/// <param name="axis"></param>
/// <param name="best_axis"></param>
/// <param name="multiplier"></param>
/// <returns></returns>
static float get_score_from_one_side(int axis, int other_axis, float multiplier = 1.0f)
{
	return (7.0f - abs(other_axis - axis)) * 0.5f;
}

/// <summary>
/// Returns score based on how close to the corner (0/7) axis is
/// min = 0
/// max = 3.5
/// </summary>
/// <param name="axis"></param>
/// <returns></returns>
static float get_score_from_both_side(int axis)
{
	return (abs(3 - axis) - 0.5f) * 1.166f;
}

/// <summary>
/// Returns score based on how close to the center (3.5) axis is
/// min = 0
/// max = 3
/// </summary>
/// <param name="axis"></param>
/// <returns></returns>
static float get_score_from_middle(int axis)
{
	return (3.5f - abs(3 - axis));
}
/// <summary>
/// Returns score based on how close is the piece to center of the board
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="multiplier"></param>
/// <returns></returns>
static float get_center_score(int row, int column, float multiplier = 1.0f)
{
	float center_score = 0.0f;
	center_score += get_score_from_middle(column) * multiplier;
	center_score += get_score_from_middle(row) * multiplier;
	return center_score * 0.5f;
}


/// <summary>
/// Returns score based on how close is the piece to a corner
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="multiplier"></param>
/// <returns></returns>
static float get_corner_score(int row, int column, float multiplier = 1.0f)
{
	float corner_score = 0.0f;
	corner_score = get_score_from_both_side(row);
	corner_score = get_score_from_both_side(column);
	return corner_score * 0.5f;
}