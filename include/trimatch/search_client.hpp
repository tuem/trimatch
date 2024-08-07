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
Main interface for search operations
*/

#ifndef TRIMATCH_SEARCH_CLIENT
#define TRIMATCH_SEARCH_CLIENT

#include <sftrie/util.hpp>

namespace trimatch{

template<class text, class integer, class trie, class approximate_matcher>
class search_client
{
public:
	using value_type = typename trie::value_type;
	using prefix_search_iterator = typename trie::prefix_iterator;
	using predictive_search_iterator = typename trie::subtree_iterator;

	// matched text, associated value, edits
	struct approximate_search_result;
	// predicted text, associated value, edits (matched part), edits (whole text)
	struct approximate_predictive_search_result;

	struct approximate_search_iterator;

	search_client(const trie& T);

	// exact match
	bool exact(const text& query) const;

	// common prefix search
	prefix_search_iterator prefix(const text& query);

	// predictive search
	predictive_search_iterator predict(const text& query);
	template<class back_insert_iterator>
	void predict(const text& query, back_insert_iterator bi);

	// approximate search
	approximate_search_iterator approx(const text& query, integer max_edits = 1) const;
	template<class back_insert_iterator>
	void approx(const text& query, integer max_edits, back_insert_iterator bi) const;

	// approximate predictive search
	template<class back_insert_iterator>
	void approx_predict(const text& query, integer max_edits, back_insert_iterator bi) const;

private:
	const trie& T;
	typename trie::common_searcher trie_search_client;

	template<class back_insert_iterator>
	void approx_step(approximate_matcher& matcher,
		typename trie::node_type root, text& current, back_insert_iterator& bi) const;

	template<class back_insert_iterator>
	void approx_predict_step(integer max_edits, approximate_matcher& matcher,
		typename trie::node_type root, text& current, back_insert_iterator& bi) const;
	template<class back_insert_iterator>
	void correct_approx_predict_results(integer max_edits, approximate_matcher& matcher,
		typename trie::node_type root, text& current, integer prefix_edits, integer current_edits, back_insert_iterator& bi) const;
};


template<class text, class integer, class trie, class approximate_matcher>
struct search_client<text, integer, trie, approximate_matcher>::approximate_search_result
{
	text key;
	typename trie::value_type value;
	integer edits;
};

template<class text, class integer, class trie, class approximate_matcher>
struct search_client<text, integer, trie, approximate_matcher>::approximate_predictive_search_result
{
	text key;
	typename trie::value_type value;
	integer edits_prefix;
	integer edits_whole;
};

template<class text, class integer, class trie, class approximate_matcher>
struct search_client<text, integer, trie, approximate_matcher>::approximate_search_iterator
{
	const trie &T;

	const text& query;
	const integer max_edits;

	approximate_matcher matcher;

	std::vector<typename trie::child_iterator> path;
	text current;

	approximate_search_iterator(const trie& T, const text& query, integer max_edits):
		T(T), query(query), max_edits(max_edits), matcher(query, max_edits)
	{
		if(!query.empty()){
			path.push_back(typename trie::child_iterator(T));
			++*this;
		}
	}

	approximate_search_iterator& begin()
	{
		return *this;
	}

	approximate_search_iterator end()
	{
		return approximate_search_iterator(T, text(), 0);
	}

	bool operator!=(const approximate_search_iterator& i) const
	{
		return path != i.path;
	}

	approximate_search_result operator*() const
	{
		return {current, (*path.back()).value(), static_cast<integer>(matcher.distance())};
	}

	void back_transition()
	{
		path.pop_back();
		current.pop_back();
		matcher.back();
	}

	bool try_transition(const typename trie::child_iterator& next)
	{
		auto c = (*next).label();
		auto result = matcher.update(c);
		path.push_back(next);
		current.push_back(c);

		return result;
	}

