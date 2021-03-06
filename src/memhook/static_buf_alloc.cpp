#include "static_buf_alloc.h"

#include <cerrno>
#include <cstring>

namespace memhook {
  char   StaticBufAlloc::tmpbuf_[8192];
  size_t StaticBufAlloc::tmppos_ = 0;

  void *StaticBufAlloc::malloc(size_t size) {
    if (tmppos_ + size >= sizeof(tmpbuf_)) {
      errno = ENOMEM;
      return NULL;
    }

    void *ptr = tmpbuf_ + tmppos_;
    tmppos_ += size;
    return ptr;
  }

  void *StaticBufAlloc::calloc(size_t nmemb, size_t size) {
    const size_t total_size = nmemb * size;
    void *ptr = StaticBufAlloc::malloc(total_size);
    if (ptr == NULL)
      return ptr;
    memset(ptr, 0, total_size);
    return ptr;
  }

  void StaticBufAlloc::free(void *) {
    // do nothing
  }
}
