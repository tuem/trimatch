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

#include <trimatch/trimatch.hpp>

// TODO: use DFA
#include "../eval/competitors/online_edit_distance_dp.hpp"

using text = std::string;
using integer = text::size_type;

int main(int argc, char* argv[])
{
	if(argc < 2){
		std::cerr << "usage: " << argv[0] << " corpus" << std::endl;
		return 0;
	}

	std::string corpus_path = argv[1];
	std::cerr << "loading...";
	std::vector<text> texts;
	std::ifstream ifs(corpus_path);
	if(!ifs.is_open()){
		std::cerr << "input file is not available: " << corpus_path << std::endl;
		return 1;
	}
	while(ifs.good()){
		std::string line;
		std::getline(ifs, line);
		if(ifs.eof())
			break;
		texts.push_back(line);
	}

	sftrie::sort_texts(std::begin(texts), std::end(texts));
	std::cerr << "done." << std::endl;

	auto index = trimatch::build(std::begin(texts), std::end(texts));
	texts.clear();
	texts.shrink_to_fit();

	auto searcher = index.searcher();
	int max_edits = 3;
	while(true){
		std::cerr << "> ";
		std::string query;
		std::getline(std::cin, query);
		if(std::cin.eof() || query == "exit" || query == "quit" || query == "bye")
			break;

		auto back = query.back();
		integer count = 0;
		if(back != '*' && back != '?'){
			if((count = searcher.exact(query) ? 1 : 0) > 0)
				std::cout << query << ": found" << std::endl;
		}
		else{
			query.pop_back();
			if(back == '*'){
				for(const auto& t: searcher.predict(query))
					std::cout << std::setw(4) << ++count << ": " << t << std::endl;
			}
			else if(back == '?'){
				std::vector<std::pair<text, integer>> results;
				OnlineEditDistance<text> ed(query, max_edits);
				searcher.approx(ed, results);
				for(const auto& r: results)
					std::cout << std::setw(4) << ++count << ": " << r.first << ", distance=" << r.second << std::endl;
			}
		}
		if(count == 0)
			std::cout << query << ": " << "not found" << std::endl;
	}

	return 0;
}
