#include "screenviewer_interface.h"

void* viewer_thread(void* ptr);

screenviewer_interface::screenviewer_interface()
{
	thread = 0;
}

void screenviewer_interface::init()
{
	std::cout << "[screen]initializing..." << std::endl;
	pthread_create(&thread, NULL, viewer_thread, this);
}

void* viewer_thread(void* ptr)
{
	screenviewer_interface* _this = (screenviewer_interface*)ptr;
	int sock, sock_finder;
	struct sockaddr_in addr = {0};
	socklen_t socklen;

	_this->thread_lock();
	sock = socket(PF_INET, SOCK_STREAM, 0);
	sock_finder = socket(PF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(11);
	}
	if(bind(sock_finder, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(11);
	}
	listen(sock, 99);
	_this->viewers.clear();
	_this->thread_unlock();

	pthread_detach(pthread_self());
	{
		int d_sock;
		struct sockaddr_in d_addr = {0};

		d_sock = socket(PF_INET, SOCK_DGRAM, 0);
		d_addr.sin_family = AF_INET;
		d_addr.sin_port = htons(30000);
		d_addr.sin_addr.s_addr = inet_addr("240.240.240.240");
		connect(d_sock, (struct sockaddr*)&d_addr, sizeof(d_addr));
		socklen = sizeof(d_addr);
		getsockname(d_sock, (struct sockaddr*)&d_addr, &socklen);
		std::cout << "[screen]self IP:" << inet_ntoa(d_addr.sin_addr) << std::endl;
		close(d_sock);
	}
	std::cout << "[screen]started recepting..." << std::endl;
	while(1)
	{
		struct timeval tv = {0, 1};
		fd_set fd;

		FD_ZERO(&fd);
		FD_SET(sock, &fd);
		FD_SET(sock_finder, &fd);
		select(0, &fd, NULL, NULL, &tv);
		if(FD_ISSET(sock, &fd))
		{
			int sock_con;
			struct sockaddr_in addr;
			socklen_t len = sizeof(addr);
			struct viewer_info* vi = new struct viewer_info;

			sock_con = accept(sock, (struct sockaddr*)&addr, &len);
			std::cout << "[screen]viewer incoming from "
				<< inet_ntoa(addr.sin_addr) << ":"
				<< ntohs(addr.sin_port) << "." << std::endl;
			
			vi->sock = sock_con;
			vi->addr = addr;
			_this->thread_lock();
			_this->viewers.push_back(vi);
			_this->thread_unlock();
		}
	}

	pthread_exit(0);
	return NULL;
}

void screenviewer_interface::thread_lock()
{
	pthread_mutex_lock(&(this->mutex_msg));
}
void screenviewer_interface::thread_unlock()
{
	pthread_mutex_unlock(&(this->mutex_msg));
}

void screenviewer_interface::dispatch_initializer(character_interface& c, character_interface& h, map& m)
{
	thread_lock();
	// dispatch process
	
	struct mapdata_header hdr = {'M', 'A', 'P', ' ', m.get_width(), m.get_height(),
		m.get_init_cool().x, m.get_init_cool().y, m.get_init_hot().x, m.get_init_hot().y,
		m.get_turns(), c.get_length(), h.get_length()};
	std::vector<struct viewer_info*> receive_queue;
	
	int packet_length = sizeof(hdr) + c.get_length() + h.get_length() + hdr.mw * hdr.mh;
	char* packet = (char*)malloc(packet_length+16);
	memcpy(packet, (char*)&hdr, sizeof(hdr));
	memcpy(packet + sizeof(hdr), c.get_name(), c.get_length());
	memcpy(packet + sizeof(hdr) + c.get_length(), h.get_name(), h.get_length());
	memcpy(packet + sizeof(hdr) + c.get_length() + h.get_length(), m.get_data(), hdr.mw * hdr.mh);
	
	std::vector<struct viewer_info*> viewer_left;
	for(std::vector<struct viewer_info*>::iterator it = this->viewers.begin();
		it != this->viewers.end(); it++)
	{
		if((*it)->sock == 0)
		{
			delete *it;
			continue;
		}
		send((*it)->sock, packet, packet_length, 0);
		receive_queue.push_back(*it);
		viewer_left.push_back(*it);
	}
	std::cout << "[screen]send initializer packet to " << viewer_left.size() << " clients." << std::endl;
	this->viewers = viewer_left;
	while(receive_queue.size())
	{
		struct timeval tv = {0, 1};
		std::vector<struct viewer_info*> rq_next;
		fd_set fd;

		FD_ZERO(&fd);
		for(std::vector<struct viewer_info*>::iterator it = receive_queue.begin();
			it != receive_queue.end(); it++)
		{
			FD_SET((*it)->sock, &fd);
		}
		if(select(0, &fd, NULL, NULL, &tv) < 0)
		{
			std::cout << "die from select at screenviewer_interface::dispatch_initializer" << std::endl;
			exit(10);
		}
		for(std::vector<struct viewer_info*>::iterator it = receive_queue.begin();
			it != receive_queue.end(); it++)
		{
			if(FD_ISSET((*it)->sock, &fd))
			{
				char data[256];
				if(recv((*it)->sock, data, 256, 0) <= 0)
				{
					std::cout << "[screen]viewer disconnected." << std::endl;
					close((*it)->sock);
					(*it)->sock = 0;
				}
				else
				{
					std::cout << "[screen]received ok. from "
						<< inet_ntoa((*it)->addr.sin_addr) << ":" << ntohs((*it)->addr.sin_port) << std::endl;
				}
			}
			else
			{
				rq_next.push_back(*it);
			}
		}
		receive_queue = rq_next;
	}

	free(packet);
	thread_unlock();
}
void screenviewer_interface::dispatch_update(character_interface& c, character_interface& h, map& m)
{
	thread_lock();
	// dispatch process
	
	struct mapdata_header_with_methods hdr = {'M', 'H', 'W', 'M', 
		c.get_pos().x, c.get_pos().y, h.get_pos().x, h.get_pos().y,
		c.get_method(), h.get_method(), c.get_direction(), h.get_direction(), 
		c.get_items(), h.get_items(), m.get_turns()};
	std::vector<struct viewer_info*> receive_queue;
	
	int packet_length = sizeof(hdr) + m.get_width() * m.get_height();
	char* packet = (char*)malloc(packet_length+16);
	memcpy(packet, (char*)&hdr, sizeof(hdr));
	memcpy(packet + sizeof(hdr), m.get_data(), m.get_width() * m.get_height());
	
	std::vector<struct viewer_info*> viewer_left;
	for(std::vector<struct viewer_info*>::iterator it = this->viewers.begin();
		it != this->viewers.end(); it++)
	{
		if((*it)->sock == 0)
		{
			delete *it;
			continue;
		}
		send((*it)->sock, packet, packet_length, 0);
		receive_queue.push_back(*it);
		viewer_left.push_back(*it);
	}
	std::cout << "[screen]send update packet. packet size=" << packet_length
		<< "/map size=(" << m.get_width() << ", " << m.get_height() << ")" << std::endl;
	this->viewers = viewer_left;
	while(receive_queue.size())
	{
		struct timeval tv = {0, 1};
		std::vector<struct viewer_info*> rq_next;
		fd_set fd;

		FD_ZERO(&fd);
		for(std::vector<struct viewer_info*>::iterator it = receive_queue.begin();
			it != receive_queue.end(); it++)
		{
			FD_SET((*it)->sock, &fd);
		}
		select(0, &fd, 0, 0, &tv);
		for(std::vector<struct viewer_info*>::iterator it = receive_queue.begin();
			it != receive_queue.end(); it++)
		{
			if(FD_ISSET((*it)->sock, &fd))
			{
				char data[256];
				if(recv((*it)->sock, data, 256, 0) <= 0)
				{
					std::cout << "[screen]viewer disconnected." << std::endl;
					close((*it)->sock);
					(*it)->sock = 0;
				}
				else
				{
					std::cout << "[screen]received ok. from "
						<< inet_ntoa((*it)->addr.sin_addr) << ":" << ntohs((*it)->addr.sin_port) << std::endl;
				}
			}
			else
			{
				rq_next.push_back(*it);
			}
		}
		receive_queue = rq_next;
	}

	free(packet);
	thread_unlock();
}
void screenviewer_interface::dispatch_result(char winner_head, char reason)
{
	thread_lock();
	// dispatch process
	
	struct result_header hdr = {'R', 'S', 'L', 'T', winner_head, reason};
	
	int packet_length = sizeof(hdr);
	char* packet = (char*)malloc(packet_length+16);
	memcpy(packet, (char*)&hdr, sizeof(hdr));
	
	for(std::vector<struct viewer_info*>::iterator it = this->viewers.begin();
		it != this->viewers.end(); it++)
	{
		if((*it)->sock == 0)
		{
			delete *it;
			continue;
		}
		send((*it)->sock, packet, packet_length, 0);
		close((*it)->sock);
		delete *it;
	}
	this->viewers.clear();
}
