#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
///////////////////////////////////
//////StudentWorld Implementation
///////////////////////////////////
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getGhostRacer();
    void recordSoulSaved();
    bool overlaps(const Actor* a1, const Actor* a2) const;
    Actor* findCAWabove(ZombieCab* ZC);
    Actor* findCAWbelow(ZombieCab* ZC);
    Actor* Sprayable(Spray* water);
    void addActor(Actor*);
private:
    std::list<Actor*> li;
    GhostRacer* m_racer;
    int m_soulsLeftToSave = 0;
    int m_bonus = 5000;
};

#endif // STUDENTWORLD_H_
