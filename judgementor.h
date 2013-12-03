#pragma once

#include "map.h"
#include "character_interface.h"

class judgementor
{
public:
	static bool judge_map(character_interface& c, character_interface& h, map& m_inst);

	static char target;
	static char reason;
};
