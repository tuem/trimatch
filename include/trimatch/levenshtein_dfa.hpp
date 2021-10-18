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
	DFA version of Levenshtein automata by Jules Jacobs
	https://julesjacobs.com/2015/06/17/disqus-levenshtein-simple-and-fast.html
*/

#ifndef TRIMATCH_LEVENSHTEIN_DFA_HPP
#define TRIMATCH_LEVENSHTEIN_DFA_HPP

#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#include "levenshtein_nfa.hpp"

namespace trimatch
{

template<typename text>
class LevenshteinDFA
{
public:
	using symbol = typename text::value_type;
	using integer = typename text::size_type;
	using nfa_state = typename LevenshteinNFA<text>::state;

	struct state;
	struct transition;

	static constexpr const symbol nullchar();

	LevenshteinDFA(const LevenshteinNFA<text>& nfa);
	LevenshteinDFA(const text& pattern, integer max_edits);

	bool update(symbol c);
	bool matched() const;
	void back();
	integer max_distance() const;
	integer distance();

	template<class ostream> void dump(ostream& os) const;

	const text pattern;
	const integer max_edits;

private:
	std::vector<state> states;
	std::vector<transition> transitions;

	std::vector<integer> current_states;

	integer explore(const LevenshteinNFA<text>& nfa,
		const std::vector<nfa_state>& nfa_states, const std::set<symbol>& nfa_transitions,
		std::map<std::vector<nfa_state>, integer>& dfa_states, integer& counter);
};

template<typename text>
struct LevenshteinDFA<text>::state
{
	integer start;
	bool match;
	integer edits;
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
	explore(nfa, nfa_states, nfa_transitions, dfa_states, counter);
	states.resize(counter);

	std::sort(transitions.begin(), transitions.end());
	for(integer i = 0; i < transitions.size(); ++i)
		if(i == 0 || transitions[i - 1].id < transitions[i].id)
			states[transitions[i].id].start = i;
	// sentinel
	states.push_back({transitions.size(), false, max_edits + 1});

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
	integer current = states[current_states.back()].start, last = states[current_states.back() + 1].start - 1;
	for(; current < last && c < transitions[current].label; ++current);
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
inline typename LevenshteinDFA<text>::integer LevenshteinDFA<text>::distance()
{
	return states[current_states.back()].edits;
}

template<typename text>
template<class ostream>
void LevenshteinDFA<text>::dump(ostream& os) const
{
	for(integer i = 0; i < transitions.size(); ++i){
		const auto& t = transitions[i];
		if(i == 0 || transitions[i - 1].id < t.id){
			const auto& s = states[t.id];
			os << "i=" << i << ": id=" << t.id << ", start=" << s.start
				<< ", distance=" << s.edits << (s.match ? " (match)" : "") << std::endl;
		}
		os << "  (" << t.id << ", ";
		if(t.label != nullchar())
			os << t.label;
		else
			os << '*';
		os << ") => " << t.next;
		os << std::endl;
	}
}

template<typename text>
typename LevenshteinDFA<text>::integer
LevenshteinDFA<text>::explore(const LevenshteinNFA<text>& nfa,
	const std::vector<nfa_state>& nfa_states, const std::set<symbol>& nfa_transitions,
	std::map<std::vector<nfa_state>, integer>& dfa_states, integer& counter)
{
	// skip if current DFA state already exists
	const auto p = dfa_states.find(nfa_states);
	if(p != dfa_states.end())
		return p->second;

	integer current_node_id = counter++;
	dfa_states.insert(p, std::make_pair(nfa_states, current_node_id));

	integer best_edits = max_edits + 1;
	for(const auto& s: nfa_states)
		if(s.second < best_edits)
			best_edits = s.second;
	states.push_back({current_node_id, nfa.is_match(nfa_states), best_edits}); // current_node_id is a placeholder

	// *-transition
	auto new_nfa_states = nfa.step(nfa_states, nullchar());
	auto next0 = explore(nfa, new_nfa_states, nfa_transitions, dfa_states, counter);
	transitions.push_back({current_node_id, next0, nullchar()});

	// check other transitions with symbols in the pattern
	for(auto label: nfa_transitions){
		auto new_nfa_states = nfa.step(nfa_states, label);
		auto next = explore(nfa, new_nfa_states, nfa_transitions, dfa_states, counter);
		if(next != next0)
			transitions.push_back({current_node_id, next, label});
	}

	return current_node_id;
}

}

#endif
