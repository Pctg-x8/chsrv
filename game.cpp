#include "game.h"
#include <iostream>

game::game()
	: cool("cool", 40000), hot("hot", 50000), m_inst()
{

}

void game::init(char* mapfile)
{
	std::cout << "[game  ]initializing..." << std::endl;
	cool.init();
	hot.init();
	viewer.init();
	m_inst.init(maploader_factory::create(mapfile));
}

void game::entry()
{
	while(true)
	{
		cool.check_entry();
		hot.check_entry();
		if(cool.is_connected() && hot.is_connected()) break;
	}

	std::cout << "press any key to start...";
	getchar();
}

void game::loop()
{
	viewer.dispatch_initializer(cool, hot, m_inst);
	while(true)
	{
		cool.enter();
		cool.get_ready();
		viewer.dispatch_update(cool, hot, m_inst);
		cool.method();
		cool.leave();
	}
}
