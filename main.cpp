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

	gm.init();

	gm.entry();
	return 0;
}
