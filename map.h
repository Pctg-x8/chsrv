#pragma once

// map

#include <stdlib.h>
#include "maploader.h"
#include "normalmaploader.h"

class map
{
public:
	map();
	virtual ~map();
	void init(maploader* loader);

	int get_width();
	int get_height();
	struct point& get_init_cool();
	struct point& get_init_hot();
	int get_turns();
	char* get_data();
private:
	char* mapdata;
	int width, height, turns;
	struct point cool, hot;
};

