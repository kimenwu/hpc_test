/*
 * config.h
 *
 *  Created on: 2018Äê10ÔÂ19ÈÕ
 *      Author: kimen
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <list>
#include <vector>

#include "expadition.h"

using namespace std;

//enum DIRC {u,l,lu,ld,d,r,rd,ru};

class config
{
private:
	int m_T; /*how many time the programe run*/
	list<expadition> m_expaditions;
	int m_expaditon_cnt;

public:
	config();
	~config();
	void load_config(const char *p);
	int get_run_period();
	void dump_all_expaditions();

	list<expadition> get_expaditions()
	{
		return m_expaditions;
	}

	int get_expaditon_cnt()
	{
		return m_expaditon_cnt;
	}
};

#endif /* CONFIG_H_ */

