#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define MAX_CLIENT 5

/* 常见文件后缀名以及其对应content-type */
typedef struct
{
	char *ext;
	char *mediatype;
} extn;

extn extensions[] = {
	{"gif", "image/gif" },
	{"txt", "text/plain" },
	{"jpg", "image/jpg" },
    {"jpeg","image/jpeg"},
	{"png", "image/png" },
	{"ico", "image/ico" },
	{"zip", "image/zip" },
	{"gz",  "image/gz"  },
	{"tar", "image/tar" },
	{"htm", "text/html" },
	{"html","text/html" },
	{"php", "text/html" },
	{"pdf","application/pdf"},
	{"zip","application/octet-stream"},
	{"rar","application/octet-stream"},
	{"\0","text/plain"} };

/**************************************
 * 包装send()
 * 用作单次发送单个文件
 * socketfd为socket地址
 * buf为文件地址
 * len为文件长度
 * 成功返回0
 * 失败返回-1,并返回剩余发送字节数至len
 * ************************************/
int sendfile( int socketfd, const char* buf, int *len )
{
	int byte_total = 0;		/* 成功发送字节数 */
	int byte_left = *len;	/* 剩余字节数 */
	int count;
	while( byte_total < *len )
	{
		count = send( socketfd, buf + byte_total, byte_left, 0 );
		if ( count == -1 )
		{
			printf( "Send error!\n" );
			continue;
		}
		byte_total += count;
		byte_left -= count;
	}
	*len = byte_left;			/* 返回剩余发送字节数 */
	return count == -1 ? -1 : 0;	/* 成功返回0,失败返回-1 */
}

/**************************************
 * 错误请求返回函数
 * socketfd为目标socket地址
 * 返回客户端501未实现错误
 * 如果发送失败，打印”501 Send error！“
 * ************************************/
void err_req( int socketfd )
{
	int ret = 0;
	char error_msg[] = "HTTP/1.1 501 Not Implemented\r\n"
					  "Content-type: text/html\r\n"
					  "\r\n"
					  "<html><head>\r\n"
					  "<title>501 Not Implemented</title>\r\n"
					  "</head><body>\r\n"
					  "<h1>501: Not Implemented</h1>\r\n"
					  "<p>The command is not yet completed.</p>\r\n"
					  "<hr>\r\n"
					  "<address>Made by Zyson-sue</address>\r\n"
					  "</body></html>\r\n";
	int error_len = strlen( error_msg );
	ret = sendfile( socketfd, error_msg, &error_len );
	assert( ret == 0 );
	return;
}
	
/********************************
 * 判断文件存在性
 * 存在返回true，不存在返回false
 * ******************************/
bool if_exit( char* file_name, int* file_len )
{
	struct stat dir_info;
	int bol;
	bol = (stat( file_name, &dir_info) == 0 );
	*file_len = (int) dir_info.st_size;
	return bol;
}

/***************************************
 * 文件不存在返回函数
 * file_name为文件
 * socketfd为socket地址
 * 向客户端发送404 Not Found
 * *************************************/
void file_not_found( int socketfd, const char* file_name )
{
	int ret = 0;
	char error_msg[] = "HTTP/1.1 404 Not Found\r\n"
					  "Content-type: text/html\r\n"
					  "\r\n"
					  "<html><head>\r\n"
					  "<title>404 Not Found</title>\r\n"
					  "</head><body>\r\n"
					  "<h1>404: Not Found</h1>\r\n"
					  "<p>The requested URL:";

	char error_msg2[] =  " was not found on this server.</p>\r\n"
						"<hr>\r\n"
						"<address>Made by Zyson-sue</address>\r\n"
						"</body></html>\r\n";
	int error_len = strlen( error_msg );
	ret = sendfile( socketfd, error_msg, &error_len );
	error_len = strlen( file_name );
	ret = sendfile( socketfd, file_name, &error_len );
	error_len = strlen( error_msg2 );
	ret = sendfile( socketfd, error_msg2, &error_len );
	assert( ret == 0 );
	return;
}

/***************************************
 * 发送Http协议报文头部信息
 * 包括响应类型和Content_type
 * *************************************/
void send_head( int fd, char* content_type )
{
	int ret = 0, len = 0;
	char head[] = "HTTP/1.1 200 OK\r\n"
					"Content-type: ";
	strcat( head, content_type );
	strcat( head, "\r\n\r\n" );
	len = strlen( head );
	ret = sendfile( fd, head, &len );
	assert( ret == 0 );
	return;
}

