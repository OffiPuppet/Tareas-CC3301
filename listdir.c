#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
       
#define MAX_PATH 1000

void listdir(int indent, char *name) {
  DIR *current_dir;
  struct dirent *this_entry;
  char cwd[MAX_PATH+1];
  int i;  
  struct stat status;
      
  /* imprimimos directorio actual */
  for (i= 1; i<=indent; i++)
    printf("    ");
  printf("%s\n", name);
      
  if (stat(name, &status)!=0) {
    perror(name);
    exit(1);
  }
  if(!S_ISDIR(status.st_mode))
    return;   
      
  /* abrimos el directorio para lectura */
  if ((current_dir= opendir(name))==NULL) {
    perror(name);
    return;
  }
      
  if (getcwd(cwd, MAX_PATH+1)==NULL) {
    perror("getcwd");
    return;
  }
  if (chdir(name)!=0) {
    perror(name);
    return;
  }
      
  while ((this_entry= readdir(current_dir))!=NULL) {
    if (strcmp(this_entry->d_name, ".")==0 || 
        strcmp(this_entry->d_name, "..")==0)
      continue;   
      
    listdir(indent+1, this_entry->d_name);
  }         
      
  closedir(current_dir);
  chdir(cwd);
}       

int main(int argc, char *argv[]) {
  for (int i= 1; i<argc; i++)
    listdir(0, argv[i]);
  return 0;
}
