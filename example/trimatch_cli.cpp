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
Text search program using trimatch.
Usage: trimatch_cli corpus_path [max_edits=1]
If a query ends with '*' or '?', predictive search or approximate search will be done, respectively.
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <trimatch/index.hpp>


using text = std::string;
using integer = text::size_type;


int main(int argc, char* argv[])
{
	if(argc < 2){
		std::cerr << "usage: " << argv[0] << " corpus_path [max_edits=1]" << std::endl;
		return 0;
	}

	std::string corpus_path = argv[1];
	int max_edits = argc >= 3 ? std::stoi(argv[2]) : 2;

	std::vector<text> texts;
	std::ifstream ifs(corpus_path);

	std::cerr << "loading...";
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

	auto index = trimatch::build(texts);
	auto searcher = index.searcher();
	while(true){
		std::cerr << "> ";
		std::string query;
		std::getline(std::cin, query);
		if(std::cin.eof() || query == "exit" || query == "quit" || query == "bye")
			break;

		auto last = query.back();
		if(last == '%' || last == '*' || last == '?' || last == '&')
			query.pop_back();
		integer count = 0;
		if(last == '%'){
			// common prefix search
			for(const auto& p: searcher.prefix(query))
				std::cout << std::setw(4) << ++count << ": " << p << std::endl;
		}
		else if(last == '*'){
			// predictive search
			for(const auto& t: searcher.predict(query))
				std::cout << std::setw(4) << ++count << ": " << t << std::endl;
		}
		else if(last == '?'){
			// approximate search
			for(const auto& p: searcher.approx(query, max_edits))
				std::cout << std::setw(4) << ++count << ": text=" << p.first << ", distance=" << p.second << std::endl;
		}
		else if(last == '&'){
			// approximate predictive search
			std::vector<std::tuple<text, integer, integer>> results;
			searcher.approx_predict(query, max_edits, std::back_inserter(results));
			for(const auto& t: results)
				std::cout << std::setw(4) << ++count << ": text=" << std::get<0>(t) << ", distance(prefix)=" << std::get<1>(t) << ", distance(whole)=" << std::get<2>(t) << std::endl;
		}
		else{
			// exact match
			if((count = searcher.exact(query) ? 1 : 0) > 0)
				std::cout << query << ": found" << std::endl;
		}
		if(count == 0)
			std::cout << query << ": " << "not found" << std::endl;
	}

	return 0;
}
