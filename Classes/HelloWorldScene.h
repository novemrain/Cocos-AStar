#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "AStar.h"

#define WIDTH 128
#define HEIGHT 70
#define CELL_WIDTH	10
#define CELL_HEIGHT	10

class AStarGame : public cocos2d::Layer
{
public:
	AStarGame();
	virtual ~AStarGame();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
	void findCallback(cocos2d::Ref* pSender);
	void restartCallback(cocos2d::Ref* pSender);

	
    // implement the "static create()" method manually
    CREATE_FUNC(AStarGame);

	Node*	root;
	short** map;
	void drawMap();
	void drawPath(Step*);

	// thread
	bool bOver;
	Step* path;
	void asyncUpdate(float t);
	void findPathAsync();
	std::thread	*m_pWorkThread;
};

#endif // __HELLOWORLD_SCENE_H__
