#pragma once

// CHaser2013 Electro Clusters Server Communiating API

#pragma pack(push, 1)
struct mapdata_header
{
	char signature[4];
	unsigned char mw, mh;
	signed char cx, cy, hx, hy;
	unsigned int turns, ctname_length, htname_length;
};

enum method_type
{
	undefined,
	walk,
	look,
	search,
	put,
	getready
};
enum direction
{
	unknown,
	left,
	right,
	up,
	down
};
struct mapdata_header_with_methods
{
	char signature[4];
	signed char cx, cy, hx, hy;
	char cm, hm;
	char cd, hd;
	unsigned int ci, hi, trurns;
};

enum result_reason
{
	not_defined,
	item_gets,
	block_put,
	round_block,
	block_shock,
	timeout,
	disconnected
};
struct result_header
{
	char signature[4];
	char winner_head;
	char reason;
};
#pragma pack(pop)