/***************************************
 * 获取用户所请求的文件类型
 * 即文件后缀
 * *************************************/
char* file_type( char* file_name )
{
	char * temp;

	/* 取得文件后缀 */
	if ( ( temp = strrchr( file_name, '.' ) ) != NULL )
	{
		return temp + 1;
	}
	return file_name + strlen(file_name);	/* 如果文件名中没有.则返回空串 */
}

/***************************************
 * 识别文件类型，发送文件
 * filename是文件名，fd是socket地址
 * *************************************/
void send_file( int fd, char* filename, int filelen )
{
	char* extension = file_type( filename );	/* 获取后缀名 */
	char* content_type = extensions[1].mediatype;	/* 初始化type为文本格式 */
	FILE* read_from;							/* 本地文件指针 */
	int readed = -1;							/* 读取字节数 */
	int ret = 0, i = 0;

	for ( i = 0; extensions[i].ext != NULL; i++ )
	{
		if( strcmp( extension, extensions[i].ext ) == 0 )
		{
			content_type = extensions[i].mediatype;
		}
	}
	printf( "content_type: %s\n", content_type );
	read_from = fopen( filename, "r" );
	if( read_from != NULL )
	{
		char read_buf[128];		/* 缓存数组 */
		send_head( fd, content_type );
		while( !feof(read_from) )		/* 判断文件是否读取完毕 */
		{
			fgets( read_buf, 128, read_from );
			int len = strlen(read_buf);
			while( sendfile( fd, read_buf, &len ) == -1 )
			{
				printf( "Send file %s error!", filename );
				continue;
			}
		}
	}
	return;
}

/***************************************
 * 解析用户请求
 * 成功返回0, 失败返回-1
 * *************************************/
int handle_req( char* request, int client_sock )
{
	char command[BUFSIZ];		/* 保存解析到的命令字段 如GET*/
	char file_name[BUFSIZ];		/* 保存解析到的文件名 */
	int filelen = 0;			/* 文件长度 */

	strcpy( file_name, "." );	/* 加上当前目录 */
	
	/* 读取命令和文件名 */
	if ( sscanf( request, "%s%s", command, file_name + 1) == -1)
	{
		err_req( client_sock );
		return -1;
	}
	
	printf( "cmd: %s\n", command );
	printf( "path: %s\n", file_name );

	if ( strcmp( command, "GET" ) != 0 )
	{
		err_req( client_sock );
		return -1;
	}

	if ( !if_exit(file_name, &filelen ) )
	{
		file_not_found( client_sock, file_name);
		return -1;
	}
	send_file( client_sock, file_name, filelen );
	return 0;
}


/***************************************
 * 创建新的TCP连接
 * 传入fd为socket地址
 * *************************************/
void CreateNewTcp( int fd )
{
	char buffer[ BUFFER_SIZE ];
	memset( buffer, '\0', BUFFER_SIZE );
	int data_read = 0;			/* 读取字符数 */
	int checked_index = 0;		/* 规范判断符 */
	int start_line = 0;			/* 读取开始行 */
	int data_send = 0;
	char buffer_send[ BUFFER_SIZE ];
	while( 1 )
	{
		data_read = recv( fd, buffer, BUFFER_SIZE, 0 );
		if ( data_read == -1 )
		{
			printf( "reading failed\n" );
			break;
		}
		else if ( data_read == 0 )
		{
			printf( "remote client has closed the connection\n" );
			break;
		}
		/* 读取部分 */
		printf ("%s", buffer);
		if( handle_req( buffer, fd ) )
		  printf( "Send success!" );
		close( fd );
	}
}

int main( int argc, char* argv[] )
{
	if( argc <=2 )
	{
		printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	char * file_name;
	int client_count = 0;
    //printf("ip: %s\nport: %d\n", ip, port);	
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );	/* 创建socket */
	assert( listenfd >= 0 );
	int ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );		/* 命名socket */
	assert( ret != -1 );
	ret = listen( listenfd, 5 );
	assert( ret != -1 );
	while( 1 )
	{
		struct sockaddr_in client_address;
		socklen_t client_addrlength = sizeof( client_address );
		int fd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
		printf( "New client touched……%d\n", client_count );
		if( fd < 0 )
		{
			printf( "errno is : %d\n", errno );
		}
		else
		{
			client_count++;
			CreateNewTcp( fd );
			client_count--;
		}
	}
	close( listenfd );
	return 0;
}


