#include "screenviewer_interface.h"

void* viewer_thread(void* ptr);

screenviewer_interface::screenviewer_interface()
{
	message_queue.clear();
	thread = 0;
}

void screenviewer_interface::init()
{
	std::cout << "[screen]initializing..." << std::endl;
	message_queue.clear();
	pthread_create(&thread, NULL, viewer_thread, this);
}

void* viewer_thread(void* ptr)
{
	screenviewer_interface* _this = (screenviewer_interface*)ptr;
	int sock, sock_finder;
	struct sockaddr_in addr = {0};
	socklen_t socklen;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	sock_finder = socket(PF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(30000);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	bind(sock_finder, (struct sockaddr*)&addr, sizeof(addr));
	listen(sock, 99);

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

			sock_con = accept(sock, (struct sockaddr*)&addr, &len);
			std::cout << "[screen]viewer incoming from " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << "." << std::endl;
		}
	}

	pthread_exit(0);
	return NULL;
}
