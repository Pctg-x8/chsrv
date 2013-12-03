#pragma once

#include <stdio.h>
#include <iostream>
#include "character_interface.h"
#include "screenviewer_interface.h"
#include "map.h"
#include "maploader_factory.h"

class game
{
public:
	game();

	void init(char* mapfile);
	void entry();
	void loop();
private:
	character_interface cool, hot;
	screenviewer_interface viewer;
	map m_inst;
};
