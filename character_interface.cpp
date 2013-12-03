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
		std::cout << "[" << char_name << "  ]connected from "
			<< inet_ntoa(this->addr.sin_addr) << ":" << ntohs(this->addr.sin_port) << "." << std::endl;
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

bool character_interface::enter()
{
	std::cout << "[" << char_name << "  ]entering turn." << std::endl;
	if(sock_connected == 0) return false;
	send(sock_connected, "@\r\n", 3, 0);
	return true;
}
bool character_interface::get_ready(map& m_inst, struct point& enemy_pos, bool is_ended)
{
	char buffer[256];
	char packet[32] = {0};

	std::cout << "[" << char_name << "  ]getting ready at (" << pos.x << ", " << pos.y << ")." << std::endl;
	if(sock_connected == 0) return false;
	if(recv(sock_connected, buffer, 256, 0) <= 0) return false;
	if(strncmp(buffer, "gr", 2) != 0) return false;
	packet[0] = is_ended ? '0' : '1';
	packet[1] = '0'+m_inst.get_pattern(pos.x - 1, pos.y - 1);
	packet[2] = '0'+m_inst.get_pattern(pos.x + 0, pos.y - 1);
	packet[3] = '0'+m_inst.get_pattern(pos.x + 1, pos.y - 1);
	packet[4] = '0'+m_inst.get_pattern(pos.x - 1, pos.y + 0);
	packet[5] = '0'+m_inst.get_pattern(pos.x + 0, pos.y + 0);
	packet[6] = '0'+m_inst.get_pattern(pos.x + 1, pos.y + 0);
	packet[7] = '0'+m_inst.get_pattern(pos.x - 1, pos.y + 1);
	packet[8] = '0'+m_inst.get_pattern(pos.x + 0, pos.y + 1);
	packet[9] = '0'+m_inst.get_pattern(pos.x + 1, pos.y + 1);
		 if(enemy_pos.x == pos.x - 1 && enemy_pos.y == pos.y - 1) packet[1] = '1';
	else if(enemy_pos.x == pos.x + 0 && enemy_pos.y == pos.y - 1) packet[2] = '1';
	else if(enemy_pos.x == pos.x + 1 && enemy_pos.y == pos.y - 1) packet[3] = '1';
	else if(enemy_pos.x == pos.x - 1 && enemy_pos.y == pos.y + 0) packet[4] = '1';
	else if(enemy_pos.x == pos.x + 0 && enemy_pos.y == pos.y + 0) packet[5] = '1';
	else if(enemy_pos.x == pos.x + 1 && enemy_pos.y == pos.y + 0) packet[6] = '1';
	else if(enemy_pos.x == pos.x - 1 && enemy_pos.y == pos.y + 1) packet[7] = '1';
	else if(enemy_pos.x == pos.x + 0 && enemy_pos.y == pos.y + 1) packet[8] = '1';
	else if(enemy_pos.x == pos.x + 1 && enemy_pos.y == pos.y + 1) packet[9] = '1';
	packet[10] = '\r';
	packet[11] = '\n';
	send(sock_connected, packet, 13, 0);
	return true;
}
bool character_interface::process_method(map& m_inst, struct point& enemy_pos, bool is_ended)
{
	char buffer[256];
	char packet[32] = {0};

	std::cout << "[" << char_name << "  ]getting methods." << std::endl;
	if(sock_connected == 0) return false;
	if(recv(sock_connected, buffer, 256, 0) <= 0) return false;
	std::cout << "[" << char_name << "  ]received method:" << buffer[0] << buffer[1] << std::endl;
	if(buffer[0] == 'w') method = walk;
	else if(buffer[0] == 'l') method = look;
	else if(buffer[0] == 's') method = search;
	else if(buffer[0] == 'p') method = put;
	else return false;
	if(buffer[1] == 'l') dir = left;
	else if(buffer[1] == 'r') dir = right;
	else if(buffer[1] == 'u') dir = up;
	else if(buffer[1] == 'd') dir = down;
	else return false;

	if(method == walk)
	{
		int ox = pos.x, oy = pos.y;
		pos.x += (dir == left ? (-1) : (dir == right ? 1 : 0));
		pos.y += (dir == up ? (-1) : (dir == down ? 1 : 0));
		if(m_inst.get_pattern(pos.x, pos.y) == 3)
		{
			m_inst.set_pattern(pos.x, pos.y, 0);
			m_inst.set_pattern(ox, oy, 2);
			items++;
		}
	}
	else if(method == put)
	{
		int cx = pos.x + (dir == left ? (-1) : (dir == right ? 1 : 0));
		int cy = pos.y + (dir == up ? (-1) : (dir == down ? 1 : 0));
		m_inst.set_pattern(cx, cy, 2);
	}

	packet[0] = is_ended ? '0' : '1';
	if(method == walk || method == put)
	{
		packet[1] = '0' + m_inst.get_pattern(pos.x - 1, pos.y - 1);
		packet[2] = '0' + m_inst.get_pattern(pos.x + 0, pos.y - 1);
		packet[3] = '0' + m_inst.get_pattern(pos.x + 1, pos.y - 1);
		packet[4] = '0' + m_inst.get_pattern(pos.x - 1, pos.y + 0);
		packet[5] = '0' + m_inst.get_pattern(pos.x + 0, pos.y + 0);
		packet[6] = '0' + m_inst.get_pattern(pos.x + 1, pos.y + 0);
		packet[7] = '0' + m_inst.get_pattern(pos.x - 1, pos.y + 1);
		packet[8] = '0' + m_inst.get_pattern(pos.x + 0, pos.y + 1);
		packet[9] = '0' + m_inst.get_pattern(pos.x + 1, pos.y + 1);
			 if(enemy_pos.x == pos.x - 1 && enemy_pos.y == pos.y - 1) packet[1] = '1';
		else if(enemy_pos.x == pos.x + 0 && enemy_pos.y == pos.y - 1) packet[2] = '1';
		else if(enemy_pos.x == pos.x + 1 && enemy_pos.y == pos.y - 1) packet[3] = '1';
		else if(enemy_pos.x == pos.x - 1 && enemy_pos.y == pos.y + 0) packet[4] = '1';
		else if(enemy_pos.x == pos.x + 0 && enemy_pos.y == pos.y + 0) packet[5] = '1';
		else if(enemy_pos.x == pos.x + 1 && enemy_pos.y == pos.y + 0) packet[6] = '1';
		else if(enemy_pos.x == pos.x - 1 && enemy_pos.y == pos.y + 1) packet[7] = '1';
		else if(enemy_pos.x == pos.x + 0 && enemy_pos.y == pos.y + 1) packet[8] = '1';
		else if(enemy_pos.x == pos.x + 1 && enemy_pos.y == pos.y + 1) packet[9] = '1';
	}
	else if(method == look)
	{
		int cx = pos.x + (dir == left ? (-1) : (dir == right ? 1 : 0));
		int cy = pos.y + (dir == up ? (-1) : (dir == down ? 1 : 0));
		packet[1] = '0'+m_inst.get_pattern(cx - 1, cy - 1);
		packet[2] = '0'+m_inst.get_pattern(cx + 0, cy - 1);
		packet[3] = '0'+m_inst.get_pattern(cx + 1, cy - 1);
		packet[4] = '0'+m_inst.get_pattern(cx - 1, cy + 0);
		packet[5] = '0'+m_inst.get_pattern(cx + 0, cy + 0);
		packet[6] = '0'+m_inst.get_pattern(cx + 1, cy + 0);
		packet[7] = '0'+m_inst.get_pattern(cx - 1, cy + 1);
		packet[8] = '0'+m_inst.get_pattern(cx + 0, cy + 1);
		packet[9] = '0'+m_inst.get_pattern(cx + 1, cy + 1);
			 if(enemy_pos.x == cx - 1 && enemy_pos.y == cy - 1) packet[1] = '1';
		else if(enemy_pos.x == cx + 0 && enemy_pos.y == cy - 1) packet[2] = '1';
		else if(enemy_pos.x == cx + 1 && enemy_pos.y == cy - 1) packet[3] = '1';
		else if(enemy_pos.x == cx - 1 && enemy_pos.y == cy + 0) packet[4] = '1';
		else if(enemy_pos.x == cx + 0 && enemy_pos.y == cy + 0) packet[5] = '1';
		else if(enemy_pos.x == cx + 1 && enemy_pos.y == cy + 0) packet[6] = '1';
		else if(enemy_pos.x == cx - 1 && enemy_pos.y == cy + 1) packet[7] = '1';
		else if(enemy_pos.x == cx + 0 && enemy_pos.y == cy + 1) packet[8] = '1';
		else if(enemy_pos.x == cx + 1 && enemy_pos.y == cy + 1) packet[9] = '1';
	}
	else if(method == search)
	{
		int dx = dir == left ? -1 : (dir == right ? 1 : 0);
		int dy = dir == up ? -1 : (dir == down ? 1 : 0);
	
		for(int i = 1; i <= 9; i++)
		{
			packet[i] = '0'+m_inst.get_pattern(pos.x + dx * i, pos.y + dy * i);
			if(enemy_pos.x == pos.x + dx * i && enemy_pos.y == pos.y + dy * i) packet[i] = '1';
		}
	}
	packet[10] = '\r';
	packet[11] = '\n';
	send(sock_connected, packet, 13, 0);
	return true;
}
bool character_interface::leave()
{
	char buffer[256];

	std::cout << "[" << char_name << "  ]leaving turn." << std::endl;
	if(sock_connected == 0) return false;
	if(recv(sock_connected, buffer, 256, 0) <= 0) return false;
	if(buffer[0] != '#') return false;
	return true;
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
