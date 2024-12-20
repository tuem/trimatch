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
Controller for index and searcher
*/

#ifndef TRIMATCH_INDEX
#define TRIMATCH_INDEX

#include <cstddef>
#include <iterator>
#include <fstream>

#include <sftrie/random_access_container.hpp>
#include <sftrie/util.hpp>

#include "trie_selector.hpp"
#include "levenshtein_dfa.hpp"
#include "search_client.hpp"
#include "readable.hpp"

namespace trimatch{

template<
	class text,
	class item = sftrie::empty,
	class integer = std::uint32_t,
	class trie = typename trie_selector<item>::template trie_type<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
class index
{
public:
	using text_type = text;
	using item_type = item;
	using value_type = typename sftrie::trie_traits<text, item, integer>::value_type;
	using integer_type = integer;
	using trie_type = trie;
	using matcher_type = approximate_matcher;
	using searcher_type = search_client<trie, approximate_matcher>;

	index();
	template<std::random_access_iterator iterator>
	index(iterator begin, iterator end, bool two_pass = false);
	template<sftrie::random_access_container container>
	index(const container& texts, bool two_pass = false);
	template<readable input_stream>
	index(input_stream& is);
	index(std::string path);

	template<std::random_access_iterator iterator>
	integer build(iterator begin, iterator end, bool two_pass = false);
	template<sftrie::random_access_container container>
	integer build(const container& texts, bool two_pass = false);
	template<readable input_stream>
	integer load(input_stream& is);
	integer load(std::string path);

	template<typename output_stream>
	void save(output_stream& os) const;
	void save(std::string path) const;

	searcher_type searcher() const;

	trie& raw_trie();

private:
	trie T;
};

template<class text, class item, class integer, class trie, class approximate_matcher>
index<text, item, integer, trie, approximate_matcher>::index():
	T()
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<std::random_access_iterator iterator>
index<text, item, integer, trie, approximate_matcher>::index(
	iterator begin, iterator end, bool two_pass
):
	T(begin, end, two_pass)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<sftrie::random_access_container container>
index<text, item, integer, trie, approximate_matcher>::index(
	const container& texts, bool two_pass
):
	T(std::begin(texts), std::end(texts), two_pass)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<readable input_stream>
index<text, item, integer, trie, approximate_matcher>::index(input_stream& is):
	T(is)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
index<text, item, integer, trie, approximate_matcher>::index(std::string path):
	T(path)
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<std::random_access_iterator iterator>
integer index<text, item, integer, trie, approximate_matcher>::build(
	iterator begin, iterator end, bool two_pass)
{
	return T.construct(begin, end, two_pass);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<sftrie::random_access_container container>
integer index<text, item, integer, trie, approximate_matcher>::build(
	const container& texts, bool two_pass)
{
	return T.construct(texts, two_pass);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<readable input_stream>
integer index<text, item, integer, trie, approximate_matcher>::load(input_stream& is)
{
	return T.load(is);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
integer index<text, item, integer, trie, approximate_matcher>::load(std::string path)
{
	return T.load(path);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class output_stream>
void index<text, item, integer, trie, approximate_matcher>::save(output_stream& os) const
{
	T.save(os);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
void index<text, item, integer, trie, approximate_matcher>::save(std::string path) const
{
	T.save(path);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
search_client<trie, approximate_matcher>
index<text, item, integer, trie, approximate_matcher>::searcher() const
{
	return search_client<trie, approximate_matcher>(T);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
trie& index<text, item, integer, trie, approximate_matcher>::raw_trie()
{
	return T;
}


// utility functions

template<
	class random_access_iterator,
	class text = typename std::iterator_traits<random_access_iterator>::value_type::first_type,
	class item = typename std::iterator_traits<random_access_iterator>::value_type::second_type,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, item, integer, trie, approximate_matcher> build(
	random_access_iterator begin, random_access_iterator end, bool two_pass = false)
{
	return index<text, item, integer, trie, approximate_matcher>(begin, end, two_pass);
}

template<
	class random_accessible_container,
	class text = typename std::iterator_traits<random_accessible_container>::value_type::first_type,
	class item = typename std::iterator_traits<random_accessible_container>::value_type::second_type,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, item, integer, trie, approximate_matcher> build(
	const random_accessible_container& texts, bool two_pass = false)
{
	return index<text, item, integer, trie, approximate_matcher>(texts, two_pass);
}

template<
	class text,
	class item,
	class integer = std::uint32_t,
	class trie = sftrie::map<text, item, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>,
	readable input_stream = std::ifstream
>
index<text, item, integer, trie, approximate_matcher> load_map(input_stream& is)
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
index<text, item, integer, trie, approximate_matcher> load_map(const std::string path)
{
	return index<text, item, integer, trie, approximate_matcher>(path);
}

// if item is sftrie::empty, specialized version with sftrie::set will be used

template<
	class random_access_iterator,
	class text = typename random_access_iterator::value_type,
	class integer = std::uint32_t,
	class trie = typename trie_selector<sftrie::empty>::template trie_type<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, sftrie::empty, integer, trie, approximate_matcher> build(
	random_access_iterator begin, random_access_iterator end, bool two_pass = false)
{
	return index<text, sftrie::empty, integer, trie, approximate_matcher>(begin, end, two_pass);
}

template<
	class random_accessible_container,
	class text = typename random_accessible_container::value_type,
	class integer = std::uint32_t,
	class trie = typename trie_selector<sftrie::empty>::template trie_type<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, sftrie::empty, integer, trie, approximate_matcher> build(
	const random_accessible_container& texts, bool two_pass = false)
{
	return index<text, sftrie::empty, integer, trie, approximate_matcher>(texts, two_pass);
}

template<
	class text,
	class integer = std::uint32_t,
	class trie = typename trie_selector<sftrie::empty>::template trie_type<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>,
	class input_stream = std::ifstream
>
index<text, sftrie::empty, integer, trie, approximate_matcher> load_set(input_stream& is)
{
	return index<text, sftrie::empty, integer, trie, approximate_matcher>(is);
}

template<
	class text,
	class integer = std::uint32_t,
	class trie = typename trie_selector<sftrie::empty>::template trie_type<text, integer>,
	class approximate_matcher = LevenshteinDFA<text, integer>
>
index<text, sftrie::empty, integer, trie, approximate_matcher> load_set(const std::string path)
{
	return index<text, sftrie::empty, integer, trie, approximate_matcher>(path);
}

}

#endif
