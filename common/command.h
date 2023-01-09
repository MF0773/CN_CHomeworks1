#ifndef COMMAND_H
#define COMMAND_H

//command name , args
//importCommand : name and arg , makeString
void command_makeString(char *output, char *name, char* args);
void command_format(char *output, char *name, const char *format,... );
void command_exportString(char* command , char* name , char* args);
void command_send(int fd , char *name ,const char *format,... );
void command_waitName(int fd , char*targetName , const char *format,...  );
#endif // COMMAND_H
