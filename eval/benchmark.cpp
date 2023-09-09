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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <set>
#include <map>

#include <random>
#include <chrono>

#include <trimatch/index.hpp>

#include "matcher/edit_distance_dp.hpp"
#include "matcher/edit_distance_bp.hpp"
#include "matcher/online_edit_distance_dp.hpp"

#include "string_util.hpp"
#include "history.hpp"


using text = std::u32string;
using symbol = typename text::value_type;
using integer = std::uint32_t;


template<typename text, typename integer>
size_t exec_approx_dp(const std::vector<text>& texts,
	const std::vector<text>& queries, integer max_edits = 1)
{
	size_t found = 0;
	EditDistance ed;
	for(const auto& q: queries)
		for(const auto& t: texts)
			if(ed(t, q) <= max_edits)
				++found;
	return found;
}

template<typename text, typename integer>
size_t exec_approx_bp(const std::vector<text>& texts,
	const std::vector<text>& queries, integer max_edits = 1)
{
	size_t found = 0;
	std::vector<std::pair<text, integer>> results;
	for(const auto& q: queries){
		EditDistanceBP<text> ed(q);
		ed(texts, max_edits, std::back_inserter(results));
		found += results.size();
		results.clear();
	}
	return found;
}

template<typename set>
size_t exec_approx_dp_trie(const set& trie,
	const std::vector<typename set::text_type>& queries, typename set::integer_type max_edits = 1)
{
	// since trie is already built, directly create searcher without trimatch::index
	trimatch::set::searcher<text, integer, sftrie::set<text, integer>, OnlineEditDistance<text, integer>> searcher(trie);
	std::vector<std::pair<text, integer>> results;
	size_t found = 0;
	for(const auto& query: queries){
		searcher.approx(query, max_edits, std::back_inserter(results));
		found += results.size();
		results.clear();
	}
	return found;
}

template<typename set>
size_t exec_approx_dfa_trie(const set& trie,
	const std::vector<typename set::text_type>& queries, typename set::integer_type max_edits = 1)
{
	trimatch::set::searcher<text, integer> searcher(trie);
	std::vector<std::pair<text, integer>> results;
	size_t found = 0;
	for(const auto& query: queries){
		searcher.approx(query, max_edits, std::back_inserter(results));
		found += results.size();
		results.clear();
	}
	return found;
}

