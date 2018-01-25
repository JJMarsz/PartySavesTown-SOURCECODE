#ifndef ROOM_H
#define ROOM_H

#include "lib.h"
#include "party.h"



struct RoomTile {
	bool up;
	bool left;
	bool down;
	bool right;
	UnitType type;
	//used during move to determine what is in range
	//also used for abilities to determine what is in range
	bool special;
};

class Room
{
public:
	Room();
	void rollInit();
	void placeUnits();
	void passControl();
	int getWidth();
	int getHeight();
	RoomTile* getTile(int x, int y);
	Unit* getCurrUnit();
	std::vector<Unit*>* getInititiveOrder();
private:
	//maintain for initiative purposes
	std::vector<Unit*> unitList;

	std::vector<Unit*>::iterator currUnit;
	std::vector<RoomTile> roomMap;
	int width, height;
};


extern Room* room;

#endif

