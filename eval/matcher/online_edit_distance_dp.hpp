/*
trimatch
https://github.com/tuem/trimatch

Copyright 2021 Takashi Uemura

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
An online version of dynamic programming-based edit distance
*/

#ifndef TRIMATCH_EVAL_ONLINE_EDIT_DISTANCE_DP
#define TRIMATCH_EVAL_ONLINE_EDIT_DISTANCE_DP

#include <cstddef>
#include <string>

// symbol-wise edit distance computation using dynamic programing
template<typename text = std::string, typename integer = std::uint32_t>
class OnlineEditDistance
{
public:
	using symbol = typename text::value_type;

	const text& pattern;
	const integer max;

	OnlineEditDistance(const text& pattern, integer max = 2):
		pattern(pattern), max(max), D(pattern.size() + max + 2), i(0)
	{
		for(auto& d: D)
			d.resize(pattern.size() + 1);
		for(integer j = 0; j < D[i].size(); ++j)
			D[i][j] = static_cast<integer>(j);
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

	bool matched() const
	{
		return D[i].back() <= max;
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
