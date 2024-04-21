

#ifndef _SCHED_H
#define _SCHED_H

#define NR_TASKS 64
#define HZ 100

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]


#ifndef _HEAD_H
#define _HEAD_H

typedef struct desc_struct {
	unsigned long a,b;
} desc_table[256];

extern unsigned long pg_dir[1024];
extern desc_table idt,gdt;

#define GDT_NUL 0
#define GDT_CODE 1
#define GDT_DATA 2
#define GDT_TMP 3

#define LDT_NUL 0
#define LDT_CODE 1
#define LDT_DATA 2


#endif




#ifndef _FS_H
#define _FS_H


#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t;
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

typedef int pid_t;
typedef unsigned short uid_t;
typedef unsigned char gid_t;
typedef unsigned short dev_t;
typedef unsigned short ino_t;
typedef unsigned short mode_t;
typedef unsigned short umode_t;
typedef unsigned char nlink_t;
typedef int daddr_t;
typedef long off_t;
typedef unsigned char u_char;
typedef unsigned short ushort;

typedef struct { int quot,rem; } div_t;
typedef struct { long quot,rem; } ldiv_t;

struct ustat {
	daddr_t f_tfree;
	ino_t f_tinode;
	char f_fname[6];
	char f_fpack[6];
};

#endif




#define IS_BLOCKDEV(x) ((x)==2 || (x)==3)

#define READ 0
#define WRITE 1

void buffer_init(void);

#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff)

#define NAME_LEN 14

#define I_MAP_SLOTS 8
#define Z_MAP_SLOTS 8
#define SUPER_MAGIC 0x137F

#define NR_OPEN 20
#define NR_INODE 32
#define NR_FILE 64
#define NR_SUPER 8
#define NR_HASH 307
#define NR_BUFFERS nr_buffers
#define BLOCK_SIZE 1024
#ifndef NULL
#define NULL ((void *) 0)

#endif

#define INODES_PER_BLOCK ((BLOCK_SIZE)/(sizeof (struct d_inode)))
#define DIR_ENTRIES_PER_BLOCK ((BLOCK_SIZE)/(sizeof (struct dir_entry)))

typedef char buffer_block[BLOCK_SIZE];

struct buffer_head {
	char * b_data;			
	unsigned short b_dev;		
	unsigned short b_blocknr;	
	unsigned char b_uptodate;
	unsigned char b_dirt;		
	unsigned char b_count;		
	unsigned char b_lock;		
	struct task_struct * b_wait;
	struct buffer_head * b_prev;
	struct buffer_head * b_next;
	struct buffer_head * b_prev_free;
	struct buffer_head * b_next_free;
};

struct d_inode {
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned long i_size;
	unsigned long i_time;
	unsigned char i_gid;
	unsigned char i_nlinks;
	unsigned short i_zone[9];
};

struct m_inode {
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned long i_size;
	unsigned long i_mtime;
	unsigned char i_gid;
	unsigned char i_nlinks;
	unsigned short i_zone[9];

	struct task_struct * i_wait;
	unsigned long i_atime;
	unsigned long i_ctime;
	unsigned short i_dev;
	unsigned short i_num;
	unsigned short i_count;
	unsigned char i_lock;
	unsigned char i_dirt;
	unsigned char i_pipe;
	unsigned char i_mount;
	unsigned char i_seek;
	unsigned char i_update;
};

#define PIPE_HEAD(inode) (((long *)((inode).i_zone))[0])
#define PIPE_TAIL(inode) (((long *)((inode).i_zone))[1])
#define PIPE_SIZE(inode) ((PIPE_HEAD(inode)-PIPE_TAIL(inode))&(PAGE_SIZE-1))
#define PIPE_EMPTY(inode) (PIPE_HEAD(inode)==PIPE_TAIL(inode))
#define PIPE_FULL(inode) (PIPE_SIZE(inode)==(PAGE_SIZE-1))
#define INC_PIPE(head) __asm__("incl %0ntandl $4095,%0"::"m" (head))

struct file {
	unsigned short f_mode;
	unsigned short f_flags;
	unsigned short f_count;
	struct m_inode * f_inode;
	off_t f_pos;
};

struct super_block {
	unsigned short s_ninodes;
	unsigned short s_nzones;
	unsigned short s_imap_blocks;
	unsigned short s_zmap_blocks;
	unsigned short s_firstdatazone;
	unsigned short s_log_zone_size;
	unsigned long s_max_size;
	unsigned short s_magic;

