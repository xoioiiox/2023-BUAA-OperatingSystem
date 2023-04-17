#include <drivers/dev_mp.h>
#include <mmu.h>
#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	u_long va;
	u_long *pa;
	va = tf->cp0_epc; //fault va
	Pte *ppte;
	struct Page *pp;
	curenv->env_ov_cnt++;
	pp = page_lookup(curenv->env_pgdir, va, &ppte);
	pa = page2pa(pp) + (va & 0xfff);
	//pgdir_walk(curenv->env_pgdir, va, 0, &ppte);
	//pa = ((*ppte) & 0xfffff) | (va & 0xfff);
	if (((*pa) & 0xf0000000) == 0) {
		if (((*pa) & 0xf) == 0) {
			*pa = (*pa) | 1;
			printk("add ov handled\n");
		}
		else {
			*pa = (*pa) | 1;
			printk("sub ov handled\n");
		}
	}
	else {
		u_int t_n;
		u_int s_n;
		u_int imm;
		u_int t;
		u_int s;
		t = (*pa) & 0x1f0000;
		s = (*pa) & 0x3e00000;
		t_n = tf->regs[t];
		s_n = tf->regs[s];
		imm = (*pa) & 0xffff;
		tf->regs[t] = s_n / 2 + imm / 2;
		tf->cp0_epc += 4;
		printk("addi ov handled\n");
	}
}
