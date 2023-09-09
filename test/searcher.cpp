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
#include <vector>
#include <set>
#include <iostream>

#include <Catch2/catch.hpp>

#include <trimatch/index.hpp>


TEST_CASE("searcher / small dictionary / exact matching", "[index][exact]"){
	std::vector<std::string> texts = {
		"A",
		"AM",
		"AMD",
		"AMP",
		"CAD",
		"CA",
		"CAM",
		"CAMP",
		"CM",
		"CMD",
		"DM",
		"MD",
	};
	sftrie::sort_texts(texts.begin(), texts.end());

	auto index = trimatch::set::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("exact matching (will be succeeded)"){
		for(const auto& t: texts)
			CHECK(searcher.exact(t));
	}
	SECTION("exact matching (will be failed)"){
		CHECK_FALSE(searcher.exact(""));
		CHECK_FALSE(searcher.exact("AMF"));
		CHECK_FALSE(searcher.exact("C"));
		CHECK_FALSE(searcher.exact("CDA"));
		CHECK_FALSE(searcher.exact("FM"));
	}
}

TEST_CASE("searcher / small dictionary / prefix search", "[index][prefix]"){
	std::vector<std::string> texts = {
		"A",
		"AM",
		"AMD",
		"AMP",
		"CAD",
		"CA",
		"CAM",
		"CAMP",
		"CM",
		"CMD",
		"DM",
		"MD",
	};
	sftrie::sort_texts(texts.begin(), texts.end());

	auto index = trimatch::set::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("prefix search (empty query)"){
		std::string query = "";
		std::set<std::string> results;
		for(const auto& result: searcher.prefix(query))
			results.insert(result);
		CHECK(results.size() == 0);
	}
	SECTION("prefix search"){
		std::string query = "AMPLIFY";
		std::set<std::string> results;
		for(const auto& result: searcher.prefix(query))
			results.insert(result);
		CHECK(results.size() == 3);
		CHECK(results.count("A") > 0);
		CHECK(results.count("AM") > 0);
		CHECK(results.count("AMP") > 0);
	}
	SECTION("prefix search (will be failed)"){
		std::string query = "BMP";
		std::set<std::string> results;
		for(const auto& result: searcher.prefix(query))
			results.insert(result);
		CHECK(results.size() == 0);
	}
}

TEST_CASE("searcher / small dictionary / predictive search", "[index][predict]"){
	std::vector<std::string> texts = {
		"A",
		"AM",
		"AMD",
		"AMP",
		"CAD",
		"CA",
		"CAM",
		"CAMP",
		"CM",
		"CMD",
		"DM",
		"MD",
	};
	sftrie::sort_texts(texts.begin(), texts.end());

	auto index = trimatch::set::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("predictive search (empty query)"){
		std::string query = "";
		std::vector<std::string> results;
		searcher.predict(query, std::back_inserter(results));
		CHECK(results.size() == texts.size());
		std::set<std::string> results_set(results.begin(), results.end());
		for(const auto& t: texts)
			CHECK(results_set.count(t) > 0);
	}
	SECTION("predictive search"){
		std::string query = "A";
		std::vector<std::string> results;
		searcher.predict(query, std::back_inserter(results));
		CHECK(results.size() == 4);
		std::set<std::string> results_set(results.begin(), results.end());
		CHECK(results_set.count("A") > 0);
		CHECK(results_set.count("AM") > 0);
		CHECK(results_set.count("AMD") > 0);
		CHECK(results_set.count("AMP") > 0);
	}
	SECTION("predictive search"){
		std::string query = "D";
		std::vector<std::string> results;
		searcher.predict(query, std::back_inserter(results));
		CHECK(results.size() == 1);
		std::set<std::string> results_set(results.begin(), results.end());
		CHECK(results_set.count("DM") > 0);
	}
	SECTION("predictive search (will be failed)"){
		std::string query = "CAS";
		std::vector<std::string> results;
		searcher.predict(query, std::back_inserter(results));
		CHECK(results.size() == 0);
	}
}

TEST_CASE("searcher / small dictionary / approximate search", "[index][approx]"){
	std::vector<std::string> texts = {
		"A",
		"AM",
		"AMD",
		"AMP",
		"CAD",
		"CA",
		"CAM",
		"CAMP",
		"CM",
		"CMD",
		"DM",
		"MD",
	};
	sftrie::sort_texts(texts.begin(), texts.end());

	auto index = trimatch::set::build(texts.begin(), texts.end());
	auto searcher = index.searcher();

	SECTION("approximate match (empty query)"){
		std::string query = "";
		std::vector<std::pair<std::string, unsigned long>> results;

		searcher.approx(query, 0, std::back_inserter(results));
		CHECK(results.size() == 0);

		results.clear();
		searcher.approx(query, 1, std::back_inserter(results));
		CHECK(results.size() == 1);

		results.clear();
		searcher.approx(query, 2, std::back_inserter(results));
		CHECK(results.size() == 6);
	}
	SECTION("approximate match"){
		std::string query = "AD";
		std::vector<std::pair<std::string, unsigned long>> results;

		searcher.approx(query, 1, std::back_inserter(results));
		CHECK(results.size() == 5);
		CHECK(std::get<0>(results[0]) == std::string("A"));
		CHECK(std::get<1>(results[0]) == 1);
		CHECK(std::get<0>(results[1]) == std::string("AM"));
		CHECK(std::get<1>(results[1]) == 1);
		CHECK(std::get<0>(results[2]) == std::string("AMD"));
		CHECK(std::get<1>(results[2]) == 1);
		CHECK(std::get<0>(results[3]) == std::string("CAD"));
		CHECK(std::get<1>(results[3]) == 1);
		CHECK(std::get<0>(results[4]) == std::string("MD"));
		CHECK(std::get<1>(results[4]) == 1);
	}
	SECTION("approximate match (different minimum edits)"){
		std::string query = "CORP";
		std::vector<std::pair<std::string, unsigned long>> results;

		searcher.approx(query, 1, std::back_inserter(results));
		CHECK(results.size() == 0);

		results.clear();
		searcher.approx(query, 2, std::back_inserter(results));
		CHECK(results.size() == 1);
	}
}
