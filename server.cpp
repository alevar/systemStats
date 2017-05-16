#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>

#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <utility>

#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <pwd.h>
#include <tuple>

#include <bitset>

typedef struct DiskSpace {
    unsigned long asb; //available space in bytes
    unsigned long fsb; //free space in bytes
    unsigned long asp; //available space in percent
    unsigned long fsp; //free space in percent
} disksp;
// #pragma pack(0)
// #pragma pack(1)
typedef struct RAM {
    unsigned long mtb; //total memory in bytes
    unsigned long mab; //memory available in bytes
} memory;
// #pragma pack(0)
// #pragma pack(1)
typedef struct OTHER {
    unsigned long stb; //total swap in bytes
    unsigned long sab; //available swap in bytes
    long upt; //uptime in seconds
} other;

// #pragma pack(0)
// #pragma pack(1)
typedef struct procSTAT {
    //"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d"
     /*1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22   23  24  25  26  27  28  29  30  37 38 39 40 41   42  43  44  45  46  47  48  49  50  51 52*/
    int pid; //%d The process ID.

    int comm; /*%s The filename of the executable, in parentheses.
                    This is visible whether or not the executable is
                    swapped out.*/

    int state; /*%c One of the following characters, indicating process
                        state:
                        R  Running
                        S  Sleeping in an interruptible wait
                        D  Waiting in uninterruptible disk sleep
                        Z  Zombie
                        T  Stopped (on a signal) or (before Linux 2.6.33)trace stopped
                        t  Tracing stop (Linux 2.6.33 onward)
                        W  Paging (only before Linux 2.6.0)
                        X  Dead (from Linux 2.6.0 onward)
                        x  Dead (Linux 2.6.33 to 3.13 only)
                        K  Wakekill (Linux 2.6.33 to 3.13 only)
                        W  Waking (Linux 2.6.33 to 3.13 only)
                        P  Parked (Linux 3.9 to 3.13 only)*/

    double ppid; //%d The PID of the parent of this process.

    double pgrp; //%d The process group ID of the process.

    double session; //%d The session ID of the process.

    double tty_nr; /*%d The controlling terminal of the process.  (The minor
                        device number is contained in the combination of
                        bits 31 to 20 and 7 to 0; the major device number is
                        in bits 15 to 8.)*/

    double tpgid; /*%d The ID of the foreground process group of the
                        controlling terminal of the process.*/

    unsigned int flags; /*%u The kernel flags word of the process.  For bit
                        meanings, see the PF_* defines in the Linux kernel
                        source file include/linux/sched.h.  Details depend
                        on the kernel version.

                        The format for this field was %lu before Linux 2.6.*/

    unsigned long minflt; /*%lu The number of minor faults the process has made
                        which have not required loading a memory page from
                        disk.*/

    unsigned long cminflt; /*%lu The number of minor faults that the process's
                        waited-for children have made.*/

    unsigned long majflt; /*%lu
                        The number of major faults the process has made
                        which have required loading a memory page from disk.*/

    unsigned long cmajflt; /*%lu The number of major faults that the process's
                        waited-for children have made.*/

    unsigned long utime; /*%lu Amount of time that this process has been scheduled
                        in user mode, measured in clock ticks (divide by
                        sysconf(_SC_CLK_TCK)).  This includes guest time,
                        guest_time (time spent running a virtual CPU, see
                        below), so that applications that are not aware of
                        the guest time field do not lose that time from
                        their calculations.*/

    unsigned long stime; /*%lu Amount of time that this process has been scheduled
                        in kernel mode, measured in clock ticks (divide by
                        sysconf(_SC_CLK_TCK)).*/

    unsigned long cutime; /*%ld Amount of time that this process's waited-for
                        children have been scheduled in user mode, measured
                        in clock ticks (divide by sysconf(_SC_CLK_TCK)).
                        (See also times(2).)  This includes guest time,
                        cguest_time (time spent running a virtual CPU, see
                        below).*/

    unsigned long cstime; /*%ld Amount of time that this process's waited-for
                        children have been scheduled in kernel mode,
                        measured in clock ticks (divide by
                        sysconf(_SC_CLK_TCK)).*/

    long double priority; /*%ld (Explanation for Linux 2.6) For processes running a
                        real-time scheduling policy (policy below; see
                        sched_setscheduler(2)), this is the negated
                        scheduling priority, minus one; that is, a number in
                        the range -2 to -100, corresponding to real-time
                        priorities 1 to 99.  For processes running under a
                        non-real-time scheduling policy, this is the raw
                        nice value (setpriority(2)) as represented in the
                        kernel.  The kernel stores nice values as numbers in
                        the range 0 (high) to 39 (low), corresponding to the
                        user-visible nice range of -20 to 19.

                        Before Linux 2.6, this was a scaled value based on
                        the scheduler weighting given to this process.*/

    long double nice; /*%ld The nice value (see setpriority(2)), a value in the
                        range 19 (low priority) to -20 (high priority).*/

    long double num_threads; /*%ld Number of threads in this process (since Linux 2.6).
                        Before kernel 2.6, this field was hard coded to 0 as
                        a placeholder for an earlier removed field.*/

    long double itrealvalue; /*%ld The time in jiffies before the next SIGALRM is sent
                        to the process due to an interval timer.  Since
                        kernel 2.6.17, this field is no longer maintained,
                        and is hard coded as 0.*/

    unsigned long long starttime; /*%llu The time the process started after system boot.  In
                        kernels before Linux 2.6, this value was expressed
                        in jiffies.  Since Linux 2.6, the value is expressed
                        in clock ticks (divide by sysconf(_SC_CLK_TCK)).

                        The format for this field was %lu before Linux 2.6.*/

    unsigned long vsize; /*%lu Virtual memory size in bytes.*/

    long double rss; /*%ld Resident Set Size: number of pages the process has
                        in real memory.  This is just the pages which count
                        toward text, data, or stack space.  This does not
                        include pages which have not been demand-loaded in,
                        or which are swapped out.*/

    unsigned long rsslim; /*%lu Current soft limit in bytes on the rss of the
                        process; see the description of RLIMIT_RSS in
                        getrlimit(2).*/

    unsigned long startcode; /*%lu  [PT] The address above which program text can run.*/

    unsigned long endcode; /*%lu  [PT] The address below which program text can run.*/

    unsigned long startstack; /*%lu  [PT] The address of the start (i.e., bottom) of the
                        stack.*/

    unsigned long kstkesp; /*%lu  [PT] The current value of ESP (stack pointer), as found
                        in the kernel stack page for the process.*/

    unsigned long kstkeip; /*%lu  [PT] The current EIP (instruction pointer).*/

    unsigned long signal; /*%lu The bitmap of pending signals, displayed as a
                        decimal number.  Obsolete, because it does not
                        provide information on real-time signals; use
                        /proc/[pid]/status instead.*/

    unsigned long blocked; /*%lu The bitmap of blocked signals, displayed as a
                        decimal number.  Obsolete, because it does not
                        provide information on real-time signals; use
                        /proc/[pid]/status instead.*/

    unsigned long sigignore; /*%lu The bitmap of ignored signals, displayed as a
                        decimal number.  Obsolete, because it does not
                        provide information on real-time signals; use
                        /proc/[pid]/status instead.*/

    unsigned long sigcatch; /*%lu
                        The bitmap of caught signals, displayed as a decimal
                        number.  Obsolete, because it does not provide
                        information on real-time signals; use
                        /proc/[pid]/status instead.*/

    unsigned long wchan; /*%lu  [PT] This is the "channel" in which the process is
                        waiting.  It is the address of a location in the
                        kernel where the process is sleeping.  The
                        corresponding symbolic name can be found in
                        /proc/[pid]/wchan.*/

    unsigned long nswap; /*%lu Number of pages swapped (not maintained).*/

    unsigned long cnswap; /*%lu Cumulative nswap for child processes (not
                        maintained).*/

    double exit_signal; /*%d  (since Linux 2.1.22) Signal to be sent to parent when we die.*/

    double processor; /*%d  (since Linux 2.2.8) CPU number last executed on.*/

    unsigned int rt_priority; /*%u  (since Linux 2.5.19) Real-time scheduling priority, a number in the range
                        1 to 99 for processes scheduled under a real-time
                        policy, or 0, for non-real-time processes (see
                        sched_setscheduler(2)).*/

    unsigned int policy; /*%u  (since Linux 2.5.19) Scheduling policy (see sched_setscheduler(2)).
                        Decode using the SCHED_* constants in linux/sched.h.

                        The format for this field was %lu before Linux
                        2.6.22.*/

    unsigned long long delayacct_blkio_ticks; /*%llu  (since Linux 2.6.18) Aggregated block I/O delays, measured in clock ticks
                        (centiseconds).*/

    unsigned long guest_time; /*%lu  (since Linux 2.6.24) Guest time of the process (time spent running a
                        virtual CPU for a guest operating system), measured
                        in clock ticks (divide by sysconf(_SC_CLK_TCK)).*/

    long double cguest_time; /*%ld  (since Linux 2.6.24) Guest time of the process's children, measured in
                        clock ticks (divide by sysconf(_SC_CLK_TCK)).*/

    unsigned long start_data; /*%lu  (since Linux 3.3)  [PT] Address above which program initialized and
                        uninitialized (BSS) data are placed.*/

    unsigned long end_data; /*%lu  (since Linux 3.3)  [PT] Address below which program initialized and
                        uninitialized (BSS) data are placed.*/

    unsigned long start_brk; /*%lu  (since Linux 3.3)  [PT] Address above which program heap can be expanded
                        with brk(2).*/

    unsigned long arg_start; /*%lu  (since Linux 3.5)  [PT] Address above which program command-line arguments
                        (argv) are placed.*/

    unsigned long arg_end; /*%lu  (since Linux 3.5)  [PT] Address below program command-line arguments (argv)
                        are placed.*/

    unsigned long env_start; /*%lu  (since Linux 3.5)  [PT] Address above which program environment is placed.*/

    unsigned long env_end; /*%lu  (since Linux 3.5)  [PT] Address below which program environment is placed.*/

    double exit_code; /*%d  (since Linux 3.5)  [PT] The thread's exit status in the form reported by
                        waitpid(2).*/
} procinfo;

