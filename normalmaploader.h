#pragma once

#include "maploader.h"

class normalmaploader
	: public maploader
{
public:
	normalmaploader(char* filename);

	virtual int get_width();
	virtual int get_height();
	virtual int get_turns();
	virtual struct point& get_cool_pos();
	virtual struct point& get_hot_pos();
	virtual char get_mapdata(int x, int y);
	virtual std::string get_title();
private:
	char* mpd;
	int w, h, t;
	struct point _c, _h;
	std::string _t;
};

