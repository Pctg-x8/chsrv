#include "normalmaploader.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

normalmaploader::normalmaploader(char* filename)
{
	std::ifstream fp(filename);
	std::string line;
	int data_ptr = 0;

	if(!fp) exit(4);

	_t = "unnamed";
	w = 23;
	h = 17;
	t = 200;
	mpd = (char*)malloc(w * h);

	while(std::getline(fp, line))
	{
		if(line[1] != ':') continue;
		if(line[0] == 'N')
		{
			_t = line.substr(2);
		}
		else if(line[0] == 'S')
		{
			std::stringstream ws, hs;

			ws << line.substr(2, line.find_last_of(',') - 2);
			hs << line.substr(line.find_last_of(',') + 1);
			ws >> w;
			hs >> h;
			if(mpd) free(mpd);
			mpd = (char*)malloc(w * h);
		}
		else if(line[0] == 'T')
		{
			std::stringstream ts;
			ts << line.substr(2);
			ts >> t;
		}
		else if(line[0] == 'D')
		{
			int p;
			for(int i = 2; (p = line.find_first_of(',', i)), i != 0;)
			{
				std::stringstream ss;
				if(p == std::string::npos)
				{
					ss << line.substr(i);
				}
				else
				{
					ss << line.substr(i, p - i);
				}
				int val;
				ss >> val;
				mpd[data_ptr] = (char)val;
				data_ptr++;
				i = p + 1;
			}
		}
		else if(line[0] == 'H')
		{
			std::stringstream xs, ys;
			xs << line.substr(2, line.find_last_of(',') - 2);
			ys << line.substr(line.find_last_of(',') + 1);
			xs >> _h.x;
			ys >> _h.y;
		}
		else if(line[0] == 'C')
		{
			std::stringstream xs, ys;
			xs << line.substr(2, line.find_last_of(',') - 2);
			ys << line.substr(line.find_last_of(',') + 1);
			xs >> _c.x;
			ys >> _c.y;	
		}
		else continue;
	}
}

int normalmaploader::get_width(){ return w; }
int normalmaploader::get_height(){ return h; }
int normalmaploader::get_turns(){ return t; }
struct point& normalmaploader::get_cool_pos(){ return _c; }
struct point& normalmaploader::get_hot_pos(){ return _h; }
char normalmaploader::get_mapdata(int x, int y){ return mpd[x + y * w]; }
std::string normalmaploader::get_title(){ return _t; }
