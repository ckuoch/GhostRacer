#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}
int StudentWorld::init()
{
    m_soulsLeftToSave = (getLevel() * 2) + 5;
    m_bonus = 5000;
    m_racer = new GhostRacer(this);
    li.push_back(m_racer);
    for (int n = 0; n != VIEW_HEIGHT / SPRITE_HEIGHT; n++)
    {
        li.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, n * SPRITE_HEIGHT, this, YELLOW_TYPE_BL));
        li.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, n * SPRITE_HEIGHT, this, YELLOW_TYPE_BL));
    }

    for (int m = 0; m != VIEW_HEIGHT/(4*SPRITE_HEIGHT); m++)
    {
        li.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE+(ROAD_WIDTH/3), m * (4*SPRITE_HEIGHT), this, WHITE_TYPE_BL));
        li.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE-(ROAD_WIDTH/3), m * (4*SPRITE_HEIGHT), this, WHITE_TYPE_BL));
    }
    ostringstream status;
    status << "Score: " << getScore() << "  Lvl: " << getLevel() <<  "  Souls2Save: " << m_soulsLeftToSave << "  Lives: " << getLives() << "  Health: " << getGhostRacer()->health() << "  Sprays: " << getGhostRacer()->getWater() << "  Bonus: " << m_bonus;
    setGameStatText(status.str());
    return GWSTATUS_CONTINUE_GAME;  
}