	struct buffer_head * s_imap[8];
	struct buffer_head * s_zmap[8];
	unsigned short s_dev;
	struct m_inode * s_isup;
	struct m_inode * s_imount;
	unsigned long s_time;
	unsigned char s_rd_only;
	unsigned char s_dirt;
};

struct dir_entry {
	unsigned short inode;
	char name[NAME_LEN];
};

extern struct m_inode inode_table[NR_INODE];
extern struct file file_table[NR_FILE];
extern struct super_block super_block[NR_SUPER];
extern struct buffer_head * start_buffer;
extern int nr_buffers;

extern void truncate(struct m_inode * inode);
extern void sync_inodes(void);
extern void wait_on(struct m_inode * inode);
extern int bmap(struct m_inode * inode,int block);
extern int create_block(struct m_inode * inode,int block);
extern struct m_inode * namei(const char * pathname);
extern int open_namei(const char * pathname, int flag, int mode,
	struct m_inode ** res_inode);
extern void iput(struct m_inode * inode);
extern struct m_inode * iget(int dev,int nr);
extern struct m_inode * get_empty_inode(void);
extern struct m_inode * get_pipe_inode(void);
extern struct buffer_head * get_hash_table(int dev, int block);
extern struct buffer_head * getblk(int dev, int block);
extern void ll_rw_block(int rw, struct buffer_head * bh);
extern void brelse(struct buffer_head * buf);
extern struct buffer_head * bread(int dev,int block);
extern int new_block(int dev);
extern void free_block(int dev, int block);
extern struct m_inode * new_inode(int dev);
extern void free_inode(struct m_inode * inode);

extern void mount_root(void);

extern inline struct super_block * get_super(int dev)
{
	struct super_block * s;

	for(s = 0+super_block;s < NR_SUPER+super_block; s++)
		if (s->s_dev == dev)
			return s;
	return NULL;
}


#endif


#ifndef _MM_H
#define _MM_H

#define PAGE_SIZE 4096

extern unsigned long get_free_page(void);
extern unsigned long put_page(unsigned long page,unsigned long address);
extern void free_page(unsigned long addr);


#endif


#if (NR_OPEN > 32)
#error "Currently the close-on-exec-flags are in one word, max 32 files/proc"


#endif

#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_ZOMBIE		3
#define TASK_STOPPED		4

#ifndef NULL
#define NULL ((void *) 0)


#endif

extern int copy_page_tables(unsigned long from, unsigned long to, long size);
extern int free_page_tables(unsigned long from, long size);

extern void sched_init(void);
extern void schedule(void);
extern void trap_init(void);
extern void panic(const char * str);
extern int tty_write(unsigned minor,char * buf,int count);

typedef int (*fn_ptr)();

struct i387_struct {
	long	cwd;
	long	swd;
	long	twd;
	long	fip;
	long	fcs;
	long	foo;
	long	fos;
	long	st_space[20];	
};

struct tss_struct {
	long	back_link;	
	long	esp0;
	long	ss0;		
	long	esp1;
	long	ss1;		
	long	esp2;
	long	ss2;		
	long	cr3;
	long	eip;
	long	eflags;
	long	eax,ecx,edx,ebx;
	long	esp;
	long	ebp;
	long	esi;
	long	edi;
	long	es;		
	long	cs;		
	long	ss;		
	long	ds;		
	long	fs;		
	long	gs;		
	long	ldt;		
	long	trace_bitmap;	
	struct i387_struct i387;
};

struct task_struct {

	long state;	
	long counter;
	long priority;
	long signal;
	fn_ptr sig_restorer;
	fn_ptr sig_fn[32];

	int exit_code;
	unsigned long end_code,end_data,brk,start_stack;
	long pid,father,pgrp,session,leader;
	unsigned short uid,euid,suid;
	unsigned short gid,egid,sgid;
	long alarm;
	long utime,stime,cutime,cstime,start_time;
	unsigned short used_math;

	int tty;		
	unsigned short umask;
	struct m_inode * pwd;
	struct m_inode * root;
	unsigned long close_on_exec;
	struct file * filp[NR_OPEN];

	struct desc_struct ldt[3];

	struct tss_struct tss;
};


