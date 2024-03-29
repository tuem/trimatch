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
Main interface for exact matching, predictive search and approximate search
*/

#ifndef TRIMATCH_SEARCHER_MAP
#define TRIMATCH_SEARCHER_MAP

#include "index_map.hpp"

namespace trimatch{

namespace map{

template<class text, class item, class integer, class trie, class approximate_matcher>
class index<text, item, integer, trie, approximate_matcher>::search_client
{
public:
	//using value_type = typename trie::value_type;
	using prefix_search_iterator = typename trie::prefix_iterator;
	using predictive_search_iterator = typename trie::subtree_iterator;
	// matched text, associated value, edits
	using approximate_search_result = std::tuple<text, item, integer>;
	// predicted text, associated value, edits (matched part), edits (whole text)
	using approximate_predictive_search_result = std::tuple<text, item, integer, integer>;

	struct approximate_search_iterator;

	search_client(trie& T);

	// exact match
	bool exact(const text& query) const;

	// common prefix search
	prefix_search_iterator prefix(const text& query);

	// predictive search
	predictive_search_iterator predict(const text& query);
	template<class back_insert_iterator>
	void predict(const text& query, back_insert_iterator bi) const;

	// approximate search
	approximate_search_iterator approx(const text& query, integer max_edits = 1) const;
	template<class back_insert_iterator>
	void approx(const text& query, integer max_edits, back_insert_iterator bi) const;

	// approximate predictive search
	template<class back_insert_iterator>
	void approx_predict(const text& query, integer max_edits, back_insert_iterator bi) const;

private:
	trie& T;
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


template<class text, class item, class integer, class trie, class approximate_matcher>
struct index<text, item, integer, trie, approximate_matcher>::search_client::approximate_search_iterator
{
	trie &T;

	const text& query;
	const integer max_edits;

	approximate_matcher matcher;

	std::vector<typename trie::child_iterator> path;
	text current;

	approximate_search_iterator(trie& T, const text& query, integer max_edits):
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

	const approximate_search_result operator*()
	{
		return std::make_tuple(current, (*path.back()).value(), static_cast<integer>(matcher.distance()));
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
			else if(!transition_succeeded && path.size() > 1 && path.back() != path.back().end()){
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

				while(path.size() > 1 && path.back() != path.back().end())
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


template<class text, class item, class integer, class trie, class approximate_matcher>
index<text, item, integer, trie, approximate_matcher>::search_client::search_client(trie& T):
	T(T), trie_search_client(T.searcher())
{}

template<class text, class item, class integer, class trie, class approximate_matcher>
bool index<text, item, integer, trie, approximate_matcher>::search_client::exact(const text& query) const
{
	return T.exists(query);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
typename index<text, item, integer, trie, approximate_matcher>::search_client::prefix_search_iterator
index<text, item, integer, trie, approximate_matcher>::search_client::prefix(const text& query)
{
	return trie_search_client.prefix(query);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
typename index<text, item, integer, trie, approximate_matcher>::search_client::predictive_search_iterator
index<text, item, integer, trie, approximate_matcher>::search_client::predict(const text& query)
{
	return trie_search_client.predict(query);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void index<text, item, integer, trie, approximate_matcher>::search_client::predict(
	const text& query, back_insert_iterator bi) const
{
	typename trie::common_searcher searcher(T);
	for(const auto& r: searcher.predict(query))
		*bi++ = r;
}

template<class text, class item, class integer, class trie, class approximate_matcher>
typename index<text, item, integer, trie, approximate_matcher>::search_client::approximate_search_iterator
index<text, item, integer, trie, approximate_matcher>::search_client::approx(const text& query, integer max_edits) const
{
	return approximate_search_iterator(T, query, max_edits);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void index<text, item, integer, trie, approximate_matcher>::search_client::approx(
	const text& query, integer max_edits, back_insert_iterator bi) const
{
	approximate_matcher matcher(query, max_edits);
	text current;
	approx_step(matcher, T.root(), current, bi);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void index<text, item, integer, trie, approximate_matcher>::search_client::approx_step(
	approximate_matcher& matcher, typename trie::node_type root, text& current, back_insert_iterator& bi) const
{
	if(root.match() && matcher.matched())
		*bi++ = std::make_tuple(current, root.value(), static_cast<integer>(matcher.distance()));
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

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void index<text, item, integer, trie, approximate_matcher>::search_client::approx_predict(
	const text& query, integer max_edits, back_insert_iterator bi) const
{
	approximate_matcher matcher(query, max_edits);
	text current;
	approx_predict_step(max_edits, matcher, T.root(), current, bi);
}

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void index<text, item, integer, trie, approximate_matcher>::search_client::approx_predict_step(
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

template<class text, class item, class integer, class trie, class approximate_matcher>
template<class back_insert_iterator>
void index<text, item, integer, trie, approximate_matcher>::search_client::correct_approx_predict_results(
	integer max_edits, approximate_matcher& matcher, typename trie::node_type root,
	text& current, integer prefix_edits, integer current_edits, back_insert_iterator& bi) const
{
	if(root.match())
		*bi++ = std::make_tuple(current, root.value(), std::min(prefix_edits, current_edits), current_edits);
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

}

#endif
