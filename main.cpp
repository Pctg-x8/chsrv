#include <iostream>
#include "game.h"

int main(int argc, char** argv)
{
	game gm;
	std::cout << "CHaser2013 Electro Clusters Server for Linux" << std::endl;
	if(argc <= 1)
	{
		std::cout << "usage>chsrv mapfile_name" << std::endl;
		return 1;
	}

	std::cout << "Note: heap limit: " << sbrk(0) << std::endl;

	gm.init(argv[1]);
	gm.entry();
	gm.loop();

	std::cout << "exiting..." << std::endl;

	return 0;
}
