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

    cout << "Rangefinder and both cameras' instances successfully created" << endl;
    //sleep(1);

    Contr* control = new Contr(cam, range);

    control->operating();
}
