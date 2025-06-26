#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void do_ls(char[]);
void dostat(char *);
void show_file_info(char *, struct stat*);
void mode_to_letters(int, char []);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);

int main(int ac, char *av[])
{
    if(ac == 1)
        do_ls(".");
    else {
        while(--ac) {
            printf("%s:\n", *++av);
            do_ls(*av);
        }
    }
    return 0;
}

void do_ls(char dirname[])
{
    DIR *dir_ptr;
    struct dirent *direntp;
    char fullpath[PATH_MAX];

    if((dir_ptr = opendir(dirname)) == NULL)
        fprintf(stderr, "ls2: cannot open %s\n", dirname);
    else {
        while((direntp = readdir(dir_ptr)) != NULL) {

            if(direntp->d_name[0] == '.')
                continue;
            if(strcmp(dirname, ".") == 0)
                dostat(direntp->d_name);
            else {
                snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, direntp->d_name);
                dostat(fullpath);
            }
        }
        closedir(dir_ptr);
    }
}

void dostat(char *filename)
{
    struct stat info;
    if(stat(filename, &info) == -1)
        perror(filename);
    else
        show_file_info(filename, &info);
}

void show_file_info(char *filename, struct stat *info_p)
{
    char modestr[11];
    mode_to_letters(info_p->st_mode, modestr);


    printf("%s ", modestr);
    printf("%4d ", (int)info_p->st_nlink);
    printf("%-8s ", uid_to_name(info_p->st_uid));
    printf("%-8s ", gid_to_name(info_p->st_gid));
    printf("%8ld ", (long)info_p->st_size);
    printf("%.12s ", 4 + ctime(&info_p->st_mtime));
    printf("%s\n", filename);
}

void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");

    if(S_ISDIR(mode)) str[0] = 'd';
    if(S_ISCHR(mode)) str[0] = 'c';
    if(S_ISBLK(mode)) str[0] = 'b';

    if(mode & S_IRUSR) str[1] = 'r';
    if(mode & S_IWUSR) str[2] = 'w';
    if(mode & S_IXUSR) str[3] = 'x';

    if(mode & S_IRGRP) str[4] = 'r';
    if(mode & S_IWGRP) str[5] = 'w';
    if(mode & S_IXGRP) str[6] = 'x';

    if(mode & S_IROTH) str[7] = 'r';
    if(mode & S_IWOTH) str[8] = 'w';
    if(mode & S_IXOTH) str[9] = 'x';
}

char *uid_to_name(uid_t uid)
{
    struct passwd *pw_ptr;
    static char numstr[10];
    if((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid)
{
    struct group *grp_ptr;
    static char numstr[10];

    if((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
        return grp_ptr->gr_name;
}

