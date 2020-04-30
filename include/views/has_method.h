#pragma once

#include <type_traits>

#define DEFINE_HAS_METHOD(name, traitName)\
	template<typename T>\
	class HasMethodTrait##traitName {\
	private:\
		template<typename U> static auto Test(int) -> decltype(std::declval<U>().name(), char{});\
		template<typename U> static int Test(...);\
	public:\
		static constexpr bool Value = sizeof(Test<T>(0)) == sizeof(char);\
	};\
	template<typename T>\
	inline static constexpr bool HasMethod##traitName = HasMethodTrait##traitName<T>::Value;
