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
	cool.prepare(m_inst.get_init_cool());
	hot.prepare(m_inst.get_init_hot());
	while(m_inst.get_turns())
	{
		if(!cool.enter())
		{
c_err:
			std::cout << "[game  ]error from cool." << std::endl;
			break;
		}
		if(!cool.get_ready(m_inst, hot.get_pos())) goto c_err;
		viewer.dispatch_update(cool, hot, m_inst);
		if(!cool.process_method(m_inst, hot.get_pos())) goto c_err;
		if(judgementor::judge_map(cool, hot, m_inst))
		{
			if(judgementor::target == 'c') goto c_err; else goto h_err;
		}
		viewer.dispatch_update(cool, hot, m_inst);
		if(!cool.leave()) goto c_err;
		if(!hot.enter())
		{
h_err:
			std::cout << "[game  ]error from hot." << std::endl;
			break;
		}
		if(!hot.get_ready(m_inst, cool.get_pos())) goto h_err;
		viewer.dispatch_update(cool, hot, m_inst);
		if(!hot.process_method(m_inst, cool.get_pos())) goto h_err;
		if(judgementor::judge_map(cool, hot, m_inst))
		{
			if(judgementor::target == 'c') goto c_err; else goto h_err;
		}
		viewer.dispatch_update(cool, hot, m_inst);
		if(!hot.leave()) goto h_err;
		m_inst.decrease_turns();
	}
	viewer.dispatch_update(cool, hot, m_inst);
	viewer.dispatch_result(
		cool.get_items() == hot.get_items() ? 'd' : (cool.get_items() > hot.get_items() ? 'c' : 'h'),
		judgementor::reason);
	cool.enter();
	cool.get_ready(m_inst, hot.get_pos(), true);
	cool.process_method(m_inst, hot.get_pos(), true);
	cool.leave();
	hot.enter();
	hot.get_ready(m_inst, cool.get_pos(), true);
	hot.process_method(m_inst, cool.get_pos(), true);
	hot.leave();
}
