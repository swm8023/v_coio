#pragma once


#include <ev++.h>
#include <coio/common/noncopyable.h>

namespace v {
namespace io {

class IOContext : NonCopyable {
public:
	IOContext() {
		loop_ = new ev::dynamic_loop();
	}

	struct ev_loop *GetRawLoop() {
		return loop_->raw_loop;
	}

	void Run() {
		loop_->loop();
	}

private:
	ev::dynamic_loop *loop_{ nullptr };
};


namespace this_io {

inline IOContext* Get() {
	static thread_local IOContext io_ctx_;
	return &io_ctx_;
}
} // this_io

} // io
} // v