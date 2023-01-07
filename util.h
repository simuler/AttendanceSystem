#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <fcntl.h>

void errif(bool, const char*);

char* JsonToData(char* , const char*);

int JsonToData_num(char* , const char*);

void setnonblocking(int fd);
  
#endif