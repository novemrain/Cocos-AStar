#include "AStar.h"
#include "cocos2d.h"
#include <vector>
#include <set>
#include <algorithm>
#if 1
static void sort(std::vector<Step*> &vec)
{
	// only care about the best one
	// do not sort the others
	size_t index = 0;
	for (size_t i = 1; i < vec.size(); i++)
	{
		if (vec[i]->getF() < vec[index]->getF())
			index = i;
	}
	if (index != 0) std::swap(vec[0], vec[index]);
}

AStarMap::AStarMap(short **_map, short _width, short _height)
{
	this->width = _width;
	this->height = _height;
	map = _map;
}

std::vector<Step*> AStarMap::getNeighbors(Step* step, Pos origin, Pos destination, short direction)
{
	std::vector<Step*> neighbors;
	short x;
	short y;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (!(i == 0 && j == 0))
			{
				//neighbor不能等于自己
				if (i != 0 && j != 0 && direction == 4)
				{
					//direction 4 时不考虑对角线
				}
				else
				{
					x = step->x + i;
					y = step->y + j;
					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						//没有出界

						if (map[x][y] == 1)
						{
							// whether this neighbor can get through
							Step * neighbor = new Step(x, y, step);

							float stepCost = (i == 0 || j == 0) ? 1.0f : 1.4f;
							neighbor->g = step->g + stepCost;
							neighbor->h = abs(destination.x - x) + abs(destination.y - y);

							neighbors.push_back(neighbor);
						}

					}
				}
			}
		}
	}
	return neighbors;
}

Step* AStarMap::findPath(AStarMap * map, Pos origin, Pos destination, short direction, bool isAStar)
{
	clock_t start, finish;
	start = clock();

	// result , the final step if there is a path , otherwise nullptr
	Step* target = nullptr;
	// open & closed set
	std::vector<Step*> open;
	bool **closed = new bool*[map->width];
	// G array & OpenSet array
	// Faster when querying than vector
	float **G = new float*[map->width];
	Step ***O = new Step**[map->width];

	// data init
	for (int i = 0; i < map->width; i++)
	{
		closed[i] = new bool[map->height];
		G[i] = new float[map->height];
		O[i] = new Step*[map->height];
		for (int j = 0; j < map->height; j++)
		{
			closed[i][j] = false;
			G[i][j] = -1.0f;
			O[i][j] = nullptr;
		}
	}

	// make first step , add it to open
	Step* currentStep = new Step(origin.x, origin.y, nullptr);
	currentStep->g = 0;
	currentStep->h = abs(destination.x - origin.x) + abs(destination.y - origin.y);
	open.push_back(currentStep);

	bool bOver = false; 
	std::vector<Step*>::iterator n;
	while (open.size() > 0 && bOver == false)
	{
		currentStep = open[0];
		open.erase(open.begin());
		closed[currentStep->x][currentStep->y] = true;

		// neighbors u can go(without forbidden pos)
		std::vector<Step*> neighbors = map->getNeighbors(currentStep, origin, destination, direction);

		for (n = neighbors.begin(); n != neighbors.end();)
		{
			// Condition 1/4 , whether this neighbor in closed
			if (closed[(*n)->x][(*n)->y])
			{
				delete *n;
				n = neighbors.erase(n);
				continue;
			}

			// Condition 2/4 , whether this neighbor in open
			if (G[(*n)->x][(*n)->y] > 0.0f)
			{
				if ((*n)->g < G[(*n)->x][(*n)->y])
				{
					// this neighbor in open got a better G then before
					G[(*n)->x][(*n)->y] = (*n)->g;
					O[(*n)->x][(*n)->y]->g = (*n)->g;
					O[(*n)->x][(*n)->y]->parent = (*n)->parent;
				}
				delete *n;
				n = neighbors.erase(n);
				continue;
			}

			// Condition 3/4 , whether this neighbor is TARGET
			if ((*n)->x == destination.x && (*n)->y == destination.y)
			{
				bOver = true;
				target = *n;
				// clear neighbors
				for (n = ++n; n != neighbors.end(); ++n)
				{
					delete *n;
				}
				neighbors.clear();
				break;
			}

			// Condition 4/4 , well , it's just a new neighbor
			// add to open
			open.push_back(*n);
			G[(*n)->x][(*n)->y] = (*n)->g;
			O[(*n)->x][(*n)->y] = *n;
			// go to next
			++n;
		}

		// ready for the next loop , if needed(not over)
		if (bOver == false)
		{
			neighbors.clear();
			sort(open);
		}
	}

	// print the totaltime used
	finish = clock();
	float totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	CCLOG("AStarMap::findPath cost %f", totaltime);

	// delete & relase
	for (std::vector<Step*>::iterator o = open.begin(); o != open.end(); ++o)
	{
		delete *o;
	}
	open.clear();
	delete[] closed;
	delete[] G;
	delete[] O;

	// target is nullptr if no path can be found
	return target;
}
#endif
#if 0
static const int SORT_LENGTH = 10;
static bool compare(Step* i, Step* j)
{
	return i->getF() < j->getF();
}
static bool compareReverse(Step* i, Step* j)
{
	return i->getF() > j->getF();
}

