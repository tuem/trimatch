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
concept for readable streams
*/

#ifndef TRIMATCH_READABLE
#define TRIMATCH_READABLE

namespace trimatch{

template<typename stream_type>
concept readable = requires(stream_type& s, char buf[1])
{
	s.read(&buf, 1);
};

}

#endif
