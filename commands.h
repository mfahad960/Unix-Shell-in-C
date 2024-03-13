//This file includes implementation of actual commands
//Each command has at least 1 function
//36 commands currently coded
//Total 42 functions in file


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h> 	//for reporting and retrieving error conditions using the symbol errno(error number)
#include <string.h>	//string manipulation
#include <sys/wait.h>   //used for holding processes
#include <sys/sysinfo.h>	//information about the system environment.
#include <sys/types.h>	   //defines data types used in system source code
#include <sys/stat.h>	  //contains constructs that facilitate getting information about files attributes
#include <sys/utsname.h>  //os and kernel info structures
#include <fcntl.h>	//contains constructs that refer to file control, e.g. opening a file, retrieving and changing the permissions of file, locking a file for edit, etc.
#include <errno.h>	//reporting error numbers
#include <pwd.h>
#include <grp.h>	//system group database
#include <time.h>  	//for system date/time
#include <utmp.h>	//current user attributes
#include <sys/vfs.h>
#include <math.h>	//standard math functions
#include <dirent.h>  //contains constructs that facilitate directory traversing.

#define SIZE 9999999
#define NLINE '\n'
#define FN "/etc/mtab"
#define FORMAT "%5s %s\t%8s %s\n"
#define BUFFER_SIZE 1024

//prototypes
int func_makers(int argc, char **argv);
int func_help(int argc, char **args);
int func_clear(int argc, char **argv);
int func_exit(int argc, char **argv);
int func_cd(int argc, char **argv);
int func_whoami(int argc, char **argv);
int func_link(int argc, char **argv);
int func_tac(int argc, char **argv);
int func_ls(int argc, char **argv);
int func_date(int argc, char **argv);
int func_cal(int argc, char **argv);
int func_uname(int argc, char **argv);
int func_finger(int argc, char **argv);
int func_df(int argc, char **argv);
int func_gedit(int argc, char **argv);
int func_nano(int argc, char **argv);
int func_grep(int argc, char **argv);
int func_head(int argc, char **argv);
int func_echo(int argc, char **argv);
int func_kill(int argc, char **argv);
int func_mkdir(int argc, char **argv);
int func_ps(int argc, char **argv);
int func_mv(int argc, char **argv);
int func_touch(int argc, char **argv);
int func_tail(int argc, char **argv);
int func_cat(int argc, char **argv);
int func_rmdir(int argc, char **argv);
int func_rm(int argc, char **argv);
int func_cp(int argc, char **argv);
int func_shutdown(int argc, char **argv);
int func_reboot(int argc, char **argv);
int func_hostname(int argc, char **argv);
int func_pwd(int argc, char **argv);
int func_chown(int argc, char **argv);
int func_chmod(int argc, char **argv);
int func_ifconfig(int argc, char **argv);
int func_uptime(int argc, char **argv);
int func_free(int argc, char **argv);

//function pointers for all command functions
int (*func_ptr[]) (int , char **) = {
  &func_pwd,
  &func_cd,
  &func_help,
  &func_exit,
  &func_uptime,
  &func_free,
  &func_chown,
  &func_clear,
  &func_chmod,
  &func_ifconfig,
  &func_link,
  &func_whoami,
  &func_tac,
  &func_ls,
  &func_date,
  &func_cal,
  &func_df,
  &func_gedit,
  &func_nano,
  &func_grep,
  &func_head,
  &func_echo,
  &func_kill,
  &func_mkdir,
  &func_mv,
  &func_ps,
  &func_uname,
  &func_finger,
  &func_touch,
  &func_tail,
  &func_shutdown,
  &func_reboot,
  &func_rmdir,
  &func_rm,
  &func_cp,
  &func_cat,
  &func_hostname,
  &func_makers };

