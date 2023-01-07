/*员工考勤系统-服务端*/
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
// #include <conio.h>
#include <time.h>
#include "util.h"

#define MAX_EVENTS 100
#define READ_BUFFER 4096
#define INFO_LENGTH 2048
#define MAX_CLIENT 20

int first_time_hour = 8, first_time_min = 30;
int second_time_hour = 8, second_time_min = 30;

typedef struct staff{
	char name[20];
	char id[10];
	int sex;
	char info[INFO_LENGTH];
}Staff;

typedef struct sNode
{
  Staff stu;          //员工信息
  struct sNode* pNext;	//指向下一个员工
  struct sNode* pPrev;	//指向前一个员工
}Node;

Node *pHead = (Node *)malloc(sizeof(Node)); //定义双向链表头结点
Node *pTail = (Node *)malloc(sizeof(Node)); //定义双向链表尾节点

void addStaff(int client_fd, char* buf){

	Staff add_staff;

	sprintf(add_staff.id, "%s", JsonToData(buf, "id"));
	sprintf(add_staff.name, "%s", JsonToData(buf, "name"));
	add_staff.sex = JsonToData_num(buf, "sex");

	Node* new_staff = (Node *)malloc(sizeof(Node)); 
	memset(new_staff,'\0', sizeof(Node));

	new_staff->stu = add_staff;

	//将新添加的员工放入双向链表的结尾
	pTail->pPrev->pNext = new_staff;
	new_staff->pPrev = pTail->pPrev;
	new_staff->pNext = pTail;
	pTail->pPrev = new_staff;

    char ret[READ_BUFFER];
	bzero(&ret, sizeof(ret));
	int j;

   	j = sprintf(ret, "%s", "{\"mes\":\"新员工已添加\"}");

    ssize_t write_bytes = write(client_fd, ret, sizeof(ret));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

void FindStaff(int client_fd, char* buf){

	char id[10];
	bzero(&id, sizeof(id));
	sprintf(id, "%s", JsonToData(buf, "id"));
	//根据员工id找到节点
	Node* cur = pHead->pNext;
	char ret[READ_BUFFER];
	bzero(&ret, sizeof(ret));
	int j;
	bool flag = 0;

	while(cur != pTail){
		// printf("cur:%s %ld\n",cur->stu.id, strlen(cur->stu.id));
		// printf("cur:%s %ld\n",id, strlen(id));
		if(strncmp(cur->stu.id, id, sizeof(cur->stu.id)) == 0){
			flag = 1;
			j = sprintf(ret, "{\"id\":\"%s\",", cur->stu.id);
			j += sprintf(ret + j, "\"name\":\"%s\",", cur->stu.name);
			j += sprintf(ret + j, "\"sex\":\"%d\",", cur->stu.sex);
			j += sprintf(ret + j, "\"info\":\"%s\"}", cur->stu.info);
			// printf("查询成功\n");
			break;
		}else{
			// printf("else%s\n", cur->stu.id);
			cur = cur->pNext;
		}
	}

	if(flag == 0)
		sprintf(ret, "{\"mes\":\"您要查询的员工不存在\"}");
    ssize_t write_bytes = write(client_fd, ret, strlen(ret));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

void DeleteStaff(int client_fd, char* buf){
	
	char id[10];
	sprintf(id, "%s", JsonToData(buf, "id"));
	//根据员工id找到节点

	Node* cur = pHead->pNext;
	char ret[READ_BUFFER];
	bzero(&ret, sizeof(ret));
	bool flag;

	while(cur != pTail){
		if(strncmp(cur->stu.id, id, strlen(cur->stu.id)) == 0){
			flag = 1;
			cur->pPrev->pNext = cur->pNext;
			cur->pNext->pPrev = cur->pPrev;
			break;
		}else{
			cur = cur->pNext;
		}
	}

	if(flag){
   		sprintf(ret, "{\"mes\":\"员工已删除\"}");
	}else{
		sprintf(ret, "{\"mes\":\"您要删除的员工不存在\"}");
	}

    ssize_t write_bytes = write(client_fd, ret, sizeof(ret));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

void modifystaff(int client_fd, char* buf){

	Staff mod_staff;

	sprintf(mod_staff.id, "%s", JsonToData(buf, "id"));
	sprintf(mod_staff.name, "%s", JsonToData(buf, "name"));
	mod_staff.sex = JsonToData_num(buf, "sex");

	//根据员工id找到节点
	Node* cur = pHead->pNext;
	char ret[READ_BUFFER];
	bzero(&ret, sizeof(ret));
	int j;
	bool flag;

	while(cur != pTail){
		if(strncmp(cur->stu.id, mod_staff.id, strlen(cur->stu.id)) == 0){
			flag = 1;
			sprintf(cur->stu.name, "%s", mod_staff.name);
			cur->stu.sex = mod_staff.sex;
			break;
		}else{
			cur = cur->pNext;
		}
	}
	if(flag){
   		sprintf(ret, "{\"mes\":\"员工信息已被修改\"}");
	}else{
		sprintf(ret, "{\"mes\":\"您要修改的员工不存在\"}");
	}

    ssize_t write_bytes = write(client_fd, ret, sizeof(ret));
    if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

void addInfo(int client_fd, char* id){

	char ret[READ_BUFFER], log[50];
	bzero(&ret, sizeof(ret));
	bzero(&log, sizeof(log));
	int j;

	time_t timep;
	struct tm *p;
	time (&timep);

	p = gmtime(&timep);
	int cur_hour = 8+p->tm_hour;
	int cur_min = p->tm_min;

	j = sprintf(log, "%d年%d月%d日:",1900+p->tm_year,1+p->tm_mon,p->tm_mday);

	if(cur_hour < first_time_hour || (cur_hour == first_time_hour && cur_min < first_time_min)){
		sprintf(ret, "%s", "{\"mes\":\"打卡成功\"}");
		j += sprintf(log+j, "打卡成功\n");
	}else if(cur_hour > second_time_hour || (cur_hour == second_time_hour && cur_min > second_time_min)){
		sprintf(ret, "%s", "{\"mes\":\"今天你旷工了\"}");
		j += sprintf(log+j, "旷工\n");
	}else{
		sprintf(ret, "%s", "{\"mes\":\"今天你迟到了\"}");
		j += sprintf(log+j, "迟到\n");
	}

	Node* cur = pHead->pNext;

	bool flag = 0;
	while(cur != pTail){
		if(strncmp(cur->stu.id, id, strlen(cur->stu.id)) == 0){
			flag = 1;
			int log_len = strlen(cur->stu.info);
			sprintf(cur->stu.info + log_len, "%s", log);
			// printf("%s\n", cur->stu.info);
			break;
		}else{
			cur = cur->pNext;
		}
	}
	if(flag == 0){
		sprintf(ret, "%s", "{\"mes\":\"您还没有注册\"}");
	}

    ssize_t write_bytes = write(client_fd, ret, sizeof(ret));
	if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

void findInfo(int client_fd, char* id){
	char ret[READ_BUFFER];
	bzero(&ret, sizeof(ret));
	bool flag = 0;

	Node* cur = pHead->pNext;

	while(cur != pTail){
		if(strncmp(cur->stu.id, id, strlen(cur->stu.id)) == 0){
			flag = 1;
			sprintf(ret, "{\"mes\":\"%s\"}", cur->stu.info);
			// printf("%s\n", cur->stu.info);
			break;
		}else{
			cur = cur->pNext;
		}
	}
	if(flag == 0){
		sprintf(ret, "{\"mes\":\"打卡记录为空\"}");
	}
    ssize_t write_bytes = write(client_fd, ret, sizeof(ret));
	if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

void settime(int client_fd, char* buf){
	first_time_hour = JsonToData_num(buf, "first_hour");
	first_time_min = JsonToData_num(buf, "first_min");
	second_time_hour = JsonToData_num(buf, "second_hour");
	second_time_min = JsonToData_num(buf, "second_min");

	char ret[READ_BUFFER];
	bzero(&ret, sizeof(ret));

	sprintf(ret, "{\"mes\":\"设置成功\"}");
    ssize_t write_bytes = write(client_fd, ret, sizeof(ret));
	if(write_bytes == -1){
        printf("socket already disconnected, can't write any more!\n");
        return ;
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
	int option;
	socklen_t optlen;
	optlen = sizeof(option);
	option = true;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
    
    int epfd = epoll_create(MAX_CLIENT);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);

    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	memset(pHead,'\0', sizeof(Node));
	memset(pTail,'\0', sizeof(Node));

	pHead->pPrev = NULL;
	pHead->pNext = pTail;
	pTail->pPrev = pHead;
	pTail->pNext = NULL;

    while(true){
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		// printf("%d\n", nfds);
        errif(nfds == -1, "epoll wait error");
        for(int i = 0; i < nfds; ++i){
			// printf("sockfd%d\n", sockfd);
			// printf("event%d\n", events[i].data.fd);
            if(events[i].data.fd == sockfd){        //新客户端连接
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if(events[i].events & EPOLLIN){      //可读事件
                char buf[READ_BUFFER], client_name[20];
				bzero(&buf, sizeof(buf));
				bzero(&client_name, sizeof(client_name));
                while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
				
                    if(bytes_read > 0){
						sprintf(client_name, "%s", JsonToData(buf, "clientName"));
						// printf("%s\n", buf);
						// printf("%s%ld\n", client_name, strlen(client_name));
						if(strncmp(client_name, "ClockClient", strlen("ClockClient")) == 0){
							int flag;
							char id[10];
							bzero(&id, sizeof(id));
							sprintf(id, "%s", JsonToData(buf, "id"));
							flag = JsonToData_num(buf, "flag");
							if(flag == 1){
								//添加打卡信息
								// printf("flag==1\n");
								addInfo(events[i].data.fd, id);
							}else{
								//返回该员工所有考勤记录
								// printf("flag==2\n");
								findInfo(events[i].data.fd, id);
							}
						}else{
							int number = JsonToData_num(buf, "number");
							// printf("%s\n", buf);
							// int number = 1;
							switch (number){
								case 1:        //1、添加员工信息
									addStaff(events[i].data.fd, buf);
									break;
								case 2:       //2、查询员工信息 
									FindStaff(events[i].data.fd, buf);
									break;		
								case 3:       //3、删除员工信息
									DeleteStaff(events[i].data.fd, buf);
									break;
								case 4:       //4、修改员工信息 
									modifystaff(events[i].data.fd, buf);
									break;
								case 5:       //5、设置迟到时间 
									settime(events[i].data.fd, buf);
									break;
								default:
									sprintf(buf, "您的输入有误，请重新输入！\n");
									write(events[i].data.fd, buf, sizeof(buf));
									break;
							}
						}
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("此次服务完成\n");
                        break;
                    } else if(bytes_read == 0){  //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            } else{         //其他事件
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}
