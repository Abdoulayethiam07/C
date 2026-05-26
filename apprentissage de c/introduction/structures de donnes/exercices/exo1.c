#include <stdio.h>
typedef struct point
{
    int x;
    int y;
} Point;

int main()
{
    Point p1, p2;
    p1.x = 0;
    p1.y = 1;
    p2.x = 3;
    p2.y = 7;
    printf("p1 :(%d,%d)\n", p2.x, p2.y);
    printf("p2 :(%d,%d)\n", p1.x, p1.y);
}
