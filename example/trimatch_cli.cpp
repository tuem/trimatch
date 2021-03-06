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
Usage: trimatch_cli input_path [max_edits=1]
If a query ends with '*' or '?', predictive search or approximate search will be done, respectively.
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <trimatch/index.hpp>


using text = std::string;
using integer = std::uint32_t;
using index_type = trimatch::index<text, integer>;


int main(int argc, char* argv[])
{
	if(argc < 2){
		std::cerr << "usage: " << argv[0] << " input_path [max_edits=1] [load_index=false]" << std::endl;
		return 0;
	}

	std::string input_path = argv[1];
	int max_edits = argc >= 3 ? std::stoi(argv[2]) : 2;
	bool load_index = argc >= 4 && std::string(argv[3]) == "true";

	std::shared_ptr<index_type> index;
	if(load_index){
		std::cerr << "loadinag index...";
		index = std::make_shared<index_type>(input_path);
		std::cerr << "done." << std::endl;
	}
	else{
		std::vector<text> texts;
		std::ifstream ifs(input_path);

		std::cerr << "loading texts...";
		if(!ifs.is_open()){
			std::cerr << "input file is not available: " << input_path << std::endl;
			return 1;
		}
		while(ifs.good()){
			std::string line;
			std::getline(ifs, line);
			if(ifs.eof())
				break;
			texts.push_back(line);
		}
		sftrie::sort_texts(texts.begin(), texts.end());
		std::cerr << "done." << std::endl;

		std::cerr << "building index...";
		index = std::make_shared<index_type>(texts.begin(), texts.end());
		std::cerr << "done." << std::endl;
	}

	auto searcher = index->searcher();
	while(true){
		std::cerr << "> ";
		std::string query;
		std::getline(std::cin, query);
		if(std::cin.eof() || query == "exit" || query == "quit" || query == "bye"){
			break;
		}
		else if(query.substr(0, 5) == "save="){
			std::string output_path = query.substr(5);
			index->save(output_path);
			std::cout << "index saved to " << output_path << std::endl;
			continue;
		}

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
