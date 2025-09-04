# trimatch: trie-based (exact|predictive|approximate) string matching library

## Usage

1. Add [include](include) and [external/stfrie/include](external/sftrie/include) to your include path

2. Include header
```c++
#include <trimatch/index.hpp>
```

3. Build index
```c++
std::vector<std::string> texts; // you can also use u16string, u32string, etc.
...
auto index = trimatch::set::build(texts);
```

4. Search texts
```c++
std::string query = "...";
auto searcher = index.searcher();

for(const auto& [text, distance]: searcher.approx(query))
	std::cout << "text=" << text << ", distance=" << distance << std::endl;
```
