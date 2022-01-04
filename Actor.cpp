#include "Actor.h"
#include "StudentWorld.h"
#include <list>
#include <math.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//////////////////////////////////////
///////Actor Base Class Implementation
//////////////////////////////////////

Actor::Actor(int imageID, double x, double y, int direction, double size, int depth, StudentWorld* world)
	: GraphObject(imageID, x, y, direction, size, depth), m_world(world)
{}


bool Actor::isAlive() const
{
	return m_alive;
}

void Actor::setDead()
{
	m_alive = false;
}
void Actor::setSpeed(double vSpeed, double hSpeed)
{
	m_vSpeed = vSpeed;
	m_hSpeed = hSpeed;
}
StudentWorld* Actor::getWorld() const
{
	return m_world;
}
double Actor::getvSpeed() const
{
	return m_vSpeed;
}
double Actor::gethSpeed() const
{
	return m_hSpeed;
}
bool Actor::beSprayedIfAppropriate()
{
	return false;
}
bool Actor::isCollisionAvoidanceWorthy() const
{
	return false;
}
int Actor::getLane()
{
	if (getX() >= LEFT_EDGE && getX() < LEFT_EDGE + (ROAD_WIDTH / 3))
		return 1;
	if (getX() >= LEFT_EDGE + (ROAD_WIDTH / 3) && getX() < RIGHT_EDGE - (ROAD_WIDTH / 3))
		return 2;
	if (getX() >= RIGHT_EDGE - (ROAD_WIDTH / 3) && getX() < RIGHT_EDGE)
		return 3;
	return 0;
}

bool Actor::moveRelativeToGhostRacerSpeed()
{
	double vert_speed = getvSpeed() - getWorld()->getGhostRacer()->getvSpeed();
	moveTo(getX() + gethSpeed(), getY() + vert_speed);
	if (getX() <= 0 || getY() <= 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		setDead();
	return isAlive();
}

//////////////////////////////////////
//////BorderLine Class Implementation
//////////////////////////////////////
BorderLine::BorderLine(int imageID, double x, double y, StudentWorld* world, std::string type)
	: Actor(imageID, x, y, BL_STARTDIR, BL_SIZE, BL_DEPTH, world), m_type(type)
{
	setSpeed(-4, 0);
}

void BorderLine::doSomething()
{
	if (isAlive() == false)
		return;
	moveRelativeToGhostRacerSpeed();
};

std::string BorderLine::description() const
{
	return m_type;
}

//////////////////////////////////////
///////Agent Base Class Implementation
//////////////////////////////////////
Agent::Agent(int imageID, double x, double y, int direction, double size, int depth, int HP, StudentWorld* world)
	: Actor(imageID, x, y, direction, size, depth, world) , m_hp(HP)
{}
bool Agent::isCollisionAvoidanceWorthy() const
{
	return true;
}
int Agent::health() const
{
	return m_hp;
}
void Agent::getHP(int hp)
{
	if(hp<=90)
	m_hp += hp;
}
int Agent::soundWhenHurt()
{
	return SOUND_PED_HURT;
}
int Agent::soundWhenDie()
{
	return SOUND_PED_DIE;
}

bool Agent::doDamage(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0)
		setDead();
	if (!isAlive()) {
		getWorld()->playSound(soundWhenDie());
		return isAlive();
	}
	else {
		getWorld()->playSound(soundWhenHurt());
		return isAlive();
	}
}
//////////////////////////////////////
//////Ghost Racer Class Implementation
//////////////////////////////////////

GhostRacer::GhostRacer(StudentWorld* world)
	: Agent(IID_GHOST_RACER, GR_STARTX, GR_STARTY, GR_STARTDIR, GR_SIZE, GR_DEPTH, GR_HP, world)
{
	setSpeed(0, 0);
}