#define INIT_TASK 	{ 0,15,15, 	0,NULL,{(fn_ptr) 0,}, 	0,0,0,0,0, 	0,-1,0,0,0, 	0,0,0,0,0,0, 	0,0,0,0,0,0, 	0, 	-1,0133,NULL,NULL,0, 	{NULL,}, 	{ 		{0,0}, 	{0x9f,0xc0fa00}, 		{0x9f,0xc0f200}, 	}, 	{0,PAGE_SIZE+(long)&init_task,0x10,0,0,0,0,(long)&pg_dir,	 0,0,0,0,0,0,0,0, 	 0,0,0x17,0x17,0x17,0x17,0x17,0x17, 	 _LDT(0),0x80000000, 		{} 	}, }

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *last_task_used_math;
extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;

#define CURRENT_TIME (startup_time+jiffies/HZ)

extern void sleep_on(struct task_struct ** p);
extern void interruptible_sleep_on(struct task_struct ** p);
extern void wake_up(struct task_struct ** p);


#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY+1)
#define _TSS(n) ((((unsigned long) n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long) n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a" (_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a" (_LDT(n)))
#define str(n) __asm__("str %%axnt" 	"subl %2,%%eaxnt" 	"shrl $4,%%eax" 	:"=a" (n) 	:"a" (0),"i" (FIRST_TSS_ENTRY<<3))

#define switch_to(n) {struct {long a,b;} __tmp; __asm__("cmpl %%ecx,_currentnt" 	"je 1fnt" 	"xchgl %%ecx,_currentnt" 	"movw %%dx,%1nt" 	"ljmp %0nt" 	"cmpl %%ecx,%2nt" 	"jne 1fnt" 	"cltsn" 	"1:" 	::"m" (*&__tmp.a),"m" (*&__tmp.b), 	"m" (last_task_used_math),"d" _TSS(n),"c" ((long) task[n])); }

#define PAGE_ALIGN(n) (((n)+0xfff)&0xfffff000)

#define _set_base(addr,base) __asm__("movw %%dx,%0nt" 	"rorl $16,%%edxnt" 	"movb %%dl,%1nt" 	"movb %%dh,%2" 	::"m" (*((addr)+2)), 	  "m" (*((addr)+4)), 	  "m" (*((addr)+7)), 	  "d" (base) 	:"dx")

#define _set_limit(addr,limit) __asm__("movw %%dx,%0nt" 	"rorl $16,%%edxnt" 	"movb %1,%%dhnt" 	"andb $0xf0,%%dhnt" 	"orb %%dh,%%dlnt" 	"movb %%dl,%1" 	::"m" (*(addr)), 	  "m" (*((addr)+6)), 	  "d" (limit) 	:"dx")

#define set_base(ldt,base) _set_base( ((char *)&(ldt)) , base )
#define set_limit(ldt,limit) _set_limit( ((char *)&(ldt)) , (limit-1)>>12 )

#define _get_base(addr) ({unsigned long __base; __asm__("movb %3,%%dhnt" 	"movb %2,%%dlnt" 	"shll $16,%%edxnt" 	"movw %1,%%dx" 	:"=d" (__base) 	:"m" (*((addr)+2)), 	 "m" (*((addr)+4)), 	 "m" (*((addr)+7))); __base;})

#define get_base(ldt) _get_base( ((char *)&(ldt)) )

#define get_limit(segment) ({ unsigned long __limit; __asm__("lsll %1,%0ntincl %0":"=r" (__limit):"r" (segment)); __limit;})



#endif



void verify_area(void * addr,int count);
volatile void panic(const char * str);
int printf(const char * fmt, ...);
int printk(const char * fmt, ...);
int tty_write(unsigned ch,char * buf,int count);


#ifndef _SIGNAL_H
#define _SIGNAL_H


#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;

#endif

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;

#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t;

#endif

#ifndef NULL
#define NULL ((void *) 0)

#endif

typedef int pid_t;
typedef unsigned short uid_t;
typedef unsigned char gid_t;
typedef unsigned short dev_t;
typedef unsigned short ino_t;
typedef unsigned short mode_t;
typedef unsigned short umode_t;
typedef unsigned char nlink_t;
typedef int daddr_t;
typedef long off_t;
typedef unsigned char u_char;
typedef unsigned short ushort;

typedef struct { int quot,rem; } div_t;
typedef struct { long quot,rem; } ldiv_t;

struct ustat {
	daddr_t f_tfree;
	ino_t f_tinode;
	char f_fname[6];
	char f_fpack[6];
};


