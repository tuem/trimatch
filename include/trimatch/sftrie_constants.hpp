/*
sftrie
https://github.com/tuem/sftrie

Copyright 2017 Takashi Uemura

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

#ifndef SFTRIE_CONSTANTS_HPP
#define SFTRIE_CONSTANTS_HPP

#include <cstdint>

namespace sftrie{

template<typename T>
struct _constants
{
	static const char signature[4];

	static const std::uint8_t current_major_version;
	static const std::uint8_t current_minor_version;

	static const std::uint8_t container_type_set;
	static const std::uint8_t container_type_map;

	static const std::uint8_t index_type_basic;
	static const std::uint8_t index_type_tail;
	static const std::uint8_t index_type_decomp;

	static const std::uint8_t text_charset_system_default;
	static const std::uint8_t text_charset_unicode;

	static const std::uint8_t text_encoding_system_default;
	static const std::uint8_t text_encoding_utf8;
	static const std::uint8_t text_encoding_utf16;
	static const std::uint8_t text_encoding_utf32;

	static const std::uint8_t integer_type_uint8;
	static const std::uint8_t integer_type_int8;
	static const std::uint8_t integer_type_uint16;
	static const std::uint8_t integer_type_int16;
	static const std::uint8_t integer_type_uint32;
	static const std::uint8_t integer_type_int32;
	static const std::uint8_t integer_type_uint64;
	static const std::uint8_t integer_type_int64;

	static const std::uint8_t value_type_uint8;
	static const std::uint8_t value_type_int8;
	static const std::uint8_t value_type_uint16;
	static const std::uint8_t value_type_int16;
	static const std::uint8_t value_type_uint32;
	static const std::uint8_t value_type_int32;
	static const std::uint8_t value_type_uint64;
	static const std::uint8_t value_type_int64;
};

template<typename T> const char _constants<T>::signature[4] = {'S', 'F', 'T', 'I'};

template<typename T> const std::uint8_t _constants<T>::current_major_version = 0;
template<typename T> const std::uint8_t _constants<T>::current_minor_version = 0;

template<typename T> const std::uint8_t _constants<T>::container_type_set = 0;
template<typename T> const std::uint8_t _constants<T>::container_type_map = 1;

template<typename T> const std::uint8_t _constants<T>::index_type_basic = 0;
template<typename T> const std::uint8_t _constants<T>::index_type_tail = 1;
template<typename T> const std::uint8_t _constants<T>::index_type_decomp = 2;

template<typename T> const std::uint8_t _constants<T>::text_charset_system_default = 0;
template<typename T> const std::uint8_t _constants<T>::text_charset_unicode = 1;

template<typename T> const std::uint8_t _constants<T>::text_encoding_system_default = 0;
template<typename T> const std::uint8_t _constants<T>::text_encoding_utf8 = 1;
template<typename T> const std::uint8_t _constants<T>::text_encoding_utf16 = 2;
template<typename T> const std::uint8_t _constants<T>::text_encoding_utf32 = 3;

template<typename T> const std::uint8_t _constants<T>::integer_type_uint8 = 0;
template<typename T> const std::uint8_t _constants<T>::integer_type_int8 = 1;
template<typename T> const std::uint8_t _constants<T>::integer_type_uint16 = 2;
template<typename T> const std::uint8_t _constants<T>::integer_type_int16 = 3;
template<typename T> const std::uint8_t _constants<T>::integer_type_uint32 = 4;
template<typename T> const std::uint8_t _constants<T>::integer_type_int32 = 5;
template<typename T> const std::uint8_t _constants<T>::integer_type_uint64 = 6;
template<typename T> const std::uint8_t _constants<T>::integer_type_int64 = 7;

template<typename T> const std::uint8_t _constants<T>::value_type_uint8 = 0;
template<typename T> const std::uint8_t _constants<T>::value_type_int8 = 1;
template<typename T> const std::uint8_t _constants<T>::value_type_uint16 = 2;
template<typename T> const std::uint8_t _constants<T>::value_type_int16 = 3;
template<typename T> const std::uint8_t _constants<T>::value_type_uint32 = 4;
template<typename T> const std::uint8_t _constants<T>::value_type_int32 = 5;
template<typename T> const std::uint8_t _constants<T>::value_type_uint64 = 6;
template<typename T> const std::uint8_t _constants<T>::value_type_int64 = 7;

using constants = _constants<void>;


template<typename text>
struct _text_constants
{
	static const std::uint8_t text_charset;
	static const std::uint8_t text_encoding;
};

template<> const std::uint8_t _text_constants<std::u16string>::text_charset = constants::text_charset_unicode;
template<> const std::uint8_t _text_constants<std::u32string>::text_charset = constants::text_charset_unicode;


template<typename integer>
struct _integer_constants
{
	static const std::uint8_t integer_type;
};

template<> const std::uint8_t _integer_constants<std::uint8_t>::integer_type = constants::integer_type_uint8;
template<> const std::uint8_t _integer_constants<std::int8_t>::integer_type = constants::integer_type_int8;
template<> const std::uint8_t _integer_constants<std::uint16_t>::integer_type = constants::integer_type_uint16;
template<> const std::uint8_t _integer_constants<std::int16_t>::integer_type = constants::integer_type_int16;
template<> const std::uint8_t _integer_constants<std::uint32_t>::integer_type = constants::integer_type_uint32;
template<> const std::uint8_t _integer_constants<std::int32_t>::integer_type = constants::integer_type_int32;
template<> const std::uint8_t _integer_constants<std::uint64_t>::integer_type = constants::integer_type_uint64;
template<> const std::uint8_t _integer_constants<std::int64_t>::integer_type = constants::integer_type_int64;


template<typename integer>
struct _value_constants
{
	static const std::uint8_t value_type;
};

template<> const std::uint8_t _value_constants<std::uint8_t>::value_type = constants::value_type_uint8;
template<> const std::uint8_t _value_constants<std::int8_t>::value_type = constants::value_type_int8;
template<> const std::uint8_t _value_constants<std::uint16_t>::value_type = constants::value_type_uint16;
template<> const std::uint8_t _value_constants<std::int16_t>::value_type = constants::value_type_int16;
template<> const std::uint8_t _value_constants<std::uint32_t>::value_type = constants::value_type_uint32;
template<> const std::uint8_t _value_constants<std::int32_t>::value_type = constants::value_type_int32;
template<> const std::uint8_t _value_constants<std::uint64_t>::value_type = constants::value_type_uint64;
template<> const std::uint8_t _value_constants<std::int64_t>::value_type = constants::value_type_int64;

};

#endif
