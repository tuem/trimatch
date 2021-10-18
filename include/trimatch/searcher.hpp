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
	template<class approximate_matcher, class back_insert_iterator>
	void approx(approximate_matcher& matcher, back_insert_iterator bi);

private:
	const trie& T;
	typename trie::common_searcher trie_searcher;

	template<class approximate_matcher, class back_insert_iterator>
	void approx_step(approximate_matcher& matcher,
		integer root, text& current, back_insert_iterator& bi);
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
template<class approximate_matcher, class back_insert_iterator>
void searcher<text, integer, trie>::approx(
	approximate_matcher& matcher, back_insert_iterator bi)
{
	text current;
	approx_step(matcher, 0, current, bi);
}

template<class text, class integer, class trie>
template<class approximate_matcher, class back_insert_iterator>
void searcher<text, integer, trie>::approx_step(
	approximate_matcher& matcher, integer root, text& current, back_insert_iterator& bi)
{
	if(T.data[root].match && matcher.matched()){
		// LevenshteinDFA may return incorrect distance
		*bi++ = std::make_pair(current, matcher.distance());
	}
	if(T.data[root].leaf)
		return;
	// TODO: for(const auto& u: T.children(n)){...
	for(integer i = T.data[root].next; i < T.data[T.data[root].next].next; ++i){
		if(matcher.update(T.data[i].label)){
			current.push_back(T.data[i].label);
			approx_step(matcher, i, current, bi);
			current.pop_back();
			matcher.back();
		}
	}
}

}

#endif
