#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

/////////////////////////////////////////////////////////////////
///////Actor Base Class Declaration
/////////////////////////////////////////////////////////////////
class StudentWorld;
class Actor : public GraphObject
{
public:
	Actor(int imageID, double x, double y, int direction, double size, int depth, StudentWorld* world);
	virtual ~Actor(){}
	virtual void doSomething() = 0;
	virtual std::string description() const = 0;
	bool isAlive() const;
	void setDead();
	void setSpeed(double vSpeed, double hSpeed);
	StudentWorld* getWorld() const;
	double gethSpeed() const;
	double getvSpeed() const;
	virtual bool beSprayedIfAppropriate();
	virtual bool isCollisionAvoidanceWorthy() const;
	virtual bool moveRelativeToGhostRacerSpeed();
	virtual bool isSprayable() const = 0;
	virtual int getLane();
private:
	bool m_alive = true;
	double m_vSpeed = 0;
	double m_hSpeed = 0;
	StudentWorld* m_world;
};

///////////////////////////////////
//////BorderLine Constants
///////////////////////////////////
const int RIGHT_EDGE = (ROAD_CENTER + (ROAD_WIDTH / 2));
const int LEFT_EDGE = (ROAD_CENTER - (ROAD_WIDTH / 2));
const int BL_STARTDIR = 0;
const int BL_DEPTH = 1;
const double BL_SIZE = 2.0;
const int BL_HP = 1;
const std::string YELLOW_TYPE_BL = "0";
const std::string WHITE_TYPE_BL = "1";
const int NEW_BORDER_Y = (VIEW_HEIGHT - SPRITE_HEIGHT);
///////////////////////////////////
//////BorderLine Class Declaration
///////////////////////////////////

class BorderLine : public Actor
{
public:
	BorderLine(int ImageID, double x, double y, StudentWorld* world, std::string type);
	virtual ~BorderLine(){}
	virtual void doSomething();
	virtual std::string description() const;
	virtual bool isSprayable() const { return false; }
private:
	std::string m_type;
};
//////////////////////////////////////////////////////////////////
///////Agent Base Class Declaration
//////////////////////////////////////////////////////////////////
class Agent : public Actor
{
public:
	Agent(int imageID, double x, double y, int direction, double size, int depth, int HP, StudentWorld* world);
	virtual ~Agent(){}
	virtual bool isCollisionAvoidanceWorthy() const;
	int health() const;
	void getHP(int hp);
	virtual bool doDamage(int damage);
	virtual int soundWhenHurt();
	virtual int soundWhenDie();
	virtual bool isSprayable() const { return true; }
private: 
	int m_hp;
};
///////////////////////////////////
//////Ghost Racer Constants
///////////////////////////////////
const double GR_STARTX = 128;
const double GR_STARTY = 32;
const int GR_STARTDIR = 90;
const double GR_SIZE = 4.0;
const int GR_DEPTH = 0;
const int GR_HP = 100;
const double MAX_SHIFT = 4.0;
const double PI = 3.14159265358979323846;
///////////////////////////////////
//////Ghost Racer Class Declaration
///////////////////////////////////

class GhostRacer : public Agent
{
public:
	GhostRacer(StudentWorld* world);
	virtual ~GhostRacer(){}
	virtual int soundWhenDie();
	virtual int soundWhenHurt();
	virtual void doSomething();
	virtual std::string description() const;
	int getWater()const;
	void GR_move();
	virtual bool isSprayable() const { return false; };
	void addWater(int water);
	void spin();
private:
	int m_amountHolyWater = 10;

};
///////////////////////////////////
//////Pedestrian Constants
///////////////////////////////////
const int PED_STARTDIR = 0;
const double HPED_SIZE = 2.0;
const double ZPED_SIZE = 3.0;
const int PED_DEPTH = 0;
const int PED_HP = 2;
/////////////////////////////////////////
///////Pedestiran Base Class Declaration
/////////////////////////////////////////
class Pedestrian : public Agent
{
public:
	Pedestrian(StudentWorld* world, int imageID, double x, double y, double size);
	virtual ~Pedestrian(){}
	virtual int soundWhenHurt() const;
	virtual int soundWhenDie() const;

	// Move the pedestrian.  If the pedestrian doesn't go off screen and
	// should pick a new movement plan, pick a new plan.
	void moveAndPossiblyPickPlan();
private:
	int m_MPD = 0;
};

