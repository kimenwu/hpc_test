/*
 * expadition.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: zhailu
 */

#ifndef EXPADITION_H_
#define EXPADITION_H_

#include <string>
#include <vector>

using namespace std;

#define EXP_STATUS_LIVE (1)

class expadition
{
public:
	int m_x;
	int m_y;
	int m_speed;
	int m_direction;
	int m_id;

	/**
	 * the status of a expadition.this is use to stor runtime staus like live or
	 * dead because of conflict with the other expaditon.
	 */
	int m_status;

	/*
	 * how many steps must be walk left in a second.
	 * the origin value equal to m_speed
	 */
	int m_left_step;


private:

	/**
	 * This vector used to convert the directtion representation.In the configure
	 * file,this variable is represent as a string but in the run time it is a
	 * integer.
	 */
	const vector<string> STR_DIR = {"U","L","LU","LD","D","D","RD","RU"};

	int direction_s2i(const string str_direction);
	string direction_i2s(int direction);

public:
	expadition();
	expadition(int x, int y,const string str_direction,int speed,int id);
	~expadition();

	string direction_tostring();

	int get_id()
	{
		return m_id;
	}

	/**
	 * Print all property of a expaditon
	 */
	void dump_property();

};

#endif /* EXPADITION_H_ */
