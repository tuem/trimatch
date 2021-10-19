# Trimatch: An (Exact|Predictive|Approximate) String Matching Library

## Usage
```c++
// copy include/trimatch to your include path
#include <trimatch/builder.hpp>

...

std::vector<std::u32string> texts; // you can also use string, wstring, u16string etc.
std::u32string query;

...

auto index = trimatch::build(texts.begin(), texts.end());
auto searcher = index.searcher();

std::cout << "exact match: " << searcher.exact(query) ? "found" : "not found" << std::endl;
for(const auto& result: searcher.predict(query))
    std::cout << "predictive search: " << result << std::endl;
for(const auto& result: searcher.approx(query))
    std::cout << "approximate search: " << result << std::endl;
```
