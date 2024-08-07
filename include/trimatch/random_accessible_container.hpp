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
concept for randam accessible containers
*/

#ifndef TRIMATCH_RANDOM_ACCESSIBLE_CONTAINER
#define TRIMATCH_RANDOM_ACCESSIBLE_CONTAINER

namespace trimatch{

template<typename container_type>
concept random_accessible_container = requires(container_type& c)
{
	typename container_type::value_type;
	std::begin(c) != std::end(c);
	c[0];
	std::size(c);
};

}

#endif