//all available commands
char *commands_array[] = {
  "pwd",
  "cd",
  "help",
  "exit",
  "uptime",
  "free",
  "chown",
  "clear",
  "chmod",
  "ifconfig",
  "ln",
  "whoami",
  "tac",
  "ls",
  "date",
  "cal",
  "df",
  "gedit",
  "nano",
  "grep",
  "head",
  "echo",
  "kill",
  "mkdir",
  "mv",
  "ps",
  "uname",
  "finger",
  "touch",
  "tail",
  "shutdown",
  "reboot",
  "rmdir",
  "rm",
  "cp",
  "cat",
  "hostname",
  "makers" };

//calculates and returns number of functions for commands
int func_num_builtins() {
  return sizeof(commands_array) / sizeof(char *);
}

int func_help(int argc, char **args){
  int i;
  printf("\t\t\t <---SHELL COMMANDS HELP--->\n");
  printf("Type the command name and arguments, then press enter to execute the command.\n");
  printf("Supported commands are listed below:\n");

  for (i = 0; i < func_num_builtins(); i++) {
    printf("  %s\n", commands_array[i]);
  }

  return 1;
}

int func_clear(int argc, char **argv){
    const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
    printf("\b\b");
    return 1;
}

int func_exit(int argc, char **argv){
    return 0;
}

char *split(char *deli){
    char buffer[100];
    getcwd(buffer, 100);
    char *s = strtok(buffer, deli);   //splitting in tokens
    char *temp;
    while (s != NULL){
        temp = s;
        s = strtok(NULL, deli);
    }
    return temp;
}

int func_cd(int argc, char **argv){
    char *temp = split("/");
    if (argc == 1 || !strcmp(argv[1], "~")){
        while (strcmp(temp, getenv("USER")) != 0){
            chdir("..");                  //changing working directory
            temp = split("/");
        }
    }
    else{
        if (chdir(argv[1]) != 0)
            perror("lsh");
    }
    return 1;
}

int func_whoami(int argc, char **argv){
    char *p = getenv("USER");
    if (p){
        printf("%s\n", p);
    }
    return 1;
}

int func_link(int argc, char **argv){
    int i;
    struct stat s;

    if (argc < 3){
        perror("ERROR:Too Few Arguments");
        return 1;
    }
    if (argc >= 4){
        perror("ERROR:Too Many Arguments");
        return 1;
    }
    if (argc == 3){
        i = 0;
    }
    if (access(argv[i + 1], F_OK)){
        perror("ERROR:File name not Found");
        return 1;
    }
    if (!access(argv[i + 2], F_OK)){
        perror("ERROR:File Name already exist");
        return 1;
    }
    if (stat(argv[i + 1], &s) < 0){
        perror("ERROR:Unable to reterive stat information");
        return 1;
    }
    if (!S_ISREG(s.st_mode)){
        perror("ERROR:Not a Regular File");
        return 1;
    }
    if (argc == 3){
        if (link(argv[i + 1], argv[i + 2]) < 0){
            perror("ERROR:Unable to create the Link");
            return 1;
        }
    }
    return 1;
}

int func_tac(int argc, char **argv){ 	//tac used to concatenate and print files in reverse
    char buffer[SIZE];
    char *p1, *p2, *p3, *p4;
    struct stat *fp;
    int fd;
    fp = (struct stat *)malloc(sizeof(struct stat));

    if (argc != 2){
        fprintf(stderr, "input error %s \n", strerror(errno));
        return 1;
    }
    if ((fd = open(argv[1], O_RDONLY)) == -1){
        fprintf(stderr, "open error %s \n", strerror(errno));
        return 1;
    }
    if (fstat(fd, fp) == -1){
        fprintf(stderr, "fstat error %s \n", strerror(errno));
        return 1;
    }
    if (fp->st_size > (SIZE - 1)){
        fprintf(stderr, "buffer size is too low \n");
        return 1;
    }
    if (read(fd, buffer, fp->st_size) == -1){
        fprintf(stderr, "read error.\n");
        return 1;
    }

    p1 = strchr(buffer, NLINE);  // give the first occurence
    p2 = strrchr(buffer, NLINE); // give the last occurence
    *p2 = '\0';

    do{
        p2 = strrchr(buffer, NLINE);
        p4 = p2;
        p3 = p2 + sizeof(char);
        printf("%s\n", p3);
        *p4 = '\0';
    } while (p2 != p1);

    if (p2 == p1){
        *p2 = '\0';
        printf("%s\n", buffer);
    }
    return 1;
}

