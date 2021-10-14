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

#ifndef TRIMATCH_SEARCHER_HPP
#define TRIMATCH_SEARCHER_HPP

#include <vector>

namespace trimatch
{

// (exact|predictive|approximate) searcher
template<class text, class integer, class trie>
class searcher
{
public:
	using predictive_search_result_iterator = typename trie::traversal_iterator;
	using approximate_search_result = std::pair<text, integer>;
	// TODO: class approximate_searxh_result_iterator;

	searcher(const trie& T);

	bool exact(const text& query) const;
	predictive_search_result_iterator predict(const text& query);
	template<class approximate_matcher>
	void approx(approximate_matcher& matcher, std::vector<approximate_search_result>& results);

private:
	const trie& T;
	typename trie::common_searcher trie_searcher;

	template<class approximate_matcher>
	void approx_step(approximate_matcher& matcher,
		integer node_id, text& current, std::vector<std::pair<text, integer>>& results);
};


template<class text, class integer, class trie>
searcher<text, integer, trie>::searcher(const trie& T):
	T(T), trie_searcher(T.searcher())
{}

template<class text, class integer, class trie>
bool searcher<text, integer, trie>::exact(const text& query) const
{
	return T.exists(query);
}

template<class text, class integer, class trie>
typename searcher<text, integer, trie>::predictive_search_result_iterator
searcher<text, integer, trie>::predict(const text& query)
{
	return trie_searcher.traverse(query);
}

template<class text, class integer, class trie>
template<class approximate_matcher>
void searcher<text, integer, trie>::approx(
	approximate_matcher& matcher, std::vector<std::pair<text, integer>>& results)
{
	text current;
	approx_step(matcher, 0, current, results);
}

template<class text, class integer, class trie>
template<class approximate_matcher>
void searcher<text, integer, trie>::approx_step(
	approximate_matcher& matcher, integer node_id, text& current, std::vector<std::pair<text, integer>>& results)
{
	// TODO: for(const auto& u: T.children(n)){...
	for(integer i = T.data[node_id].next; i < T.data[T.data[node_id].next].next; ++i){
		if(matcher.update(T.data[i].label)){
			current.push_back(T.data[i].label);
			if(T.data[i].match && matcher.distance() <= matcher.max)
				results.push_back(std::make_pair(current, matcher.distance()));
			if(!T.data[i].leaf)
				approx_step(matcher, i, current, results);
			current.pop_back();
			matcher.back();
		}
	}
}

}

#endif