int GhostRacer::soundWhenHurt()
{
	return SOUND_VEHICLE_CRASH;
}
int GhostRacer::soundWhenDie()
{
	return SOUND_PLAYER_DIE;
}
void GhostRacer::doSomething()
{
	// If Racer is not alive method must return immediately
	if (health() <= 0)
		return;

	// If the Ghost Racer's center X coordinate is less than or equal to the left road boundary(meaning it’s swerving off the road), it must :

	if (getX() <= LEFT_EDGE)
	{
		// Check if the GhostRacer is still facing toward the left side of the road(has an angle > 90 degrees) and if so, damage itself by 10 hit points.See the
		//	What a Ghost Racer Object Must Do In Other Circumstances section below for details on what it means to “damage” the Ghost Racer
		if (getDirection() > 90)
		{
			doDamage(10);
			if (!isAlive())
			{
				return;
			}
		}
		setDirection(82);
		//move
	}

	//If the Ghost Racer's center X coordinate is greater than or equal to the right road boundary, it must :
	if (getX() >= RIGHT_EDGE)
	{
		// Check if the GhostRacer is still facing toward the left side of the road(has an angle > 90 degrees) and if so, damage itself by 10 hit points.See the
		//	What a Ghost Racer Object Must Do In Other Circumstances section below for details on what it means to “damage” the Ghost Racer
		if (getDirection() < 90)
		{
			doDamage(10);
			if (!isAlive())
			{
				return;
			}
		}
		setDirection(98);
		//move
	}

	//The Ghost Racer must check to see if the player pressed a key (the section below shows how to check this).If the player pressed a key
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_SPACE:
		{
			if (getWater() > 0)
			{
				if (m_amountHolyWater <= 0)
					break;
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				getWorld()->addActor(new Spray(getWorld(), getX(), getY() + SPRITE_HEIGHT, getDirection()));
				m_amountHolyWater--;
				break; 
			}
			else
				break;
		}
		case KEY_PRESS_LEFT:
		{
			//If the user asks to move left by pressing a directional key AND GhostRacer’s current angle is less than 114 degrees:
			if (getDirection() < 114)
			{
				setDirection(getDirection() + 8);
				//move
				break;
			}
			else
				break;
		}
		case KEY_PRESS_RIGHT:
		{
			//If the user asks to move left by pressing a directional key AND GhostRacer’s current angle is less than 114 degrees:
			if (getDirection() > 66)
			{
				setDirection(getDirection() - 8);
				//move
				break;
			}
			else
				break;
		}
		case KEY_PRESS_UP:
		{
			if (getvSpeed() < 5)
			{
				setSpeed(getvSpeed() + 1, gethSpeed());
				//move
				break;
			}
			else
			break;
		}
		case KEY_PRESS_DOWN:
		{
			if (getvSpeed() > -1)
			{
				setSpeed(getvSpeed() - 1, gethSpeed());
				//move
				break;
			}
			else
				break;
		}
		}
	}
	GR_move();
}

std::string GhostRacer::description() const
{
	return "GhostRacer";
}

int GhostRacer::getWater() const
{
	return m_amountHolyWater;
}
void GhostRacer::addWater(int water)
{
	m_amountHolyWater += water;
}

void GhostRacer::GR_move()
{
	double dir = getDirection() * (PI / 180);
	double delta_x = cos(dir) * MAX_SHIFT;
	moveTo(getX() + delta_x, getY());
}
void GhostRacer::spin()
{
	int OG_dir = getDirection();
	while (getDirection() == OG_dir)
	{
		int delta_dir = randInt(5, 20);
		if (randInt(1, 2) == 1)
			delta_dir *= -1;
		if (getDirection() + delta_dir > 60 && getDirection() + delta_dir < 120)
			setDirection(getDirection() + delta_dir);
	}
}
//////////////////////////////////////
///////PED Base Class Implementation
//////////////////////////////////////
Pedestrian::Pedestrian(StudentWorld* world, int imageID, double x, double y, double size)
	:Agent(imageID, x, y, PED_STARTDIR, size, PED_DEPTH, PED_HP, world)
{
	setSpeed(-4, 0);
}

int Pedestrian::soundWhenDie() const
{
	return SOUND_PED_DIE;
}
int Pedestrian::soundWhenHurt() const
{
	return SOUND_PED_HURT;
}
void Pedestrian::moveAndPossiblyPickPlan()
{
	moveRelativeToGhostRacerSpeed();
	if (m_MPD > 0)
	{
		m_MPD--;
		return;
	}
	setSpeed(getvSpeed(), 0);
	while (gethSpeed() == 0)
	{
		setSpeed(getvSpeed(), randInt(-3, 3));
	}
	m_MPD = randInt(4, 32);
	if (gethSpeed() < 0)
		setDirection(180);
	else
		setDirection(0);
}
//////////////////////////////////////
///////H-PED Class Implementation
//////////////////////////////////////
HumanPedestrian::HumanPedestrian(StudentWorld* world, double x, double y)
	:Pedestrian(world, IID_HUMAN_PED, x, y, HPED_SIZE)
{}
std::string HumanPedestrian::description() const
{
	return "Human";
}
void HumanPedestrian::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->overlaps(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->setDead();
		return;
	}
	moveAndPossiblyPickPlan();
}
bool HumanPedestrian::beSprayedIfAppropriate()
{
	doDamage(0);
	setSpeed(getvSpeed(), gethSpeed() * -2);
	if (gethSpeed() < 0)
		setDirection(180);
	else
		setDirection(0);
	return true;
}