int func_touch(int argc, char **argv){	  //used to create(empty file) file(s).
    if (argc <= 1){
        printf("missing file operand\n");
        return 1;
    }
    else{
        int i = 1;
        while (argc != 1){
            FILE *fp;
            fp = fopen(argv[i], "a+"); 	//open file for read and write
            i++;
            argc--;
        }
    }
    return 1;
}

int func_chmod(int argc, char **argv){
    if (argc == 3){
        int i;
        i = strtol(argv[1], 0, 8);	//converts the initial part of the string to a long int value according to the given base.
        if (chmod(argv[2], i) < 0){     //builtin function to change file permissions
            perror("Error");
        }
    }
    else{
        printf("missing operands\n");
    }
    return 1;
}

int func_chown(int argc, char **argv){
    uid_t uid;	//user id
    gid_t gid;	//group id
    struct passwd *pwd;	//from pwd.h
    struct group *grp;

    if (argc == 3){
        char *temp = argv[1];
        if (strstr(temp, ":")){
            char *res = strtok(temp, ":");
            char *user = res;
            char *group;
            while (res != NULL){
                group = res;
                res = strtok(NULL, ":");
            }
            grp = getgrnam(group);	//get information about the group with a given name
            if (grp){
                gid = grp->gr_gid;
	    }
            else{
                printf("Invalid user %s\n", argv[1]);
                return 1;
            }
            pwd = getpwnam(user);	//get information about the user with a given name
            if (pwd == NULL){
                printf("Invalid user %s\n", argv[1]);
                return 1;
            }
            uid = pwd->pw_uid;
        }
        else{
            gid = 1000;
            pwd = getpwnam(argv[1]);
            if (pwd == NULL){
                printf("Invalid user %s\n", argv[1]);
                return 1;
            }
            uid = pwd->pw_uid;
        }
        if (chown(argv[2], uid, gid) == -1){	//change ownership
            perror("chown fail");
	}
    }
    else{
        printf("missing operands\n");
    }
    return 1;
}

int func_ifconfig(int argc, char **argv){	//network interface information
    if (argc == 1){
        FILE *fp;
        char returnData[64];
        fp = popen("/sbin/ifconfig", "r");

        while (fgets(returnData, 64, fp) != NULL){
            printf("%s", returnData);
        }
    }
    else{
        printf("Error in fetching network interface information\n");
    }
    return 1;
}

FILE *file(char *name){
    FILE *fp;
    if (!(fp = fopen(name, "r"))){  	//open file for reading
        printf("Error: %s", name);
    }
    return (fp);
}

int func_uptime(int argc, char **argv){
    double load[3];
    time_t now;
    struct tm ts;
    time(&now);
    ts = *localtime(&now);
    FILE *fp;
    int numberOfUsers = 0;
    struct sysinfo sys_info;
    struct group *grp;
    struct utmp user;

    gid_t gid;
    char **users;
    int days, hours, mins, x = 1;

    if (sysinfo(&sys_info) != 0){
        perror("sysinfo");
    }

    days = sys_info.uptime / 86400;	//get uptime from system and conversion
    hours = (sys_info.uptime / 3600) - (days * 24);
    mins = (sys_info.uptime / 60) - (days * 1440) - (hours * 60);

    fp = file(_PATH_UTMP);
    while (fread((char *)&user, sizeof(user), 1, fp) == 1){
        if (*user.ut_name && *user.ut_line && *user.ut_line != '~'){		//current user info
            numberOfUsers++;
        }
    }
    printf("\033[1;33mTime \033[0;m%d:%d:%d \033[1;33mUptime\033[0;m", ts.tm_hour, ts.tm_min, ts.tm_sec);
    if (days != 0){
        printf(" %d %d:%d, ", days, hours, mins);
    }
    else if (hours != 0){
        printf(" %d:%d, ", hours, mins);
    }
    else{
        printf(" %d min, ", mins);
    }
    printf(" %d user, ", --numberOfUsers);
    if (getloadavg(load, 3) != -1){
        printf("\033[1;33m  load average : \033[0;m%.2f , %.2f , %.2f \n", load[0], load[1], load[2]);
    }
    return 1;
}

