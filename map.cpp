#include "map.h"
#include <iostream>

map::map()
{
	mapdata = NULL;
}
map::~map()
{
	if(mapdata) free(mapdata);
}

void map::init(maploader* loader)
{
	std::cout << "[map   ]loading..." << std::endl;
	if(mapdata != NULL) free(mapdata);

	width = loader->get_width();
	height = loader->get_height();
	cool = loader->get_cool_pos();
	hot = loader->get_hot_pos();
	turns = loader->get_turns();
	mapdata = (char*)malloc(width * height);
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			mapdata[x + y * width] = loader->get_mapdata(x, y);
		}
	}
	std::cout << "[map   ]loaded." << std::endl;
}

int map::get_width(){ return width; }
int map::get_height(){ return height; }
struct point& map::get_init_cool(){ return cool; }
struct point& map::get_init_hot(){ return hot; }
int map::get_turns(){ return turns; }
char* map::get_data(){ return mapdata; }
