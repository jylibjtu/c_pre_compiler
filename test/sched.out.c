






typedef struct desc_struct {
	unsigned long a,b;
} desc_table[256];

extern unsigned long pg_dir[1024];
extern desc_table idt,gdt;









typedef unsigned int size_t;
typedef long time_t;
typedef long ptrdiff_t;
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






void buffer_init(void);






typedef char buffer_block[1024];

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
	char name[14];
};

extern struct m_inode inode_table[32];
extern struct file file_table[64];
extern struct super_block super_block[8];
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

	for(s = 0+super_block;s < 8+super_block; s++)
		if (s->s_dev == dev)
			return s;
	return ((void *) 0);
}





extern unsigned long get_free_page(void);
extern unsigned long put_page(unsigned long page,unsigned long address);
extern void free_page(unsigned long addr);








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
	struct file * filp[20];

	struct desc_struct ldt[3];

	struct tss_struct tss;
};



extern struct task_struct *task[64];
extern struct task_struct *last_task_used_math;
extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;


extern void sleep_on(struct task_struct ** p);
extern void interruptible_sleep_on(struct task_struct ** p);
extern void wake_up(struct task_struct ** p);















void verify_area(void * addr,int count);
volatile void panic(const char * );
strint printf(const char * fmt, ...);
int printk(const char * fmt, ...);
int tty_write(unsigned ch,char * buf,int count);





typedef unsigned int size_t;

typedef long time_t;

typedef long ptrdiff_t;


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



typedef int sig_atomic_t;
typedef unsigned int sigset_t;		







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



extern void mem_use(void);

extern int timer_interrupt(void);
extern int system_call(void);

union task_union {
	struct task_struct task;
	char stack[4096];
};

static union task_union init_task = {	{ 0,15,15, 	0,((void *) 0),{(fn_ptr) 0,}, 	0,0,0,0,0, 	0,-1,0,0,0, 	0,0,0,0,0,0, 	0,0,0,0,0,0, 	0, 	-1,0133,((void *) 0),((void *) 0),0, 	{((void *) 0),}, 	{ 		{0,0}, 	{0x9f,0xc0fa00}, 		{0x9f,0xc0f200}, 	}, 	{0,4096+(long)&init_task,0x10,0,0,0,0,(long)&pg_dir,	 0,0,0,0,0,0,0,0, 	 0,0,0x17,0x17,0x17,0x17,0x17,0x17, 	  ((((unsigned long) 0)<<4)+((4+1)<<3))),0x80000000, 		{} 	}, },};

long volatile jiffies=0;
long startup_time=0;
struct task_struct *current = &(init_task.task), *last_task_used_math = ((void *) 0);

struct task_struct * task[64] = {&(init_task.task), };

long user_stack [ 4096>>2 ] ;

struct {
	long * a;
	short b;
	} stack_start = { & user_stack [4096>>2] , 0x10 };

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



	for(p = &task[64-1] ; p > &task[0] ; --p)
		if (*p) {
			if ((*p)->alarm && (*p)->alarm < jiffies) {
					(*p)->signal |= (1<<(	14-1));
					(*p)->alarm = 0;
				}
			if ((*p)->signal && (*p)->state==1)
				(*p)->state=	0;
		}



	while (1) {
		c = -1;
		next = 0;
		i = 64;
		p = &task[64];
		while (--i) {
			if (!*--p)
				continue;
			if ((*p)->state == 	0 && (*p)->counter > c)
				c = (*p)->counter, next = i;
		}
		if (c) break;
		for(p = &task[64-1] ; p > &task[0] ; --p)
			if (*p)
				(*p)->counter = ((*p)->counter >> 1) +
						(*p)->priority;
	}
	 {struct {long a,b;} __tmp; __asm__("cmpl %%ecx,_currentnt" 	"je 1fnt" 	"xchgl %%ecx,_currentnt" 	"movw %%dx,%1nt" 	"ljmp %0nt" 	"cmpl %%ecx,%2nt" 	"jne 1fnt" 	"cltsn" 	"1:" 	::"m" (*&__tmp.a),"m" (*&__tmp.b), 	"m" (last_task_used_math),"d"  ((((unsigned long) next)<<4)+(4<<3))),"c" ((long) task[next])); });
}

