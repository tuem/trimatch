# Trimatch: Trie-based (Exact|Predictive|Approximate) String Matching Library

## Usage

1. Add [include](include) and [external/stfrie/include](external/sftrie/include) to your include path

2. Include header
```c++
#include <trimatch/index.hpp>
```

3. Build index
```c++
std::vector<std::string> texts; // you can also use wstring, u16string, u32string, etc.
...
auto index = trimatch::build(texts);
```

4. Search from index
```c++
std::string query = "...";
auto searcher = index.searcher();

std::cout << "exact match: " << searcher.exact(query) ? "found" : "not found" << std::endl;
for(const auto& result: searcher.predict(query))
	std::cout << "predictive search: " << result << std::endl;
for(const auto& result: searcher.approx(query))
	std::cout << "approximate search: text=" << result.first << ", score=" << result.second << std::endl;
```
