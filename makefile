.PHONY:all
cc = g++
all:poll_server poll_client 
poll_server:Poll.hpp Poll_Server.hpp Tcp_Server.cc 
	$(cc) -std=c++11 -o  $@ $^ -g
poll_client:Tcp_Client.cc
	$(cc) -o $@ $^
.PHONY:clean
clean:
	rm -rf poll_server poll_client

