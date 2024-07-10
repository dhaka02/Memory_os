#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define FLAGS MAP_ANONYMOUS | MAP_PRIVATE

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE  4096

void *head;
struct sub_chain {
    void* start_address;
    size_t size;
    int is_process; // 0 for HOLE, 1 for PROCESS
    struct sub_chain* next;
    struct sub_chain* prev;
};

// Structure for a node in the MeMS main chain
struct MainChainNode {
    int start;
    int finish;
    size_t pages;
    struct sub_chain* sub_chain_head;
    struct MainChainNode* next;
};

// Global variables
struct sub_chain* sub_list = NULL;
struct MainChainNode* main_chain = NULL;
void* mems_heap_start = NULL;
int* main_size =  NULL;
void* sub_size;
long int* ptr[10000][2];
int i =0;
int pag =0;
int p ;
int ad=0;

/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/

void mems_init() {
    sub_list = NULL;
    main_chain = NULL;
    int *mem = (int *)mmap(NULL, 1024 * 1024 * sizeof(struct MainChainNode), PROT_READ | PROT_WRITE, FLAGS, -1, 0);
    // main_size = (void *)mem;
    main_size = mem;
    for(int k =0;k<10000;k++){
        ptr[i][0] = -1;
        ptr[i][0] = -1;
    }
    p = 0;


}

