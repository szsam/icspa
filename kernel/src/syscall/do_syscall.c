#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
uint32_t mm_brk(uint32_t);
int fs_ioctl(int, uint32_t, void *);
void serial_printc(char);

// file system
int fs_open(const char *pathname, int flags);
int fs_read(int fd, void *buf, int len);
int fs_write(int fd, const void *buf, int len);
int fs_lseek(int fd, int offset, int whence);
int fs_close(int fd);

static void sys_brk(TrapFrame *tf) {
	tf->eax = mm_brk(tf->ebx);
}

static void sys_ioctl(TrapFrame *tf) {
	tf->eax = fs_ioctl(tf->ebx, tf->ecx, (void *)tf->edx);
}

static ssize_t sys_write(int fd, const void *buf, size_t len) {
//	asm volatile (".byte 0xd6" : : "a"(2), "c"(buf), "d"(len));
	if (fd == 1 || fd == 2) {	// stdout or stderr
		const char *pbuf = buf;
		for (size_t i = 0; i < len; ++i)
			serial_printc(pbuf[i]);
		return len;
	}
	else {
		return fs_write(fd, buf, len);
	}
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The `add_irq_handle' system call is artificial. We use it to
		 * let user program register its interrupt handlers. But this is
		 * very dangerous in a real operating system. Therefore such a
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;
		case SYS_ioctl: sys_ioctl(tf); break;

		/* TODO: Add more system calls. */
		case SYS_write: 
			tf->eax = sys_write(tf->ebx, (void *)tf->ecx, tf->edx); 
			break;
		case SYS_open:
			tf->eax = fs_open((const char *)tf->ebx, tf->ecx);
			break;
		case SYS_read:
			tf->eax = fs_read(tf->ebx, (void *)tf->ecx, tf->edx);
			break;
		case SYS_lseek:
			tf->eax = fs_lseek(tf->ebx, tf->ecx, tf->edx);
			break;
		case SYS_close:
			tf->eax = fs_close(tf->ebx);
			break;

		default: panic("Unhandled system call: id = %d, eip = 0x%08x", tf->eax, tf->eip);
	}
}