int StudentWorld::move()
{
    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
    {
        Actor* d = *p;
        if (d->isAlive())
        {
            d->doSomething();
            if (!getGhostRacer()->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_soulsLeftToSave == 0)
            {
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        // delete any dead actors
        if (!getGhostRacer()->isAlive())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (!d->isAlive())
        {
            delete d;
            p = li.erase(p);
            p--;
        }
    }

    //add any new actors
    //Add Oil Slicks
    int ChanceOilSLick = max(150 - getLevel() * 10, 40);
    if (randInt(0, ChanceOilSLick) == 0)
        li.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    //Add pedestrians
    int ChanceHumanPed = max(200 - (getLevel() * 10), 30);
    if (randInt(0, ChanceHumanPed) == 0)
    li.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    int ChanceZombiePed = max(200 - (getLevel() * 10), 30);
    if (randInt(0, ChanceZombiePed) == 0)
        li.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    //add New Borderline
    double old_y = 0;
    list<Actor*>::iterator p = li.end();
    p--;
    while (old_y == 0 || p != li.begin())
    {
        Actor* d = *p;
        if (d->description() == WHITE_TYPE_BL)
        {
            old_y = d->getY();
            break;
        }
        p--;
    }
    if ((NEW_BORDER_Y - old_y) >= SPRITE_HEIGHT)
    {
        li.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, NEW_BORDER_Y, this, YELLOW_TYPE_BL));
        li.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, NEW_BORDER_Y, this, YELLOW_TYPE_BL));
    }
    if ((NEW_BORDER_Y - old_y) >= (4 * SPRITE_HEIGHT))
    {
        li.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + (ROAD_WIDTH / 3), NEW_BORDER_Y, this, WHITE_TYPE_BL));
        li.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - (ROAD_WIDTH / 3), NEW_BORDER_Y, this, WHITE_TYPE_BL));
    }
    // Add Zombie cabs
    int lane = randInt(1, 3);
    double ZC_startY = 0;
    double ZC_startX = 0;
    bool lane1 = false;
    bool lane2 = false;
    bool lane3 = false;
    int ZC_startSpeed = 0;
    int ChanceVehicle = max(100 - (getLevel() * 10), 20);
    if (randInt(0, ChanceVehicle) == 0)
    {
        for (int i = 0; i < 3; i++)
        {
            switch (lane)
            {
            case 1 /*Left*/:
            {
                lane1 = true;
                Actor* closestToBottom = nullptr;
                Actor* closestToTop = nullptr;
                // Find the out if there is an CAW-actor that is in the lane
                for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                {
                    Actor* d = *p;
                    if (d->isCollisionAvoidanceWorthy())
                    {
                        if (d->getX() >= LEFT_EDGE && d->getX() < LEFT_EDGE + (ROAD_WIDTH / 3))
                        {
                            closestToBottom = d;
                            closestToTop = d;
                            break;
                        }
                    }
                }
                // Compare all other CAW actors against it to find the one closest to the bottom
                if (closestToBottom != nullptr)
                {
                    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                    {
                        Actor* d = *p;
                        if (d->isCollisionAvoidanceWorthy())
                        {
                            if (d->getX() >= LEFT_EDGE && d->getX() < LEFT_EDGE + (ROAD_WIDTH / 3))
                            {
                                if (d->getY() < closestToBottom->getY())
                                    closestToBottom = d;
                            }
                        }
                    }
                }
                // If there is no such candidate or the actor has a Y value greater than view height /3 then...
                if (closestToBottom == nullptr || closestToBottom->getY() > VIEW_HEIGHT / 3)
                {
                    ZC_startX = ROAD_CENTER - (ROAD_WIDTH / 3);
                    ZC_startY = SPRITE_HEIGHT / 2;
                    ZC_startSpeed = getGhostRacer()->getvSpeed() + randInt(2, 4);
                    break;
                }

                // Compare all other CAW actors against it to find the one closest to the Top
                if (closestToTop != nullptr)
                {
                    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                    {
                        Actor* d = *p;
                        if (d->isCollisionAvoidanceWorthy())
                        {
                            if (d->getX() >= LEFT_EDGE && d->getX() < LEFT_EDGE + (ROAD_WIDTH / 3))
                            {
                                if (d->getY() > closestToTop->getY())
                                    closestToTop = d;
                            }
                        }
                    }
                }
                // If there is no such candidate or the actor has a Y value greater than view height /3 then...
                if (closestToTop == nullptr || closestToTop->getY() < VIEW_HEIGHT * 2 / 3)
                {
                    ZC_startX = ROAD_CENTER - (ROAD_WIDTH / 3);
                    ZC_startY = VIEW_HEIGHT - (SPRITE_HEIGHT / 2);
                    ZC_startSpeed = getGhostRacer()->getvSpeed() - randInt(2, 4);
                    break;
                }
                break;
            }
            case 2 /*Middle*/:
            {
                lane2 = true;
                Actor* closestToBottom = nullptr;
                Actor* closestToTop = nullptr;
                // Find the out if there is an CAW-actor that is in the lane
                for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                {
                    Actor* d = *p;
                    if (d->isCollisionAvoidanceWorthy())
                    {
                        if (d->getX() >= LEFT_EDGE + (ROAD_WIDTH / 3) && d->getX() < RIGHT_EDGE - (ROAD_WIDTH / 3))
                        {
                            closestToBottom = d;
                            closestToTop = d;
                            break;
                        }
                    }
                }
                // Compare all other CAW actors against it to find the one closest to the bottom
                if (closestToBottom != nullptr)
                {
                    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                    {
                        Actor* d = *p;
                        if (d->isCollisionAvoidanceWorthy())
                        {
                            if (d->getX() >= LEFT_EDGE + (ROAD_WIDTH / 3) && d->getX() < RIGHT_EDGE - (ROAD_WIDTH / 3))
                            {
                                if (d->getY() < closestToBottom->getY())
                                    closestToBottom = d;
                            }
                        }
                    }
                }
                // If there is no such candidate or the actor has a Y value greater than view height /3 then...
                if (closestToBottom == nullptr || closestToBottom->getY() > VIEW_HEIGHT / 3)
                {
                    ZC_startX = ROAD_CENTER;
                    ZC_startY = SPRITE_HEIGHT / 2;
                    ZC_startSpeed = getGhostRacer()->getvSpeed() + randInt(2, 4);
                    break;
                }

                // Compare all other CAW actors against it to find the one closest to the Top
                if (closestToTop != nullptr)
                {
                    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                    {
                        Actor* d = *p;
                        if (d->isCollisionAvoidanceWorthy())
                        {
                            if (d->getX() >= LEFT_EDGE + (ROAD_WIDTH / 3) && d->getX() < RIGHT_EDGE - (ROAD_WIDTH / 3))
                            {
                                if (d->getY() > closestToTop->getY())
                                    closestToTop = d;
                            }
                        }
                    }
                }
                // If there is no such candidate or the actor has a Y value greater than view height /3 then...
                if (closestToTop == nullptr || closestToTop->getY() < VIEW_HEIGHT * 2 / 3)
                {
                    ZC_startX = ROAD_CENTER;
                    ZC_startY = VIEW_HEIGHT - (SPRITE_HEIGHT / 2);
                    ZC_startSpeed = getGhostRacer()->getvSpeed() - randInt(2, 4);
                    break;
                }
                break;
            }
            case 3 /*Right*/:
            {
                lane3 = true;
                Actor* closestToBottom = nullptr;
                Actor* closestToTop = nullptr;
                // Find the out if there is an CAW-actor that is in the lane
                for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                {
                    Actor* d = *p;
                    if (d->isCollisionAvoidanceWorthy())
                    {
                        if (d->getX() >= RIGHT_EDGE - (ROAD_WIDTH / 3) && d->getX() < RIGHT_EDGE)
                        {
                            closestToBottom = d;
                            closestToTop = d;
                            break;
                        }
                    }
                }
                // Compare all other CAW actors against it to find the one closest to the bottom
                if (closestToBottom != nullptr)
                {
                    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                    {
                        Actor* d = *p;
                        if (d->isCollisionAvoidanceWorthy())
                        {
                            if (d->getX() >= RIGHT_EDGE - (ROAD_WIDTH / 3) && d->getX() < RIGHT_EDGE)
                            {
                                if (d->getY() < closestToBottom->getY())
                                    closestToBottom = d;
                            }
                        }
                    }
                }
                // If there is no such candidate or the actor has a Y value greater than view height /3 then...
                if (closestToBottom == nullptr || closestToBottom->getY() > VIEW_HEIGHT / 3)
                {
                    ZC_startX = ROAD_CENTER + (ROAD_WIDTH / 3);
                    ZC_startY = SPRITE_HEIGHT / 2;
                    ZC_startSpeed = getGhostRacer()->getvSpeed() + randInt(2, 4);
                    break;
                }

                // Compare all other CAW actors against it to find the one closest to the Top
                if (closestToTop != nullptr)
                {
                    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
                    {
                        Actor* d = *p;
                        if (d->isCollisionAvoidanceWorthy())
                        {
                            if (d->getX() >= RIGHT_EDGE - (ROAD_WIDTH / 3) && d->getX() < RIGHT_EDGE)
                            {
                                if (d->getY() > closestToTop->getY())
                                    closestToTop = d;
                            }
                        }
                    }
                }
                // If there is no such candidate or the actor has a Y value greater than view height /3 then...
                if (closestToTop == nullptr || closestToTop->getY() < VIEW_HEIGHT * 2 / 3)
                {
                    ZC_startX = ROAD_CENTER + (ROAD_WIDTH / 3);
                    ZC_startY = VIEW_HEIGHT - (SPRITE_HEIGHT / 2);
                    ZC_startSpeed = getGhostRacer()->getvSpeed() - randInt(2, 4);
                    break;
                }
                break;
            }
            default:
                break;
            }
            // if the Cabs components have been set, break out of the loop
            if (ZC_startX != 0)
                break;
            if (!lane1)
                lane = 1;
            else if (!lane2)
                lane = 2;
            else if (!lane3)
                lane = 3;
            else
                break;
        }
        if (ZC_startX != 0)
        {
            li.push_back(new ZombieCab(this, ZC_startX, ZC_startY, ZC_startSpeed));
        }
    }
    //Add Holy Water Refills
    int ChanceHolyWater = 100 + (10 * getLevel());
    // Add Lost Souls
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul) == 0)
    {
        li.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    if (randInt(0, ChanceHolyWater) == 0)
    {
        li.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    if (m_bonus > 0)
    {
        m_bonus--;
    }
    ostringstream status;
    status << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << m_soulsLeftToSave << "  Lives: " << getLives() << "  Health: " << getGhostRacer()->health() << "  Sprays: " << getGhostRacer()->getWater() << "  Bonus: " << m_bonus;
    setGameStatText(status.str());
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (list<Actor*>::iterator p = li.begin(); p != li.end();)
    {
        delete *p;
        p = li.erase(p);
    }
}

GhostRacer* StudentWorld::getGhostRacer()
{
    return m_racer;
}

void StudentWorld::recordSoulSaved()
{
    m_soulsLeftToSave--;
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const
{
    double delta_x = a1->getX() - a2->getX();
    if (delta_x < 0)
        delta_x = delta_x * -1;
    double delta_y = a1->getY() - a2->getY();
    if (delta_y < 0)
        delta_y = delta_y * -1;
    double r_sum = a1->getRadius() + a2->getRadius();
    if (delta_x < r_sum * .25 && delta_y < r_sum * .6)
        return true;
    else 
        return false;
}

Actor* StudentWorld::findCAWabove(ZombieCab* ZC)
{
    Actor* CAW = nullptr;
    for (list<Actor*>::iterator p = li.begin(); p != li.end();p++)
    {
        Actor* d = *p;
        if (d->isCollisionAvoidanceWorthy())
        {
            if ((d->getY() > ZC->getY()) && (d->getLane() == ZC->getLane()))
            {
                CAW = d;
                break;
            }
        }
    }
    if (CAW == nullptr)
        return nullptr;
    for (list<Actor*>::iterator p = li.begin(); p != li.end();p++)
    {
        Actor* d = *p;
        if (d->isCollisionAvoidanceWorthy())
        {
            if ((d->getY() > ZC->getY()) && (d->getY() <= CAW->getY()) && (d->getLane() == ZC->getLane()))
                CAW = d;
        }
    }
    return CAW;
}
Actor* StudentWorld::findCAWbelow(ZombieCab* ZC)
{
    Actor* CAW = nullptr;
    
    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
    {
        Actor* d = *p;
        if (d->isCollisionAvoidanceWorthy())
        {
            if (d->getY() < ZC->getY() && (d->getLane() == ZC->getLane()))
            {
                CAW = d;
                break;
            }
        }
    }
    if (CAW == nullptr)
        return nullptr;
    for (list<Actor*>::iterator p = li.begin(); p != li.end();p++)
    {
        Actor* d = *p;
        if (d->isCollisionAvoidanceWorthy())
        {
            if ((d->getY() < ZC->getY()) && (d->getY() >= CAW->getY()) && (d->getLane() == ZC->getLane()))
                CAW = d;
        }
    }
    return CAW;
}

Actor* StudentWorld::Sprayable(Spray* water)
{
    Actor* Sprayable = nullptr;
    for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
    {
        Actor* d = *p;
        if (d->isSprayable() && overlaps(d, water))
        {
            Sprayable = d;
            return Sprayable;
        }
    }
    return Sprayable;
}
void StudentWorld::addActor(Actor* a)
{
    li.push_back(a);
}