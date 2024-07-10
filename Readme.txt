void mems_init():
Calling MMAP for linked list and for using for future cases by moving a pointer named main_size and adding sizeof(struct sub_chain) and sizeof(struct MainChainNode)
void* mems_malloc(size_t size) For first time I am calling mmap since there is no space and then after that checking free list 
I am using  2d array named ptr for storing virtual address and physical address 
Mems free getting virtual address checking in ed array and then removing it from it linked list 
void mems_print_stats(): for printing free list which storing all data 
// 1 is for Process
// 0 is for HOLE
void *mems_get(void*v_ptr):again receiving virtual address and returning physical address from 2d array
void mems_finish(): unmapping all mmap call that are stored in 1st subchain part for main chain part


