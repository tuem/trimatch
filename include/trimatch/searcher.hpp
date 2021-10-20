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
Main interface for exact matching, predictive search and approximate search
*/

#ifndef TRIMATCH_SEARCHER
#define TRIMATCH_SEARCHER

#include "sftrie.hpp"
#include "levenshtein_dfa.hpp"

namespace trimatch
{

template<
	class text,
	class integer,
	class trie = sftrie::set<text, integer>,
	class approximate_matcher = LevenshteinDFA<text>
>
class searcher
{
public:
	using predictive_search_result_iterator = typename trie::traversal_iterator;
	using approximate_search_result = std::pair<text, integer>;
	// TODO: class approximate_search_iterator;

	searcher(const trie& T);

	bool exact(const text& query) const;
	predictive_search_result_iterator predict(const text& query);
	template<class back_insert_iterator>
	void predict(const text& query, back_insert_iterator bi) const;
	template<class back_insert_iterator>
	void approx(const text& query, back_insert_iterator bi, integer max_edits = 1) const;
	// TODO: approximate_search_iterator approx
	// TODO: void approx_predict
	// TODO: approximate_search_iterator approx_predict

private:
	const trie& T;
	typename trie::common_searcher trie_searcher;

	template<class back_insert_iterator>
	void approx_step(approximate_matcher& matcher,
		integer root, text& current, back_insert_iterator& bi) const;
};


template<class text, class integer, class trie, class approximate_matcher>
searcher<text, integer, trie, approximate_matcher>::searcher(const trie& T):
	T(T), trie_searcher(T.searcher())
{}

template<class text, class integer, class trie, class approximate_matcher>
bool searcher<text, integer, trie, approximate_matcher>::exact(const text& query) const
{
	return T.exists(query);
}

template<class text, class integer, class trie, class approximate_matcher>
typename searcher<text, integer, trie, approximate_matcher>::predictive_search_result_iterator
searcher<text, integer, trie, approximate_matcher>::predict(const text& query)
{
	return trie_searcher.traverse(query);
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void searcher<text, integer, trie, approximate_matcher>::predict(
	const text& query, back_insert_iterator bi) const
{
	typename trie::common_searcher searcher(T);
	for(const auto& r: searcher.traverse(query))
		*bi++ = r;
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void searcher<text, integer, trie, approximate_matcher>::approx(
	const text& query, back_insert_iterator bi, integer max_edits) const
{
	approximate_matcher matcher(query, max_edits);
	text current;
	approx_step(matcher, 0, current, bi);
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void searcher<text, integer, trie, approximate_matcher>::approx_step(
	approximate_matcher& matcher, integer root, text& current, back_insert_iterator& bi) const
{
	// TODO: use abstract interface
	const auto& nodes = T.raw_data();
	if(nodes[root].match && matcher.matched())
		*bi++ = std::make_pair(current, matcher.distance());
	if(nodes[root].leaf)
		return;
	// TODO: after distance() leaches max_distance(), all we need to do is the exact matching process
	for(integer i = nodes[root].next; i < nodes[nodes[root].next].next; ++i){
		if(matcher.update(nodes[i].label)){
			current.push_back(nodes[i].label);
			approx_step(matcher, i, current, bi);
			current.pop_back();
			matcher.back();
		}
	}
}

}

#endif