///////////////////////////////////
//////H-PED Class Declaration
///////////////////////////////////
class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(StudentWorld* world, double x, double y);
	virtual ~HumanPedestrian(){}
	virtual std::string description() const;
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
};
///////////////////////////////////
//////Z-PED Class Declaration
///////////////////////////////////
class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(StudentWorld* world, double x, double y);
	virtual ~ZombiePedestrian(){}
	virtual std::string description() const;
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
private:
	int m_grunt = 0;
};
///////////////////////////////////
//////Zombie Cab Constants
///////////////////////////////////
const int ZC_HP = 3;
///////////////////////////////////
//////Zombie Cab Class Declaration
///////////////////////////////////
class ZombieCab : public Agent
{
public:
	ZombieCab(StudentWorld* world, double x, double y, double speed);
	virtual ~ZombieCab(){}
	virtual void doSomething();
	std::string description() const;
	virtual bool beSprayedIfAppropriate();
	virtual int soundWhenHurt() const;
	virtual int soundWhenDie() const;
private:
	int m_numCollisions = 0;
	int m_planL = 0;
};

/////////////////////////////////////////
///////Spray Class Constants
/////////////////////////////////////////
const double SPRAY_SIZE = 1.0;
const int SPRAY_DEPTH = 1;
/////////////////////////////////////////
///////Spray Class Declaration
/////////////////////////////////////////
class Spray : public Actor
{
public:
	Spray(StudentWorld* world, double x, double y, int dir);
	virtual ~Spray(){}
	virtual void doSomething();
	virtual std::string description() const;
	virtual bool isSprayable() const { return false; }
private:
	double travelDist = 160;
};

/////////////////////////////////////////
///////Activator Base Class Constants
/////////////////////////////////////////
const int ACT_DEPTH = 2;
const int OS_DIR = 0;
const double HG_SIZE = 2.0;
const int HG_DIR = 90;
/////////////////////////////////////////
///////Activator Base Class Declaration
/////////////////////////////////////////
class Activator : public Actor
{
public:
	Activator(StudentWorld* world, int imageID, double x, double y, double size, int dir);
	virtual ~Activator(){}
	virtual bool beSprayedIfAppropriate();

	virtual void doSomething();
	// Do the object's special activity (increase health, spin Ghostracer, etc.)
	virtual void doActivity(GhostRacer* gr) = 0;

	// Return the object's increase to the score when activated.
	virtual int getScoreIncrease() const = 0;

	// Return the sound to be played when the object is activated.
	virtual int getSound() const;

	// Return whether the object dies after activation.
	virtual bool selfDestructs() const = 0;

	// Return whether the object is affected by a holy water projectile.
	virtual bool isSprayable() const { return false; }
};

/////////////////////////////////////////
///////Oil Slick Class Declaration
/////////////////////////////////////////
class OilSlick : public Activator
{
public:
	OilSlick(StudentWorld* world, double x, double y);
	virtual ~OilSlick(){}
	virtual std::string description() const;
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const;
	virtual int getSound() const;
	virtual bool selfDestructs() const;
};
/////////////////////////////////////////
///////Healing Goodie Class Declaration
/////////////////////////////////////////
class HealingGoodie : public Activator
{
public:
	HealingGoodie(StudentWorld* world, double x, double y);
	virtual ~HealingGoodie(){}
	virtual std::string description() const;
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const;
	virtual bool selfDestructs() const;
	virtual bool isSprayable() const;
};

/////////////////////////////////////////
//////Holy Water Class Declaration
/////////////////////////////////////////
class HolyWaterGoodie : public Activator
{
public:
	HolyWaterGoodie(StudentWorld* world, double x, double y);
	virtual ~HolyWaterGoodie(){}
	virtual void doActivity(GhostRacer* gr);
	virtual std::string description() const { return "HW"; }
	virtual int getScoreIncrease() const { return 50; }
	virtual bool selfDestructs() const { return true; }
	virtual bool isSprayable() const { return true; }
};
/////////////////////////////////////////
//////Soul Class Declaration
/////////////////////////////////////////
class SoulGoodie : public Activator
{
public:
	SoulGoodie(StudentWorld* world, double x, double y);
	virtual ~SoulGoodie(){}
	std::string description() const { return "SG"; }
	virtual void doActivity(GhostRacer* gr);
	virtual int getScoreIncrease() const { return 100; }
	virtual int getSound() const { return SOUND_GOT_SOUL; }
	virtual bool selfDestructs() const { return true; }
};
#endif // ACTOR_H_
