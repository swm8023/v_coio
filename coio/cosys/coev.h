#pragma once
#include <ev++.h>
#include <cassert>
#include <unordered_map>

#include <co/coroutine.h>
#include <coio/common/noncopyable.h>
#include "context.h"

namespace v {
namespace io {

class CoEvent : NonCopyable{
public:
	CoEvent() {
	}

	~CoEvent() {
	}

	CoEvent(CoEvent &&e)  {

	}
	CoEvent& operator = (CoEvent&& e) {
	
	}

	void Init(int fd) {
		auto loop = this_io::Get()->GetRawLoop();
		io_r_.set(loop);
		io_w_.set(loop);

		io_r_.set(fd, ev::READ);
		io_w_.set(fd, ev::WRITE);

		io_r_.set<CoEvent, &CoEvent::OnIOReadEvent>(this);
		io_w_.set<CoEvent, &CoEvent::OnIOWriteEvent>(this);
	}

	void Release() {
		io_r_.stop();
		io_w_.stop();
		// TODO, maybe waiting ?
		assert(r_co_ == nullptr);
		assert(w_ro_ == nullptr);
	}

	void EnterCoRead(){
		assert(co::this_co::InCoroutine());
		// only allow one reader
		assert(r_co_ == nullptr);

		io_r_.start();
		r_co_ = co::this_co::Get();
		r_co_->CoYield();
	}

	void EnterCoWrite() {
		assert(co::this_co::InCoroutine());
		// only allow one writer
		assert(w_co_ == nullptr);

		io_w_.start();
		w_co_ = co::this_co::Get();
		w_co_->CoYield();
	}


private:
	void OnIOWriteEvent(ev::io &watcher, int rev) {
		assert(0 < (rev & ev::WRITE));

		if (w_co_ != nullptr) {
			auto co = std::exchange(w_co_, nullptr);
			io_w_.stop();
			co->CoResume();
		}
	}

	void OnIOReadEvent(ev::io &watcher, int rev) {
		assert(0 < (rev & ev::READ));

		if (r_co_ != nullptr) {
			auto co = std::exchange(r_co_, nullptr);
			io_r_.stop();
			co->CoResume();
		}
	}
	ev::io io_r_;
	ev::io io_w_;

	co::Context* r_co_{nullptr};
	co::Context* w_co_{nullptr};

};

extern thread_local std::unordered_map<int, CoEvent> co_evts;

CoEvent* GetCoEvent(int fd);
void ReleaseCoEvent(int fd);

}
}