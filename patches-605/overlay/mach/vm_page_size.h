/*
 * mach/vm_page_size.h — shim for the MacOSX 10.6 SDK.
 *
 * The 10.6 SDK does not ship <mach/vm_page_size.h>; it was added in 10.9.
 * 605's bmalloc/bmalloc/VMAllocate.h:39 does `#include <mach/vm_page_size.h>`
 * under #if BOS(DARWIN) and uses vm_kernel_page_size (VMAllocate.h:96).
 *
 * vm_page_size / vm_page_mask / vm_page_shift are provided at runtime by the
 * 10.6 kernel (declared in <mach/mach_init.h>, already in the SDK); only the
 * declarations are repeated here for completeness.
 *
 * vm_kernel_page_size / vm_kernel_page_mask / vm_kernel_page_shift are 10.9+
 * runtime symbols; they are DEFINED in sdk_stubs_605.mm as 4096 / 0xFFF / 12
 * (the x86_64 kernel page size), so bmalloc::pageSize() returns the right value.
 */
#ifndef _VM_PAGE_SIZE_H_
#define _VM_PAGE_SIZE_H_

#include <mach/mach_types.h>

__BEGIN_DECLS

extern vm_size_t vm_page_size;
extern vm_size_t vm_page_mask;
extern int vm_page_shift;

#define trunc_page(x)         ((x) & (~(vm_page_size - 1)))
#define round_page(x)         trunc_page((x) + (vm_page_size - 1))
#define mach_vm_trunc_page(x) ((mach_vm_offset_t)(x) & ~((signed)vm_page_mask))
#define mach_vm_round_page(x) (((mach_vm_offset_t)(x) + vm_page_mask) & ~((signed)vm_page_mask))

extern vm_size_t vm_kernel_page_size;
extern vm_size_t vm_kernel_page_mask;
extern int vm_kernel_page_shift;

#define trunc_page_kernel(x) ((x) & (~vm_kernel_page_mask))
#define round_page_kernel(x) trunc_page_kernel((x) + vm_kernel_page_mask)

__END_DECLS

#endif /* _VM_PAGE_SIZE_H_ */
