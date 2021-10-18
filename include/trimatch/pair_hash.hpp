#ifndef TRIMATCH_PAIR_HASH
#define TRIMATCH_PAIR_HASH

#include <utility>

namespace trimatch{

struct pair_hash
{
	template<typename T1, typename T2>
	size_t operator()(const std::pair<T1, T2>& p) const
	{
		return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
	}
};

}

#endif
