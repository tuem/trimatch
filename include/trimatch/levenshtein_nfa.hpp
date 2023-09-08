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
Simple Levenshtein automata originally by Jules Jacobs
https://julesjacobs.com/2015/06/17/disqus-levenshtein-simple-and-fast.html
*/

#ifndef TRIMATCH_LEVENSHTEIN_NFA
#define TRIMATCH_LEVENSHTEIN_NFA

#include <cstddef>
#include <vector>
#include <set>

namespace trimatch
{

template<
	typename text,
	typename integer = std::uint32_t
>
class LevenshteinNFA
{
public:
	using symbol = typename text::value_type;
	using state = std::pair<integer, integer>; // position, edits

	const text pattern;
	const integer max_edits;

	LevenshteinNFA(const text& pattern, integer max_edits):
		pattern(pattern), max_edits(max_edits)
	{}

	std::vector<state> start() const
	{
		std::vector<state> states;
		for(integer i = 0; i <= max_edits; ++i)
			states.emplace_back(i, i);
		return states;
	}

	std::vector<state> step(const std::vector<state>& states, symbol c) const
	{
		std::vector<state> new_states;
		if(!states.empty() && states.front().first == 0 && states.front().second < max_edits)
			new_states.emplace_back(0, states.front().second + 1);

		for(integer j = 0; j < states.size(); ++j){
			auto i = states[j].first;
			if(i == pattern.size())
				break;
			auto d = states[j].second + (pattern[i] == c ? 0 : 1);
			if(!new_states.empty() && new_states.back().first == i)
				d = std::min(d, new_states.back().second + 1);
			if(j + 1 < states.size() && states[j + 1].first == i + 1)
				d = std::min(d, states[j + 1].second + 1);
			if(d <= max_edits)
				new_states.emplace_back(i + 1, d);
		}

		return new_states;
	}

	bool is_match(const std::vector<state>& states) const
	{
		return !states.empty() && states.back().first == pattern.size();
	}

	bool can_match(const std::vector<state>& states) const
	{
		return !states.empty();
	}

	std::set<symbol> transitions() const
	{
		return std::set<symbol>(pattern.begin(), pattern.end());
	}
};

}

#endif
