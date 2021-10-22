# Trimatch: An (Exact|Predictive|Approximate) String Matching Library

## Usage

1. Copy [include/trimatch](include/trimatch) to your include path

2. Include header
```c++
#include <trimatch/index.hpp>
```

3. Build index
```c++
std::vector<std::u32string> texts; // you can also use string, wstring, u16string, etc.
...
auto index = trimatch::build(texts.begin(), texts.end());
```

4. search queries
```c++
std::u32string query = ...;
auto searcher = index.searcher();

std::cout << "exact match: " << searcher.exact(query) ? "found" : "not found" << std::endl;
for(const auto& result: searcher.predict(query))
	std::cout << "predictive search: " << result << std::endl;
for(const auto& result: searcher.approx(query))
	std::cout << "approximate search: " << result << std::endl;
```