typedef struct Stats {
    unsigned long asb; //available space in bytes
    unsigned long fsb; //free space in bytes
    unsigned long asp; //available space in percent
    unsigned long fsp; //free space in percent

    unsigned long mtb; //total memory in bytes
    unsigned long mab; //memory available in bytes

    unsigned long stb; //total swap in bytes
    unsigned long sab; //available swap in bytes

    long upt; //uptime in seconds

    long loadavg;
} stats;

#define PORT    1234        // Port used by the server service
#define BACKLOG 1           // Number of connections to queue
#define BUFFERLENGTH 3000000    // 300 bytes

void deserialize(char *data, Stats* msgPacket)
{
    unsigned long *q = (unsigned long*)data;    
    msgPacket->asb = *q;     q++;    
    msgPacket->asp = *q;     q++;    
    msgPacket->fsb = *q;     q++;
    msgPacket->fsp = *q;     q++;

    msgPacket->mtb = *q;     q++;
    msgPacket->mab = *q;     q++;

    msgPacket->stb = *q;     q++;
    msgPacket->sab = *q;     q++;

    long *w = (long*)q;
    msgPacket->upt = *w;     w++;
    msgPacket->loadavg = *w; w++;
}

int main(int argc , char *argv[])
{
    int c;

    int serverPort;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"port",     required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "t:d:0", long_options, &option_index);
        if (c == -1){
            break;
        }

        switch (c) {
            case 0:
                serverPort = atoi(optarg);
                break;

            case '?':
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    char parrentBuffer[BUFFERLENGTH];
    int parentSocket, childSocket, addrlen;
    struct sockaddr_in server, client;

    if((parentSocket = socket(AF_INET , SOCK_STREAM , 0)) < 0)
    {
        printf("Could not create socket");
        close(parentSocket);
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("0.0.0.0"); // Autofill with my ip (lcalhost)
    server.sin_port = htons( serverPort );

    memset(&(server.sin_zero), 0, 8); // Set zero values to the rest of the server structure
     
    //Bind
    if( bind(parentSocket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        close(parentSocket);
        exit(-1);
    }
    puts("binding successful");
     
    listen(parentSocket , BACKLOG);
    puts("Waiting for incoming connections...");
    addrlen = sizeof(struct sockaddr_in);

    while(true){
        if((childSocket = accept(parentSocket, (struct sockaddr *)&client, (socklen_t*)&addrlen)) < 0){
            close(childSocket);
            exit(-1);
        }

        else{
            switch(pid_t new_fork = fork())
            {
                case -1: // Error
                    {
                        perror("forking failed");
                        close(parentSocket);
                        close(childSocket);
                        exit(-1);
                    }
                case 0: // Child
                    {
                        bool connectionStatus = true;

                        while (connectionStatus){

                            close(parentSocket);

                            char *client_ip = inet_ntoa(client.sin_addr);
                            int client_port = ntohs(client.sin_port);

                            int numbytes;
                            char childBuffer[BUFFERLENGTH];
                            float number;
                            Stats disksp;

                            std::string putData = "close";

                            char data[sizeof(Stats)];

                            std::cout<<sizeof(Stats)<<std::endl;

                            if((numbytes = recv(childSocket, &data, sizeof(data), 0)) == -1){

                                std::cout<<"NO"<<std::endl;
                                perror("recv()");
                                exit(1);
                            }
                            else{
                                Stats* temp = new Stats;
                                deserialize(data, temp);

                                childBuffer[numbytes] = '\0';
                                std::cout<<sizeof(disksp)<<std::endl;
                                printf("MessageIn: \n\tasb>%lu \n\tfsb>%lu \n\tasp>%lu \n\tfsp>%lu \n\tupt>%li \n\tloadavg>%li\n",temp->asb,temp->fsb,temp->asp,temp->fsp,temp->upt,temp->loadavg);
                                // std::cout << "Message: " << disksp.asb << std::endl;
                                send(childSocket,putData.c_str(),putData.size(),MSG_CONFIRM);
                                close(childSocket);
                                exit(-1);
                            }
                        }

                        // close(childSocket);
                        // exit(-1);
                    }
                default:    // Parent
                    close(childSocket);
                    continue;
            }
        }
    }
     
    return 0;

}