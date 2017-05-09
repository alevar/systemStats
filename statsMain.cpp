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

int pageSize = getpagesize();

// include timestamp for when the recording was made
// include all three load averages
// include top n users by cpu and by memory
// see if possible to not swap the process without root permission

struct DiskSpace {
    unsigned long asb; //available space in bytes
    unsigned long fsb; //free space in bytes
    unsigned long asp; //available space in percent
    unsigned long fsp; //free space in percent
};

struct RAM {
    unsigned long mtb; //total memory in bytes
    unsigned long mab; //memory available in bytes
};

struct OTHER {
    unsigned long stb; //total swap in bytes
    unsigned long sab; //available swap in bytes
    long upt; //uptime in seconds
};

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

DiskSpace getDiskSpace(const char *path){
    struct statvfs buf;
    struct stat fi;

    int ret = statvfs("/",&buf);
    unsigned long freeBlocks = buf.f_bfree;
    stat("/",&fi);
    unsigned long availableSpaceB = buf.f_bavail*buf.f_bsize;
    unsigned long freeSpaceB = buf.f_bfree*buf.f_bsize;
    unsigned long availableSpacePercent = 100.0 * (double)buf.f_bavail / (double)buf.f_blocks;
    unsigned long freeSpacePercent = 100.0 * (double)buf.f_bfree / (double)buf.f_blocks;
    return {availableSpaceB,freeSpaceB,availableSpacePercent,freeSpacePercent};
}

float getCPU(){
    long double a[4], b[4], loadavg;
    FILE *fp;
    char dump[50];

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    printf("The current CPU utilization is : %Lf\n",loadavg);

    return loadavg;
}

OTHER getOther(){
    struct sysinfo info;
    sysinfo(&info);
    return{info.totalswap,info.freeswap,info.uptime};
}

RAM getRAM(){
    FILE* fp = fopen( "/proc/meminfo", "r" );
    if ( fp != NULL )
    {
        size_t bufsize = 1024 * sizeof(char);
        char* buf = (char*)malloc( bufsize );
        long totalMem = -1L;
        long freeMem = -1L;
        long bufMem = -1L;
        long cacheMem = -1L;
        while ( getline( &buf, &bufsize, fp ) >= 0 )
        {
            if ( strncmp( buf, "MemTotal", 8 ) == 0 ){
                sscanf( buf, "%*s%ld", &totalMem );
            }
            if ( strncmp( buf, "MemFree", 7 ) == 0 ){
                sscanf( buf, "%*s%ld", &freeMem );
            }
            if ( strncmp( buf, "Buffers", 7 ) == 0 ){
                sscanf( buf, "%*s%ld", &bufMem );
            }
            if ( strncmp( buf, "Cached", 6 ) == 0 ){
                sscanf( buf, "%*s%ld", &cacheMem );
            }

        }
        fclose(fp);
        return{(size_t)totalMem,(size_t)(freeMem+cacheMem+bufMem)};
    }
}

/*
This function will likely iterate over the /proc/<procID>/stat and /proc/<procID>statm data
Sort the data by highest CPU and RAM consumption
and return top n processes

Perhaps it will only be called if the system load is above a certain threshhold
*/

bool comp(const std::tuple<long,int,std::string>& a, const std::tuple<long,int,std::string>& b){
    return std::get<1>(a) > std::get<1>(b);
}

