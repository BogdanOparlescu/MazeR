#include <math.h>
#include "List.h"

class image
{
public:
    int width, height;
    char **data;

    static int getLength(FILE *file)
    {
        fseek(file, 0, SEEK_END);
        int lenght = ftell(file);
        fseek(file, 0, SEEK_SET);
        return lenght;
    }

    static int getLines(FILE *file)
    {
        int l = 0;
        char c;
        while ((c = getc(file)) != EOF)
            l += (c == '\n');
        fseek(file, 0, SEEK_SET);
        return l;
    }

    image() {}

    image(int width, int height) : width(width), height(height)
    {
        data = (char **)malloc(sizeof(char *) * height);
        for (int i = 0; i < height; i++)
            data[i] = (char *)malloc(sizeof(char) * width);
    }

    void initialize() //-_-
    {
        data = (char **)malloc(sizeof(char *) * height);
        for (int i = 0; i < height; i++)
            data[i] = (char *)malloc(sizeof(char) * width);
    }

    void DrwaAll(char c)
    {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                data[i][j] = c;
    }

    void Replace(char a,char b)
    {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                if(data[i][j]==a)
                    data[i][j] = b;
    }

    image(FILE *source) : image(getLength(source) / getLines(source), getLines(source))
    {
        int l = getLength(source);
        int li = getLines(source);
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                data[i][j] = getc(source);
        fseek(source, 0, SEEK_SET);
    }

    char *operator[](int index)
    {
        return data[index];
    }

    friend std::ostream &operator<<(std::ostream &os, image img)
    {
        for (int i = 0; i < img.height; i++)
        {
            for (int j = 0; j < img.width; j++)
                os << img.data[i][j]<<' ';
            os<<'\n';
        }
        return os;
    }

    ~image()
    {
        for (int i = 0; i < height; i++)
            free(data[i]);
        free(data);
    }
};

class classifier : public image
{
public:
    classifier() {}
    classifier(FILE *maze_f)
    {
        height = getLines(maze_f);
        width = getLength(maze_f) / height;
        width /= 2;
        // image(width, height); down below -_-
        initialize();
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
            {
                data[i][j] = getc(maze_f) == '#';
                fseek(maze_f, 1, SEEK_CUR);
            }
    }

    void display()
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
                printf("%i ", data[i][j]);
            printf("\n");
        }
    }
};

class position
{
public:
    int x, y;
    position() : x(0), y(0) {}
    position(int x, int y) : x(x), y(y) {}

    friend int operator==(position p1, position p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }

    friend int operator!=(position p1, position p2)
    {
        return !(p1==p2);
    }
};

class HNode
{
public:
    position pos;
    float f = 0, g = 0, h = 0;
    HNode *previous = NULL;

    HNode() {}
    HNode(int x, int y) : HNode(position(x, y)) {}
    HNode(position pos) : pos(pos) {}

    void Cost(float g, float h)
    {
        this->g = g;
        this->h = h;
        F();
    }

    void F()
    {
        f = g + h;
    }

    void debug()
    {
        printf("G %.1f + H %.1f = F %.1f\n", g, h, f);
    }
};

class pathfinder
{
public:
    classifier map;
    HNode **nodes;
    pathfinder(FILE *file)
    {
        map = classifier(file);
        int rows = map.height;
        int cols = map.width;

        nodes = (HNode **)malloc(sizeof(HNode *) * rows);
        for (int i = 0; i < rows; i++)
            nodes[i] = (HNode *)malloc(sizeof(HNode) * cols);

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                nodes[i][j] = HNode(j, i);
    }

    void debug()
    {
        int rows = map.height;
        int cols = map.width;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)

                printf("(%i %i) ", nodes[i][j].pos.x, nodes[i][j].pos.y), nodes[i][j].debug();
            printf("\n");
        }
    }

    int is_wall(position pos)
    {
        return map[pos.y][pos.x];
    }

    // gets the neighbor references
    List<HNode *> getNeighbors(HNode current)
    {
        auto inBounds = [=](int x, int y) -> int
        { return (x >= 0) & (x < map.width) & (y >= 0) & (y < map.height); };
        position deltas[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}}; //+diag :*
        List<HNode *> neighbors;
        int x = current.pos.x, y = current.pos.y;
        for (int i = 0; i < 4; i++)
        {
            int dx = x + deltas[i].x;
            int dy = y + deltas[i].y;
            if (inBounds(dx, dy))
                neighbors.append(&nodes[dy][dx]);
        }
        return neighbors;
    }

    float heuristics_A(position a, position b)
    {
        int dx = (b.x - a.x);
        int dy = (b.y - a.y);
        return sqrt(dx * dx + dy * dy);
        // return abs(a.y-b.y)+abs(a.x-b.x);
    }

    List<position> path = List<position>();
    List<HNode *> Open_A = List<HNode *>();
    List<HNode *> Closed_A = List<HNode *>();
    void Astar(position start, position end)
    {
        auto ref = [=](position pos) -> HNode *
        {
            return &nodes[pos.y][pos.x];
        };
        HNode *start_node = ref(start);
        HNode *end_node = ref(end);
        Open_A.append(start_node);

        while (!Open_A.empty())
        {
            // printf("Step!\n");
            HNode *winner = start_node;
            for (List<HNode *>::Node<HNode *> *c = Open_A.head; c; c = c->next)
            {
                if (c->value->f < winner->f)
                    winner = c->value;

                if (c->value->g > winner->g)
                    winner = c->value;
            }

            HNode *current = winner;
            if (current->pos == end)
            {
                Closed_A.append(current);
                printf("Done!\n");
                return;
            }

            Open_A.remove(current);
            Closed_A.append(current);

            List<HNode *> neighbors = getNeighbors(*current);
            for (List<HNode *>::Node<HNode *> *n = neighbors.head; n; n = n->next)
            {
                if (!Closed_A.contains(n->value) && !is_wall(n->value->pos))
                {
                    float tempG = current->g + heuristics_A(n->value->pos, current->pos);

                    if (!Open_A.contains(n->value))
                        Open_A.append(n->value);
                    else if (tempG >= n->value->g)
                        continue;

                    n->value->g = tempG;
                    n->value->h = heuristics_A(n->value->pos, end);
                    n->value->F();
                    n->value->previous = current;
                }
            }
        }

        printf("No sollution!:(\n");
    }

    void construct_path()
    {
        printf("Constructing path...\n");
        for (List<HNode *>::Node<HNode *> *h = Closed_A.head; h; h = h->next)
            path.append(h->value->pos);
    }

    void write_path()
    {
        printf("The path is: ");
        for (List<position>::Node<position> *p = path.head; p; p = p->next)
            printf("(%i %i) ", p->value.x, p->value.y);
        printf("\n");
    }

    void draw_path()
    {
        printf("Drawing...\n");
        image p_i = image(map.width, map.height);
        p_i.DrwaAll('0');
        for (List<position>::Node<position> *p = path.head; p; p = p->next)
            p_i[p->value.y][p->value.x]='x';
        std::cout<<p_i;
    }
};