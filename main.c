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
#include<signal.h>

#define IP "192.168.1.104"
#define PORT 10002

void read_header(int cfd) ;
int read_line(int cfd, void **buf, int *rlen);


int main()
{
    struct sockaddr_in serv_addr;
	int serv_sock_f = socket(AF_INET, SOCK_STREAM, 0);

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

    client_sock_f = accept(serv_sock_f, (struct sockaddr *)&client_addr, &client_sock_l);

    /*

    char buf[65535] ;

    int readlen = recv(client_sock_f, buf, 65535, 0);

    if(readlen>0){

        int sendlen = send(client_sock_f, buf, readlen, 0);

    }
    */

    read_header(client_sock_f);

    char *html = "<h1>Hello C</h1>";
    int html_l = strlen(html);
    char *format = "HTTP/1.1 200 OK\r\nContent-Length:%d\r\n\r\n%s" ;
    int format_l = strlen(format);

    size_t sss = html_l+format_l+100 ;
    char *response_buf = (char *)malloc( sss );
    memset(response_buf, '\0', sss);

    sprintf(response_buf, format, html_l, html);
    printf("%s \n", response_buf);

    int __l = send(client_sock_f, response_buf, strlen(response_buf), 0);


    if(__l<0){
    
        perror("send") ;
        exit(-1) ;
    }

    printf("send %d \n", __l);
    
    close(client_sock_f) ;
    close(serv_sock_f);

    exit(0);
}

void read_header(int cfd)
{
    int buf_size = 10;
    void *buf = NULL;

    int i = 0 ;

    int rlen = 0;

    FILE *fp = fdopen(cfd, "r") ;

    while(1){
        buf = malloc(buf_size);

        while(fgets((char *)buf, buf_size, fp)){
            if(strncmp("\r\n",(char *)buf,2)==0){
                printf("END \n");
                break;
            }

        }
        
        printf("%s", (char *)buf);

        if(strncmp("\r\n",(char *)buf,2)==0){
            printf("END \n");
            break;
        }
    }

    /*
    while(read_line(cfd, &buf, &rlen) > 0){
        
        if(strncmp("\r\n",(char *)buf,2)==0){
            printf("END \n");
            break;
        }

        printf("%s", (char *)buf);

        if(buf != NULL){
            free(buf);
            buf = NULL;
        }

        if(++i > 20){
            break;
        }
    }

    */
}

int read_line(int cfd, void **buf, int *rlen){
    int alloc_size = 1024; 

    int buf_size = alloc_size;// 缓冲区块大小 
    *buf = malloc(buf_size) ;
    

    int i = 0;
    char s;

    while(read(cfd, &s, 1) > 0){
        //*(buf+i) = s;
        memcpy(*buf+i, &s, sizeof(char));
        i++ ;

        if(i>=buf_size){
            buf_size+=alloc_size;
            *buf = realloc(*buf, buf_size) ;
        }
        
        if(s == '\n'){
            break;
        }
    }

    //*(buf+i) = '\0';
    s = '\0';
    memcpy(buf+i, &s, sizeof(char));
    *rlen = i;

    return 1;
}