void parseStatm(std::vector<std::tuple <long,int,std::string>> *procs) {
    DIR* proc = opendir("/proc");
    struct dirent* ent;
    long tgid;

    long double a[4];
    FILE *fp;
    char path[40];

    while(ent = readdir(proc)) {
        if(!isdigit(*ent->d_name))
            continue;

        tgid = strtol(ent->d_name, NULL, 10);

        snprintf(path, 40, "/proc/%ld/statm", tgid);
        fp = fopen(path,"r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
        procs->push_back(std::make_tuple(tgid,a[0],"sparrow"));
    }
    closedir(proc);

    sort(procs->begin(),procs->end(),comp);
    procs->resize(10);
}

void parseStat(procinfo *pinfo,std::vector<std::tuple <long,float,std::string>> *procs,long uptime){
    DIR* proc = opendir("/proc");
    int hertz = sysconf(_SC_CLK_TCK);
    struct dirent* ent;
    long tgid;

    long double a[33];
    FILE *fp;
    char path[40];

    while(ent = readdir(proc)) {
        if(!isdigit(*ent->d_name))
            continue;

        tgid = strtol(ent->d_name, NULL, 10);
        snprintf(path, 40, "/proc/%ld/stat", tgid);
        fp = fopen(path,"r");
        fscanf(fp,"%i %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
                  /*1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22   23  24  25  26  27  28  29  30  37 38 39 40 41   42  43  44  45  46  47  48  49  50  51 52*/
        &(pinfo->pid),                     //1
        &(pinfo->comm),                    //2
        &(pinfo->state),                   //3
        &(pinfo->ppid),                    //4
        &(pinfo->pgrp),                    //5
        &(pinfo->session),                 //6
        &(pinfo->tty_nr),                  //7
        &(pinfo->tpgid),                   //8
        &(pinfo->flags),                   //9
        &(pinfo->minflt),                  //10
        &(pinfo->cminflt),                 //11
        &(pinfo->majflt),                  //12
        &(pinfo->cmajflt),                 //13
        &(pinfo->utime),                   //14
        &(pinfo->stime),                   //15
        &(pinfo->cutime),                  //16
        &(pinfo->cstime),                  //17
        &(pinfo->priority),                //18
        &(pinfo->nice),                    //19
        &(pinfo->num_threads),             //20
        &(pinfo->itrealvalue),             //21
        &(pinfo->starttime),               //22
        &(pinfo->vsize),                   //23
        &(pinfo->rss),                     //24
        &(pinfo->rsslim),                  //25
        &(pinfo->startcode),               //26
        &(pinfo->endcode),                 //27
        &(pinfo->startstack),              //28
        &(pinfo->kstkesp),                 //29
        &(pinfo->kstkeip),                 //30
        &(pinfo->signal),                  //31
        &(pinfo->blocked),                 //32
        &(pinfo->sigignore),               //33
        &(pinfo->sigcatch),                //34
        &(pinfo->wchan),                   //35
        &(pinfo->nswap),                   //36
        &(pinfo->cnswap),                  //37
        &(pinfo->exit_signal),             //38
        &(pinfo->processor),               //39
        &(pinfo->rt_priority),             //40
        &(pinfo->policy),                  //41
        &(pinfo->delayacct_blkio_ticks),   //42
        &(pinfo->guest_time),              //43
        &(pinfo->cguest_time),             //44
        &(pinfo->start_data),              //45
        &(pinfo->end_data),                //46
        &(pinfo->start_brk),               //47
        &(pinfo->arg_start),               //48
        &(pinfo->arg_end),                 //49
        &(pinfo->env_start),               //50
        &(pinfo->env_end),                 //51
        &(pinfo->exit_code)                //52
        );
        fclose(fp);
        unsigned long totalTime = pinfo->utime+pinfo->stime;
        float seconds = (float)uptime-((float)pinfo->starttime/(float)hertz);
        float cpu_usage = 100*(((float)totalTime/(float)hertz)/seconds);
        procs->push_back(std::make_tuple(tgid,cpu_usage,"sparrow"));
    }
    closedir(proc);
    sort(procs->begin(),procs->end(),comp);
    procs->resize(10);
}

int main(int argc , char *argv[]){
    auto paramsDisk = getDiskSpace("/");
    float loadavg = getCPU();
    auto paramsRAM = getRAM();
    auto paramsOther = getOther();
    printf("Available Space Percent: %ld%%\n",paramsDisk.asp);
    printf("Free CPU load average: %lf%%\n",loadavg);
    printf("Free Memory Percent: %lf%%\n",(float)paramsRAM.mab/(float)paramsRAM.mtb);
    printf("Free Swap Percent: %lf%%\n",(float)paramsOther.sab/(float)paramsOther.stb);
    printf("Uptime: %lds\n",paramsOther.upt);

    std::vector<std::tuple <long,int,std::string>> procMem;
    parseStatm(&procMem);
    for(int i=0;i<procMem.size();i++){
        std::cout<<"PID: "<<std::get<0>(procMem[i])<<"  MEM: "<<(std::get<1>(procMem[i])*pageSize)/1024/1024<<"MB"<<std::endl;
    }
    std::cout<<"============================="<<std::endl;
    procinfo pinfo;
    std::vector<std::tuple <long,float,std::string>> procCPU;
    parseStat(&pinfo,&procCPU,paramsOther.upt);
    for(int i=0;i<procCPU.size();i++){
        std::cout<<"PID: "<<std::get<0>(procCPU[i])<<"  CPU: "<<std::get<1>(procCPU[i])<<"\%"<<std::endl;
    }

    return 0;
}