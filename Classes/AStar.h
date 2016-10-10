#pragma once
#include <vector>

struct Pos{
	short x, y;
	Pos(short _x, short _y)
	:x(_x),
	y(_y)
	{
	}
};
class Step
{
public:
	Step(short _x, short _y, Step* _parent)
	:x(_x),
	y(_y),
	parent(_parent)
	{};
	~Step(){};

	// position
	short x, y;
	// previous step
	Step* parent;

	float g;
	float h;
	float getF()
	{
		// the smaller F is better
		return g + h;
	};
};



class AStarMap
{
public:
	AStarMap(short **_map, short _width, short _height);
	~AStarMap()
	{
		delete[] map;
	};

	std::vector<Step*> getNeighbors(Step* step, Pos origin, Pos destination, short direction);
	static Step* findPath(AStarMap * map , Pos origin, Pos destination, short direction = 8, bool isAStar = true);
private:
	short **map;
	short width;
	short height;
};

