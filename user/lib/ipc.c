// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

u_int get_time(u_int *us) {
	u_int tmp = 0;
	u_int sum = 0;
	u_int n = 0;
	syscall_read_dev(&tmp, 0x15000000, 4);
	syscall_read_dev(&sum, 0x15000010, 4);
	syscall_read_dev(&n, 0x15000020, 4);
	*us = n;
	return sum;
}

void usleep(u_int us) {
	// 读取进程进入 usleep 函数的时间
	u_int start;
	u_int start_u;
	u_int tmp;
	u_int cur;
	u_int cur_u;
	start = get_time(&start_u);
	while (1) {
		// 读取当前时间
		cur = get_time(&cur_u);
		if (((int)cur - (int)start) * 100000 >= ((int)start_u + (int)us - (int)cur_u)/* 当前时间 >= 进入时间 + us 微秒*/) {
			return;
		} else {
			syscall_yield();
			// 进程切换
		}
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