int sys_pause(void)
{
	current->state = 1;
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
	current->state = 2;
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
repeat:	current->state = 1;
	schedule();
	if (*p && *p != current) {
		(**p).state=0;
		goto repeat;
	}
	*p=((void *) 0);
	if (tmp)
		tmp->state=0;
}

void wake_up(struct task_struct **p)
{
	if (p && *p) {
		(**p).state=0;
		*p=((void *) 0);
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
	current->alarm = (seconds>0)?(jiffies+100*seconds):0;
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
		case 	 1: case 	 2: case 	 3: case 	 4:
		case 	 5: case 	 6: case 	 8: case 	10:
		case 	11: case 	12: case 	13: case 	14:
		case 	17:
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

	  __asm__ ("movw $104,%1nt" 	"movw %%ax,%2nt" 	"rorl $16,%%eaxnt" 	"movb %%al,%3nt" 	"movb $" ,"0x89" ",%4nt" 	"movb $0x00,%5nt" 	"movb %%ah,%6nt" 	"rorl $16,%%eax" 	::"a" (,,&(init_task.task.tss)), "m" (*(((char *) (gdt+4)))), "m" (*(((char *) (gdt+4))+2)), "m" (*(((char *) (gdt+4))+4)), 	 "m" (*(((char *) (gdt+4))+5)), "m" (*(((char *) (gdt+4))+6)), "m" (*(((char *) (gdt+4))+7)) 	)));
	  __asm__ ("movw $104,%1nt" 	"movw %%ax,%2nt" 	"rorl $16,%%eaxnt" 	"movb %%al,%3nt" 	"movb $" ,"0x82" ",%4nt" 	"movb $0x00,%5nt" 	"movb %%ah,%6nt" 	"rorl $16,%%eax" 	::"a" (,,&(init_task.task.ldt)), "m" (*(((char *) (gdt+(4+1))))), "m" (*(((char *) (gdt+(4+1)))+2)), "m" (*(((char *) (gdt+(4+1)))+4)), 	 "m" (*(((char *) (gdt+(4+1)))+5)), "m" (*(((char *) (gdt+(4+1)))+6)), "m" (*(((char *) (gdt+(4+1)))+7)) 	)));
	p = gdt+2+4;
	for(i=1;i<64;i++) {
		task[i] = ((void *) 0);
		p->a=p->b=0;
		p++;
		p->a=p->b=0;
		p++;
	}
	 __asm__(" %%ltrax"::"a" ( ((((unsigned long) 0)<<4)+(4<<3))))));
	 __asm__(" %%lldtax"::"a" ( ((((unsigned long) 0)<<4)+((4+1)<<3))))));
	 __asm__ (" %%outbal,%%dxn" 		"tjmp 1fn" 		"1:tjmp 1fn" 		"1:"::"a" (0x36),"d" (,0x43)));		
	 __asm__ (" %%outbal,%%dxn" 		"tjmp 1fn" 		"1:tjmp 1fn" 		"1:"::"a" ((1193180/100) & 0xff ),"d" (, 0x40)));	
	 __asm__ (" %%outbal,%%dx"::"a" ((1193180/100) >> 8 ),"d" (, 0x40)));	
	 	 __asm__ ("movw %%dx,%%axnt" 	"movw %0,%%dxnt" 	"movl %%eax,%1nt" 	"movl %%edx,%2" 	: 	: "i" ((short) (0x8000+(,0<<13)+(,14<<8))), 	"o" (*((char *) (&idt[0x20]))), 	"o" (*(4+(char *) (&idt[0x20]))), 	"d" ((char *) (,,&timer_interrupt)),"a" (0x00080000))));
	 __asm__ (" %%outbal,%%dx"::"a" ( ({ unsigned char _v; __asm__ volatile (" %%inbdx,%%aln" 	"tjmp 1fn" 	"1:tjmp 1fn" 	"1:":"=a" (_v):"d" (0x21)); _v; }))&~0x01),"d" (,0x21)));
	 	 __asm__ ("movw %%dx,%%axnt" 	"movw %0,%%dxnt" 	"movl %%eax,%1nt" 	"movl %%edx,%2" 	: 	: "i" ((short) (0x8000+(,3<<13)+(,15<<8))), 	"o" (*((char *) (&idt[0x80]))), 	"o" (*(4+(char *) (&idt[0x80]))), 	"d" ((char *) (,,&system_call)),"a" (0x00080000))));
}
