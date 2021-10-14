#ifndef TRIMATCH_EVAL_EDIT_DISTANCE_DP_HPP
#define TRIMATCH_EVAL_EDIT_DISTANCE_DP_HPP

#include <vector>

struct EditDistance
{
public:
	using integer = size_t;

private:
	std::vector<integer> D;

public:
	template<typename text>
	integer operator()(const text& a, const text& b)
	{
		if(a.empty())
			return b.empty() ? 0 : b.size();
		else if(b.empty())
			return a.size() < b.size() ? b.size() : a.size();

		// prepare work table
		if(a.size() > D.size())
			D.reserve(a.size());
		D[0] = 0;
		for(typename text::size_type i = 1; i < a.size() + 1; ++i)
			D[i] = D[i - 1] + 1.0;

		// compute edit distance
		for(const auto c: b){
			auto prev = D[0];
			D[0] += 1.0;
			for(typename text::size_type i = 1; i < a.size() + 1; ++i){
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
