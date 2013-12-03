#pragma once

#include "maploader.h"

class maploader_factory
{
public:
	static maploader* create(char* filename);
};
