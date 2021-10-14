/*
An online version of dynamic programming-based edit distance
*/

#ifndef TRIMATCH_EVAL_ONLINE_EDIT_DISTANCE_DP_HPP
#define TRIMATCH_EVAL_ONLINE_EDIT_DISTANCE_DP_HPP

#include <string>

// symbol-wise edit distance computation using dynamic programing
template<typename text = std::string>
class OnlineEditDistance
{
public:
	using symbol = typename text::value_type;
	using integer = typename text::size_type;

	const text& pattern;
	const integer max;

	OnlineEditDistance(const text& pattern, integer max = 2):
		pattern(pattern), max(max), D(pattern.size() + max + 2), i(0)
	{
		for(auto& d: D)
			d.resize(pattern.size() + 1);
		for(integer j = 0; j < D[i].size(); ++j)
			D[i][j] = j;
	}

	bool update(const symbol c)
	{
		++i;

		D[i][0] = D[i - 1][0] + 1;
		integer min = D[i][0];
		for(integer j = 1; j < D[i].size(); ++j){
			auto del = D[i][j - 1] + 1;
			auto ins = D[i - 1][j] + 1;
			auto sub = D[i - 1][j - 1] + (pattern[j - 1] == c ? 0 : 1);
			D[i][j] = std::min({del, ins, sub});
			min = std::min(min, D[i][j]);
		}
		if(min > max)
			back();

		return min <= max;
	}

	integer distance() const
	{
		return D[i].back();
	}

	void back()
	{
		if(i > 0)
			--i;
	}

private:
	std::vector<std::vector<integer>> D;
	integer i;
};

#endif
