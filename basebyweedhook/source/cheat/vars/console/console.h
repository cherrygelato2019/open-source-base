#pragma once
#include <cstdarg>
#include <cstdio>
#include "framework.h"


enum class Type : u32 {
	OK = 0,
	LOAD = 1,
	FAIL = 2,
	DEBUG = 3,
};

struct Colors {
	static constexpr const char* COLOR_OK = "\033[92m";
	static constexpr const char* COLOR_LOAD = "\033[32m";
	static constexpr const char* COLOR_FAIL = "\033[91m";
	static constexpr const char* COLOR_DEBUG = "\033[94m";
	static constexpr const char* RESET = "\033[0m";
};

namespace Logger {
	inline const char* GetPrefix(Type t) {
		switch (t) {
		case Type::OK:    return "\033[92m[<OK>]\033[0m ";
		case Type::LOAD:  return "\033[32m[<-->]\033[0m ";
		case Type::FAIL:  return "\033[91m[FAIL]\033[0m ";
		case Type::DEBUG: return "\033[94m[<DB>]\033[0m ";
		default:          return "";
		}
	}

	inline void Log(Type t, const char* fmt, ...) {
		std::printf("%s", GetPrefix(t));

		va_list args;
		va_start(args, fmt);
		std::vprintf(fmt, args);
		va_end(args);

		std::fflush(stdout);
	}
}
