#include"kernel/types.h"
#include"kernel/stat.h"
#include"user/user.h"
#include"kernel/fs.h"

char* fmtname(char *path)	
{
  char *p;
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}


void find(char *path,char *target)
{
    char buffer[512],*p;                //存放路径名称/p指向buffer用于拼接
    int fd;                             //open文件描述符
    struct dirent de;                   //存read的文件名
    struct stat st;                     //存放文件的类型：目录：T_DIR 普通文件：T_FILE

    // 打开路径
    fd = open(path, 0);                 ///只读打开
    if(fd < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取路径类型
    if(fstat(fd, &st) < 0)             //读取st文件信息
    {         
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 如果是普通文件，就判断文件名是否匹配
    if(st.type == T_FILE)
    {
        if(strcmp(fmtname(path),target)==0)
        {
            printf("%s\n",path);
        }

    }

    // 如果是目录，递归进入每个有效子目录项
    if(st.type == T_DIR)
    {
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buffer){
            fprintf(2, "find: path too long\n");
            close(fd);
            return;
        }

        strcpy(buffer, path);
        p = buffer + strlen(buffer);
        *p++ = '/';

        while(read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if(de.inum == 0)
                continue;

            // 跳过 "." 和 ".."
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // 递归调用 find
            find(buffer, target);
        }
    }

    close(fd);

}

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        fprintf(2,"error");
        exit(1);
    }

    find(argv[1],argv[2]);
    exit(0);
}
