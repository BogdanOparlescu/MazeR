#include<stdio.h>
#include "maze.h"
#include<time.h>
int main()
{
    srand(time(NULL));
    maze m=maze(13,13);
    m.path(0,0);
    m.display();
    //m.display(fopen("13x13.txt","wb"));
}