int func_gedit(int argc, char **argv){
    char buffer[100]; 
    if (argc == 1){
        system(argv[0]);
    }
    else{
        int i = 1;
        while (argc != 1){
            char *temp = argv[i];
            snprintf(buffer, sizeof(buffer), "gedit %s",temp);                               
            system(buffer); 
            i++;
            argc--;
        }
    }
    return 1;
}

int func_nano(int argc, char **argv){
    char buffer[100]; 
    if (argc == 1){
        system(argv[0]);
    }
    else{
        int i = 1;
        while (argc != 1){
            char *temp = argv[i];
            snprintf(buffer, sizeof(buffer), "nano %s",temp);                                   
            system(buffer); 
            i++;
            argc--;
        }
    }
    return 1;
}

int func_free(int argc, char **argv){
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0){	//get system info
        perror("sysinfo");
        return 1;
    }
    printf("\033[1;33m  Total Ram: \033[0;m %ldk \t Free: %ldk \n", sys_info.totalram / 1024, sys_info.freeram / 1024);
    printf(" \033[1;33m Shared Ram: \033[0;m %ldk ", sys_info.sharedram / 1024);
    printf("  Buffered Ram: %ldk \n", sys_info.bufferram / 1024);
    printf("\033[1;33m  Total Swap: \033[0;m %ldk \t Free swap: %ldk \n", sys_info.totalswap / 1024, sys_info.freeswap / 1024);
    printf("\033[1;33m  Total High Memory: \033[0;m %ldk  Free high memory: %ldk \033[0;m \n", sys_info.totalhigh / 1024, sys_info.freehigh / 1024);
    printf(" \n");
    return 1;
}

int func_df(int argc, char **argv){
    char *array[] = {"udev", "tmpfs", "/dev"};
    const int SIZE_1 = 30000; 
    const char SPACE = ' ';
    struct statfs buffer;
    int usage;
    FILE *fp;
    printf("\033[1;33m Filesystem\t1K-blocks\tUsed\tAvailable\tUse%%\tMounted on\033[0;m\n");
    for (int i = 0; i < 3; i++){
        if ((fp = fopen(FN, "r")) == NULL){
            fprintf(stderr, "%s \n", strerror(errno));
            return 1;
        }
        char temp[SIZE_1];
        char *pt1;
        char *pt2;
        char *pt3;
        int c=0;
        while (fgets(temp, SIZE_1, fp) != NULL){ 
            c++;
            pt1 = strchr(temp, SPACE);
            pt2 = pt1 + sizeof(char);
            *pt1 = '\0';
            pt3 = strchr(pt2, SPACE);
            *pt3 = '\0';
            if (strstr(temp, array[i]) != NULL){	//searches for the first occurrence of a string inside another string
                if(c<=50){
			statfs(pt2, &buffer);      //system call returns information about a mounted filesystem.
			usage = ceil((buffer.f_blocks - buffer.f_bfree) * 100 / buffer.f_blocks);
			printf("%8s", temp);		//format for output
			printf("\t%9ld", buffer.f_blocks);
			printf("%11ld", buffer.f_blocks - buffer.f_bfree);
			printf("\t%9ld", buffer.f_bavail);
			printf("\t%d%%", usage);
			printf("\t%s", pt2);
			printf("\n");
		}
                else break;
            }
        }
    }
    return 1;
}

