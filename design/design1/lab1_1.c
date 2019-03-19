#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#define buf_size 4096
#define buf_count 5

int shmt[buf_count];
int semid;
pid_t p_write, p_read;
FILE *fp_r;
FILE *fp_w;
struct shmid_ds share_buf;


static long long get_file_size(const char* file) {
    struct stat tbuf;
    stat(file, &tbuf);
    return tbuf.st_size;
}


void P(int semid, int index){
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void V(int semid, int index){
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};


struct buffer{
    char ptr[buf_size];
    int file_finish;
    unsigned long file_size;
};

int main(int argc, char *argv[]) {
    
    struct buffer *buf[buf_count];
    
    char *input_file = argv[1];
    char *output_file = argv[2];

    fp_r = fopen(input_file, "r");
    fp_w = fopen(output_file, "w");

    long long file_r_size = get_file_size(input_file);
    long long file_w_size = 0;
    float progress;
    
    union semun mysemun_w;
    union semun mysemun_r;
    
    mysemun_w.val = buf_count;
    mysemun_r.val = 0;
    
    for(int i = 0; i < buf_count; i++)
    {
        shmt[i] = shmget(IPC_PRIVATE, buf_size * sizeof(char) + sizeof(int) + sizeof(unsigned long), IPC_CREAT|0666);//缓冲区大小4096
        if (shmt[i] < 0)
        {
            printf("get shm ipc_id error\n") ;
            return -1 ;
        }
    }
    
    
    semid = semget(IPC_PRIVATE , 2, IPC_CREAT|0666);
    if(semid == -1)
    {
        printf("Create semid failed\n");
        exit(1);
    }
    
    semctl(semid, 0, SETVAL, mysemun_w);
    semctl(semid, 1, SETVAL, mysemun_r);
    

    if((p_write = fork()) == -1)
    {
        printf("Create process failed");
        exit(1);
    }
    else if(p_write == 0)
    {
        for(int i = 0; i < buf_count; i++)
        {
            buf[i] = shmat(shmt[i], NULL, SHM_R|SHM_W);
            if((int)buf[i]->ptr == -1)
            {
                printf("shmat addr error\n");
                exit(1);
            }
            buf[i]->file_finish = 1;
        }
        int k = 0;
        while(1)
        {
            P(semid, 0);
            buf[k]->file_size = fread(buf[k]->ptr, 1, buf_size, fp_r);
            if(buf[k]->file_size < buf_size)
            {
                buf[k]->file_finish = 0;
                V(semid, 1);
                break;
            }
            V(semid, 1);
            k = (k + 1) % buf_count;
        }
        for(int i = 0; i < buf_count; i++)
        {
            shmdt(buf[i]);
        }
        exit(1);
    }
    
    if((p_read = fork()) == -1)
    {
        printf("Create process failed");
        exit(1);
    }
    else if(p_read == 0)
    {
        for(int i = 0; i < buf_count; i++)
        {
            buf[i] = shmat(shmt[i], NULL, SHM_R|SHM_W);
            if((int)buf[i]->ptr == -1)
            {
                printf("shmat addr error\n");
                exit(1);
            }
        }
        int k = 0;
        while(1)
        {
            P(semid, 1);
            fwrite(buf[k]->ptr, 1, buf[k]->file_size, fp_w);
            
            file_w_size = file_w_size + buf_size;
            progress = file_w_size * 10 / file_r_size;
            int i;
            printf("\r||");
            for(i = 0; i < progress; i++)
            {
                printf("==");
            }
            printf(">>");
            for(; i < 10; i++)
            {
                printf("  ");
            }
            printf("||");
            if(buf[k]->file_finish == 0)
            {
                printf("\n");
                break;
            }
            V(semid, 0);
            k = (k + 1) % buf_count;
        }
        
        printf("finish!\n");
        
        for(int i = 0; i < buf_count; i++)
        {
            shmdt(buf[i]);
        }
        exit(1);
    }
    
    waitpid(p_write, NULL, 0);
    waitpid(p_read, NULL, 0);

    if((semctl(semid, 0, IPC_RMID)) < 0)
    {
        printf("delete semid failed\n");
        exit(1);
    }
    
    for(int i = 0; i < buf_count; i++)
    {
        shmctl(shmt[i], IPC_RMID, 0);
    }
    return 0;
}
