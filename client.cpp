#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 4096

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    
    while(true){
        char id[10], buf[BUFFER_SIZE];
        bzero(&id, sizeof(id));
        bzero(&buf, sizeof(buf));
        int flag, j;

        printf("请输入您的员工号:");
        scanf("%s", id);
        printf("请选择打卡还是查看您的考勤数据\n");
        printf("|1、打卡                |\n");
        printf("|2、查看您的考勤数据      |\n");
        scanf("%d", &flag);

        if(flag != 1 && flag != 2){
            printf("输入错误，请重新输入\n");
            continue;
        }

        j = sprintf(buf, "{\"clientName\":\"ClockClient\",");
        j += sprintf(buf + j, "\"flag\":\"%d\",", flag);
        j += sprintf(buf + j, "\"id\":\"%s\"}", id);

        // printf("%s\n", buf);
        ssize_t write_bytes = write(sockfd, buf, strlen(buf));
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        // printf("%ld\n", write_bytes);

        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            printf("%s\n", JsonToData(buf, "mes"));
        }else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){
            close(sockfd);
            errif(true, "socket read error");
        }
    }

    close(sockfd);
    return 0;
}
