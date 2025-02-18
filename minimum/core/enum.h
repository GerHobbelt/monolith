#pragma once
// Petter Strandmark
//
// Defines a macro MAKE_ENUM to replace enum class if conversion of enum values to
// and from strings is desired.
//
// Example:
//
//     MAKE_ENUM(MyEnum, FOO, BAR, BAZ, BIZ);
//     DEFINE_enum_flag(MyEnum, some_flag, MyEnum::FOO, "Controls some feature");
#include <string>
#include <string_view>

#include <minimum/core/check.h>

#define EXPAND_VISUAL_STUDIO_HELPER(x) x
#define ALL_ARGUMENTS_TO_STRING_HELPER(zero,                                                     \
                                       a1,                                                       \
                                       a2,                                                       \
                                       a3,                                                       \
                                       a4,                                                       \
                                       a5,                                                       \
                                       a6,                                                       \
                                       a7,                                                       \
                                       a8,                                                       \
                                       a9,                                                       \
                                       a10,                                                      \
                                       a11,                                                      \
                                       a12,                                                      \
                                       a13,                                                      \
                                       a14,                                                      \
                                       a15,                                                      \
                                       a16,                                                      \
                                       a17,                                                      \
                                       a18,                                                      \
                                       a19,                                                      \
                                       a20,                                                      \
                                       ...)                                                      \
#a1, #a2, #a3, #a4, #a5, #a6, #a7, #a8, #a9, #a10, #a11, #a12, #a13, #a14, #a15, #a16, #a17, \
	    #a18, #a19, #a20

#define ALL_ARGUMENTS_TO_STRING(...)                            \
	EXPAND_VISUAL_STUDIO_HELPER(ALL_ARGUMENTS_TO_STRING_HELPER( \
	    0, __VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0))
// Add more than 20 arguments as needed.

namespace {
template <typename... Ts>
constexpr auto number_of_arguments_for_enum_creation(Ts...) {
	return sizeof...(Ts);
}
}  // namespace

#define MAKE_ENUM(Name, ...)                                                                 \
	enum class Name { __VA_ARGS__ };                                                         \
	namespace {                                                                              \
	constexpr std::string_view Name##_enum_names[] = {ALL_ARGUMENTS_TO_STRING(__VA_ARGS__)}; \
	constexpr std::string_view to_string(Name value) {                                       \
		return Name##_enum_names[static_cast<int>(value)];                                   \
	}                                                                                        \
	constexpr auto Name##_number_of_entries() {                                              \
		enum TempEnum { __VA_ARGS__ };                                                       \
		return number_of_arguments_for_enum_creation(__VA_ARGS__);                           \
	}                                                                                        \
	Name Name##_from_string(std::string_view str) {                                          \
		for (int i = 0; i < Name##_number_of_entries(); ++i) {                               \
			if (Name##_enum_names[i] == str) {                                               \
				return static_cast<Name>(i);                                                 \
			}                                                                                \
		}                                                                                    \
		throw std::runtime_error("Invalid enum string.");                                    \
	}                                                                                        \
	Name Name##_from_int(int i) {                                                            \
		if (i >= 0 && i < Name##_number_of_entries()) {                                      \
			return static_cast<Name>(i);                                                     \
		}                                                                                    \
		throw std::runtime_error("Invalid enum string.");                                    \
	}                                                                                        \
	bool Name##_flag_validator(const char* flagname, const std::string& value) {             \
		try {                                                                                \
			Name##_from_string(value);                                                       \
		} catch (std::runtime_error&) {                                                      \
			return false;                                                                    \
		}                                                                                    \
		return true;                                                                         \
	}                                                                                        \
	}  // namespace

#define DEFINE_enum_flag(Enum, name, default_value, description)             \
	DEFINE_string(name, std::string{to_string(default_value)}, description); \
	static bool dummy##name = gflags::RegisterFlagValidator(&FLAGS_##name, &Enum##_flag_validator);
