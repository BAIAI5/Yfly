CXX=g++
CFLAGS=-lpthread -g -Wall

all:server client

server:server_handler.cpp server_epoll.cpp pingpong_server.cpp server_main.cpp
	$(CXX) -o $@ $^ $(CFLAGS)

client:client_main.cpp pingpong_client.cpp
	$(CXX) -o $@ $^ $(CFLAGS)


clean:
	rm server client

.PHONY:all clean