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

#ifndef TRIMATCH_BUILDER_HPP
#define TRIMATCH_BUILDER_HPP

#include <vector>

#include "builder.hpp"
// use modified version to access internal data
// TODO: update original sftrie
#include "sftrie.hpp"

namespace trimatch
{

// helper function
template<class iterator,
	class text = typename iterator::value_type,
	class integer = typename text::size_type,
	class trie = sftrie::set<text, integer>>
index<text, integer, trie> build(iterator begin, iterator end)
{
	return index<text, integer, trie>(begin, end);
}

}

#endif
