#pragma once

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include "chsrv_comapi.h"
#include "point.h"
#include "map.h"

class character_interface
{
public:
	character_interface(std::string chr_name, unsigned short port);
	void init();
	void check_entry();
	bool is_connected();
	void prepare(struct point& init_pos);

	bool enter();
	bool get_ready(map& m_inst, struct point& enemy_pos, bool is_ended = false);
	bool process_method(map& m_inst, struct point& enemy_pos, bool is_ended = false);
	bool leave();

	int get_length();
	char* get_name();
	struct point& get_pos();
	char get_method();
	char get_direction();
	unsigned int get_items();
private:
	std::string char_name;
	unsigned short port;
	int sock_receptor, sock_connected;
	struct sockaddr_in addr;
	char team_name[256];
	struct point pos;
	char method;
	char dir;
	unsigned int items;
};