/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_finish(){
    struct MainChainNode* temp = main_chain;
    while(temp != NULL){
        struct sub_chain* sub_main = temp->sub_chain_head;
        if (munmap(sub_main->start_address, PAGE_SIZE) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
        printf("Memory released using munmap.\n");
        temp = temp->next;
    }
    printf("Memory released using munmap.\n");
    main_chain = NULL;

}


void* mems_malloc(size_t size){
    // p +=size;
    if(main_chain == NULL){
        int *mem = (int *)mmap(NULL, size, PROT_READ | PROT_WRITE, FLAGS, -1, 0);
        p = 0;
        main_chain = (struct MainChainNode *)main_size;
        main_size += sizeof(struct MainChainNode);
        
        int value = size / PAGE_SIZE;
        if (PAGE_SIZE % size) value++;
        pag =ad;
        ad +=value;

        main_chain->pages = value;
        main_chain->next = NULL;
        main_chain->start = ((pag)*PAGE_SIZE); 
        main_chain->finish = ((ad)*PAGE_SIZE) ; 
        main_chain->sub_chain_head = (struct sub_chain *)main_size;
        main_size += sizeof(struct sub_chain);

        struct sub_chain* sub_main = main_chain->sub_chain_head;
        sub_main->is_process = 1;
        sub_main->prev = NULL;
        sub_main->start_address = mem;
        sub_main->size = size;

        struct sub_chain* sub_main_hole = (struct sub_chain *)main_size;
        sub_main->next = (struct sub_chain *)sub_main_hole;

        main_size += sizeof(struct sub_chain);
        sub_main_hole->is_process = 0;
        sub_main_hole->prev = sub_main;
        sub_main_hole->start_address = mem + size;
        sub_main_hole->size = (value * PAGE_SIZE) - size;
        sub_main_hole->next = NULL;
        ptr[i][0] = mem;
        ptr[i][1] = (pag*PAGE_SIZE) + p;
        i++;
        p +=size;
        return ptr[i-1][1];
        // return sub_main->start_address;
    }
    else{
        int have = 0;
        struct MainChainNode* temp = main_chain;
        while(temp != NULL){
            struct sub_chain* sub_main = (struct sub_chain *)temp->sub_chain_head;
            while(sub_main != NULL){
                if(sub_main->is_process == 0){
                    if(sub_main->size >= size){
                        if(sub_main->size != size){
                        struct sub_chain* new_sub = (struct sub_chain *)main_size;
                        main_size +=sizeof(struct sub_chain);
                        sub_main->is_process = 1;
                        new_sub->is_process = 0;
                        new_sub->next = sub_main->next;
                        struct sub_chain* comp = (struct sub_chain *)main_size;
                        comp->prev = new_sub;
                        sub_main->next = new_sub;
                        new_sub->size = sub_main->size - size;
                        sub_main->size = size;
                        new_sub->start_address = sub_main->start_address + size;
                        have = 1;
                        int has = 0;
                        for(int j =0;j<10000;j++){
                            if(ptr[j][0] ==sub_main->start_address ){
                                has = 1;
                                return ptr[j][1];
                            }
                        }
                        if(has == 0){
                        ptr[i][0] = sub_main->start_address;
                        ptr[i][1] = (pag * PAGE_SIZE) + p ;
                        i++;
                        p +=size;
                        return ptr[i-1][1];
                        }
                        // return sub_main->start_address;

                        break;
                        }
                        else{
                            sub_main->is_process=1;
                            have = 1;
                            // return sub_main->start_address;
                            // ptr[i][0] = sub_main->start_address;
                            // ptr[i][1] = p + (pag*PAGE_SIZE) ;
                            // i++;
                            // p +=size;
                            // return ptr[i-1][1];
                            for(int j =0;j<10000;j++){
                            if(ptr[j][0] ==sub_main->start_address ){
                                return ptr[j][1];
                            }
                            }
                            break;
                        }
                    }
                }
                sub_main = sub_main->next;
            }
            if(have)break;
            temp = temp->next;
        }
        if (have == 0){
            char *mem = (char *)mmap(NULL, 20, PROT_READ | PROT_WRITE, FLAGS, -1, 0);
            p = 0;
            struct MainChainNode* new_main = (struct MainChainNode *)main_size;
            struct MainChainNode* find_main = main_chain;
            while(find_main->next != NULL){
                find_main = find_main->next;
            }
            find_main->next = new_main;
            main_size += sizeof(struct MainChainNode);
            
            int value = size / PAGE_SIZE;
            if (PAGE_SIZE % size) value++;
            pag =ad;
            ad +=value;

            new_main->pages = value;
            new_main->next = NULL;
            new_main->start = ((pag)*PAGE_SIZE);
            new_main->finish = ((ad)*PAGE_SIZE);
            new_main->sub_chain_head = (struct sub_chain *)main_size;
            main_size += sizeof(struct sub_chain);

            struct sub_chain* sub_main = new_main->sub_chain_head;
            sub_main->is_process = 1;
            sub_main->prev = NULL;
            sub_main->start_address = mem;
            sub_main->size = size;
    
            struct sub_chain* sub_main_hole = (struct sub_chain *)main_size;
            sub_main->next = sub_main_hole;

            main_size += sizeof(struct sub_chain);
            sub_main_hole->is_process = 0;
            sub_main_hole->prev = sub_main;
            sub_main_hole->start_address = mem + size;
            sub_main_hole->size = (value * PAGE_SIZE) - size;
            sub_main_hole->next = NULL;

            ptr[i][0] = sub_main->start_address;
            ptr[i][1] = (pag * PAGE_SIZE) + p ;
            i++;
            p +=size;
            // return mem;
            return ptr[i-1][1];
        }
    }
    return NULL;
}



/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
void mems_print_stats(){
    printf("\n****************************************************************************************************************\n");
    struct MainChainNode* temp = main_chain;
    int page = 0,ans = 0,used=0,nused = 0;
        while(temp != NULL){
            printf("\n---------------Pages : %d---------------\n",temp->pages);
            printf("---------------start : %d---------------\n",temp->start);
            printf("---------------end : %d---------------\n",temp->finish);
            page += temp->pages;
            ans++;
            int subin = 0;
            struct sub_chain* sub_main = temp->sub_chain_head;
            int in = temp->start;
            while(sub_main != NULL){
                subin++;
                printf("|| process type: %d, process size: %zu|| start : %d, end : %lu ||  \n",sub_main->is_process,sub_main->size,in,in+sub_main->size);
                in +=sub_main->size;
                if(sub_main->is_process==1)used +=sub_main->size;
                else nused += sub_main->size;
                sub_main = sub_main->next;
                }
                printf("----SUB_CHAIN NUMBER: %d---------\n",subin);
            temp = temp->next;
        }
        printf("pages: %d\n",page);
        printf("main: %d\n",ans);
        printf("Used Space : %d\n",used);
        printf("Unused Space : %d\n",nused);
    printf("\n****************************************************************************************************************\n");

}


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/
void *mems_get(void*v_ptr){
    // for(int j =0;j<10000;j++){
    //     if(ptr[j][1] == v_ptr){
    //         return ptr[j][0];
    //     }
    // }
    if(*(int*)v_ptr <0){
        return -111;
    }
    for(int j =0;j<10000;j++){
        if(ptr[j][1] <= v_ptr && (v_ptr <= ptr[j+1][1] || ptr[j+1][1])){
            return ptr[j][0] + ((long int*)v_ptr - ptr[j][1]);
        }
    }
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void *v_ptr){
    for(int j = 0;j<10000;j++){
        if(ptr[j][1] == v_ptr ){
            v_ptr = ptr[j][0];
            break;
        }
    }
    struct MainChainNode* present = main_chain;
    while(present != NULL){
        int stop = 0;
            struct sub_chain* sub_finding = present->sub_chain_head;
            while(sub_finding != NULL){
                if(sub_finding->start_address == v_ptr){
                    sub_finding->is_process=0;
                    stop = 1;
                    break;
                }
                sub_finding = sub_finding->next;
                }
                if(stop)break;
        present = present->next;
    }
    struct MainChainNode* temp = main_chain;
        while(temp != NULL){
            struct sub_chain* sub_main = temp->sub_chain_head;
            while(sub_main != NULL){
                if(sub_main->is_process == 0){
                    if(sub_main->next != NULL){
                        struct sub_chain* comp = sub_main->next;
                        if(comp->is_process == 0){
                            sub_main->size += comp->size;
                            sub_main->next = comp->next;
                            if(sub_main->next !=NULL){
                                struct sub_chain* has = comp->next;
                                has->prev = comp->prev;
                            }
                        }
                    }
                }
                sub_main = sub_main->next;
            }
            temp = temp->next;
        }
    *(int*)v_ptr = -12134534; 
}