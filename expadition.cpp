/*
 * expadition.cpp
 *
 */
#include <iostream>
#include <string>

#include "expadition.h"
#include "parallel.h"

expadition::expadition()
{

}

expadition::~expadition()
{
}


/**
 * Initiatlize a expaditon with the propetes that stor in configure file
 */
expadition::expadition(int x,int y,const string str_direction,int speed,int id)
{
	expadition();

	m_x = x;
	m_y = y;
	m_direction = direction_s2i(str_direction);
	m_speed = speed;
	m_id = id;
}

/**
 * Convert the direction representation from string to integer
 * NOTE:This function is case sensitive.
 */
int expadition::direction_s2i(const string str_direction)
{
	int idx = 0;
	int ret = -1;

	for(;idx < (int)expadition::STR_DIR.size();idx++)
	{
		if(0 == str_direction.compare(STR_DIR[idx])){
			ret = idx;
			break;
		}
	}

	return ret;
}

/**
 * convert the direction repesenting method from integer to string
 */
string expadition::direction_i2s(int direction)
{
	if(direction < 0 || direction >= (int)expadition::STR_DIR.size()){
		parallel_error("direction is error {direction:%d}",direction);
		return "";
	}

	return expadition::STR_DIR[direction];
}

void expadition::dump_property()
{
	parallel_debug("x:%d,\ty:%d,\tdirection:%s,\tspeed:%d",m_x,m_y,
			direction_i2s(m_direction).c_str(),m_speed);
}
