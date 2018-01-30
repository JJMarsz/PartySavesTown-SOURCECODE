#ifndef MOB_H
#define MOB_H

#include "lib.h"
#include "LButton.h"
#include "party.h"

class Mob: public Unit {
public:
	Mob();
	Mob(int AC_,int HP_,int dmg_, int dice_, int atk_mod_, int dmg_mod_, int range_, SDL_Rect icon, std::string name_, int s, int d, int c, int i, int w, int ch, int move_, void(*handler_)(), UnitType type_);

	/* using globals, update threat and select target player to attack */
	void updateThreat();

	/* attack the target, returns true on kill */
	void attack(int index);
	void roll_attack();
	void moveMob(int RMO_target);
	SDL_Rect getIcon50();
	void callHandler() { handler(); }
	void addAttackThreat(int t);
	void clearAttackThreat(int i){threat[i] = 0;}
private:
	int target_index;
	std::vector<int> threat;
	SDL_Rect icon_50;
	int atk_mod;
	int dmg_mod;
	int dice;
	int dmg;
	int range;
	void(*handler)();
};

extern std::unordered_map<std::string, std::vector<Mob>> mobEncMap;

void loadMobs();

void mobHandler();

#endif

