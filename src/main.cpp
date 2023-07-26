#include <iostream>
#include <string>
#include <time.h>
#include "Cam.h"
#include "Contr.h"
#include "RangeF.h"

using namespace std;


int main()
{
    RangeF* range = new RangeF();
    Cam* cam = new Cam();
    cout << "Rangefinder and both cameras' instances created successfully" << endl;

    Contr* control = new Contr(cam, range);
    cout << "Control instance created successfully" << endl;
    control->operating();
}