AStarMap::AStarMap(short **_map, short _width, short _height)
{
	this->width = _width;
	this->height = _height;
	map = _map;
}

std::vector<Step*> AStarMap::getNeighbors(Step* step, Pos origin, Pos destination, short direction)
{
	std::vector<Step*> neighbors;
	short x;
	short y;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (!(i == 0 && j == 0))
			{
				//neighbor不能等于自己
				if (i != 0 && j != 0 && direction == 4)
				{
					//direction 4 时不考虑对角线
				}
				else
				{
					x = step->x + i;
					y = step->y + j;
					if (x >= 0 && x < width && y >= 0 && y < height)
					{
						//没有出界

						if (map[x][y] == 1)
						{
							// whether this neighbor can get through
							Step * neighbor = new Step(x, y, step);

							float stepCost = (i == 0 || j == 0) ? 1.0f : 1.4f;
							neighbor->g = step->g + stepCost;
							neighbor->h = abs(destination.x - x) + abs(destination.y - y);

							neighbors.push_back(neighbor);
						}

					}
				}
			}
		}
	}
	// to make more front inserts
	// TODO test
	std::sort(neighbors.begin(), neighbors.end(), compareReverse);
	return neighbors;
}

Step* AStarMap::findPath(AStarMap * map, Pos origin, Pos destination, short direction, bool isAStar)
{
	//F越小越优先
	CCLOG("AStarMap::findPath");
	clock_t start, finish;
	float totaltime;
	start = clock();

	// result
	Step* target = nullptr;
	// report value
	int loopOpens = 0;
	int loopTimes = 0;
	int sortTimes = 0;
	int sortReduceInside = 0;
	int sortReduceOutside = 0;
	int sortReduceLast = 0;
	// open set
	std::vector<Step*> open;
	// close set
	bool **closed = new bool*[map->width];
	float **G = new float*[map->width];
	for (int i = 0; i < map->width; i++)
	{
		closed[i] = new bool[map->height];
		G[i] = new float[map->height];
		for (int j = 0; j < map->height; j++)
		{
			closed[i][j] = false;
			G[i][j] = -1.0f;
		}
	}

	// make first step , add it to open
	Step* currentStep = new Step(origin.x, origin.y, nullptr);
	currentStep->g = 0;
	currentStep->h = abs(destination.x - origin.x) + abs(destination.y - origin.y);
	open.push_back(currentStep);

	bool bOver;
	bool bSort;
	bool bOpen;
	//bool bSorted = true;
	short frontSorted = 1;
	std::vector<Step*>::iterator n;
	while (open.size() > 0)
	{
		loopTimes++;
		bOver = false;
		bSort = true;
		currentStep = open[0];
		open.erase(open.begin());
		closed[currentStep->x][currentStep->y] = true;
		if (frontSorted > 0)frontSorted--;

		//neighbors u can go(without forbidden pos)
		std::vector<Step*> neighbors = map->getNeighbors(currentStep, origin, destination, direction);
		//CCLOG("LOOP %d TIMES and neighbors %d", loopTimes, neighbors.size());
		//CCLOG("LOOP current x : %d , y : %d", currentStep->x, currentStep->y);

		for (n = neighbors.begin(); n != neighbors.end();)
		{
			// whether this neighbor in closed
			if (closed[(*n)->x][(*n)->y])
			{
				delete *n;
				n = neighbors.erase(n);
				continue;
			}

			// whether this neighbor in open
			// whether this neighbor in open
			bOpen = G[(*n)->x][(*n)->y] > 0.0f;
			if (bOpen)
			{
				if ((*n)->g < G[(*n)->x][(*n)->y])
				{
					loopOpens++;

					// this neighbor in open got a better G then before
					G[(*n)->x][(*n)->y] = (*n)->g;
					for (int i = 0; i < open.size(); i++)
					{
						if (open[i]->x == (*n)->x && open[i]->y == (*n)->y)
						{
							open[i]->g = (*n)->g;
							open[i]->parent = (*n)->parent;

							if (frontSorted > 0 && open[i]->getF() <= open[0]->getF())
							{
								// this neighbor got best F , swap it to the front
								std::swap(open[0], open[i]);
								if (bSort)sortReduceInside++;
								bSort = false;
								frontSorted++;
							}
							else
							{
								if (frontSorted > 0)frontSorted = 1;
							}

							break;
						}
					}
				}
				delete *n;
				n = neighbors.erase(n);
				continue;
			}

			// whether this neighbor is TARGET
			if ((*n)->x == destination.x && (*n)->y == destination.y)
			{
				bOver = true;
				target = *n;
				// clear neighbors
				for (n = ++n; n != neighbors.end(); ++n)
				{
					delete *n;
				}
				neighbors.clear();
				break;
			}

			// well , it's just a new neighbor
			// add to open
			if (open.size() == 0 || (/*bSorted == true*/frontSorted > 0 && (*n)->getF() <= open[0]->getF()))
			{
				// it must be the best in open
				// insert it to the front
				open.insert(open.begin(), *n);
				if (bSort)sortReduceOutside++;
				bSort = false;
				frontSorted++;
			}
			else
			{
				open.push_back(*n);
				if (frontSorted > 0)frontSorted = 1;
			}

			G[(*n)->x][(*n)->y] = (*n)->g;

			// go to next
			++n;
		}

		// check whether the loop is over
		if (bOver)
		{
			for (std::vector<Step*>::iterator o = open.begin(); o != open.end(); ++o)
			{
				delete *o;
			}
			open.clear();
			break;
		}
		else
		{
			neighbors.clear();
			// sort open
			if (bSort && /*!bSorted*/frontSorted == 0)
			{
				//this time did not found the best , 
				//while the front is not sure the best
				std::sort(open.begin(), open.end(), compare);
				sortTimes++;
				frontSorted = open.size();
				/*bSorted = true*/
			}
			else if (!bSort)
			{
				//this time found the best , 
				//already swapped to the front
				/*bSorted = true;*/
			}
			else/*bSorted == true*/
			{
				//last time sorted
				//next time may need to sort
				/*bSorted = false;*/
				sortReduceLast++;
			}
		}
	}

	if (target == nullptr)
		CCLOG("AStarMap::findPath over can't find path %d", loopTimes);
	else
		CCLOG("AStarMap::findPath over path found! %d", loopTimes);

	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	CCLOG("AStarMap::findPath sortTimes %d , inside reduce %d , outside reduce %d , frontSorted %d", sortTimes, sortReduceInside, sortReduceOutside, sortReduceLast);
	CCLOG("AStarMap::findPath loopOpens %d , cost %f", loopOpens, totaltime);
	delete[] G;
	delete[] closed;
	return target;
}
#endif