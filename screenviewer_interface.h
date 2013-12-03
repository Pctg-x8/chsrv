#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <deque>
#include <pthread.h>
#include "map.h"
#include "character_interface.h"
#include "chsrv_comapi.h"

struct viewer_info
{
	int sock;
	struct sockaddr_in addr;
};

class screenviewer_interface
{
public:
	screenviewer_interface();

	void init();
	void dispatch_initializer(character_interface& c, character_interface& h, map& m);
	void dispatch_update(character_interface& c, character_interface& h, map& m);
	void dispatch_result(char winner_head, char reason);

	void thread_lock();
	void thread_unlock();
private:
	pthread_t thread;
	pthread_mutex_t mutex_msg;
public:
	std::vector<struct viewer_info*> viewers;
};