int func_head(int argc, char **argv){	//prints first n lines of file
    FILE *fp;
    char *line = NULL;
    int len = 0;
    size_t i = 0;
    int cnt = 0;

    if (argc == 3){
        if (argv[1][0] != '-'){
            len = 10;
	}
        else{
            len = atoi(argv[1]) * -1;
            if (len <= 0)
                len = 10;
        }
        fp = fopen(argv[2], "r");
        if (fp == NULL){
            printf("\n%s file can not be opened!\n", argv[2]);
            return 1;
        }
    }
    else{
        len = 10;
        fp = fopen(argv[1], "r");
        if (fp == NULL){
            printf("\n%s file can not be opened!\n", argv[2]);
            return 1;
        }
    }
    while (getline(&line, &i, fp) != -1){
        cnt++;
        if (cnt > len){
            break;
	}
        printf("%s", line);
        fflush(stdout);
    }

    fclose(fp);
    return 1;
}

//cat command functions

void readfile(char **argv, int bflag, int nflag, int argc, int flag){
    int fp, i, ch;
    for (i = 1; i < argc; i++){
        if (i != flag){
            FILE *fp = fopen(argv[i], "r+"); //open the file for read and write.

            if (fp == NULL){
                printf("%s file is not present.\n", argv[i]);
            }
            else{

                char buffer[BUFFER_SIZE];
                int index = 0;
                while (fgets(buffer, BUFFER_SIZE, fp)){

                    if (nflag){
                        printf("     %d  %s", index, buffer);
                        index++;
                    }
                    if (bflag){
                        if (*buffer == '\n'){
                            printf("%s", buffer);
                        }
                        else{
                            printf("     %d  %s", index, buffer);
                            index++;
                        }
                    }
                    else{
                        printf("%s", buffer);
                    }
                }
                fclose(fp);
                printf("\n");
            }
        }
    }
}

void appendFile(char **argv, int flag, int check){
    FILE *fp;
    if (check){
        fp = fopen(argv[flag + 1], "w+");
    }
    else{
        fp = fopen(argv[flag + 1], "a+");
    }
    for (int i = 1; i < flag; i++){
        FILE *fp1 = fopen(argv[i], "r+"); //open the file for read and write.

        if (fp1 == NULL){
            printf("%s file is not present.\n", argv[i]);
        }
        else{

            char buffer[BUFFER_SIZE];
            int index = 0;
            while (fgets(buffer, BUFFER_SIZE, fp1)){
                fputs(buffer, fp);
            }
            fclose(fp1);
        }
    }
    fclose(fp);
}

int func_cat(int argc, char **argv){
    int flag = 0;
    if (argc >= 2){

        for (int i = 1; i < argc; i++){
            if (argv[i][0] == '-' && argv[i][1] == 'n'){
                flag = 1;
                readfile(argv, 0, 1, argc, i);
                break;
            }
            else if (argv[i][0] == '-' && argv[i][1] == 'b'){
                flag = 1;
                readfile(argv, 1, 0, argc, i);
                break;
            }
            else if (strcmp(argv[i], ">") == 0){
                flag = 1;
                appendFile(argv, i, 1);
                break;
            }
            else if (strcmp(argv[i], ">>") == 0){
                flag = 1;
                appendFile(argv, i, 0);
		break;
            }
        }
    }
    if (flag == 0){
        readfile(argv, 0, 0, argc, 0);
    }
    return 1;
}

//cat command functions

int func_tail(int argc, char **argv){	//print last n lines of file
    FILE *fp;
    unsigned long long pos;
    char str[2 * SIZE];
    int len = 0;
    int cnt = 0;

    if (argc == 3){
        if (argv[1][0] != '-'){
            len = 10;
	}
        else{
            len = atoi(argv[1]) * -1; //string to int
            if (len <= 0){
                len = 10;
	    }
        }
        fp = fopen(argv[2], "r");
        if (fp == NULL){
            printf("\n%s file can not be opened !!!\n", argv[2]);
            return 1;
        }
    }
    else{
        len = 10;
        fp = fopen(argv[1], "r");
        if (fp == NULL){
            printf("\n%s file can not be opened !!!\n", argv[2]);
            return 1;
        }
    }

    // Go to End of file
    if (fseek(fp, 0, SEEK_END)){
        return 1;
    }
    else{
        pos = ftell(fp);

        // search for '\n' on last line
        while (pos){
            // Move pos lines away from end of file.
            if (!fseek(fp, --pos, SEEK_SET)){
                fgetc(fp);
                if (fgetc(fp) == '\n'){
                    if (cnt++ == len)
                        break;
                }
            }
            else
                return 1;
        }

        while (fgets(str, sizeof(str), fp)){
            printf("%s", str);
	}
    }
    printf("\n");
    return 1;
}

