#include "maploader_factory.h"
#include "normalmaploader.h"

maploader* maploader_factory::create(char* filename)
{
	return new normalmaploader(filename);
}
