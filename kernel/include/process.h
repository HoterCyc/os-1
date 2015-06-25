#ifndef _PROCESS_H
#define _PROCESS_H

#include <system.h>
#include <vfs.h>
#include <sys/types.h>
#include <signal.h>
#include <fpu.h>

#define USER_STACK		0x7FC0000000
#define USER_STACK_SIZE	0x8000

typedef struct
{
	uint64_t 
		r15, r14, r13, r12, r11, r10, r9, r8, 
		rdi, rsi, rbp, rbx, rcx, rdx, rax,
		rip, rflags, rsp;
} __attribute__((packed)) stat_t;

typedef struct
{
	inode_t *inode;
	uint32_t offset;
}process_file_t;

typedef struct
{
	uint32_t len;
	uint32_t max_len;
	process_file_t *ent;
} file_list_t;

typedef enum
{
	READY,
	WAITING,
	//WAITING_CHILD,
}process_status_t;

typedef struct signal_queue_t
{
	signal_num_t signum;
	struct signal_queue_t *next;
}signal_queue_t;

typedef struct process_struct process_t;
typedef struct process_struct
{
	uint8_t 	*name;
	uint32_t 	pid;
	file_list_t fds;
	uint64_t 	pdpt;
	uint64_t	size;	// Actuall allocated memory
	uint64_t	heap;
	process_t   *parent;
	stat_t		stat;
	void		*fstat;	// FPU state if avilable
	process_status_t status;
	
	struct sigaction handlers[21];
	signal_queue_t *sigqueue;
	
	uint32_t	ticks;
	uint32_t	sub_ticks;
	uint32_t	wait_us;
	
	// used for procsses queue
	process_t 	*prev;
	process_t 	*next;
} __attribute__((packed)) process_t;

extern process_t *current_process;
extern void switch_context(stat_t*);

process_t *load_elf(char*);
void exit_process(process_t*);

uint32_t get_fd(process_t*);
process_t *process_by_pid(pid_t pid);

void signal_raise(process_t *p, signal_num_t signal);
uint32_t validate(process_t *, void *);
#endif
