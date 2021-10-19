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
Controller for tries and searchers
*/

#ifndef TRIMATCH_INDEX
#define TRIMATCH_INDEX

#include <vector>

#include "searcher.hpp"
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
class index
{
public:
	using searcher_type = searcher<text, integer, trie, approximate_matcher>;

	template<typename iterator>
	index(iterator begin, iterator end);

	searcher_type searcher() const;

private:
	const trie T;
};

template<class text, class integer, class trie, class approximate_matcher>
template<class iterator>
index<text, integer, trie, approximate_matcher>::index(iterator begin, iterator end): T(begin, end)
{}

template<class text, class integer, class trie, class approximate_matcher>
typename index<text, integer, trie, approximate_matcher>::searcher_type
index<text, integer, trie, approximate_matcher>::searcher() const
{
	return searcher_type(T);
}

}

#endif
