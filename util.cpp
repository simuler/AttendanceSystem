#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void errif(bool condition, const char *errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

char* JsonToData(char* buf, const char* label){
    // printf("%s\n", buf);
    // printf("%s\n", label);
    char *cur;
	cur = strstr(buf, label);
    if(cur == NULL)
        return cur;
    
    //指向值的第一个字符
    cur += strlen(label) + 3;
    char *head = cur;
    int len = 0;    

    while(*cur != '\"'){
        len++;
        cur++;
    }

    char *ret = (char*)malloc(len+1);
    strncpy(ret, head, len);
    return ret;
}

int JsonToData_num(char* buf, const char* label){
    char *cur;
	cur = strstr(buf, label);
    
    if(cur == NULL)
        return 1;
    //指向值的第一个字符
    cur += strlen(label) + 3;
    char *head = cur;
    int len = 0;    
    while(*cur != '\"'){
        len++;
        cur++;
    }
    char *ret = (char*)malloc(len+1);
    strncpy(ret, head, len);
    int val = atoi(ret);
    return val;
}

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}