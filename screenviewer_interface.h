#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <deque>
#include <pthread.h>

class screenviewer_interface
{
public:
	screenviewer_interface();

	void init();
	void dispatch_initializer();
	void dispatch_update();
private:
	pthread_t thread;
	pthread_mutex_t msg_mutex;
	std::deque<int> message_queue;
};
