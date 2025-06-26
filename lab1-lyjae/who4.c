#include <stdio.h>
#include <sys/types.h>
#include <utmp.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define SHOWHOST

int utmp_open(char *);
void utmp_close();
void show_info(struct utmp *);
void showtime(time_t);

int main(){
        struct utmp *utbufp,
                    *utmp_next();
        if(utmp_open(UTMP_FILE) == -1){
                perror(UTMP_FILE);
                exit(1);
        }
        while( ( utbufp=utmp_next() ) != ((struct utmp *) NULL))
                show_info(utbufp);
        utmp_close();
        return 0;
}

void show_info(struct utmp *utbufp){
        if(utbufp->ut_type != USER_PROCESS)
                return;
        printf("%-8.8s", utbufp->ut_name);
        printf(" ");
        printf("%-12.12s", utbufp->ut_line);
        printf(" ");
        showtime(utbufp->ut_time);

#ifdef SHOWHOST
        if (utbufp->ut_host[0]!= '\0')
                printf(" (%s)", utbufp->ut_host);
#endif
        printf("\n");
}

void showtime(time_t timeval)
{
        struct tm *tm_ptr;
        char buf[64];

       
        tm_ptr = localtime(&timeval);
       
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tm_ptr);
       
        printf("%16s", buf);
}
