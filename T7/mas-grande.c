#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH 1000

char filename[MAX_PATH + 1];
ssize_t filesize = 0;

void mas_grande(int indent, char *name, char *path) {
    DIR *current_dir;
    struct dirent *this_entry;
    char cwd[MAX_PATH + 1];
    struct stat status;
    
    // se agrega el directorio o archivo actual <name> al path completo actual
    char current_path[MAX_PATH + 1] = {0};
    if (strcmp(path, "") == 0) {
        strcpy(current_path, name);
    } else {
        snprintf(current_path, sizeof(current_path), "%s/%s", path, name);
    }

    // stat() returna 0 si es exitoso
    if (stat(name, &status) != 0) {
        perror(name);
        exit(1);
    }

    // si no es directorio -> es archivo -> comparamos su tamano con el maximo actual
    if (!S_ISDIR(status.st_mode)) {
        if (status.st_size > filesize) {
            strcpy(filename, current_path);
            filesize = status.st_size;
        }
        return;
    }
    /* abrimos directorio para lectura */
    if ((current_dir = opendir(name)) == NULL) {
        perror(name);
        return;
    }

    // almacenar el directorio actual en la variable <cwd>
    if (getcwd(cwd, MAX_PATH + 1) == NULL) {
        perror("getcwd");
        return;
    }
    if (chdir(name) != 0) {
        perror(name);
        return;
    }

    // por cada directorio o archivo dentro del directorio actual:
    while ((this_entry = readdir(current_dir)) != NULL) {
        if (strcmp(this_entry->d_name, ".") == 0 || strcmp(this_entry->d_name, "..") == 0)
            continue;
        // recursivamente buscar el mas grande
        mas_grande(indent + 1, this_entry->d_name, current_path);
    }

    // volver al directorio <cwd> previo a las recursiones
    closedir(current_dir);
    chdir(cwd);
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++)
        mas_grande(0, argv[i], "");
    if (filesize)
        printf("mas grande: %s de %ld bytes\n", filename, filesize);
    return 0;
}