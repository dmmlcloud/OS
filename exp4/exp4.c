//
//  main.c
//  printdir
//
//  Created by chengdan on 2018/12/27.
//  Copyright © 2018 chengdan. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct inform{
    char authority[11];//权限
    int link;//硬连接个数
    char *user;//所属用户
    char *group;//所属组
    long long size;//大小
    char time[64];//时间
};

void printdir(const char *, int);
struct inform transform(struct stat);

int main(int argc, const char * argv[]) {
    if(argc == 1)
    {
        printdir(".", 0);//当前目录
    }
    else if(argc > 1)
    {
        printdir(argv[1], 0);//输入的目录
    }

    return 0;
}

void printdir(const char *dir, int depth){
    DIR *dp;
    struct inform ts;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL)
    {
        printf("open dir failed\n");
        exit(1);
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);//lstat读取符号连接本身的状态信息
        ts = transform(statbuf);
        if(S_ISDIR(statbuf.st_mode))//如果是目录文件
        {
            if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)//跳过上级目录项和当前目录项
            {
                continue;
            }
            printf("%s%*d%*d  %s  %s  %*lld  %s  %s\n",ts.authority, 4, ts.link, 4, depth, ts.user, ts.group, 10, ts.size, ts.time ,entry->d_name);
            printf("%s:\n", entry->d_name);
            printdir(entry->d_name, depth + 4);//打开这个目录
            printf("..\n");
        }
        else
        {
            printf("%s%*d%*d  %s  %s  %*lld  %s  %s\n",ts.authority, 4, ts.link, 4, depth, ts.user, ts.group, 10, ts.size, ts.time ,entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);
}


struct inform transform(struct stat st)
{
    struct inform result;
    char perms[11];
    // 判断文件类型
    switch(st.st_mode & S_IFMT)
    {
        case S_IFSOCK:   // 套接字文件
            perms[0] = 's';
            break;
        case S_IFLNK:     // 软连接文件
            perms[0] = 'l';
            break;
        case S_IFREG:     // 普通文件
            perms[0] = '-';
            break;
        case S_IFBLK:    // 块设备文件
            perms[0] = 'b';
            break;
        case S_IFDIR:    // 目录文件
            perms[0] = 'd';
            break;
        case S_IFCHR:    // 字符设备文件
            perms[0] = 'c';
            break;
        case S_IFIFO:    // 管道文件
            perms[0] = 'p';
            break;
        default:
            break;
    }
    
    // 判断文件的访问权限
    // 文件的所有者
    perms[1] = (st.st_mode & S_IRUSR) ? 'r':'-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w':'-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x':'-';
    
    // 文件的所属组
    perms[4] = (st.st_mode & S_IRGRP) ? 'r':'-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w':'-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x':'-';
    
    // 文件的其他用户
    perms[7] = (st.st_mode & S_IROTH) ? 'r':'-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w':'-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x':'-';
    
    strcpy(result.authority, perms);
    
    // 硬链接计数
    result.link = st.st_nlink;
    
    // 文件所有者
    result.user = getpwuid(st.st_uid)->pw_name;
    
    // 文件所属组
    result.group = getgrgid(st.st_gid)->gr_name;
    
    // 文件大小
    result.size = st.st_size;
    
    // 文件修改时间
    char *time = ctime(&st.st_mtimespec.tv_sec);
    time[strlen(time)-1] = '\0';
    strcpy(result.time,time);
    
    return result;
}
