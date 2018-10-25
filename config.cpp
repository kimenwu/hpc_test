#include <fstream>
#include <iostream>
#include "config.h"
#include "parallel.h"

config::config()
{
	m_expaditon_cnt = 0;
}

config::~config()
{

}

void config::dump_all_expaditions()
{
	parallel_debug("------------- all expadition initialize like below:-----");
	for(auto exp = m_expaditions.begin();exp != m_expaditions.end(); ++exp){
		(*exp).dump_property();
	}
}

/**
 * get_run_period - Get the total time the program must run.
 */
int config::get_run_period()
{
	return m_T;
}

void config::load_config(const char *p)
{
	int x,y,speed;
	string str_direction;
	int id = 0;

	ifstream config_stream(p);
	if(!config_stream.is_open())
	{
		/**
		 * when we find that there is not a configure file exist,just
		 * exit and do nothing.
		 */
		cout << "[config]could not open configure file" << p <<endl;
		exit(-1);
	}

	/*How long do we need to run it stor in the configurefile*/
	config_stream >> m_T;

	/*Get all the expadition from the configure file*/
	/*TODO: We must check the configure before add it the list of expadition*/
	while(config_stream >> x ){
		config_stream >> y >> str_direction >> speed;
		m_expaditions.emplace_back(x,y,str_direction,speed,++id);
		m_expaditon_cnt++;
	}

	dump_all_expaditions();
	config_stream.close();
}
