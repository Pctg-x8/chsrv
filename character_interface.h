#pragma once

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class character_interface
{
public:
	character_interface(std::string chr_name, unsigned short port);
	void init();
	void check_entry();
	bool is_connected();
private:
	std::string char_name;
	unsigned short port;
	int sock_receptor, sock_connected;
	struct sockaddr_in addr;
	char team_name[256];
};
