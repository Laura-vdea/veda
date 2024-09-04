OBJS = tcpServer.o tcpClient.o
CROSSCC = aarch64-linux-gnu-gcc-12
all : tcpServer tcpClient
tcpServer: tcpServer.o
	$(CROSSCC) -o $@ tcpServer.o
tcpServer.o:
	$(CROSSCC) -c tcpServer.c
tcpClient: tcpClient.o
	$(CC) -o $@ tcpClient.o
	$(CC) -c tcpClient.c

clean :
	rm *.o tcpServer tcpClient
