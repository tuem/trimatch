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

#ifndef TRIMATCH_INDEX_MAP
#define TRIMATCH_INDEX_MAP

#include <iterator>

#include "sftrie_map.hpp"
#include "levenshtein_dfa.hpp"

namespace trimatch
{

template<
	class text,
	class item,
	class integer,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
class index_map
{
public:
	class search_client;

	template<typename random_access_iterator>
	index_map(random_access_iterator begin, random_access_iterator end);
	template<typename random_accessible_container>
	index_map(const random_accessible_container& texts);
	template<typename input_stream>
	index_map(input_stream& is);
	index_map(std::string path);

	search_client searcher();

	template<typename output_stream>
	void save(output_stream& os) const;
	void save(std::string path) const;

private:
	trie T;
};

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class random_access_iterator>
index_map<text, item, integer, trie, approximate_matcher>::index_map(
	random_access_iterator begin, random_access_iterator end
):
	T(begin, end)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<typename random_accessible_container>
index_map<text, item, integer, trie, approximate_matcher>::index_map(
	const random_accessible_container& texts
):
	T(std::begin(texts), std::end(texts))
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class input_stream>
index_map<text, item, integer, trie, approximate_matcher>::index_map(input_stream& is):
	T(is)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
index_map<text, item, integer, trie, approximate_matcher>::index_map(std::string path):
	T(path)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class output_stream>
void index_map<text, item, integer, trie, approximate_matcher>::save(output_stream& os) const
{
	T.save(os);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
void index_map<text, item, integer, trie, approximate_matcher>::save(std::string path) const
{
	T.save(path);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
typename index_map<text, item, integer, trie, approximate_matcher>::search_client
index_map<text, item, integer, trie, approximate_matcher>::searcher()
{
	return search_client(T);
}


// Syntax sugar

template<
	class random_access_iterator,
	class text = typename std::iterator_traits<random_access_iterator>::value_type,
	class item = std::int32_t,
	class integer = unsigned long,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index_map<text, item, integer, trie, approximate_matcher> build(random_access_iterator begin, random_access_iterator end)
{
	return index_map<text, item, integer, trie, approximate_matcher>(begin, end);
}

template<
	class random_accessible_container,
	class text = typename random_accessible_container::value_type,
	class item = std::int32_t,
	class integer = unsigned long,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index_map<text, item, integer, trie, approximate_matcher> build(const random_accessible_container& texts)
{
	return index_map<text, item, integer, trie, approximate_matcher>(texts);
}

template<
	class input_stream,
	class text,
	class item,
	class integer,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index_map<text, item, integer, trie, approximate_matcher> build(input_stream& is)
{
	return index_map<text, item, integer, trie, approximate_matcher>(is);
}


template<
	class text,
	class item,
	class integer,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
using searcher_map = typename index_map<text, item, integer, trie, approximate_matcher>::search_client;

}

#include "searcher_map.hpp"

#endif
