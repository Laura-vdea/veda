OBJS = udpServer.o udpClient.o
CROSSCC = aarch64-linux-gnu-gcc-12
all : udpServer udpClient
udpServer: udpServer.o
	$(CROSSCC) -o $@ udpServer.o
udpServer.o:
	$(CROSSCC) -g -c udpServer.c
udpClient: udpClient.o
	$(CC) -o $@ udpClient.o
udpClient.o:
	$(CC) -c udpClient.c

clean :
	rm *.o udpServer udpClient