int func_mkdir(int argc, char **argv){
    if (argc >= 2){
        int i = 1;
        while (argc != 1){
            if (mkdir(argv[i], 0777) == -1){	//function to create a new directory with name path
                printf("could not create directory '%s': File exists\n", argv[i]);
	    }
            i++;
            argc--;
        }
    }
    else{
        printf("missing operand\n");
    }
    return 1;
}

int func_rmdir(int argc, char **argv){
    if (argc < 2){
        printf("missing operand\n");
    }
    else{
        int i = 1;
        while (argc != 1){
            if (rmdir(argv[i]) == -1){		//function to remove a directory with name path
                printf("failed to remove '%s': No such file or directory\n", argv[i]);
		}
            i++;
            argc--;
        }
    }
    return 1;
}

int func_uname(int argc, char **argv){	//get OS/kernel name/version/release
    struct utsname buffer;	//os info
    if (uname(&buffer) < 0)
        return 1;
    if (argc == 1){
        printf("%s\n", buffer.sysname);
    }
    else if (argc == 2){
        if (argv[1][1] == 'r'){
            printf("%s\n", buffer.release);
        }
	else if (argv[1][1] == 'v'){
            printf("%s\n", buffer.version);
        }
	else if (argv[1][1] == 'm'){
            printf("%s\n", buffer.machine);
        }
        else{
            printf("invalid option -- '%c'\n", argv[1][1]);
        }
    }
    return 1;
}

int func_finger(int argc, char **argv){
    if(argc == 1){
	system("finger");
    }
    else{
	char *line = malloc(sizeof(char) * BUFFER_SIZE);
	int i = 0;
	while(*(argv+i) != NULL){
		if(i != 0){
			strcat(line, " ");
		}
		strcat(line, *(argv+i));
		i++;
	}
	system(line);
    }
    return 1;
}

int func_ps(int argc, char **argv){
    system("ps");
    return 1;
}

int func_kill(int argc, char **argv){
    if (argc < 2){
        printf("usage: kill PID\n");
        return 1;
    }

    kill(atoi(argv[1]), SIGKILL);	//kill after string to integer conversion
    return 1;
}

int func_pwd(int argc, char **argv){
    long size;
    char *buf;

    size = pathconf(".", _PC_PATH_MAX); // returns the maximum length of a relative path name.

    if ((buf = (char *)malloc((size_t)size)) != NULL){
        getcwd(buf, (size_t)size);
    }
    printf("%s", buf);

    return 1;
}

int func_rm(int argc, char **argv){
    if (argc <= 1){
        printf("rm: missing operand\n");
    }
    else{
        int i = 1;
        while (argc != 1){
            if (remove(argv[i]) != 0){		//builtin
                printf("rm: cannot remove '%s': No such file or directory\n", argv[i]);
	    }
            i++;
            argc--;
        }
    }
    return 1;
}

int func_shutdown(int argc, char **argv){
    system("shutdown -P now");
    return 1;
}

int func_reboot(int argc, char **argv){
    system("reboot");
    return 1;
}

int func_mv(int argc, char **argv){
    if (argc == 1){
        printf("mv: missing file operand\n");
    }
    else if (argc == 2){
        printf("mv: missing destination file operand after '%s'\n", argv[1]);
    }
    else{
        char *file = argv[1];
        char *location = argv[2];
        char newplace[50];

        if (location[0] == '/'){ //check to see if input is a path{
            strcat(location, "/"); //if argument is a Full Path, prepare to mv to end of path.
            strcat(location, file);
            if (rename(file, location) != 0){
                printf("Error:\nDirectory not found\n");
		}
        }
        else{
            DIR *isD;
            isD = opendir(location); // check if argument is a DIR in CWD

            if (isD == NULL){
                if (rename(file, location) != 0){
                    printf("Error: File not moved\n");
		    }
            }
            else{
                char *ptrL;
                ptrL = getcwd(newplace, 50); // get current working directory path
                strcat(newplace, "/");
                strcat(newplace, location); // attach mv location to path ptrL
                strcat(newplace, "/");
                strcat(newplace, file); // keep original file name
                if (rename(file, ptrL) != 0){
                    printf("Error:\nDirectory not found in CWD\n");
		}
                closedir(isD);
            }
        }
    }
    return 1;
}