#endif


typedef int sig_atomic_t;
typedef unsigned int sigset_t;		

#define _NSIG             32
#define NSIG		_NSIG

#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGUNUSED	 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22


#define SA_NOCLDSTOP	1

#define SIG_BLOCK          0	
#define SIG_UNBLOCK        1	
#define SIG_SETMASK        2	

#define SIG_DFL		((void (*)(int))0)	
#define SIG_IGN		((void (*)(int))1)	

struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
	int sa_flags;
};

void (*signal(int _sig, void (*_func)(int)))(int);
int raise(int sig);
int kill(pid_t pid, int sig);
int sigaddset(sigset_t *mask, int signo);
int sigdelset(sigset_t *mask, int signo);
int sigemptyset(sigset_t *mask);
int sigfillset(sigset_t *mask);
int sigismember(sigset_t *mask, int signo); 
int sigpending(sigset_t *set);
int sigprocmask(int how, sigset_t *set, sigset_t *oldset);
int sigsuspend(sigset_t *sigmask);
int sigaction(int sig, struct sigaction *act, struct sigaction *oldact);

#endif 


extern int sys_setup();
extern int sys_exit();
extern int sys_fork();
extern int sys_read();
extern int sys_write();
extern int sys_open();
extern int sys_close();
extern int sys_waitpid();
extern int sys_creat();
extern int sys_link();
extern int sys_unlink();
extern int sys_execve();
extern int sys_chdir();
extern int sys_time();
extern int sys_mknod();
extern int sys_chmod();
extern int sys_chown();
extern int sys_break();
extern int sys_stat();
extern int sys_lseek();
extern int sys_getpid();
extern int sys_mount();
extern int sys_umount();
extern int sys_setuid();
extern int sys_getuid();
extern int sys_stime();
extern int sys_ptrace();
extern int sys_alarm();
extern int sys_fstat();
extern int sys_pause();
extern int sys_utime();
extern int sys_stty();
extern int sys_gtty();
extern int sys_access();
extern int sys_nice();
extern int sys_ftime();
extern int sys_sync();
extern int sys_kill();
extern int sys_rename();
extern int sys_mkdir();
extern int sys_rmdir();
extern int sys_dup();
extern int sys_pipe();
extern int sys_times();
extern int sys_prof();
extern int sys_brk();
extern int sys_setgid();
extern int sys_getgid();
extern int sys_signal();
extern int sys_geteuid();
extern int sys_getegid();
extern int sys_acct();
extern int sys_phys();
extern int sys_lock();
extern int sys_ioctl();
extern int sys_fcntl();
extern int sys_mpx();
extern int sys_setpgid();
extern int sys_ulimit();
extern int sys_uname();
extern int sys_umask();
extern int sys_chroot();
extern int sys_ustat();
extern int sys_dup2();
extern int sys_getppid();
extern int sys_getpgrp();
extern int sys_setsid();

fn_ptr sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
sys_write, sys_open, sys_close, sys_waitpid, sys_creat, sys_link,
sys_unlink, sys_execve, sys_chdir, sys_time, sys_mknod, sys_chmod,
sys_chown, sys_break, sys_stat, sys_lseek, sys_getpid, sys_mount,
sys_umount, sys_setuid, sys_getuid, sys_stime, sys_ptrace, sys_alarm,
sys_fstat, sys_pause, sys_utime, sys_stty, sys_gtty, sys_access,
sys_nice, sys_ftime, sys_sync, sys_kill, sys_rename, sys_mkdir,
sys_rmdir, sys_dup, sys_pipe, sys_times, sys_prof, sys_brk, sys_setgid,
sys_getgid, sys_signal, sys_geteuid, sys_getegid, sys_acct, sys_phys,
sys_lock, sys_ioctl, sys_fcntl, sys_mpx, sys_setpgid, sys_ulimit,
sys_uname, sys_umask, sys_chroot, sys_ustat, sys_dup2, sys_getppid,
sys_getpgrp,sys_setsid};


#define move_to_user_mode() __asm__ ("movl %%esp,%%eaxnt" 	"pushl $0x17nt" 	"pushl %%eaxnt" 	"pushflnt" 	"pushl $0x0fnt" 	"pushl $1fnt" 	"iretn" 	"1:tmovl $0x17,%%eaxnt" 	"movw %%ax,%%dsnt" 	"movw %%ax,%%esnt" 	"movw %%ax,%%fsnt" 	"movw %%ax,%%gs" 	:::"ax")

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)
#define nop() __asm__ ("nop"::)

