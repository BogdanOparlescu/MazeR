#include "pathfinder.h"

void prev(pathfinder ph)
{
    int rows = ph.map.height;
    int cols = ph.map.width;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)

            printf("(%i %i) ", ph.nodes[i][j].previous->pos.x, ph.nodes[i][j].previous->pos.y);
        printf("\n");
    }
}

int main()
{
    FILE *file = fopen("mazes/4x4.txt", "rb");
    pathfinder ph = pathfinder(file);
    // std::cout<<ph.map;
    ph.map.DrwaAll(0);
    // ph.map.Replace(0, '0');
    // ph.map.Replace(1, '1');
    for (int i = 0; i < ph.map.height - 1; i++)
        ph.map[i][1] = 1;
    for (int i = 1; i < ph.map.height; i++)
        ph.map[i][3] = 1;
    ph.Astar((position){0,0}, (position){8,8});
    ph.map.Replace(0, '0');
    ph.map.Replace(1, '1');
    std::cout << ph.map;
    ph.construct_path();
    ph.write_path();
    ph.draw_path();
}