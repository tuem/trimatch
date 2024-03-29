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

#include <string>

#include <Catch2/catch.hpp>

#include <trimatch/levenshtein_dfa.hpp>


using text = std::string;
using symbol = typename text::value_type;
using integer = std::uint32_t;


TEST_CASE("levenshtein_dfa / small pattern", "[DFA][approx]"){
	text pattern = "CORP";

	std::vector<text> texts0 = {
		"CORP",
	};

	std::vector<text> texts1 = {
		"ORP",
		"COP",
		"COR",
		"CCORP",
		"COORP",
		"CORPS",
		"KORP",
		"CARP",
		"CORE",
	};

	std::vector<text> texts2 = {
		"RP",
		"CO",
		"CR",
		"CORPUS",
		"RECORP",
		"COORRP",
		"CAMP",
		"LORD",
		"CARE",
	};

	SECTION("edits = 0 / max edits = 0"){
		integer max_edits = 0;
		for(const auto& text: texts0){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK(matched);
		}
	}

	SECTION("edits = 1 / max edits = 0"){
		integer max_edits = 0;
		for(const auto& text: texts1){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK_FALSE(matched);
		}
	}

	SECTION("edits = 2 / max edits = 0"){
		integer max_edits = 0;
		for(const auto& text: texts2){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK_FALSE(matched);
		}
	}

	SECTION("edits = 0 / max edits = 1"){
		integer max_edits = 1;
		for(const auto& text: texts0){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK(matched);
		}
	}

	SECTION("edits = 1 / max edits = 1"){
		integer max_edits = 1;
		for(const auto& text: texts1){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK(matched);
		}
	}

	SECTION("edits = 2 / max edits = 1"){
		integer max_edits = 1;
		for(const auto& text: texts2){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK_FALSE(matched);
		}
	}

	SECTION("edits = 0 / max edits = 2"){
		integer max_edits = 2;
		for(const auto& text: texts0){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK(matched);
		}
	}

	SECTION("edits = 1 / max edits = 2"){
		integer max_edits = 2;
		for(const auto& text: texts1){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK(matched);
		}
	}

	SECTION("edits = 2 / max edits = 2"){
		integer max_edits = 2;
		for(const auto& text: texts2){
			auto dfa = trimatch::LevenshteinDFA(pattern, max_edits);
			bool passed = true;
			for(const auto s: text){
				if(!dfa.update(s)){
					passed = false;
					break;
				}
			}
			bool matched = passed && dfa.matched();
			CHECK(matched);
		}
	}
}