#define iret() __asm__ ("iret"::)

#define _set_gate(gate_addr,type,dpl,addr) __asm__ ("movw %%dx,%%axnt" 	"movw %0,%%dxnt" 	"movl %%eax,%1nt" 	"movl %%edx,%2" 	: 	: "i" ((short) (0x8000+(dpl<<13)+(type<<8))), 	"o" (*((char *) (gate_addr))), 	"o" (*(4+(char *) (gate_addr))), 	"d" ((char *) (addr)),"a" (0x00080000))

#define set_intr_gate(n,addr) 	_set_gate(&idt[n],14,0,addr)

#define set_trap_gate(n,addr) 	_set_gate(&idt[n],15,0,addr)

#define set_system_gate(n,addr) 	_set_gate(&idt[n],15,3,addr)

#define _set_seg_desc(gate_addr,type,dpl,base,limit) {	*(gate_addr) = ((base) & 0xff000000) | 		(((base) & 0x00ff0000)>>16) | 		((limit) & 0xf0000) | 		((dpl)<<13) | 		(0x00408000) | 		((type)<<8); 	*((gate_addr)+1) = (((base) & 0x0000ffff)<<16) | 		((limit) & 0x0ffff); }

#define _set_tssldt_desc(n,addr,type) __asm__ ("movw $104,%1nt" 	"movw %%ax,%2nt" 	"rorl $16,%%eaxnt" 	"movb %%al,%3nt" 	"movb $" type ",%4nt" 	"movb $0x00,%5nt" 	"movb %%ah,%6nt" 	"rorl $16,%%eax" 	::"a" (addr), "m" (*(n)), "m" (*(n+2)), "m" (*(n+4)), 	 "m" (*(n+5)), "m" (*(n+6)), "m" (*(n+7)) 	)

#define set_tss_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x89")
#define set_ldt_desc(n,addr) _set_tssldt_desc(((char *) (n)),addr,"0x82")


#define outb(value,port) __asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))


#define inb(port) ({ unsigned char _v; __asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); _v; })

#define outb_p(value,port) __asm__ ("outb %%al,%%dxn" 		"tjmp 1fn" 		"1:tjmp 1fn" 		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ unsigned char _v; __asm__ volatile ("inb %%dx,%%aln" 	"tjmp 1fn" 	"1:tjmp 1fn" 	"1:":"=a" (_v):"d" (port)); _v; })


extern inline unsigned char get_fs_byte(const char * addr)
{
	unsigned register char _v;

	__asm__ ("movb %%fs:%1,%0":"=r" (_v):"m" (*addr));
	return _v;
}

extern inline unsigned short get_fs_word(const unsigned short *addr)
{
	unsigned short _v;

	__asm__ ("movw %%fs:%1,%0":"=r" (_v):"m" (*addr));
	return _v;
}

extern inline unsigned long get_fs_long(const unsigned long *addr)
{
	unsigned long _v;

	__asm__ ("movl %%fs:%1,%0":"=r" (_v):"m" (*addr)); \
	return _v;
}

extern inline void put_fs_byte(char val,char *addr)
{
__asm__ ("movb %0,%%fs:%1"::"r" (val),"m" (*addr));
}

extern inline void put_fs_word(short val,short * addr)
{
__asm__ ("movw %0,%%fs:%1"::"r" (val),"m" (*addr));
}

extern inline void put_fs_long(unsigned long val,unsigned long * addr)
{
__asm__ ("movl %0,%%fs:%1"::"r" (val),"m" (*addr));
}


#define LATCH (1193180/HZ)

extern void mem_use(void);

extern int timer_interrupt(void);
extern int system_call(void);

union task_union {
	struct task_struct task;
	char stack[PAGE_SIZE];
};

static union task_union init_task = {INIT_TASK,};

long volatile jiffies=0;
long startup_time=0;
struct task_struct *current = &(init_task.task), *last_task_used_math = NULL;

struct task_struct * task[NR_TASKS] = {&(init_task.task), };

long user_stack [ PAGE_SIZE>>2 ] ;

struct {
	long * a;
	short b;
	} stack_start = { & user_stack [PAGE_SIZE>>2] , 0x10 };

