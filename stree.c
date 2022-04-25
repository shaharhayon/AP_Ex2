#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h>    /* Type definitions used by many programs */
#include <stdio.h>        /* Standard I/O functions */
#include <stdlib.h>       /* Prototypes of commonly used library functions,
                             plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>       /* Prototypes for many system calls */
#include <errno.h>        /* Declares errno and defines error constants */
#include <string.h>       /* Commonly used string-handling functions */
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define GRN  "\x1B[32m"
#define NRM  "\x1B[0m"

int count_directories = 0;
int count_files = 0;

void resetColor () {
    printf("%s", NRM);
}
void printColorGreen(){
    printf("%s", GRN);
}

void printPermissions(const mode_t mode){
        printf("[%s", (S_ISDIR(mode)) ? "d" : "-");
        printf("%s", (mode & S_IRUSR) ? "r" : "-");
        printf("%s", (mode & S_IWUSR) ? "w" : "-");
        printf("%s", (mode & S_IXUSR) ? "x" : "-");
        printf("%s", (mode & S_IRGRP) ? "r" : "-");
        printf("%s", (mode & S_IWGRP) ? "w" : "-");
        printf("%s", (mode & S_IXGRP) ? "x" : "-");
        printf("%s", (mode & S_IROTH) ? "r" : "-");
        printf("%s", (mode & S_IWOTH) ? "w" : "-");
        printf("%s", (mode & S_IXOTH) ? "x" : "-");
}

void printUser(const uid_t uid){
    struct passwd *pws;
    pws = getpwuid(uid);
    printf(" %-7s", pws->pw_name);
}

void printGroup(gid_t gid)
{
    struct group *g;
    g = getgrgid(gid);
    printf("%8s", g->gr_name);
}

void printSize(off_t size){
    printf("%14ld", size);
}
void printFile(const char* pathname, int base){
    printf("%s", "]  ");
    printColorGreen();
    printf("%s\n", &pathname[base]);     /* Print basename */
    resetColor();
}

static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    const char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    if(!strcmp(pathname, cwd)){
        printf("%s", ".\n");
        // printf("%s", "├"); 
        return 0;
    }
    if(ftwb->level > 1){
        printf("%s", "│");
    }
    for (int i = 1; i < 4 * (ftwb->level - 1); i++)
    {
        printf("%s", " ");
    }
    if (type == FTW_D && strcmp(pathname, ".") != 0)
    {
        count_directories++;
    }

    if (type == FTW_F)
    {
        count_files++;
    }
    
    // printf("%0*s", 4 * ((ftwb->level)-1), " ");
    // printf("%s", "├");
    // printf("%s", "── ");         /* Indent suitably */
//"└── "
    printf("%s","├── ");
    // printf("%s","└── ");
    if (type == FTW_NS) {                  /* Could not stat() file */
        printf("%s", "?");
    } 
    else {
        printPermissions(sbuf->st_mode);
        }
	printUser(sbuf->st_uid);
    printGroup(sbuf->st_gid);
    printSize(sbuf->st_size);
    printFile(pathname, ftwb->base);
    return 0;                                   /* Tell nftw() to continue */
}

main(int argc, char *argv[])
{
    int flags = 0;
    const char cwd[1024];
    int nftwError = nftw(getcwd(cwd, sizeof(cwd)), dirTree, 10, flags);
    if (nftwError == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    printf("\n%d directories, %d files\n", count_directories, count_files);
    exit(EXIT_SUCCESS);
}