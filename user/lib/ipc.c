// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

void barrier_alloc(int n) {
	syscall_set_barrier(n);	
	syscall_set_sum(0);
	syscall_set_f(1);
}

void barrier_wait(void) {
	int f;
	f = syscall_get_f();
//	int sum = syscall_get_sum();
	if (f==1) {
//		sum++;
		syscall_set_sum(1);
	//	debugf("@@@@@@@%d\n", syscall_get_barrier());
//		debugf("@@%d\n", syscall_get_sum());
		while (syscall_get_sum() != syscall_get_barrier()) {
//			debugf("@@%d\n", syscall_get_sum());
			syscall_yield();
		}
		syscall_set_f(0);
	}
	
}

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
