#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 4096

struct staff{
	char name[20];
	char id[10];
	int sex;
};

void menu(){
    	printf("     管理员客户端       \n");
		printf("----------------------------\n");
		printf("|1、注册员工信息            |\n");
		printf("|2、查询员工信息            |\n");
		printf("|3、删除员工信息            |\n");
		printf("|4、修改员工信息            |\n");
        printf("|5、设置迟到时间            |\n");
		printf("|6、退出                    |\n");
		printf("----------------------------\n");
		printf("请输入相应的序号选择!       \n");
}

void addStaff(int sockfd){
	printf("请输入员工: 工号 姓名 性别\n");
    struct staff add_staff;
    char buf[BUFFER_SIZE];
    bzero(&buf, sizeof(buf));
	int j;
    printf("请输入工号:");
    scanf("%s", add_staff.id);
    printf("请输入姓名:");
    scanf("%s", add_staff.name);
    printf("请输入性别(1.男生 2.女生):");
    scanf("%d", &add_staff.sex);
	
    j = sprintf(buf, "{\"clientName\":\"AdmClient\",");
    j += sprintf(buf + j, "\"number\":\"1\",");
    j += sprintf(buf + j, "\"id\":\"%s\",", add_staff.id);
    j += sprintf(buf + j, "\"name\":\"%s\",", add_staff.name);
    j += sprintf(buf + j, "\"sex\":\"%d\"}", add_staff.sex);

    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    if(read_bytes > 1){
        printf("%s\n", JsonToData(buf, "mes"));
    }else if(read_bytes == 1){
        printf("添加失败!\n");
    }else if(read_bytes == -1){
        close(sockfd);
        errif(true, "socket read error");
    }
}

void FindStaff(int sockfd){
    struct staff find_staff;
    char id[10], buf[BUFFER_SIZE];
    bzero(&id, sizeof(id));
    bzero(&buf, sizeof(buf));    
	int j;

    printf("请输入查询的员工工号:");
    scanf("%s", id);
	
    j = sprintf(buf, "{\"clientName\":\"AdmClient\",");
    j += sprintf(buf + j, "\"number\":\"2\",");
    j += sprintf(buf + j, "\"id\":\"%s\"}", id);

    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }

    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    if(strstr(buf, "mes") != NULL){
        printf("需要查找的员工没有找到!\n");
    }else if(read_bytes > 1){
        sprintf(find_staff.id, "%s", JsonToData(buf, "id"));
        sprintf(find_staff.name, "%s", JsonToData(buf, "name"));
        find_staff.sex = JsonToData_num(buf, "sex");
        printf("该员工工号为：%s\n", find_staff.id);
        printf("名字为：%s\n", find_staff.name);
        printf("性别为：%d\n", find_staff.sex);
        printf("考勤记录为：%s\n", JsonToData(buf, "info"));
    }else if(read_bytes == -1){
        close(sockfd);
        errif(true, "socket read error");
    }
}

void DeleteStaff(int sockfd){
	
    char id[10];
    bzero(&id, sizeof(id));
    char buf[BUFFER_SIZE];
	int j;
    printf("请输入删除员工的工号:\n");
    scanf("%s", id);
 
    j = sprintf(buf, "{\"clientName\":\"AdmClient\",");
    j += sprintf(buf + j, "\"number\":\"3\",");
    j += sprintf(buf + j, "\"id\":\"%s\",", id);

    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }

    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    if(read_bytes > 1){
        printf("%s\n", JsonToData(buf, "mes"));
    }else if(read_bytes == 1){
        printf("删除的员工没有找到!\n");
    }else if(read_bytes == -1){
        close(sockfd);
        errif(true, "socket read error");
    }
}

void modifystaff(int sockfd){
	printf("请输入需要修改的员工: 工号 姓名 性别,将根据工号修改,因此工号不能被修改，可修改其他信息\n");
    struct staff add_staff;
    char buf[BUFFER_SIZE];
	int j;

    printf("请输入员工工号:");
    scanf("%s", add_staff.id);
    printf("请输入员工名字:");
    scanf("%s", add_staff.name);
    printf("请输入员工性别:");
    scanf("%d", &add_staff.sex);
	
    j = sprintf(buf, "{\"clientName\":\"AdmClient\",");
    j += sprintf(buf + j, "\"number\":\"4\",");
    j += sprintf(buf + j, "\"id\":\"%s\",", add_staff.id);
    j += sprintf(buf + j, "\"name\":\"%s\",", add_staff.name);
    j += sprintf(buf + j, "\"sex\":%d\"}", add_staff.sex);

    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    if(read_bytes > 1){
        printf("%s\n", JsonToData(buf, "mes"));
    }else if(read_bytes == 1){
        printf("修改的员工没有找到!\n");
    }else if(read_bytes == -1){
        close(sockfd);
        errif(true, "socket read error");
    }
}
void settime(int sockfd){
    int first_hour, first_min;
    int second_hour, second_min;
    printf("请输入迟到时间的hour:");
    scanf("%d", &first_hour);
    printf("请输入迟到时间的min:");
    scanf("%d", &first_min);
    printf("请输入旷工时间的hour:");
    scanf("%d", &second_hour);
    printf("请输入旷工时间的min:");
    scanf("%d", &second_min);

    char buf[BUFFER_SIZE];
	int j;
	
    j = sprintf(buf, "{\"clientName\":\"AdmClient\",");
    j += sprintf(buf + j, "\"number\":\"5\",");
    j += sprintf(buf + j, "\"first_hour\":\"%d\",", first_hour);
    j += sprintf(buf + j, "\"first_min\":\"%d\",", first_min);
    j += sprintf(buf + j, "\"second_hour\":\"%d\",", second_hour);
    j += sprintf(buf + j, "\"second_min\":%d\"}", second_min);

    ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
    if(read_bytes > 1){
        printf("%s\n", JsonToData(buf, "mes"));
    }else if(read_bytes == 1){
        printf("设置成功!\n");
    }else if(read_bytes == -1){
        close(sockfd);
        errif(true, "socket read error");
    }
}
        

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    // setnonblocking(sockfd);
    bool quit = 0;
    while(true){
        menu();
	    int ch;
        scanf("%d", &ch);
        switch (ch){
            case 1:        //1、添加员工信息
                addStaff(sockfd);
                break;
            case 2:       //2、查询员工信息 
                FindStaff(sockfd);
                break;		
            case 3:       //3、删除员工信息
                DeleteStaff(sockfd);
                break;
            case 4:       //4、修改员工信息 
                modifystaff(sockfd);
                break;
            case 5:
                settime(sockfd);
                break;
            case 6:       //5、退出
                printf("\n\n   欢迎再次使用！  \n\n\n");	
                quit = 1;		
                break;
            default:
                printf("您的输入有误，请重新输入！\n");
                break;
        }
        if(quit)
            break;
    }
    close(sockfd);
    return 0;
}
