#include <sys/types.h>
#include <sys/socket.h>
//int socket ( int domain, int type, int protocol ); 
//创建socket
//domian 底层协议族种类，type 服务类型
//protocol 前两个协议基础下在选择一个具体的协议，大多数情况下默认为0
//成功返回socket描述符，失败返回-1并设置errno

//int bind( int sockfd, const struct sockaddr* my_addr, socklen_t addrlen ); 
//命名socket
//bind将my_addr所指的socket地址分配给sockfd文件描述符
//addrlen指出socket地址长度
//bind成功时返回0,失败返回-1并设置errno，常见两种errno
//EACCES	被绑定的地址是受保护的地址，仅超级用户可以访问
//EADDRINUSE	被绑定的地址正在使用中。

//int listen( int sockfd, int backlog );
//监听socket
//sockfd指定被监听的socket
//backlot提示内核监听队列的最大长度，如果队列长度超过它则不接受新的客户连接
//客户端收到ECONNREFUSED错误信息，其典型值为5
//listen成功时返回0，失败返回-1并设置errno

//int accept( int sockfd, struct sockaddr *addr, socklen_t *addrlen );
//接受连接
//sockfd是执行过listen系统调用的监听socket
//addr用来获取接受链接端socket地址，
//地址长度由addrlen指出
//失败时返回-1并设置errno

//int connetc( int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen );
//发起连接
//sockfd参数由socket系统调用返回一个socket
//serv_addr是服务器监听的socket地址
//addrlen参数是这个地址的长度
//成功是返回0 成功后客户端可以通过sockfd来和服务器通信
//失败返回-1 并设置errno，常见两种errno
//ECONNREFUSED	目标端口不存在，链接被拒绝
//ETIMEDOUT	连接超时

//int close( int fd );
//关闭连接
//fd是待关闭的socket
//close仅是将fd减一而非立即关闭，当fd=0时才是真正关闭连接

//int shutdown( int sockfd, int howto );
//关闭连接 相对与网络编程专用
//sockfd是待关闭的socket
//
//howto决定了shutdown的行为：
//SHUT_RD	关闭sockfd读行为，不能执行读操作，接受缓冲区清空
//SHUT_WD	关闭sockfd写行为，发送缓冲区数据先发送再清空，不能执行写操作
//SHUT_RDWR	同时关闭sockfd读写操作
//
//shutdown成功返回0,失败返回-1并设置errno


//TCP读写部分

//ssize_t recv( int sockfd, void *buf, size_t len, int flags );

//ssize_t send( int sockfd, const void *buf, size_t len, int flags );
//recv读取sockfd上的数据
//buf和len参数分别指定读缓冲区的位置和大小
//flags参数通常为0
//recv成功时返回成功读取的数据长度可能小于len，失败返回-1，并设置errno
//
//send往sockfd写入数据
//buf和len参数分别指定写缓冲区位置和大小
//send成功时返回实际写入的数据长度，失败返回-1并设置errno
//
//flags是控制数据的收发，通常有一下的几个值
//选项名		含义												sendrecv
//MSG_CONFIRM	持续监听，直到得到答复，仅能用于SOCK_DGRAM和SOCK_RAWY	N
//MSG_DONRTOUTE	不查看路由表，直接发送给local的主机。				Y	N
//MSG_DONTWAIT	此次对socket的操作非阻塞							Y	Y
//MSG_MORE		告诉内核还有更多数据要发送，防止tcp发送过多小报文	Y	N
//MSG_WAITALL	读操作仅在读取到指定数量的字节后才返回				N	Y
//MSG_PEEK		窥探读缓存中的数据，此次读操作不会导致数据清空		N	Y
//MSG_OOB		发送或者接受紧急数据								Y	Y
//MSG_NOSIGNAL	往读端关闭的管道或者socket链接中写数据不引发SIG信号	Y	N

//UDP读写部分

//ssize_t recvfrom( int sockfd, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen );
//ssize_t sendto( int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* src_addr, socklen_t addrlen );
//recvfrom读取sockfd上的数据
//buf和len分别指定读缓冲区的位置和大小
//我们每次都要发送socket的地址就是src_addr
//addrlen就是该地址的长度
//
//sendto往sockfd上写入数据
//buf和len分别指定写缓冲区的位置和大小
//dest_addr参数指定接受端的socket地址
//addrlen就是该地址的长度
//
//flags参数同上
//
//此系统调用也可以用于面向链接的socket 只需要把最后两个参数设置为NULL

//通用数据读写函数

