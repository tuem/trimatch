#ifndef TRIMATCH_EVAL_EDIT_DISTANCE_DP_HPP
#define TRIMATCH_EVAL_EDIT_DISTANCE_DP_HPP

#include <vector>

struct EditDistance
{
	template<typename text>
	typename text::size_type operator()(const text& a, const text& b) const
	{
		using integer = typename text::size_type;

		if(a.empty())
			return b.empty() ? 0 : b.size();
		else if(b.empty())
			return a.size();

		// prepare work table
		std::vector<integer> D(a.size() + 1);
		for(integer i = 0; i < a.size() + 1; ++i)
			D[i] = i;

		// compute edit distance
		for(const auto c: b){
			auto prev = D[0];
			D[0] += 1;
			for(integer i = 1; i < a.size() + 1; ++i){
				auto del = D[i - 1] + 1;
				auto ins = D[i] + 1;
				auto sub = prev + (a[i - 1] == c ? 0 : 1);
				prev = D[i];
				D[i] = std::min({del, ins, sub});
			}
		}
	
		return D.back();
	}
};

#endif
