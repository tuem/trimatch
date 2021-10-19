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
	DFA version of Levenshtein automata originally by Jules Jacobs
	https://julesjacobs.com/2015/06/17/disqus-levenshtein-simple-and-fast.html
*/

#ifndef TRIMATCH_LEVENSHTEIN_DFA
#define TRIMATCH_LEVENSHTEIN_DFA

#include <vector>
#include <set>
#include <map>

#include "levenshtein_nfa.hpp"

namespace trimatch
{

template<typename text>
class LevenshteinDFA
{
public:
	using symbol = typename text::value_type;
	using integer = typename text::size_type;

	struct state;
	struct transition;

	const text pattern;
	const integer max_edits;

	static constexpr const symbol nullchar();

	LevenshteinDFA(const LevenshteinNFA<text>& nfa);
	LevenshteinDFA(const text& pattern, integer max_edits);

	bool update(symbol c);
	bool matched() const;
	void back();
	integer max_distance() const;
	integer distance() const;

private:
	using nfa_state = typename LevenshteinNFA<text>::state;

	std::vector<state> states;
	std::vector<transition> transitions;

	std::vector<integer> current_states;

	integer convert(const LevenshteinNFA<text>& nfa,
		const std::vector<nfa_state>& nfa_states, const std::set<symbol>& nfa_transitions,
		std::map<std::vector<nfa_state>, integer>& dfa_states, integer& counter);
};

template<typename text>
struct LevenshteinDFA<text>::state
{
	integer start;
	bool match;
	integer edits;

	state();
	state(integer start, bool match, integer edits);
};

template<typename text>
struct LevenshteinDFA<text>::transition
{
	integer id;
	integer next;
	symbol label;

	transition(integer id, integer next, symbol label);
	bool operator<(const transition& s) const;
};

template<typename text>
LevenshteinDFA<text>::state::state(){}

template<typename text>
LevenshteinDFA<text>::state::state(integer start, bool match, integer edits):
	start(start), match(match), edits(edits)
{}

template<typename text>
LevenshteinDFA<text>::transition::transition(integer id, integer next, symbol label):
	id(id), next(next), label(label)
{}

template<typename text>
inline bool LevenshteinDFA<text>::transition::operator<(const transition& s) const
{
	if(id != s.id)
		return id < s.id;
	else if(label != s.label)
		return label == nullchar() ? false : (s.label == nullchar() ? true : label < s.label);
	else
		return next < s.next;
}

template<typename text>
LevenshteinDFA<text>::LevenshteinDFA(const LevenshteinNFA<text>& nfa):
	pattern(nfa.pattern), max_edits(nfa.max_edits)
{
	auto nfa_states = nfa.start();
	auto nfa_transitions = nfa.transitions();
	std::map<std::vector<nfa_state>, integer> dfa_states;
	integer counter = 0;
	convert(nfa, nfa_states, nfa_transitions, dfa_states, counter);
	states.resize(counter);

	std::sort(transitions.begin(), transitions.end());
	for(integer i = 0; i < transitions.size(); ++i)
		if(i == 0 || transitions[i - 1].id < transitions[i].id)
			states[transitions[i].id].start = i;
	// sentinel
	states.emplace_back(transitions.size(), false, max_edits + 1);

	// initial state
	current_states.push_back(0);
}

template<typename text>
LevenshteinDFA<text>::LevenshteinDFA(const text& pattern, integer max_edits):
	LevenshteinDFA(LevenshteinNFA<text>(pattern, max_edits))
{}

template<typename text>
constexpr const typename LevenshteinDFA<text>::symbol LevenshteinDFA<text>::nullchar()
{
	return static_cast<symbol>(0);
}

template<typename text>
inline bool LevenshteinDFA<text>::update(const symbol c)
{
	// binary search
	integer current = states[current_states.back()].start, last = states[current_states.back() + 1].start - 1;
	for(integer w = last - current, m; w > 16; w = m){
		m = w >> 1;
		current += transitions[current + m].label < c ? w - m : 0;
	}
	// linear search
	for(; current < last && transitions[current].label < c; ++current);

	current = transitions[transitions[current].label == c ? current : last].next;
	bool updatable = states[current].edits <= max_edits;
	if(updatable)
		current_states.push_back(current);

	return updatable;
}

template<typename text>
inline bool LevenshteinDFA<text>::matched() const
{
	return states[current_states.back()].match;
}

template<typename text>
inline void LevenshteinDFA<text>::back()
{
	if(current_states.size() > 1)
		current_states.pop_back();
}

template<typename text>
inline typename LevenshteinDFA<text>::integer LevenshteinDFA<text>::max_distance() const
{
	return max_edits;
}

template<typename text>
inline typename LevenshteinDFA<text>::integer LevenshteinDFA<text>::distance() const
{
	return states[current_states.back()].edits;
}

template<typename text>
typename LevenshteinDFA<text>::integer
LevenshteinDFA<text>::convert(const LevenshteinNFA<text>& nfa,
	const std::vector<nfa_state>& nfa_states, const std::set<symbol>& nfa_transitions,
	std::map<std::vector<nfa_state>, integer>& dfa_states, integer& counter)
{
	// skip if current DFA state already exists
	const auto p = dfa_states.find(nfa_states);
	if(p != dfa_states.end())
		return p->second;

	integer current_node_id = counter++;
	dfa_states.insert(p, std::make_pair(nfa_states, current_node_id));

	integer edits = !nfa_states.empty() ? nfa_states.back().second : max_edits + 1;
	states.emplace_back(current_node_id, nfa.is_match(nfa_states), edits); // current_node_id is a placeholder

	// *-transition
	auto new_nfa_states = nfa.step(nfa_states, nullchar());
	auto next0 = convert(nfa, new_nfa_states, nfa_transitions, dfa_states, counter);
	transitions.emplace_back(current_node_id, next0, nullchar());

	// check other transitions with symbols in the pattern
	for(auto label: nfa_transitions){
		auto new_nfa_states = nfa.step(nfa_states, label);
		auto next = convert(nfa, new_nfa_states, nfa_transitions, dfa_states, counter);
		if(next != next0)
			transitions.emplace_back(current_node_id, next, label);
	}

	return current_node_id;
}

}

#endif
