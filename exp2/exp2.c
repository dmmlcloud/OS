#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>

int a = 0;
int semid;
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

void *subp1(){
    for(int i = 0; i < 100; i++)
    {
        P(semid, 0);
        printf("Value of a : %d\n", a);
        V(semid, 1);
    }
    return NULL;
}

void *subp2(){
    for(int i = 1; i <= 100; i++)
    {
        P(semid, 1);
        a = a + i;
        V(semid, 0);
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    key_t key;
    void *status;
    key = ftok(".", 1);
    //创建联合用于为信号灯赋初值
    semun_t mysemun0;
    semun_t mysemun1;
    
    mysemun0.val = 0;
    mysemun1.val = 1;
    //定义线程p1，p2
    pthread_t p1, p2;
    
    semid = semget(key , 2, IPC_CREAT | 0666);//创建信号灯集，key是id号，2是该信号灯集中有两个信号灯，IPC_CREAT创建共享内存，0666权限
    if(semid == -1)//如果创建失败
    {
        printf("Create semid failed\n");
        exit(1);
    }
    semctl(semid, 0, SETVAL, mysemun0);//表示是否执行完输出，初值为0
    semctl(semid, 1, SETVAL, mysemun1);//表示是否执行完加操作，初值为1
    //创建两个线程
    pthread_create(&p1, NULL, subp1, NULL);//第一个参数指向线程标识符的指针，第二个参数设置线程的属性，第三个参数为线程运行函数的地址，第四个参数为运行函数的参数
    pthread_create(&p2, NULL, subp2, NULL);
    
    pthread_join(p1, &status);//以阻塞的方式等待线程结束,status用来存储被等待线程的返回值
    pthread_join(p2, &status);
    
    if((semctl(semid, 0, IPC_RMID)) >= 0)//删除信号灯集
    {
        printf("delete semid\n");
    }
    return 0;
}