void math_state_restore()
{
	if (last_task_used_math)
		__asm__("fnsave %0"::"m" (last_task_used_math->tss.i387));
	if (current->used_math)
		__asm__("frstor %0"::"m" (current->tss.i387));
	else {
		__asm__("fninit"::);
		current->used_math=1;
	}
	last_task_used_math=current;
}


void schedule(void)
{
	int i,next,c;
	struct task_struct ** p;



	for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
		if (*p) {
			if ((*p)->alarm && (*p)->alarm < jiffies) {
					(*p)->signal |= (1<<(SIGALRM-1));
					(*p)->alarm = 0;
				}
			if ((*p)->signal && (*p)->state==TASK_INTERRUPTIBLE)
				(*p)->state=TASK_RUNNING;
		}



	while (1) {
		c = -1;
		next = 0;
		i = NR_TASKS;
		p = &task[NR_TASKS];
		while (--i) {
			if (!*--p)
				continue;
			if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
				c = (*p)->counter, next = i;
		}
		if (c) break;
		for(p = &LAST_TASK ; p > &FIRST_TASK ; --p)
			if (*p)
				(*p)->counter = ((*p)->counter >> 1) +
						(*p)->priority;
	}
	switch_to(next);
}

int sys_pause(void)
{
	current->state = TASK_INTERRUPTIBLE;
	schedule();
	return 0;
}

void sleep_on(struct task_struct **p)
{
	struct task_struct *tmp;

	if (!p)
		return;
	if (current == &(init_task.task))
		panic("task[0] trying to sleep");
	tmp = *p;
	*p = current;
	current->state = TASK_UNINTERRUPTIBLE;
	schedule();
	if (tmp)
		tmp->state=0;
}

void interruptible_sleep_on(struct task_struct **p)
{
	struct task_struct *tmp;

	if (!p)
		return;
	if (current == &(init_task.task))
		panic("task[0] trying to sleep");
	tmp=*p;
	*p=current;
repeat:	current->state = TASK_INTERRUPTIBLE;
	schedule();
	if (*p && *p != current) {
		(**p).state=0;
		goto repeat;
	}
	*p=NULL;
	if (tmp)
		tmp->state=0;
}

void wake_up(struct task_struct **p)
{
	if (p && *p) {
		(**p).state=0;
		*p=NULL;
	}
}

void do_timer(long cpl)
{
	if (cpl)
		current->utime++;
	else
		current->stime++;
	if ((--current->counter)>0) return;
	current->counter=0;
	if (!cpl) return;
	schedule();
}

int sys_alarm(long seconds)
{
	current->alarm = (seconds>0)?(jiffies+HZ*seconds):0;
	return seconds;
}

int sys_getpid(void)
{
	return current->pid;
}

int sys_getppid(void)
{
	return current->father;
}

int sys_getuid(void)
{
	return current->uid;
}

int sys_geteuid(void)
{
	return current->euid;
}

int sys_getgid(void)
{
	return current->gid;
}

int sys_getegid(void)
{
	return current->egid;
}

int sys_nice(long increment)
{
	if (current->priority-increment>0)
		current->priority -= increment;
	return 0;
}

int sys_signal(long signal,long addr,long restorer)
{
	long i;

	switch (signal) {
		case SIGHUP: case SIGINT: case SIGQUIT: case SIGILL:
		case SIGTRAP: case SIGABRT: case SIGFPE: case SIGUSR1:
		case SIGSEGV: case SIGUSR2: case SIGPIPE: case SIGALRM:
		case SIGCHLD:
			i=(long) current->sig_fn[signal-1];
			current->sig_fn[signal-1] = (fn_ptr) addr;
			current->sig_restorer = (fn_ptr) restorer;
			return i;
		default: return -1;
	}
}

void sched_init(void)
{
	int i;
	struct desc_struct * p;

	set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
	set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));
	p = gdt+2+FIRST_TSS_ENTRY;
	for(i=1;i<NR_TASKS;i++) {
		task[i] = NULL;
		p->a=p->b=0;
		p++;
		p->a=p->b=0;
		p++;
	}
	ltr(0);
	lldt(0);
	outb_p(0x36,0x43);		
	outb_p(LATCH & 0xff , 0x40);	
	outb(LATCH >> 8 , 0x40);	
	set_intr_gate(0x20,&timer_interrupt);
	outb(inb_p(0x21)&~0x01,0x21);
	set_system_gate(0x80,&system_call);
}
