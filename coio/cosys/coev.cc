#include "coev.h"

namespace v {
namespace io {

thread_local std::unordered_map<int, CoEvent> v::io::co_evts;

CoEvent* GetCoEvent(int fd) {
	auto iter = co_evts.find(fd);
	if (iter == co_evts.end()) {
		auto new_iter = co_evts.emplace(std::make_pair(fd, CoEvent())).first;
		new_iter->second.Init(fd);
		return &new_iter->second;
	} else {
		return &iter->second;
	}
}

void ReleaseCoEvent(int fd) {
	CoEvent* cv = GetCoEvent(fd);
	if (cv != nullptr) {
		cv->Release();
	}
}

}
}
