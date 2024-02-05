#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

class maze
{
public:
    class Cell
    {
    public:
        int x;
        int y;
        enum Wall
        {
            disabled,
            enabled
        };
        Wall Up : 1, Right : 1, Left : 1, Down : 1;

        Cell() {}
        Cell(int x, int y) : x(x), y(y)
        {
            Up = enabled;
            Right = enabled;
            Left = enabled;
            Down = enabled;
        }

        int Visited()
        {
            return (Up == disabled) | (Right == disabled) | (Down == disabled) | (Left == disabled);
        }
    };

    int rows;
    int cols;
    Cell **grid;

    maze(int rows, int cols) : rows(rows), cols(cols)
    {
        grid = (Cell **)malloc(sizeof(Cell *) * rows);
        for (int i = 0; i < rows; i++)
            grid[i] = (Cell *)malloc(sizeof(Cell) * cols);

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                grid[i][j] = Cell(j, i);
    }

    int in_boundries(int x, int y)
    {
        return (x >= 0) & (x < cols) & (y >= 0) & (y < rows);
    }

    void removeWalls(Cell c, Cell n)
    {
        int dx = n.x - c.x;
        int dy = n.y - c.y;
        // if new is on the TOP => (0,-1) code 0
        // if new is on the RIGHT => (1,0) coded 1
        // if new is on the BOTTOM => (0,1) code 2
        // if new is on the LEFT => (-1,0) code 3
        int code = 0;
        if (dx == 0 & dy == -1) // TOP
            code = 0;
        if (dx == 1 & dy == 0) // RIGHT
            code = 1;
        if (dx == 0 & dy == 1) // BOTTOM
            code = 2;
        if (dx == -1 & dy == 0) // LEFT
            code = 3;
        disable(c.x, c.y, code);
    }
    void disable(int x, int y, int code)
    {
        cut(x, y, code);
        int dx = 0;
        int dy = 0;
        if (code == 0)
            dy--;
        if (code == 1)
            dx++;
        if (code == 2)
            dy++;
        if (code == 3)
            dx--;
        dx += x;
        dy += y;
        if (in_boundries(dx, dy))
            cut(dx, dy, (code + 2) % 4);
    }

    void cut(int x, int y, int code)
    {
        // Up Right Down Left
        // 0   1    2     3
        if (code == 0)
            grid[y][x].Up = Cell::Wall::disabled;
        if (code == 1)
            grid[y][x].Right = Cell::Wall::disabled;
        if (code == 2)
            grid[y][x].Down = Cell::Wall::disabled;
        if (code == 3)
            grid[y][x].Left = Cell::Wall::disabled;
    }

    Cell getUnvisited(Cell c)
    {
        return getUnvisited(c.x, c.y);
    }

    Cell getUnvisited(int x, int y)
    {
        int positions[][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        Cell unvisited[4];
        int length = 0;

        for (int i = 0; i < 4; i++)
        {
            int dx = x + positions[i][0];
            int dy = y + positions[i][1];
            if (in_boundries(dx, dy) && !grid[dy][dx].Visited())
                unvisited[length++] = grid[dy][dx];
        }
        return unvisited[rand() % length];
    }

    int checkNeighbors(Cell c)
    {
        return checkNeighbors(c.x, c.y);
    }

    int checkNeighbors(int x, int y)
    {
        // this function returns 1 if a cell has unvisited neighbors
        // else, return 0
        int positions[][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

        for (int i = 0; i < 4; i++)
        {
            int dx = x + positions[i][0];
            int dy = y + positions[i][1];
            if (in_boundries(dx, dy) && !grid[dy][dx].Visited())
                return 1; // push grid[dy][dx] to neighbors
        }
        return 0;
    }

    void path(int x, int y) // starting position
    {
        Cell current = grid[y][x];
        Cell next;
        Stack<Cell> stack = Stack<Cell>();
    again:
        while (checkNeighbors(current))
        {
            next = getUnvisited(current);
            stack.push(current);
            removeWalls(current, next);
            current = next;
        }
        if (!stack.empty())
        {
            current = stack.pop();
            goto again;
        }
    }

    int done()
    {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (!grid[i][j].Visited())
                    return 0;
        return 1;
    }

    void print()
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
                printf("(%i %i) ", grid[i][j].x, grid[i][j].y);
            printf("\n");
        }
    }

    void visited()
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
                printf("%i ", grid[i][j].Visited());
            printf("\n");
        }
    }

    void display() { display(stdout); }

    void display(FILE *file)
    {
        for (int up = 0; up < cols; up++)
            fprintf(file, "# %c%s", grid[0][up].Up == Cell::Wall::enabled ? '#' : ' ',up!=cols-1?" ":" #\n");
        //fprintf(file, "# \n");
        //^for ptinting the upper part
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
                fprintf(file, "%c   ", grid[i][j].Left == Cell::Wall::enabled ? '#' : ' ');

            fprintf(file, "%c\n", grid[i][cols - 1].Right == Cell::Wall::enabled ? '#' : ' ');

            for (int k = 0; k < cols; k++)
                fprintf(file, "# %c%s", grid[i][k].Down == Cell::Wall::enabled ? '#' : ' ',k!=cols-1?" ":" #\n");
            //fprintf(file, "# \n");
        }
    }
};