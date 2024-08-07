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

#include <trimatch/index.hpp>


using text = std::string;


TEST_CASE("index / exact matching / empty dictionary", "[index][exact]"){
	std::vector<text> texts = {
	};

	auto index = trimatch::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("exact matching (will be succeeded)"){
	}
	SECTION("exact matching (will be failed)"){
		CHECK_FALSE(searcher.exact(""));
		CHECK_FALSE(searcher.exact("A"));
		CHECK_FALSE(searcher.exact("BC"));
	}
}

TEST_CASE("index / exact matching / dictionary consists of an empty string", "[index][exact]"){
	std::vector<text> texts = {
		""
	};

	auto index = trimatch::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("exact matching (will be succeeded)"){
		CHECK(searcher.exact(""));
	}
	SECTION("exact matching (will be failed)"){
		CHECK_FALSE(searcher.exact("A"));
		CHECK_FALSE(searcher.exact("BC"));
	}
}

TEST_CASE("index / exact matching / tiny dictionary", "[index][exact]"){
	std::vector<text> texts = {
		"B",
		"D",
		"F",
	};

	auto index = trimatch::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("exact matching (will be succeeded)"){
		CHECK(searcher.exact("B"));
		CHECK(searcher.exact("D"));
		CHECK(searcher.exact("F"));
	}
	SECTION("exact matching (will be failed)"){
		CHECK_FALSE(searcher.exact(""));
		CHECK_FALSE(searcher.exact("A"));
		CHECK_FALSE(searcher.exact("C"));
		CHECK_FALSE(searcher.exact("E"));
		CHECK_FALSE(searcher.exact("BC"));
		CHECK_FALSE(searcher.exact("AB"));
		CHECK_FALSE(searcher.exact("DF"));
		CHECK_FALSE(searcher.exact("BDF"));
	}
}

TEST_CASE("index / exact matching / small dictionary", "[index][exact]"){
	std::vector<text> texts = {
		"AM",
		"AMD",
		"CAD",
		"CAM",
		"CM",
		"DM",
	};

	auto index = trimatch::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("exact matching (will be succeeded)"){
		for(const auto& t: texts)
			CHECK(searcher.exact(t));
	}
	SECTION("exact matching (will be failed)"){
		CHECK_FALSE(searcher.exact(""));
		CHECK_FALSE(searcher.exact("C"));
		CHECK_FALSE(searcher.exact("A"));
		CHECK_FALSE(searcher.exact("M"));
		CHECK_FALSE(searcher.exact("CA"));
		CHECK_FALSE(searcher.exact("MD"));
		CHECK_FALSE(searcher.exact("AMP"));
		CHECK_FALSE(searcher.exact("CMD"));
		CHECK_FALSE(searcher.exact("CAMP"));
	}
}
