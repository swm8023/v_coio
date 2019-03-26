#pragma once

#include <cstdlib>
#include <cstdint>

namespace v {
namespace co {

extern "C" {

typedef void* fcontext_t;

struct transfer_t {
    fcontext_t  fctx;
    void    *data;
};

transfer_t jump_fcontext(fcontext_t const to, void *vp);
fcontext_t make_fcontext(void *sp, std::size_t size, void (*fn)(transfer_t) );
transfer_t ontop_fcontext(fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t));

}
}
}
