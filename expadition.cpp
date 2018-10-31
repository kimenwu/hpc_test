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
}

expadition::~expadition()
{
}

expadition::expadition(expadition_attribute_t &attr)
{
	expadition();
	m_direction = attr.direction;
	m_id		= attr.id;
	m_speed 	= attr.speed;
	m_status	= attr.status;
	m_x			= attr.x;
	m_y			= attr.y;
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
	m_status = EXP_STATUS_LIVE;
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
//	parallel_debug("id:%d\tx:%d,\ty:%d,\tdirection:%s,\tspeed:%d",m_id,m_x,m_y,
//			direction_i2s(m_direction).c_str(),m_speed);
}


void expadition::copy_to_attribute(expadition_attribute_t *p_attr)
{
	p_attr->direction	= m_direction;
	p_attr->id			= m_id;
	p_attr->speed		= m_speed;
	p_attr->status		= m_status;
	p_attr->x			= m_x;
	p_attr->y			= m_y;
}

void expadition::redirection()
{
int number_of_times = 0;

	//We need do check twice
	//first redirection maybe encounter another bondary
retry:
	number_of_times++;
	switch (m_direction) {
	case DIRC::u:
		if (m_y == MAX_Y_RANGE) {
			m_direction = DIRC::rd;
		}
		break;
	case DIRC::d:
		if (m_y == 1) {
			m_direction = DIRC::lu;
		}
		break;
	case DIRC::l:
		if (m_x == 1) {
			m_direction = DIRC::ru;
		}
		break;
	case DIRC::r:
		if (m_x == MAX_X_RANGE) {
			m_direction = DIRC::ld;
		}
		break;
	case DIRC::lu:
		if (m_x == 1 || m_y == MAX_Y_RANGE) {
			m_direction = DIRC::r;
		}
		break;
	case DIRC::ld:
		if (m_x == 1 || m_y == 1) {
			m_direction = DIRC::u;
		}
		break;
	case DIRC::ru:
		if (m_x == MAX_X_RANGE || m_y == MAX_Y_RANGE) {
			m_direction = DIRC::d;
		}
		break;
	case DIRC::rd:
		if (m_x == MAX_Y_RANGE || m_y == 1) {
			m_direction = DIRC::l;
		}
		break;
	default:
		break;
	}

	if(!number_of_times)
		goto retry;
}

void expadition::do_walk_once()
{
	//need redirection?
	redirection();

	switch (m_direction) {
	case DIRC::u:
		m_y++;
		break;
	case DIRC::d:
		m_y--;
		break;
	case DIRC::l:
		m_x--;
		break;
	case DIRC::r:
		m_x++;
		break;
	case DIRC::lu:
		m_x--;
		m_y++;
		break;
	case DIRC::ld:
		m_x--;
		m_y--;
		break;
	case DIRC::ru:
		m_x++;
		m_y++;
		break;
	case DIRC::rd:
		m_x++;
		m_y--;
		break;
	default:
		break;
	}
}

void expadition::do_walk()
{
	int steps = m_speed;

	while(steps){
		do_walk_once();
		steps--;
	}
}
