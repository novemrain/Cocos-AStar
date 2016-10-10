#include "HelloWorldScene.h"
#include "UVSprite.h"
USING_NS_CC;

AStarGame::AStarGame()
:map(nullptr)
, m_pWorkThread(nullptr)
, path(nullptr)
, bOver(false)
{}
AStarGame::~AStarGame()
{
	delete[] map;
	if (m_pWorkThread)m_pWorkThread->join();
	CC_SAFE_DELETE(m_pWorkThread);
}

Scene* AStarGame::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = AStarGame::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool AStarGame::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto findItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(AStarGame::findCallback, this));
    
	findItem->setPosition(Vec2(origin.x + visibleSize.width - findItem->getContentSize().width / 2,
		origin.y + findItem->getContentSize().height / 2));

	auto restartItem = MenuItemImage::create(
		"CloseSelected.png",
		"CloseNormal.png",
		CC_CALLBACK_1(AStarGame::restartCallback, this));

	restartItem->setPosition(Vec2(origin.x + visibleSize.width - restartItem->getContentSize().width / 2,
		origin.y + restartItem->getContentSize().height * 4/ 2));


    // create menu, it's an autorelease object
	auto menu = Menu::create(findItem, restartItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	root = Node::create();
	this->addChild(root);

	UVSprite * sprite = UVSprite::create("ball.png");
	sprite->setPosition(Vec2(visibleSize / 2));
	addChild(sprite);
	sprite->setUVTexture("snowflake2.png");
	sprite->setUvStretch(false);
	sprite->setUvVelocity(Vec2(0.0f, -0.01f));
	sprite->setUvAlphaFilter(true);
	
    return true;
}

void AStarGame::drawMap()
{
	map = new short*[WIDTH];

	// make a random map
 	for (int i = 0; i < WIDTH; i++)
	{
		map[i] = new short[HEIGHT];
		for (int j = 0; j < HEIGHT; j++)
		{
			bool avaliable = CCRANDOM_MINUS1_1() > -0.6f;
			if (i <= 1 || i >= WIDTH - 2 || j <= 1 || j >= HEIGHT - 2)
			{
				//avaliable = true;
			}
			map[i][j] = avaliable ? 1 : 0;
		}
	}

	// build some walls to make it complex
	for (int i = 1; i < 16; i++)
	{
		for (int j = 0; j < HEIGHT - 10; j++)
		{
			auto x = WIDTH * i / 16;
			auto y = i % 2 == 0 ? j : HEIGHT - j - 1;
			map[x][y] = 0;
		}
			
	}
	
	map[0][0] = 1;
	map[WIDTH - 1][HEIGHT - 1] = 1;

	// draw it
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			bool avaliable = map[i][j] == 1;
			auto sprite = Sprite::create(avaliable ? "white.png" : "black.png");
			sprite->setAnchorPoint(ccp(0, 0));
			sprite->setPosition(ccp(i * CELL_WIDTH, j * CELL_HEIGHT));
			root->addChild(sprite);
		}
	}
}
void AStarGame::drawPath(Step* step)
{
	if (step)
	{
		// path was found , draw it
		int steps = 0;
		while (step != nullptr)
		{
			auto x = step->x;
			auto y = step->y;
			Step* s = step;
			step = s->parent;
			delete s;

			auto sprite = Sprite::create("stars.png");
			sprite->setAnchorPoint(ccp(0, 0));
			sprite->setPosition(ccp(x * CELL_WIDTH, y * CELL_HEIGHT));
			root->addChild(sprite);

			steps++;
		}
		CCLOG("AStarGame::drawPath steps = %d", steps);
	}
	else
	{
		CCLOG("AStarGame::drawPath step null , no path");
	}
}
void AStarGame::restartCallback(Ref* pSender)
{
	// restart , draw a new random map
	root->removeAllChildren();
	delete[] map;
	bOver = false;
	root->runAction(CCSequence::create(
		CCDelayTime::create(0.5f),
		CallFunc::create(CC_CALLBACK_0(AStarGame::drawMap, this)),
		nullptr
		));
}
void AStarGame::findCallback(Ref* pSender)
{
	// find path async
	if (m_pWorkThread == nullptr)
	{

		m_pWorkThread = new std::thread(&AStarGame::findPathAsync, this);
		auto scheduler = cocos2d::Director::getInstance()->getScheduler();
		// schedule a listener , detect whether the finding-work is over
		scheduler->schedule(schedule_selector(AStarGame::asyncUpdate), this, 0, false);
	}
}
void AStarGame::findPathAsync()
{
	AStarMap* AStar = new AStarMap(map, WIDTH, HEIGHT);
	auto path = AStarMap::findPath(AStar, Pos(0, 0), Pos(WIDTH - 1, HEIGHT - 1), 8);
	this->path = path;
	this->bOver = true;
}
void AStarGame::asyncUpdate(float t)
{
	// the listener function
	if (this->bOver)
	{
		drawPath(this->path);
		this->path = nullptr;
		auto scheduler = cocos2d::Director::getInstance()->getScheduler();
		scheduler->unschedule(schedule_selector(AStarGame::asyncUpdate), this);
		
		m_pWorkThread->join();
		CC_SAFE_DELETE(m_pWorkThread);
	}
	
}


//线程创建好之后可以自动执行
//若要强行插在主线程前面,那么join
//线程在结束前需要在主线程中对它调用join或者detach一次
//不能不调也不能重复!否则不能正确退出线程
//Cocos中工作线程里使用CCLog可能会导致死锁!

/*
void threadTest()
{
CCLOG("threadTest");
}


std::mutex mtx; // 全局互斥锁.
std::condition_variable cv; // 全局条件变量.
bool ready = false; // 全局标志位.
int sum = 0;
void do_print_id(int id)
{

std::unique_lock <std::mutex> lck(mtx);
while (!ready) // 如果标志位不为 true, 则等待...
{
//CCLOG("do_print_id start lock %d ", id);
cv.wait(lck); // 当前线程被阻塞, 当全局标志位变为 true 之后,
//CCLOG("do_print_id awake %d ", id);
//printf("waitover");
}
sum++;

}

void go()
{
CCLOG("go");
std::unique_lock <std::mutex> lck(mtx);
ready = true; // 设置全局标志位为 true.
cv.notify_all(); // 唤醒所有线程.
}
*/

//static std::thread t(threadTest);
//std::ref(t);
//t.join();
//CCLOG("AStarGame::restartCallback over");



/*
std::thread threads[10];
// spawn 10 threads:
for (int i = 0; i < 10; ++i)
threads[i] = std::thread(do_print_id, i);

CCLOG("begin%d" , sum);
go(); // go!
for (int i = 0; i < 10; ++i)
threads[i].join();
CCLOG("over%d", sum);
//线程创建好之后可以自动执行
//若要强行插在主线程前面,那么join
//线程在结束前需要在主线程中对它调用join或者detach一次
//不能不调也不能重复!否则不能正确退出线程
//Cocos中工作线程里使用CCLog可能会导致死锁!

//CCLOG("sleep");
//std::this_thread::sleep_for(std::chrono::milliseconds(500));
//CCLOG("sleep over %d" , sum);
//for (auto & th : threads)
//	th.detach();

//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//CCLOG("sleep over again %d", sum);

*/