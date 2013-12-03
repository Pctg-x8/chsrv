#include "character_interface.h"
#include <iostream>

character_interface::character_interface(std::string chr_name, unsigned short port)
{
	this->char_name = chr_name;
	if(this->char_name == "hot") this->char_name = "hot ";
	this->port = port;
}

void character_interface::init()
{
	struct sockaddr_in addr = {0};

	std::cout << "[" << char_name << "  ]initializing..." << std::endl;
	sock_receptor = socket(PF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock_receptor, (struct sockaddr*)&addr, sizeof(addr));
	listen(sock_receptor, 1);
	sock_connected = 0;
}
void character_interface::check_entry()
{
	fd_set fd;
	struct timeval tv = {0, 1};

	if(sock_receptor == 0) return;

	FD_ZERO(&fd);
	FD_SET(sock_receptor, &fd);
	if(sock_connected) FD_SET(sock_connected, &fd);
	select(0, &fd, NULL, NULL, &tv);
	if(FD_ISSET(sock_receptor, &fd))
	{
		socklen_t len = sizeof(struct sockaddr_in);

		sock_connected = accept(sock_receptor, (struct sockaddr*)&(this->addr), &len);
		if(recv(sock_connected, team_name, 256, 0) <= 0)
		{
			close(sock_connected);
			sock_connected = 0;
			return;
		}
		std::cout << "[" << char_name << "  ]connected from " << inet_ntoa(this->addr.sin_addr) << ":" << ntohs(this->addr.sin_port) << "." << std::endl;
	}
	else if(sock_connected && FD_ISSET(sock_connected, &fd))
	{
		char dummy_data[256];
		if(recv(sock_connected, dummy_data, 256, 0) == 0)
		{
			close(sock_connected);
			sock_connected = 0;
			std::cout << "[" << char_name << "  ]disconnected." << std::endl;
		}
	}
}
bool character_interface::is_connected()
{
	return sock_connected != 0;
}
void character_interface::prepare(struct point& init_pos)
{
	method = undefined;
	dir = unknown;
	items = 0;
	pos.x = init_pos.x;
	pos.y = init_pos.y;
}
int character_interface::get_length()
{
	return strlen(team_name);
}
char* character_interface::get_name()
{
	return team_name;
}
struct point& character_interface::get_pos()
{
	return pos;
}
char character_interface::get_method()
{
	return method;
}
char character_interface::get_direction()
{
	return dir;
}
unsigned int character_interface::get_items()
{
	return items;
}
