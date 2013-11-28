#include "game.h"
#include <iostream>

game::game()
	: cool("cool", 40000), hot("hot", 50000)
{

}

void game::init()
{
	std::cout << "[game  ]initializing..." << std::endl;
	cool.init();
	hot.init();
	viewer.init();
}

void game::entry()
{
	while(true)
	{
		cool.check_entry();
		hot.check_entry();
	}
}