	approximate_search_iterator& operator++()
	{
		bool transition_succeeded = true;
		do{
			if(transition_succeeded && !(*path.back()).leaf()){
				auto next = (*path.back()).children(); // first child
				transition_succeeded = try_transition(next);
			}
			else if(!transition_succeeded && path.size() > 1 && path.back().incrementable()){
				auto next = path.back();
				++next; // next sibling
				path.pop_back();
				current.pop_back();
				transition_succeeded = try_transition(next);
			}
			else{
				if(!transition_succeeded){
					path.pop_back();
					current.pop_back();
				}

				while(path.size() > 1 && !path.back().incrementable())
					back_transition();
				if(path.size() > 1){
					auto next = path.back();
					++next; // next sibling
					back_transition();
					transition_succeeded = try_transition(next);
				}
				else{
					path.pop_back();
				}
			}
		}while(!path.empty() && (!transition_succeeded || !((*path.back()).match() && matcher.matched())));

		return *this;
	}
};


template<class text, class integer, class trie, class approximate_matcher>
search_client<text, integer, trie, approximate_matcher>::search_client(const trie& T):
	T(T), trie_search_client(T.searcher())
{}

template<class text, class integer, class trie, class approximate_matcher>
bool search_client<text, integer, trie, approximate_matcher>::exact(const text& query) const
{
	return T.exists(query);
}

template<class text, class integer, class trie, class approximate_matcher>
typename search_client<text, integer, trie, approximate_matcher>::prefix_search_iterator
search_client<text, integer, trie, approximate_matcher>::prefix(const text& query)
{
	return trie_search_client.prefix(query);
}

template<class text, class integer, class trie, class approximate_matcher>
typename search_client<text, integer, trie, approximate_matcher>::predictive_search_iterator
search_client<text, integer, trie, approximate_matcher>::predict(const text& query)
{
	return trie_search_client.predict(query);
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void search_client<text, integer, trie, approximate_matcher>::predict(
	const text& query, back_insert_iterator bi)
{
	for(const auto& r: trie_search_client.predict(query))
		*bi++ = r.key();
}

template<class text, class integer, class trie, class approximate_matcher>
typename search_client<text, integer, trie, approximate_matcher>::approximate_search_iterator
search_client<text, integer, trie, approximate_matcher>::approx(const text& query, integer max_edits) const
{
	return approximate_search_iterator(T, query, max_edits);
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void search_client<text, integer, trie, approximate_matcher>::approx(
	const text& query, integer max_edits, back_insert_iterator bi) const
{
	approximate_matcher matcher(query, max_edits);
	text current;
	approx_step(matcher, T.root(), current, bi);
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void search_client<text, integer, trie, approximate_matcher>::approx_step(
	approximate_matcher& matcher, typename trie::node_type root, text& current, back_insert_iterator& bi) const
{
	if(root.match() && matcher.matched())
		*bi++ = {current, root.value(), static_cast<integer>(matcher.distance())};
	if(root.leaf())
		return;
	for(const auto& n: root.children()){
		if(matcher.update(n.label())){
			current.push_back(n.label());
			approx_step(matcher, n, current, bi);
			current.pop_back();
			matcher.back();
		}
	}
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void search_client<text, integer, trie, approximate_matcher>::approx_predict(
	const text& query, integer max_edits, back_insert_iterator bi) const
{
	approximate_matcher matcher(query, max_edits);
	text current;
	approx_predict_step(max_edits, matcher, T.root(), current, bi);
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void search_client<text, integer, trie, approximate_matcher>::approx_predict_step(
	integer max_edits, approximate_matcher& matcher, typename trie::node_type root, text& current, back_insert_iterator& bi) const
{
	if(matcher.matched()){
		correct_approx_predict_results(max_edits, matcher, root, current, matcher.distance(), matcher.distance(), bi);
		return;
	}
	if(root.leaf())
		return;
	for(const auto& n: root.children()){
		if(matcher.update(n.label())){
			current.push_back(n.label());
			approx_predict_step(max_edits, matcher, n, current, bi);
			current.pop_back();
			matcher.back();
		}
	}
}

template<class text, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void search_client<text, integer, trie, approximate_matcher>::correct_approx_predict_results(
	integer max_edits, approximate_matcher& matcher, typename trie::node_type root,
	text& current, integer prefix_edits, integer current_edits, back_insert_iterator& bi) const
{
	if(root.match())
		*bi++ = {current, root.value(), std::min(prefix_edits, current_edits), current_edits};
	if(root.leaf())
		return;
	for(const auto& n: root.children()){
		current.push_back(n.label());
		if(current_edits <= max_edits && current.size() <= matcher.pattern.size() && matcher.update(n.label())){
			correct_approx_predict_results(max_edits, matcher, n, current,
				std::min(prefix_edits, static_cast<integer>(matcher.distance())), matcher.distance(), bi);
			matcher.back();
		}
		else{
			correct_approx_predict_results(max_edits, matcher, n, current, prefix_edits, current_edits + 1, bi);
		}
		current.pop_back();
	}
}

}

#endif