template<typename text>
bool benchmark(const std::string& dictionary_path, const std::string& algorithm, size_t max_edits, size_t max_queries)
{
	History history;

	std::cerr << "loading texts...";
	history.refresh();
	std::vector<text> texts;
	std::ifstream ifs(dictionary_path);
	if(!ifs.is_open())
		throw std::runtime_error("input file is not available: " + dictionary_path);
	while(ifs.good()){
		std::string line;
		std::getline(ifs, line);
		if(ifs.eof())
			break;
		auto t = cast_string<text>(line);
		texts.push_back(t);
	}
	history.record("loading texts", texts.size());
	std::cerr << "done." << std::endl;

	std::cerr << "analyzing texts...";
	std::set<symbol> alphabet;
	symbol min_char = texts.front().front(), max_char = min_char;
	bool first = true;
	size_t min_length = 0, max_length = 0, total_length = 0;
	for(const auto& t: texts){
		for(auto c: t){
			alphabet.insert(c);
			min_char = std::min(min_char, c);
			max_char = std::max(max_char, c);
		}
		if(first){
			min_length = t.size();
			max_length = t.size();
			first = false;
		}
		else{
			min_length = std::min(t.size(), min_length);
			max_length = std::max(t.size(), max_length);
		}
		total_length += t.size();
	}
	double average_length = total_length / static_cast<double>(texts.size());
	history.record("analyzing texts", texts.size());
	std::cerr << "done." << std::endl;

	std::cerr << "sorting texts...";
	history.refresh();
	sftrie::sort_texts(texts.begin(), texts.end());
	history.record("sorting texts", texts.size());
	std::cerr << "done." << std::endl;

	std::cerr << "generating queries...";
	history.refresh();
	std::vector<text> queries = texts;
	std::vector<text> shuffled_queries = queries;
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(shuffled_queries.begin(), shuffled_queries.end(), std::default_random_engine(seed));
	if(max_queries > 0)
		shuffled_queries.erase(shuffled_queries.begin() + max_queries, shuffled_queries.end());
	history.record("generating queries", queries.size());
	std::cerr << "done." << std::endl;

	size_t node_size = 0, trie_size =0, total_space = 0;
	size_t found_approx = 0;
	std::cerr << "constructing index...";
	history.refresh();
	sftrie::set<text, integer> index(texts.begin(), texts.end());
	history.record("construction", texts.size());
	std::cerr << "done." << std::endl;

	node_size = index.node_size();
	trie_size = index.trie_size();
	total_space = index.total_space();

	std::cerr << "approximate search...";
	history.refresh();
	if(algorithm == "dp"){
		found_approx = exec_approx_dp(texts, shuffled_queries, max_edits);
	}
	else if(algorithm == "dp-trie"){
		found_approx = exec_approx_dp_trie(index, shuffled_queries, max_edits);
	}
	else if(algorithm == "bp"){
		found_approx = exec_approx_bp(texts, shuffled_queries, max_edits);
	}
	else if(algorithm == "dfa-trie"){
		found_approx = exec_approx_dfa_trie(index, shuffled_queries, max_edits);
	}
	else{
		throw std::runtime_error("input file is not available: " + algorithm);
	}
	history.record("approximate search", shuffled_queries.size());
	std::cerr << "done." << std::endl;
	std::cout << "  found_approx: " << found_approx << std::endl;


	std::cout << std::endl;
	std::cout << "[input]" << std::endl;
	std::cout << std::left << std::setw(30) << "alphabet size" << std::right << std::setw(12) << alphabet.size() << std::endl;
	std::cout << std::left << std::setw(30) << "min symbol" << std::right << std::setw(12) << static_cast<signed long long>(min_char) << std::endl;
	std::cout << std::left << std::setw(30) << "max symbol" << std::right << std::setw(12) << static_cast<signed long long>(max_char) << std::endl;
	std::cout << std::left << std::setw(30) << "number of texts" << std::right << std::setw(12) << texts.size() << std::endl;
	std::cout << std::left << std::setw(30) << "max length" << std::right << std::setw(12) << max_length << std::endl;
	std::cout << std::left << std::setw(30) << "min length" << std::right << std::setw(12) << min_length << std::endl;
	std::cout << std::left << std::setw(30) << "average length" << std::right << std::setw(12) << average_length << std::endl;
	std::cout << std::left << std::setw(30) << "total length" << std::right << std::setw(12) << total_length << std::endl;
	std::cout << std::left << std::setw(30) << "total bytes" << std::right << std::setw(12) << sizeof(symbol) * total_length << std::endl;
	std::cout << std::endl;
	std::cout << "[size]" << std::endl;
	std::cout << std::left << std::setw(30) << "symbol size" << std::right << std::setw(12) << sizeof(symbol) << std::endl;
	std::cout << std::left << std::setw(30) << "# of texts" << std::right << std::setw(12) << texts.size() << std::endl;
	std::cout << std::left << std::setw(30) << "total length" << std::right << std::setw(12) << total_length << std::endl;
	std::cout << std::left << std::setw(30) << "total bytes" << std::right << std::setw(12) << (sizeof(symbol) * total_length) << std::endl;
	std::cout << std::left << std::setw(30) << "node size" << std::right << std::setw(12) << node_size << std::endl;
	std::cout << std::left << std::setw(30) << "trie size" << std::right << std::setw(12) << trie_size << std::endl;
	std::cout << std::left << std::setw(30) << "index size" << std::right << std::setw(12) << total_space << std::endl;
	std::cout << std::endl;
	std::cout << "[time]" << std::endl;
	history.dump();

	return true;
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		std::cout << "usage: " << argv[0] << " dictionary_path [algorithm=dfa-trie] [max_edits=1] [max_queries=0]" << std::endl;
		std::cout << "  algorithm: (dp|bp|dp-trie|dfa-trie)" << std::endl;
		std::cout << "  max_edits: allowable levenshtein distance" << std::endl;
		std::cout << "  max_queries: maximum mumber of approximate search queries (set 0 to use all entries in the dictionary)" << std::endl;
		return 0;
	}

	std::string dictionary_path = argv[1];
	std::string algorithm = argc > 2 ? argv[2] : "dfa-trie";
	size_t max_edits = argc > 3 ? std::atoi(argv[3]) : 1;
	size_t max_queries = argc > 4 ? std::atoi(argv[4]) : 0;

	benchmark<text>(dictionary_path, algorithm, max_edits, max_queries);
}
