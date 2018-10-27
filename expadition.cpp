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
	m_status = EXP_STATUS_LIVE;
	m_left_step = 0;
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

string expadition::direction_tostring()
{
	return string(direction_i2s(m_direction));
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


void expadition::copy_to_attribute(expadition_attribute_t *p_attr)
{
	p_attr->direction	= m_direction;
	p_attr->id			= m_id;
	p_attr->left_step	= m_left_step;
	p_attr->status		= m_status;
	p_attr->x			= m_x;
	p_attr->y			= m_y;
}
