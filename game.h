#pragma once

#include <iostream>
#include "character_interface.h"
#include "screenviewer_interface.h"

class game
{
public:
	game();

	void init();
	void entry();
private:
	character_interface cool, hot;
	screenviewer_interface viewer;
};