//ssize_t recvmsg( int sockfd, struct msghdr* msg, int flags );
//ssize_t sendmsg( int sockfd, struct msghdr* msg, int flags );
//
//关于msghdr结构体
//struct msghdr
//{
//	void* msg_name;				/* socket 地址 */
//	sockelen_t msg_namelen;		/* socket 地址的长度 */
//	struct iovec* msg_iov;		/* 分散的内存块 */
//	int msg_iovlen;				/* 分散内存块的数量 */
//	void* msg_control;			/* 指向辅助数据的起始位置 */
//	socklen_t msg_controllen;	/* 辅助数据的大小 */
//	int msg_flags;				/* 复制函数中的flags参数，并在调用过程中更新 */
//};
//
//msg_name指向一个socket地址 TCP中它为NULL
//msg_namelen指定了msg_name地址的长度
//msg_iov 是iovec结构体的指针
//struct iovec
//{
//	void *iov_base;	/* 内存起始地址 */
//	size_t iov_len;	/* 这块内存的长度 */
//};
//
//多个iovec保存在msg_iov数组中
//msg_control和msg_controllen辅助数据传送。
//msg_flags无需设定，它会复制前文的flags参数内容

//带外数据标记

//int sockatmark( int sockfd );
//sockatmark判断sockfd是否属于带外标记，如果是返回1,如果不是返回0;

//地址信息函数
//int getsockname( int sockfd, struct sockaddr* address, socklen_t* address_len );
//int getpeername( int sockfd, struct sockaddr* address, socklen_t* address_len );
//getsockname获取sockfd对应的本端socket地址
//并将其储存在address
//长度储存在address_len
//如果实际socket地址大于address所指内存区大小，则socket地址会被截断
//成功返回0,失败返回-1并设置errno
//
//getpeername获取sockfd对应的远端socket地址
//其他参数与getsocketname相同

//socket选项
//int getsockopt( int sockfd, int level, int option_name, void* option_value, socklen_t* restrict option_len );
//int setsockopt( int sockfd, int level, int option_name, const void* option_value, socklen_t option_len );
//sockfd指定被操作的目标socket
//level参数指定要操作哪个协议的选项，IPv4、IPv6、TCP等
//option_name 指定选项名字
//option_value option_len是被操作选项的值和长度
//详见Linux高性能服务器编程.pdf 88页表5-5
//两函数成功时返回0,失败时返回-1并设置errno

//SO_REUSEADDR可以使用这个选项强制使用处于TIME_WAIT状态的连接占用的socket地址

//readv 和 writev函数
//ssize_t readv( int fd, const struct iovec* vector, int count);
//ssize_t writev( int fd, const struct iovec* vector, int count);
//fd参数是被操作的目标文件描述符
//vector内存区块
//成功返回读入/写入fd的字节数，失败返回-1并设置errno

//sendfile函数
//ssize_t sendfile( int oout_fd, int in_fd, off_t* offset, size_t count);
//in_fd是待读出文件的描述符
//out_fd是待写入文件的描述符
//offset指定读入文件流从哪个位置开始读，如果为空，则使用默认位置
//count指定传输字节数
//成功返回传输字节数，失败返回-1并设置errno

//struct sockaddr_in
//{
//	__SOCKADDR_COMMON(sin_);
//	in_port_t sin_port;					/* port number */
//	struct in_addr sin_addr;			/* Internet address */
//	
//	unsigned char sin_zero[ sizeof( struct sockaddr ) ];
//				__SOCKADDR_COMMON_SIZE-
//				sizeof( in_port_t );
//				sizeof( struct in_addr );
//}

//int stat( const char* file_name, struct stat* buf );
//将file_name所指文件状态复制到buf中
//头文件 sys/stat.h, unistd.h
//struct stat
//{
//	dev_t st_dev;		//device 文件的设备编号
//	ino_t st_ino;		//inode 文件的i-node
//	mode_t st_mode;		//protection 文件的类型和存取的权限
//  nlink_t st_nlink;	//number of hard links 连到该文件的硬连接数目, 刚建立的文件值为1.
//  uid_t st_uid;		//user ID of owner 文件所有者的用户识别码
//  gid_t st_gid;		//group ID of owner 文件所有者的组识别码
//  dev_t st_rdev;		//device type 若此文件为装置设备文件, 则为其设备编号
//  off_t st_size;		//total size, in bytes 文件大小, 以字节计算
//  unsigned long st_blksize; //blocksize for filesystem I/O 文件系统的I/O 缓冲区大小.
//  unsigned long st_blocks; //number of blocks allocated 占用文件区块的个数, 每一区块大小为512 个字节.
//	time_t st_atime;	//time of lastaccess 文件最近一次被存取或被执行的时间, 一般只有在用mknod、utime、read、write 与tructate 时改变.
//  time_t st_mtime;	//time of last modification 文件最后一次被修改的时间, 一般只有在用mknod、utime 和write 时才会改变
//  time_t st_ctime;	//time of last change i-node 最近一次被更改的时间, 此参数会在文件所有者、组、权限被更改时更新
//};
