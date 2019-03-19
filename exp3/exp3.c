//
//  main.c
//  ringr buffer
//
//  Created by chengdan on 2018/12/19.
//  Copyright © 2018 chengdan. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#define buf_size 4096
#define buf_count 5
#define input_file "al_design.pdf"
#define output_file "design.pdf"

int shmt[buf_count];
int semid;
pid_t p_write, p_read;
FILE *fp_r;
FILE *fp_w;
struct shmid_ds share_buf;

//查询文件大小
static long long get_file_size(const char* file) {
    struct stat tbuf;
    stat(file, &tbuf);
    return tbuf.st_size;
}

//P操作
void P(int semid, int index){
    struct sembuf sem;
    sem.sem_num = index;//信号灯集中信号灯的编号
    sem.sem_op = -1;//-1表示p操作
    sem.sem_flg = 0;//表示semop函数的操作是阻塞的
    semop(semid, &sem, 1);//1表示执行命令个数
    return;
}
//V操作
void V(int semid, int index){
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

//创建缓冲区的结构
struct buffer{
    char ptr[buf_size];//字符串
    int file_finish;//是否读取完
    unsigned long file_size;//字符串大小
};

int main(int argc, const char * argv[]) {
    //创建环形缓冲
    struct buffer *buf[buf_count];
    
    fp_r = fopen(input_file, "r");//打开源文件
    fp_w = fopen(output_file, "w");//打开输出文件

    long long file_r_size = get_file_size(input_file);//得到源文件大小
    long long file_w_size = 0;//输出文件大小初值为0
    float progress;//读取进度
    //创建联合用于为信号灯赋初值
    semun_t mysemun_w;
    semun_t mysemun_r;
    
    mysemun_w.val = buf_count;
    mysemun_r.val = 0;
    //创建共享内存组
    for(int i = 0; i < buf_count; i++)
    {
        shmt[i] = shmget(IPC_PRIVATE, buf_size * sizeof(char) + sizeof(int) + sizeof(unsigned long), IPC_CREAT|0666);//缓冲区大小4096
        if (shmt[i] < 0)//创建失败
        {
            printf("get shm ipc_id error\n") ;
            return -1 ;
        }
    }
    
    //创建信号灯
    semid = semget(IPC_PRIVATE , 2, IPC_CREAT|0666);//创建信号灯集，key是id号，2是该信号灯集中有两个信号灯，IPC_CREAT创建共享内存，0666权限
    if(semid == -1)//如果创建失败
    {
        printf("Create semid failed\n");
        exit(1);
    }
    
    //信号灯赋初值
    semctl(semid, 0, SETVAL, mysemun_w);//表示是否执行完加操作，初值为10
    semctl(semid, 1, SETVAL, mysemun_r);//表示是否执行完输出，初值为0
    
    //创建两个进程readbuf、writebuf
    
    //创建子进程writebuf
    if((p_write = fork()) == -1)//如果创建子进程readbuf失败
    {
        printf("Create process failed");
        exit(1);
    }
    else if(p_write == 0)
    {
        for(int i = 0; i < buf_count; i++)//建立进程的地址空间到共享内存的映射
        {
            buf[i] = shmat(shmt[i], NULL, SHM_R|SHM_W);//由内核决定共享内存出现在进程内存地址的什么位置
            if((int)buf[i]->ptr == -1)//如果建立映射失败
            {
                printf("shmat addr error\n");
                exit(1);
            }
            buf[i]->file_finish = 1;
        }
        int k = 0;
        while(1)//将文件内容写入缓冲区
        {
            P(semid, 0);
            buf[k]->file_size = fread(buf[k]->ptr, 1, buf_size, fp_r);
            if(buf[k]->file_size < buf_size)//如果文件已全部读完
            {
                buf[k]->file_finish = 0;
                V(semid, 1);
                break;
            }
            V(semid, 1);
            k = (k + 1) % buf_count;
        }
        for(int i = 0; i < buf_count; i++)//删除进程的地址空间到共享内存的映射
        {
            shmdt(buf[i]);
        }
        exit(1);
    }
    
    //创建子进程readbuf
    if((p_read = fork()) == -1)//如果创建子进程writebuf失败
    {
        printf("Create process failed");
        exit(1);
    }
    else if(p_read == 0)
    {
        for(int i = 0; i < buf_count; i++)//建立进程的地址空间到共享内存的映射
        {
            buf[i] = shmat(shmt[i], NULL, SHM_R|SHM_W);//由内核决定共享内存出现在进程内存地址的什么位置
            if((int)buf[i]->ptr == -1)//如果建立映射失败
            {
                printf("shmat addr error\n");
                exit(1);
            }
        }
        int k = 0;
        while(1)//将缓冲区中内容写入输出文件
        {
            P(semid, 1);
            fwrite(buf[k]->ptr, 1, buf[k]->file_size, fp_w);
            //进度条
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
            //
            if(buf[k]->file_finish == 0)//如果全部内容已写入
            {
                printf("\n");
                break;
            }
            V(semid, 0);
            k = (k + 1) % buf_count;
        }
        
        printf("finish!\n");
        
        for(int i = 0; i < buf_count; i++)//删除进程的地址空间到共享内存的映射
        {
            shmdt(buf[i]);
        }
        exit(1);
    }
    
    //等待两个进程运行结束
    waitpid(p_write, NULL, 0);//第三个参数对waitpid进行额外的选项进行控制
    waitpid(p_read, NULL, 0);//第二参数指向的变量用来存放子进程的退出码，即从子进程的main函数返回的值或子进程中exit()函数的参数。如果status不是一个空指针，状态信息将被写入它指向的变量
    
    //删除信号灯
    if((semctl(semid, 0, IPC_RMID)) < 0)//删除信号灯集
    {
        printf("delete semid failed\n");
        exit(1);
    }
    
    //删除共享内存组
    for(int i = 0; i < buf_count; i++)
    {
        shmctl(shmt[i], IPC_RMID, 0);
    }
    return 0;
}
