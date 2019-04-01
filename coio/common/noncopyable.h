#ifndef TIO_NONCOPYABLE_H_
#define TIO_NONCOPYABLE_H_

namespace v {
namespace io {
class NonCopyable {
public:
	NonCopyable(const NonCopyable&) = delete;
	void operator=(const NonCopyable&) = delete;

protected:
	NonCopyable() = default;
	~NonCopyable() = default;
};

}
}

#endif // TIO_NONCOPYABLE_H_
