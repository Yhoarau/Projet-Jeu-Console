#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "wesh.h"

using namespace std;
using namespace Matrix;

// Fonction timer

void newline()
{
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}
void FinalCntDwn(unsigned &sec,unsigned &msec)
{
    newline();
    unsigned save (sec);
    for (;sec>0;)
    {
            newline();
                    if(msec == 10)
                    {
                            --sec;
                            msec = 0;
                    }
            cout << "il vous reste " << sec << " secondes."<< endl;
            usleep(100000);
            ++msec;
    }
    sec=save;
}
void Timer()
{
    unsigned msec (0);
    unsigned sec (15);
    while (true)
    {
        FinalCntDwn(sec,msec);
        if (sec>3)
            sec -=2;
    }
} // Timer

int main()
{
    Timer ();
    // Menu();
    return 0;
}
