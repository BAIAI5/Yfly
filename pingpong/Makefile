CXX=g++
CFLAGS=-std=c++11 -g -Wall
LIBS=-lpthread

all:server client

server:server_handler.cpp server_epoll.cpp pingpong_server.cpp server_main.cpp
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)

client:client_main.cpp pingpong_client.cpp
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)


clean:
	rm server client

.PHONY:all clean
