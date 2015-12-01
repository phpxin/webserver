//unix 
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
//memory and io
#include <string.h>
#include <stdio.h>
//socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//signal
#include <signal.h>
//data structs
#include "utils/HString.h"

#define IP "192.168.3.17"
#define PORT 10002

#define BS 1024   ///buff size

#define METHOD_GET 1 
#define METHOD_POST 2

#define WWW_ROOT "/home/public/htdocs"

void read_header(int cfd) ;
int read_line(int cfd, void **buf, int *rlen);
void response(int);
void sigfunc_callback(int sig);
void sig_chld_func();
void sig_int_func();

typedef struct {
    int method;
    char file[1024];
    char host[1024];
} RequestInfo;

RequestInfo req_info = {0,"\0","\0"} ;

int serv_sock_f ;

int main()
{
    printf("welcome ! \n");

    signal(SIGCHLD, sigfunc_callback);
    signal(SIGINT, sigfunc_callback);

    struct sockaddr_in serv_addr;
	serv_sock_f = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	inet_pton( AF_INET, IP, &serv_addr.sin_addr.s_addr);


    int flag=1 ;
    
    // SO_REUSEADDR 设置端口重用，否则中断程序重启会提示端口被使用，需要过一段时间才能使用
    int len=sizeof(int); 
    if( setsockopt(serv_sock_f, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1)
    { 
       perror("setsockopt"); 
       exit( -1 ); 
    } 
    //发现设置端口重用有个问题，如果中断了该端口通信（通信正在进行中），重用的端口的新程序会继续接收上次未完成的数据（然后就杯具了%>_<%）
    
	flag = bind(serv_sock_f, (struct sockaddr *)&serv_addr, sizeof(serv_addr));


    if(flag == -1)
	{
        perror("bind");
		exit( -1 );
	}

	flag = listen(serv_sock_f, 5);
	if(flag == -1)
	{
		perror("listen");
		exit( -1 );
	}

	
    struct sockaddr_in client_addr;
	socklen_t client_sock_l = 0;
	int client_sock_f = -1;

    // >>>>>>>>>> test
    /*
    client_sock_f = accept(serv_sock_f, (struct sockaddr *)&client_addr, &client_sock_l);
    read_header(client_sock_f);
    response(client_sock_f);
    close(client_sock_f) ;
    close(serv_sock_f);
    */
    // >>>>>>>>>> test end

    
    while(1){
        client_sock_f = accept(serv_sock_f, (struct sockaddr *)&client_addr, &client_sock_l);
        pid_t pid = fork();

        if(pid==0){
            read_header(client_sock_f);
            response(client_sock_f);
            close(client_sock_f) ;
            break;
        }else if(pid>0){
            printf("accept next ... \n");
        }else{
            break;
        }
    }
    
    
    exit(0);
}

void sigfunc_callback(int sig)
{
    switch(sig)
    {
        case SIGCHLD:
            sig_chld_func();
            break;
        case SIGINT:
            sig_int_func();
        default:
            break;
    }
}

void sig_int_func()
{
    printf("close program ! bey-bey \n");
    if(serv_sock_f>0)
        close(serv_sock_f);

    exit(0);
}

void sig_chld_func()
{
    pid_t pid = 0 ;
    int stat = 0 ;
    if((pid = waitpid(-1, &stat, WNOHANG))>0){
          printf("子进程 %d 结束 stat = %d \n", pid, stat);

          if(WIFEXITED(stat))
              printf("子进程正常结束 status %d \n", WEXITSTATUS(stat)) ;
          else if(WIFSIGNALED(stat))
              printf("子进程是因为一个未捕获信号而终止 signal %d \n", WTERMSIG(stat)) ;
          else if(WIFSTOPPED(stat))
              printf("子进程意外终止 child id %d \n", WSTOPSIG(stat)) ;
          else
              printf("unknow \n");

     }
    
}

void response(int client_sock_f){

    if(req_info.method<=0 || strlen(req_info.file)<=0 || strlen(req_info.host)<=0){
        return ;
    }

    char *_html = "<h1>Hello C</h1>";
    int html_l = strlen(_html);

    int html_buf_size = html_l + BS;

    char *format = "HTTP/1.1 200 OK\r\nContent-Length:%d\r\n\r\n%s" ;
    char *html = (char *)malloc(html_buf_size);
    strncpy(html, _html, html_l);

    if(strlen(req_info.file)>1){

        char realpath[1024];
        sprintf(realpath, "%s%s", WWW_ROOT, req_info.file);
        printf("will read content from %s \n", realpath);
        FILE *fp = fopen(realpath, "r");
        if(fp==NULL){
            format = "HTTP/1.1 404 Not Found\r\nContent-Length:%d\r\n\r\n%s" ;
            _html = "<h1>Object Not Found !</h1>";
            html_l = strlen(_html) ;
            html = (char *)realloc(html, html_l+BS);
            strncpy(html, _html, html_l);

        }else{

            memset(html, '\0', html_buf_size);
            html_l = 0;

            int rflen = 0 ;
            while(!feof(fp)){
                if(html_buf_size <= html_l+BS) {
                    html_buf_size += BS+1;
                    html = (char *)realloc(html, html_buf_size);
                }
                rflen = fread(html+html_l, sizeof(char), BS, fp);
                html_l += rflen;
            }

        }
    }
    
    *(html+html_l) = '\0' ;
    
    int format_l = strlen(format);

    size_t sss = html_l+format_l+100 ;
    char *response_buf = (char *)malloc( sss );
    memset(response_buf, '\0', sss);
    
    // 组装响应数据
    sprintf(response_buf, format, html_l, html);
    printf("%s \n", response_buf);

    int __l = send(client_sock_f, response_buf, strlen(response_buf), 0);


    if(__l<0){
    
        perror("send") ;
        exit(-1) ;
    }

    printf("send %d \n", __l);    
}

void parse_line(const char *str)
{
    if(strncmp("GET", str, 3)==0){
        req_info.method = METHOD_GET ;
        char _m[16];
        char _f[1024];
        char _v[32];
        sscanf(str, "%s %s %s", _m, _f, _v) ;
        strncpy(req_info.file, _f, strlen(_f));
    }else if(strncmp("Host", str, 4)==0){
        char _k[16];
        char _v[1024];
        sscanf(str, "%s %s", _k, _v);
        strncpy(req_info.host, _v, strlen(_v));
    }
}

void read_header(int cfd)
{
    int buf_size = BS;
    char *buf = NULL;
    char *line = NULL;

    int i = 0 ;

    int rlen = 0;

    FILE *fp = fdopen(cfd, "r") ;

    HString hs = {NULL, 0};
    
    buf = (char *)malloc(buf_size+1);

    while(1){

        memset(buf, '\0', buf_size);
        ClearString(&hs);

        while(fgets(buf, buf_size, fp)){
            StrAppend(&hs, buf);
            int _rlen = strlen(buf);
            if(buf[_rlen-1]=='\n')
                break;
        }
        
        ToString(&hs, &line);

        if(strncmp("\r\n",line,2)==0){
            printf("END \n");
            break;
        }

        if(strlen(line)<=0)
            break;
        
        printf("%s", line);
        parse_line(line);
    }

    printf("the request info is %d,%s,%s\n", req_info.method, req_info.file, req_info.host);

}
