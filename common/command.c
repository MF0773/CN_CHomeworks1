#include "command.h"
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>

void command_makeString(char* output,char *name, char *args)
{
    sprintf(output,"< %s %s >\n" , name , args);
}

void command_exportString(char* command , char* name , char* args){
    int start,end,i;
    int nameLen;

    for(i = 0 ; command[i] != '<' ; i++);
    start = i;

    for( i = start+1 ; command[i] != '>' ; i++);
    end = i;

    sscanf(&(command[start+1]) , "%s" , name);
    nameLen = strlen(name);
    int argLen = end-1 - (start+1 + nameLen+1) ;
    strncpy(args,&(command[start+1 + nameLen+1])  ,argLen );
    args[argLen] = NULL;
    nameLen = 0;
}

void command_send(int fd , char *name ,const char *format,... ){
    va_list argList;
    char argBuf[100];
    char sendBuf[1024];
    int feedback ;

    va_start(argList,format);
    feedback = vsprintf(argBuf,format, argList );
    command_makeString(sendBuf,name,argBuf);
    send(fd, sendBuf, strlen(sendBuf), 0);

    va_end(argList);
}

void command_waitName(int fd , char*targetName , const char *format,...  ){
    char recvBuf[1024];
    char nameBuf[50];
    char argBuf[100];
    int recvLen;

    while(1){
        recvLen = recv(fd , recvBuf, 1024 , 0);
        recvBuf[recvLen] = NULL;
        command_exportString(recvBuf,nameBuf,argBuf);
        if(strcmp(targetName , nameBuf)==0){
            break;
        }
    }

    va_list argList;
    int feedback;
    va_start(argList,format);

    feedback = vsscanf(argBuf,format,argList);

    va_end(argList);
//    return feedback;
}

void command_format(char *output, char *name, const char *format,...)
{
    va_list argList;
    char argBuf[100];
    int feedback ;

    va_start(argList,format);
    feedback = vsprintf(argBuf,format, argList );
    command_makeString(output,name,argBuf);

    va_end(argList);
}
