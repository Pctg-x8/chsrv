#include "judgementor.h"

char judgementor::target, judgementor::reason;

bool judgementor::judge_map(character_interface& c, character_interface& h, map& m_inst)
{
	target = 'd';
	reason = item_gets;

	// cool side errors
	if(m_inst.get_pattern(c.get_pos().x, c.get_pos().y) == 2)
	{
		target = 'c';
		reason = block_shock;
		return true;
	}
	if(m_inst.get_pattern(c.get_pos().x - 1, c.get_pos().y) == 2
	&& m_inst.get_pattern(c.get_pos().x + 1, c.get_pos().y) == 2
	&& m_inst.get_pattern(c.get_pos().x, c.get_pos().y - 1) == 2
	&& m_inst.get_pattern(c.get_pos().x, c.get_pos().y + 1) == 2)
	{
		target = 'c';
		reason = round_block;
		return true;
	}
	
	// hot side errors
	if(m_inst.get_pattern(h.get_pos().x, h.get_pos().y) == 2)
	{
		target = 'h';
		reason = block_shock;
		return true;
	}
	if(m_inst.get_pattern(h.get_pos().x - 1, h.get_pos().y) == 2
	&& m_inst.get_pattern(h.get_pos().x + 1, h.get_pos().y) == 2
	&& m_inst.get_pattern(h.get_pos().x, h.get_pos().y - 1) == 2
	&& m_inst.get_pattern(h.get_pos().x, h.get_pos().y + 1) == 2)
	{
		target = 'h';
		reason = round_block;
		return true;
	}

	return false;
}
