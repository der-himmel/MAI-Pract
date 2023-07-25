#pragma once
#include <string>
#include "RangeF.h"
#include "Cam.h"

class Contr
{
private:
	//int status;
	//bool flag = 1;
	std::string distance;

	RangeF* range;
	Cam* cam;

public:
	Contr(Cam* cam, RangeF* range);
	void operating();
	void recording();
};