int func_cp(int argc, char **argv){
    int srcFD, destFD, nbread, nbwrite;
    char *buff[BUFFER_SIZE];
    if (argc == 1){
        printf("cp: missing file operand\n");
    }
    else if (argc == 2){
        printf("cp: missing destination file operand after '%s'\n", argv[1]);
    }
    else{
        /*Open source file*/
        srcFD = open(argv[1], O_RDONLY);

        if (srcFD == -1){
            printf("\nError opening file %s errno = %d\n", argv[1], errno);
            return 1;
        }

        destFD = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        if (destFD == -1){
            printf("\nError opening file %s errno = %d\n", argv[2], errno);
            return 1;
        }

        while ((nbread = read(srcFD, buff, BUFFER_SIZE)) > 0){
            if (write(destFD, buff, nbread) != nbread){
                printf("\nError in writing data to %s\n", argv[2]);
		}
        }

        if (nbread == -1){
            printf("\nError in reading data from %s\n", argv[1]);
	}
        if (close(srcFD) == -1){
            printf("\nError in closing file %s\n", argv[1]);
	}
        if (close(destFD) == -1){
            printf("\nError in closing file %s\n", argv[2]);
	}
    }
    return 1;
}

int func_ls(int argc, char **argv){
    DIR*p;
    struct dirent *d;		//directory entry
    if(argc == 1){
	system("ls");
	return 1;
    }
    char *dirname = malloc(sizeof(char) * BUFFER_SIZE);
    dirname = *(argv+1);
    p = opendir(dirname);
    if(p == NULL){
	printf("list_dir ; Cannot find directory : %s \n", dirname);
        exit(EXIT_FAILURE);
    }
    while(d = readdir(p)){
	printf("%s\n",d->d_name);
    }
    return 1;
}

int func_grep(int argc, char **argv){
    int i = 2;
    if (argc >= 3){
        while (argc != 2){
            char temp[200];
            char *pat = argv[1];
            FILE *fp;
            fp = fopen(argv[i], "r");
            while (!feof(fp)){
                fgets(temp, 1000, fp);
                if (strstr(temp, pat)){			//substring search
                    printf("%s : ", argv[i]);
                    printf("%s", temp);
                    printf("\n");
                }
            }
            fclose(fp);
            i++;
            argc--;
        }
    }
    else{
        printf("Usage: grep [OPTION]... PATTERN [FILE]...\n");
        return 1;
    }
    return 1;
}

int func_date(int argc, char **argv){
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[64];
    strftime(s, sizeof(s), "%c", tm);
    printf("%s\n", s);
    return 1;
}

int func_cal(int argc, char **argv){
    char *line = malloc(sizeof(char) * BUFFER_SIZE);
    int i = 0;
    while(*(argv+i) != NULL){
	if(i != 0){
		strcat(line, " ");
	}
	strcat(line, *(argv+i));
	i++;
    }
    system(line);
    return 1;
}

int func_hostname(int argc, char **argv){
    char hostname[BUFFER_SIZE];
    gethostname(hostname, BUFFER_SIZE);
    printf("%s\n", hostname);
    return 1;
}

int func_echo(int argc, char **argv){
    char stringss[BUFFER_SIZE] = "\0";
    int i = 1;
    while (argc != 1){
        strcat(stringss, argv[i]);
        strcat(stringss, " ");
        i++;
        argc--;
    }
    printf("%s\n", stringss);
    return 1;
}

int func_makers(int argc, char **argv){
    printf("Fahad 20K-0441\tWajdan 20K-0441\n");
    return 1;	
}
