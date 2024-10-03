#ifndef ZEPHYR_LIB_LIBC_COMMON_INCLUDE_MALLOC_H_
#define ZEPHYR_LIB_LIBC_COMMON_INCLUDE_MALLOC_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_COMMON_LIBC_MALLINFO2) || defined(__DOXYGEN__)

/**
 * @brief A structure containing memory allocation information.
 *
 * The mallinfo2 structure contains information about memory allocations performed by @ref calloc,
 * @ref free, @ref malloc, @ref memalign, @ref realloc, and @ref reallocarray.
 *
 * Heap usage may be expressed (in bytes) as `(arena - fordblks)` (bytes), or
 * as `(arena - fordblks) / arena` (%).
 *
 * Heap fragmentation may be expressed as `(arena - fordblks - uordblks)` (bytes), or
 * as `(arena - fordblks - uordblks) / arena` (%).
 */
struct mallinfo2 {
	/**
	 * Heap size (in bytes)
	 *
	 * This field contains the total size of the heap, including allocated and unallocated
	 * space.
	 */
	size_t arena;

	/**
	 * The number of ordinary free blocks.
	 *
	 * Currently, Zephyr's heap allocator only uses ordinary blocks (as opposed to fastbin
	 * blocks). Therefore, this field represents the total number of free blocks, and therefore
	 * somewhat duplicates 'fordblocks'. Note, however, that 'fordblocks' measures bytes, while
	 * 'ordblocks' measures blocks.
	 *
	 * This field is disabled by default but may be enabled for compatibility purposes using
	 * `CONFIG_COMMON_LIBC_MALLINFO2_ORDBLKS=y`.
	 */
	size_t ordblks;

	/**
	 * The number of free fastbin blocks.
	 *
	 * Currently, Zephyr's heap allocator does not use fastbin blocks, so this field is always
	 * zero.
	 *
	 * This field is disabled by default but may be enabled for compatibility purposes using
	 * `CONFIG_COMMON_LIBC_MALLINFO2_SMBLKS=y`.
	 */
	size_t smblks;

	/**
	 * The number of blocks allocated via mapped, anonymous pages.
	 *
	 * Support for this field is currently unimplemented. Once implemented, it will represent
	 * how many blocks have been allocated via Zephyr's native anonymous page-mapping APIs.
	 *
	 * This field is disabled by default but may be enabled for compatibility purposes using
	 * `CONFIG_COMMON_LIBC_MALLINFO2_MMAP=y`.
	 *
	 * @note this field is a superset of the number of blocks allocated via the optional, POSIX
	 * mmap() function.
	 */
	size_t hblks;

	/**
	 * The number of bytes in blocks allocated via mapped, anonymous pages.
	 *
	 * Support for this field is currently unimplemented. Once implemented, it will represent
	 * how many blocks have been allocated via Zephyr's native anonymous page-mapping APIs.
	 *
	 * This field is disabled by default but may be enabled for compatibility purposes using
	 * `CONFIG_COMMON_LIBC_MALLINFO2_MMAP=y`.
	 *
	 * @note this field is a superset of the number bytes allocated via the optional, POSIX
	 * mmap() function.
	 */
	size_t hblkhd;

	/**
	 * The high-watermark of memory allocated (in bytes).
	 *
	 * This field represents the maximum amount of memory that has been allocated at any point.
	 *
	 * This field is disabled by default but may be enabled for compatibility purposes using
	 * `CONFIG_COMMON_LIBC_MALLINFO2_MMAP=y`.
	 */
	size_t usmblks; /* See below */

	/**
	 * The number of bytes in free fastbin blocks.
	 *
	 * Currently, Zephyr's heap allocator does not use fastbin blocks, so this field is always
	 * zero.
	 *
	 * This field is disabled by default but may be enabled for compatibility purposes using
	 * `CONFIG_COMMON_LIBC_MALLINFO2_SMBLKS=y`.
	 */
	size_t fmsblks;

	/**
	 * The total number of bytes used by allocations.
	 *
	 * This field counts the number of bytes requested for allocation by the user.
	 *
	 * @note due to rounding, auxiliary structures, and other sources of fragmentation it is
	 * usually most often not the case that arena == fordblks + uordblks.
	 */
	size_t uordblks;

	/** The total number of bytes in free blocks */
	size_t fordblks;

	/**
	 * The total amount of releasable free space at the opt of the heap.
	 *
	 * This field is provided for compatbility purposes only, and can be expected to be equal to
	 * zero.
	 */
	size_t keepcost;
};

/**
 * @brief Obtain memory allocation information.
 *
 * This function returns a copy (snapshot) of heap statistics at a given instance in time.
 *
 * Although accessing heap information via `mallinfo2()` is thread-safe, heap statistics may
 * change immediately after this call is made. This call is mostly for application portability
 * and provides a simple means of tracking heap usage from the application.
 *
 * @note for an event-based way to trace memory usage, please see @ref heap_listener_register.
 *
 * @return a copy memory allocation information.
 */
struct mallinfo2 mallinfo2(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_LIB_LIBC_COMMON_INCLUDE_MALLOC_H_ */
