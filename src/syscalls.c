#include <sys/stat.h>
#include <errno.h>
#include "SEGGER_RTT.h"

/**
 * Close a file descriptor.
 * Not used on bare metal, so always return error.
 */
int _close(int file) {
    (void)file;
    return -1;
}

/**
 * Provide file status (used by fstat).
 * Pretend everything is a character device.
 */
int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * Check if stream is a terminal.
 */
int _isatty(int file) {
    (void)file;
    return 1;
}

/**
 * Set file position (not supported).
 */
int _lseek(int file, int ptr, int dir) {
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

/**
 * Read from a file (not supported).
 */
int _read(int file, char *ptr, int len) {
    (void)file;
    (void)ptr;
    (void)len;
    errno = EINVAL;
    return -1;
}

/**
 * Write to a file.
 * Redirects stdout/stderr to RTT channel 0.
 */
int _write(int file, char *ptr, int len) {
    (void)file;
    SEGGER_RTT_Write(0, ptr, len);
    return len;
}

/**
 * Needed by some libc functions.
 * Provide a dummy heap implementation.
 */
caddr_t _sbrk(int incr) {
    extern char _end;   // Defined by linker script
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}
