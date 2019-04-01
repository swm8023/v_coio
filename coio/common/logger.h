#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include "singleton.h"


namespace v {
namespace io {
enum class LOG_LEVEL {
	LDEBUG = 0,
	LINFO,
	LWARN,
	LERROR,
	LCRITICL
};

static const char* LOG_LEVEL_STR[] = {
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"CRITICL"
};

template<typename T>
class AddSpaceHelper {
public:
	AddSpaceHelper(T const& ref) : ref_(ref) {}
	friend std::ostream& operator << (std::ostream &os, AddSpaceHelper<T> t) {
		return os << t.ref_ << ' ';
	}

private:
	T const& ref_;
};

class Logger : public Singleton<Logger> {
	friend class Singleton<Logger>;
public:
	static const int BUFF_SZ = 4096;

	std::ostream& FormatHeader(std::ostream &os, LOG_LEVEL lvl) {
		return os << LOG_LEVEL_STR[int(lvl)] << " ";
	}

	inline std::ostream& GetStream() {
		if (fstream_.is_open()) {
			return fstream_;
		}
		return std::cout;
	}

	inline void QuitIfCriticl(LOG_LEVEL lvl) {
		if (lvl == LOG_LEVEL::LCRITICL) {
			std::exit(-1);
		}
	}

	template<typename... Args>
	void CLog(LOG_LEVEL lvl, char const* fmt, Args const&... args) {
		if (lvl < cur_level_) {
			return;
		}

		std::ostringstream temp_os;
		char buf[BUFF_SZ];
		snprintf(buf, BUFF_SZ, fmt, args...);
		FormatHeader(temp_os, lvl) << buf << std::endl;
		GetStream() << temp_os.str();
		QuitIfCriticl(lvl);
	}

	template<typename... Args>
	void InnerLog(LOG_LEVEL lvl, Args const&... args) {
		if (lvl < cur_level_) {
			return;
		}

		std::ostringstream temp_os;
		(FormatHeader(temp_os, lvl) << ... << AddSpaceHelper(args)) << std::endl;
		//(FormatHeader(temp_os, lvl) << ... << args) << std::endl;

		GetStream() << temp_os.str();
		QuitIfCriticl(lvl);
	}

	void SetLogLevel(LOG_LEVEL lvl) {
		cur_level_ = lvl;
	}

	void UseFileLog(bool is_use_file) {
		if (is_use_file) {
			fstream_.open("log.txt");
			if (!fstream_) {
				InnerLog(LOG_LEVEL::LCRITICL, "Use Log File Failed!!");
				return;
			}
		} else {
			if (fstream_.is_open()) {
				fstream_.close();
			}
		}
	}

protected:
	LOG_LEVEL cur_level_;
	std::ofstream fstream_;

};

#define CLogDebug(fmt, ...)   Logger::Instance().CLog(LOG_LEVEL::LDEBUG,   fmt, ##__VA_ARGS__)
#define CLogInfo(fmt, ...)    Logger::Instance().CLog(LOG_LEVEL::LINFO,    fmt, ##__VA_ARGS__)
#define CLogWarn(fmt, ...)    Logger::Instance().CLog(LOG_LEVEL::LWARN,    fmt, ##__VA_ARGS__)
#define CLogError(fmt, ...)   Logger::Instance().CLog(LOG_LEVEL::LERROR,   fmt, ##__VA_ARGS__)
#define CLogCriticl(fmt, ...) Logger::Instance().CLog(LOG_LEVEL::LCRITICL, fmt, ##__VA_ARGS__)

#define LogDebug(...)   Logger::Instance().InnerLog(LOG_LEVEL::LDEBUG,   ##__VA_ARGS__)
#define LogInfo(...)    Logger::Instance().InnerLog(LOG_LEVEL::LINFO,    ##__VA_ARGS__)
#define LogWarn(...)    Logger::Instance().InnerLog(LOG_LEVEL::LWARN,    ##__VA_ARGS__)
#define LogError(...)   Logger::Instance().InnerLog(LOG_LEVEL::LERROR,   ##__VA_ARGS__)
#define LogCriticl(...) Logger::Instance().InnerLog(LOG_LEVEL::LCRITICL, ##__VA_ARGS__)

#define RETURN_FALSE_IF_ERROR(cond, ...) do {                   \
if (cond) {                                                     \
    Logger::Instance().InnerLog(LOG_LEVEL::LERROR, ##__VA_ARGS__);   \
    return false;                                               \
}                                                               \
} while (0);

#define RETURN_IF_ERROR(cond, ...) do {                         \
if (cond) {                                                     \
    Logger::Instance().InnerLog(LOG_LEVEL::LERROR, ##__VA_ARGS__);   \
    return;                                                     \
}                                                               \
} while (0);

#define RETURN_NULL_IF_ERROR(cond, ...) do {                   \
if (cond) {                                                    \
    Logger::Instance().InnerLog(LOG_LEVEL::LERROR, ##__VA_ARGS__);  \
    return nullptr;                                            \
}                                                              \
} while (0);

#define RETURN_FALSE_IF_WARN(cond, ...) do {                   \
if (cond) {                                                     \
    Logger::Instance().InnerLog(LOG_LEVEL::LWARN, ##__VA_ARGS__);   \
    return false;                                               \
}                                                               \
} while (0);

#define RETURN_IF_WARN(cond, ...) do {                         \
if (cond) {                                                     \
    Logger::Instance().InnerLog(LOG_LEVEL::LWARN, ##__VA_ARGS__);   \
    return;                                                     \
}                                                               \
} while (0);

#define RETURN_NULL_IF_WARN(cond, ...) do {                   \
if (cond) {                                                    \
    Logger::Instance().InnerLog(LOG_LEVEL::LWARN, ##__VA_ARGS__);  \
    return nullptr;                                            \
}                                                              \
} while (0);

}
}