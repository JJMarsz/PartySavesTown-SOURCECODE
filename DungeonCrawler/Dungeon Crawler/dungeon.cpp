#include "dungeon.h"
#include "SDL.h"

Dungeon current_dungeon;

/* Disjoint Sets class definitions */
void DisjointSets::addelements(int num) {
	for (int i = 0; i<num; i++) {
		set.push_back(-1);
	}
}

int DisjointSets::find(int elem) {
	if (set[elem] < 0)
		return elem;
	else {
		set[elem] = find(set[elem]);
		return set[elem];
	}
}

bool DisjointSets::setunion(int a, int b) {
	int parentA, parentB;
	if (set[a] < 0)
		parentA = a;
	else
		parentA = find(a);
	if (set[b] < 0)
		parentB = b;
	else
		parentB = find(b);
	if (parentA == parentB)
		return false;
	int newSize = set[parentA] + set[parentB];
	if (set[parentA] < set[parentB]) {
		set[parentB] = parentA;
		set[parentA] = newSize;
	}
	else {
		set[parentA] = parentB;
		set[parentB] = newSize;
	}
	return true;
}

DisjointSets::DisjointSets() {

}

void DisjointSets::flush() {
	set.resize(0);
}
/* Dungeon class defenitions */
//creates a basic dungeon, once made, apply dungeon quest modifiers to it
Dungeon::Dungeon() {

}
Dungeon::Dungeon(Difficulty type) {
	srand(time(NULL));

	unsigned i;
	int x, y, prev_x, prev_y, down_count;
	int timeout;
	Tile emptyTile(NONE);
	Tile newTile(PATH);
	Direction dir;
	int dead_count;
	while (1) {
		switch (type) {
		case EASY:
			//8x8 to 9x9
			width = 8 + rand() % 2;
			height = 8 + rand() % 2;
			dead_count = 2;
			break;
		case MEDIUM:
			//10x10 to 11x11
			width = 10 + rand() % 2;
			height = 10 + rand() % 2;
			dead_count = 3;
			break;
		case HARD:
			//12x12 to 13x13
			width = 12 + rand() % 2;
			height = 12 + rand() % 2;
			dead_count = 4;
			break;
		default:
			//problemmmmm
			break;
		}
		start_x = rand() % width;
		start_y = height - 1;;
		//initiliaze the map
		dungMap.resize(width*height);
		mapSet.flush();
		mapSet.addelements(width*height);

		for (i = 0; (int)i < width*height; i++) {
			emptyTile.setPos(i % width, i / width);
			setTile(i, emptyTile);
		}
		x = start_x;
		y = start_y;
		setTile(x + y * width, newTile);
		path_length = 1;
		down_count = height / 2;
		timeout = 0;
		while (y != 0) {
			dir = (Direction)(rand() % 4);
			if (canTravel(x, y, dir)) {
				prev_x = x;
				prev_y = y;
				switch (dir) {
				case UP:
					y--;
					break;
				case LEFT:
					x--;
					break;
				case DOWN:
					if (down_count > 0) {
						y++;
						down_count--;
						break;
					}
					continue;
				case RIGHT:
					x++;
					break;
				default:
					while(1);
				}
				newTile.setPrev(&dungMap[prev_y*width + prev_x]);
				newTile.setPos(x, y);
				setTile(y*width + x, newTile);
				if(mapSet.setunion(prev_y*width + prev_x, y*width + x))
					path_length++;
			}
			//literally duct tape
			timeout++;
			if (timeout > 300)
				break;
		}
		if (timeout > 300)
			continue;
		end_x = x;
		end_y = y;
		//require a longish path
		if (path_length > 2*height && path_length < 3*height)
			break;
	}
	Tile* trav = dungMap[end_x + end_y * width].getPrev();
	//generate dead ends
	
	int path_count = 0;
	while (dead_count > 0 && trav != NULL) {
		timeout = 0;
		if (rand() % 100 + 1 < (dead_count) * 20 + path_count * 5) {
			//allowing a dead to be created, check which adjacent tile
			dir = (Direction)(rand() % 4);
			while (!canTravel(trav->getX(), trav->getY(), dir)) {
				dir = (Direction)(rand() % 4);
				timeout++;
				if (timeout > 300)
					break;
			}
			if (canTravel(trav->getX(), trav->getY(), dir)) {
				dead_count--;
				path_count = 0;
				switch (dir) {
				case UP:
					dungMap[trav->getX() + (trav->getY() - 1)*width].setType(DEADEND);
					break;
				case LEFT:
					dungMap[trav->getX() - 1 + (trav->getY())*width].setType(DEADEND);
					break;
				case DOWN:
					dungMap[trav->getX() + (trav->getY() + 1)*width].setType(DEADEND);
					break;
				case RIGHT:
					dungMap[trav->getX() - 1 + (trav->getY())*width].setType(DEADEND);
					break;
				default:
					continue;
				}
			}
		}
		path_count++;
	}
}

bool Dungeon::canTravel(int curr_x, int curr_y, Direction dir) {
	//boundary checks
	if (curr_x < 0 || curr_x >= width || curr_y < 0 || curr_y >= height)
		return false;
	//check if a cycle is made
	//all adjacent 4 tiles need to be cleard
	int x, y;
	switch (dir) {
	case UP:
		x = curr_x;
		y = curr_y - 1;
		break;
	case LEFT:
		x = curr_x - 1;
		y = curr_y;
		break;
	case DOWN:
		x = curr_x;
		y = curr_y + 1;
		break;
	case RIGHT:
		x = curr_x + 1;
		y = curr_y;
		break;
	}
	if (x < 0 || x >= width || y < 0 || y >= height)
		return false;
	std::unordered_map<int, bool> comp;
	if (y < height - 1)
		comp[mapSet.find(x + (y + 1)*width)] = true;
	if (y > 0) {
		if (comp.find(mapSet.find(x + (y - 1)*width)) != comp.end())
			return false;
		comp[mapSet.find(x + (y - 1)*width)] = true;
	}
	if (x > 0) {
		if (comp.find(mapSet.find(x - 1 + (y)*width)) != comp.end())
			return false;
		comp[mapSet.find(x - 1 + (y)*width)] = true;
	}
	if (x < width - 1) {
		if (comp.find(mapSet.find(x + 1 + (y)*width)) != comp.end())
			return false;
		comp[mapSet.find(x + 1 + (y)*width)] = true;
	}
	//x and y are within the map and do not create a cycle
	return true;

}

void Dungeon::setTile(int RMO_index, Tile newTile) {
	dungMap[RMO_index] = newTile;
}

Tile Dungeon::getTile(int RMO_index) {
	return dungMap[RMO_index];
}

int Dungeon::getWidth() { return width; }
int Dungeon::getHeight() { return height; }

/* Tile class defenitions */
//Tile defines each individual (x,y) location in the map
//contains info for whatever the tile represents
Tile::Tile(Encounter type_) {
	type = type_;
	prev = NULL;
	x = -1;
	y = -1;
}

Tile::Tile() {
	type = NONE;
	prev = NULL;
	x = -1;
	y = -1;
}

Tile* Tile::getPrev() { return prev; }
void Tile::setPrev(Tile* prev_) {prev = prev_;}

int Tile::getX() { return x; }
int Tile::getY() { return y; }
void Tile::setPos(int x_, int y_) {x = x_;y = y_;}

Encounter Tile::getType() {return type;}
void Tile::setType(Encounter type_) { type = type_; }
