//
//  main.c
//  process_control
//
//  Created by chengdan on 2018/12/12.
//  Copyright © 2018 chengdan. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

pid_t p1;
pid_t p2;
//父进程信号处理
void parent_pro(int sign_no){
    if(sign_no == SIGINT)
    {
        printf("Receive SIGINT.\n");
        kill(p1, SIGUSR1);
        kill(p2, SIGUSR2);
    }
}
//子进程1信号处理
void pro_func1(int sign_no){
    switch(sign_no)
    {
        case SIGINT://忽略信号
            break;
        case SIGUSR1://关闭子进程1
        {
            printf("Child Process l is Killed by Parent!\n");
            exit(0);
        }
        default:
            break;
    }
    return;
}
//子进程2信号处理
void pro_func2(int sign_no){
    switch(sign_no)
    {
        case SIGINT://忽略信号
            break;
        case SIGUSR2://关闭子进程2
        {
            printf("Child Process 2 is Killed by Parent!\n");
            exit(0);
        }
        default:
            break;
    }
    return;
}

int main(int argc, const char * argv[]) {
    //创建无名管道
    int pipefd[2];
    if(pipe(pipefd) == -1)// 如果创建管道失败
    {
        printf("Create pipe failed\n");
        exit(1);
    }
    //创建子进程1
    if((p1 = fork()) == -1)//如果创建子进程1失败
    {
        printf("Create process failed");
        exit(1);
    }
    
    if(p1 == 0)
    {
        int counter = 0;
        signal(SIGINT, pro_func1);//设置忽略信号SIGINT
        signal(SIGUSR1, pro_func1);//设置信号SIGUSR1
        while(1)
        {
            char send[40];
            sprintf(send, "Have sended %d times", counter);
            write(pipefd[1], send, 40);// 向管道中写入数据
            counter++;
            sleep(1);//睡眠1秒钟
        }
    }
    //创建子进程2
    if((p2 = fork()) == -1)//如果创建子进程2失败
    {
        printf("Create process failed");
        exit(1);
    }
    
    if(p2 == 0)
    {
        signal(SIGINT, pro_func2);//设置忽略信号SIGINT
        signal(SIGUSR2, pro_func2);//设置信号SIGUSR2
        while(1)
        {
            char receive[40];
            read(pipefd[0], receive, 40);// 从管道中读取数据
            printf("Message : %s\n", receive);
        }
    }
    //设置软中断信号SIGINT
    signal(SIGINT, parent_pro);
    
    //等待子进程1，2退出
    waitpid(p1, NULL, 0);//第三个参数对waitpid进行额外的选项进行控制
    waitpid(p2, NULL, 0);//第二参数指向的变量用来存放子进程的退出码，即从子进程的main函数返回的值或子进程中exit()函数的参数。如果status不是一个空指针，状态信息将被写入它指向的变量
    
    //关闭管道
    close(pipefd[0]);
    close(pipefd[1]);
    
    printf("Parent Process is Killed!\n");
    return 0;
}