//////////////////////////////////////
///////Z-PED Class Implementation
//////////////////////////////////////
ZombiePedestrian::ZombiePedestrian(StudentWorld* world, double x, double y)
	:Pedestrian(world, IID_ZOMBIE_PED, x, y, ZPED_SIZE)
{
	setSpeed(-4, 0);
}

std::string ZombiePedestrian::description() const
{
	return "Zombie";
}
void ZombiePedestrian::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->overlaps(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->doDamage(5);
		doDamage(2);
		getWorld()->increaseScore(150);
		return;
	}
	int dist = (getX() - getWorld()->getGhostRacer()->getX());
	if (dist < 0)
		dist *= -1;
	if (dist <=30 && getY() > getWorld()->getGhostRacer()->getY())
	{
		setDirection(270);
		if (getX() < getWorld()->getGhostRacer()->getX())
			setSpeed(getvSpeed(), 1);
		else if (getX() > getWorld()->getGhostRacer()->getX())
			setSpeed(getvSpeed(), -1);
		else
			setSpeed(getvSpeed(), 0);
		m_grunt--;
		if (m_grunt <= 0)
		{
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			m_grunt = 20;
		}
	}
	moveAndPossiblyPickPlan();
}
bool ZombiePedestrian::beSprayedIfAppropriate()
{
	doDamage(1);
	if (!isAlive())
	{
		if (!getWorld()->overlaps(this, getWorld()->getGhostRacer()))
		{
			int ChanceHealing = randInt(1, 5);
			if (ChanceHealing == 1)
			getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
		}
		getWorld()->increaseScore(150);
	}
	return true; 
}
//////////////////////////////////////
//////Zombie Cab Class Implementation
//////////////////////////////////////

ZombieCab::ZombieCab(StudentWorld* world, double x, double y, double speed)
	:Agent(IID_ZOMBIE_CAB, x, y, GR_STARTDIR, GR_SIZE, GR_DEPTH, ZC_HP, world)
{
	setSpeed(speed, 0); 
}
std::string ZombieCab::description() const
{
	return "ZC";
}
void ZombieCab::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->overlaps(this, getWorld()->getGhostRacer()))
	{
		if (m_numCollisions > 0)
		{
		}
		else
		{
			getWorld()->getGhostRacer()->doDamage(20);
			// if to cab is to the left or has the same X
			if (getX() <= getWorld()->getGhostRacer()->getX())
			{
				setSpeed(getvSpeed(), -5);
				setDirection(120 + randInt(0, 20));
			}
			if (getX() > getWorld()->getGhostRacer()->getX())
			{
				setSpeed(getvSpeed(), 5);
				setDirection(60 - randInt(0, 20));
			}
			m_numCollisions++;
		}
	}
	// move
	moveRelativeToGhostRacerSpeed();
	if (getvSpeed() > getWorld()->getGhostRacer()->getvSpeed())
	{
		Actor* CAW = getWorld()->findCAWabove(this);
		if (CAW == nullptr)
		{}
		else
		{
			int d = CAW->getY() - getY();
			if (d < 0)
				d *= -1;
			if (d < 96)
			{
				setSpeed((getvSpeed() - .5), gethSpeed());
				return;
			}
		}
	}
	if (getvSpeed() <= getWorld()->getGhostRacer()->getvSpeed())
	{
		Actor* CAW = getWorld()->findCAWbelow(this);
		if (CAW == nullptr || CAW == getWorld()->getGhostRacer())
		{}
		else
		{
			int d = CAW->getY() - getY();
			if (d < 0)
				d *= -1;
			if (d < 96)
			{
				setSpeed(getvSpeed() + .5, gethSpeed());
				return;
			}
		}
	}
	m_planL--;
	if (m_planL > 0)
		return;
	else
	{
		m_planL = randInt(4, 32);
		setSpeed(getvSpeed() + randInt(-2, 2), gethSpeed());
	}
}
bool ZombieCab::beSprayedIfAppropriate()
{
	doDamage(1);
	if (!isAlive())
	{
		int ChanceOil = randInt(1, 5);
		if (ChanceOil == 1)
		{
			getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
		}
		getWorld()->increaseScore(200);
	}
	return true;
}
int ZombieCab::soundWhenHurt() const
{
	return SOUND_VEHICLE_HURT;
}
int ZombieCab::soundWhenDie() const
{
	return SOUND_VEHICLE_DIE;
}
//////////////////////////////////////
//////Spray Class Implementation
//////////////////////////////////////
Spray::Spray(StudentWorld* world, double x, double y, int dir)
	:Actor(IID_HOLY_WATER_PROJECTILE, x, y, dir, SPRAY_SIZE, SPRAY_DEPTH, world)
{}

