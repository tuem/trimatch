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
Syntax sugar for using set::index and map::index
Controller for tries and searchers
*/

#ifndef TRIMATCH_INDEX
#define TRIMATCH_INDEX

#include <cstddef>
#include <iterator>

#include "index_set.hpp"
#include "index_map.hpp"

namespace trimatch{

namespace set{

template<
	class random_access_iterator,
	class text = typename std::iterator_traits<random_access_iterator>::value_type,
	class integer = std::uint32_t,
	class trie = sftrie::set<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, integer, trie> build(random_access_iterator begin, random_access_iterator end)
{
	return index<text, integer, trie, approximate_matcher>(begin, end);
}

template<
	class random_accessible_container,
	class text = typename random_accessible_container::value_type,
	class integer = std::uint32_t,
	class trie = sftrie::set<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, integer, trie> build(const random_accessible_container& texts)
{
	return index<text, integer, trie, approximate_matcher>(texts);
}

template<
	class input_stream,
	class text,
	class integer = std::uint32_t,
	class trie = sftrie::set<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, integer, trie> build(input_stream& is)
{
	return index<text, integer, trie, approximate_matcher>(is);
}

template<
	class text,
	class integer = std::uint32_t,
	class trie = sftrie::set<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
using searcher = typename index<text, integer, trie, approximate_matcher>::search_client;

}


namespace map{

template<
	class random_access_iterator,
	class text = typename std::iterator_traits<random_access_iterator>::value_type,
	class item = std::int32_t,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, item, integer, trie, approximate_matcher> build(random_access_iterator begin, random_access_iterator end)
{
	return index<text, item, integer, trie, approximate_matcher>(begin, end);
}

template<
	class random_accessible_container,
	class text = typename random_accessible_container::value_type,
	class item = std::int32_t,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, item, integer, trie, approximate_matcher> build(const random_accessible_container& texts)
{
	return index<text, item, integer, trie, approximate_matcher>(texts);
}

template<
	class input_stream,
	class text,
	class item,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, item, integer, trie, approximate_matcher> build(input_stream& is)
{
	return index<text, item, integer, trie, approximate_matcher>(is);
}

template<
	class text,
	class item,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
using searcher = typename index<text, item, integer, trie, approximate_matcher>::search_client;

}

}

#endif
