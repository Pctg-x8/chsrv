#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>

struct point
{
	int x, y;
};

class maploader
{
public:
	virtual int get_width() = 0;
	virtual int get_height() = 0;
	virtual int get_turns() = 0;
	virtual struct point& get_cool_pos() = 0;
	virtual struct point& get_hot_pos() = 0;
	virtual char get_mapdata(int x, int y) = 0;
	virtual std::string get_title() = 0;
};