void Spray::doSomething()
{
	if (!isAlive())
		return;
	// attempt to spray
	if (getWorld()->Sprayable(this) != nullptr)
	{
		getWorld()->Sprayable(this)->beSprayedIfAppropriate();
		setDead();
		return;
	}
	// if no object is sprayed
	moveForward(SPRITE_HEIGHT);
	if (getX() <= 0 || getY() <= 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		setDead();
	travelDist -= SPRITE_HEIGHT;
	if (travelDist <= 0)
		setDead();
}
std::string Spray::description() const
{
	return "Spray";
}

///////////////////////////////////////////
///////Activator Base Class Implementation
///////////////////////////////////////////
Activator::Activator(StudentWorld* world, int imageID, double x, double y, double size, int dir)
	:Actor(imageID, x, y, dir, size, ACT_DEPTH, world)
{
	setSpeed(-4, 0);
}
void Activator::doSomething()
{
	moveRelativeToGhostRacerSpeed();
	doActivity(getWorld()->getGhostRacer());
}
bool Activator::beSprayedIfAppropriate()
{
	if (isSprayable())
	{
		if (selfDestructs())
			setDead();
		return true;
	}
	else
		return false;
}
int Activator::getSound() const
{
	return SOUND_GOT_GOODIE;
}

///////////////////////////////////////////
///////Oil Slick Class Implementation
///////////////////////////////////////////
OilSlick::OilSlick(StudentWorld* world, double x, double y)
	: Activator(world, IID_OIL_SLICK, x, y, randInt(2,5), OS_DIR)
{}

std::string OilSlick::description() const
{
	return "Oil";
}
void OilSlick::doActivity(GhostRacer* gr)
{
	if (getWorld()->overlaps(this, gr))
	{
		getWorld()->playSound(getSound());
		gr->spin();
	}
}
int OilSlick::getScoreIncrease() const
{
	return 0;
}
int OilSlick::getSound() const
{
	return SOUND_OIL_SLICK;
}
bool OilSlick::selfDestructs() const
{
	return false;
}
///////////////////////////////////////////
///////Healing Goodie Class Implementation
///////////////////////////////////////////
HealingGoodie::HealingGoodie(StudentWorld* world, double x, double y)
	:Activator(world, IID_HEAL_GOODIE, x, y, HG_SIZE, HG_DIR)
{}

std::string HealingGoodie::description() const
{
	return "HG";
}
void HealingGoodie::doActivity(GhostRacer* gr)
{
	if (getWorld()->overlaps(this, gr))
	{
		gr->getHP(10);
		setDead();
		getWorld()->playSound(getSound());
		getWorld()->increaseScore(getScoreIncrease());
	}
}
int HealingGoodie::getScoreIncrease() const
{
	return 250;
}
bool HealingGoodie::selfDestructs() const
{
	return true;
}
bool HealingGoodie::isSprayable() const
{
	return true;
}

///////////////////////////////////////////
///////Holy Water Class Implementation
///////////////////////////////////////////
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* world, double x, double y)
	: Activator(world, IID_HOLY_WATER_GOODIE, x, y , HG_SIZE, HG_DIR)
{}

void HolyWaterGoodie::doActivity(GhostRacer* gr)
{
	if (getWorld()->overlaps(this, gr))
	{
		gr->addWater(10);
		setDead();
		getWorld()->playSound(getSound());
		getWorld()->increaseScore(getScoreIncrease());
	}
}
/////////////////////////////////////////
//////Soul Class Implementation
/////////////////////////////////////////
SoulGoodie::SoulGoodie(StudentWorld* world, double x, double y)
	: Activator(world, IID_SOUL_GOODIE, x, y, GR_SIZE, OS_DIR)
{}

void SoulGoodie::doActivity(GhostRacer* gr)
{
	if (getWorld()->overlaps(this, gr))
	{
		getWorld()->recordSoulSaved();
		setDead();
		getWorld()->playSound(getSound());
		getWorld()->increaseScore(getScoreIncrease());
	}
	setDirection(getDirection() - 10);
}