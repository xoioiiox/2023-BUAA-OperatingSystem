#ifndef SYSCALL_H
#define SYSCALL_H

#ifndef __ASSEMBLER__

enum {
	SYS_putchar,
	SYS_print_cons,
	SYS_getenvid,
	SYS_yield,
	SYS_env_destroy,
	SYS_set_tlb_mod_entry,
	SYS_mem_alloc,
	SYS_mem_map,
	SYS_mem_unmap,
	SYS_exofork,
	SYS_set_env_status,
	SYS_set_trapframe,
	SYS_panic,
	SYS_ipc_try_send,
	SYS_ipc_recv,
	SYS_cgetc,
	SYS_write_dev,
	SYS_read_dev,
	SYS_create_shellid,
	SYS_declare_value,
	SYS_unset_value,
	SYS_get_value,
	SYS_print_value,
	MAX_SYSNO,
};

#endif

#endif
