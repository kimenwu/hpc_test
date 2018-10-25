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


class expadition
{
public:
	int m_x;
	int m_y;
	int m_speed;
	int m_direction;
	int m_id;


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
