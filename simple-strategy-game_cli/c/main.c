#include <stdio.h>
#include <stdlib.h>

#define DBG(x) { printf("DEBUG | "); x; printf("\n"); }
#define bool int
#define true 1
#define false 0

typedef struct 
{
    int trOwn;  // troop owner | >=0 owner id, <0 no troop on tile
    int value;  // =0 prohodno, <0 neprohodno, >0 trupa
} 
Tile;

typedef struct 
{
    int width;
    int height;
    Tile** tiles;
} 
Map;

void renderMap(Map* map)
{
    // iterators
    int x, y;

    // Print map header
    printf("\n  X\nY   ");
    for(x = 0; x < map->width; x++) 
        printf("%d ", x);

    // Print map (and side bar)
    for(y = 0; y < map->height; y++)
    {
        // side bar number
        printf("\n  %d", y);

        // print map
        for(x = 0; x < map->width; x++)
        {
            Tile* tile = map->tiles[x] + y; // current tile pointer
            
            printf(" ");                    // space between tiles characters

            // walkable tile
            if (tile->value == 0)
                printf(".");

            // blocked tile
            else if (tile->value < 0)
            {
                printf("#");
            }

            else
                printf("%d", tile->value);  // not empty tile
        }
    }

    printf("\n");
}

void fillMap(Map* map, Tile value)
{
    int x, y;

    for(y = 0; y < map->height; y++)
    {
        for(x = 0; x < map->width; x++)
        {
            map->tiles[x][y] = value;
        }
    }
}

Tile** newTiles(int width, int height)
{
    Tile** tiles = (Tile**) malloc(sizeof(Tile*) * width); // niz pointera koji ce pokazivati na kolone
    
    int x; 
    for(x = 0; x < width; x++)
    {
        tiles[x] = (Tile*) malloc(sizeof(Tile) * height); // Tiles kolona
    }

    return tiles;
}

Map* newMap(int width, int height)
{
    Map* map = (Map*) malloc(sizeof(Map));

    map->width = width;
    map->height = height;
    map->tiles = newTiles(width, height);

    Tile tile;
    tile.value = 0;
    tile.trOwn = -1;

    fillMap(map, tile);

    return map;
}

bool addUnits(Map* map, int x, int y, int count, int owner)
{
    if (map->width <= x || map->height <= y)
        return false;
    
    Tile* tile = map->tiles[x] + y;
    
    if (tile->value == 0 || (tile->trOwn == owner && tile->value > 0)) 
    {
        tile->value += count;
        tile->trOwn = owner;
        return true;
    }
}

bool moveUnits(Map* map, int x, int y, int xt, int yt, int count, int owner)
{
    if (map->width <= x || map->height <= y)
        return false;
    
    Tile* sTile = map->tiles[x] + y;    // source tile
    Tile* tTile = map->tiles[xt] + yt;  // target tile
    
    // Realize movement
    if (sTile->trOwn == owner && sTile->value >= count)
    {
        sTile->value -= count; // move units from source troop

        // no more units on source tile
        if (sTile->value == 0)
            sTile->trOwn = -1;

        // Add units to target troop
        if (tTile->trOwn == owner)
        {
            tTile->value += count;
        }
        // Attack enemy troop
        else if (tTile->trOwn >= 0)
        {
            int diff = count - tTile->value; // difference between attacker and defender unit count
            
            // Attacker wins / Draw
            if (diff >= 0)
            {
                tTile->trOwn = (diff > 0)? owner : -1;
                tTile->value = diff;
            }
            // Defender wins
            else
            {
                tTile->value -= count;
            }
            
        }
        
        return true;
    }

    return false;
}

int main()
{
    Map* map = newMap(10, 10);
    
    int players = 2;        // number of players
    int troops[] = {5, 5};  // number of troops per player
    int turn = 0;           // player on turn
    
    renderMap(map);

    
    // Game loop
    while(1)
    {
        char cmd[8];
        bool ok = false;
        
        // Command input
        printf("\nm - move\ns - spawn\n\nplayer %d > ", turn);
        scanf("%s", cmd);

        // Bindings
        // move
        if (cmd[0] == 'm') 
        {
            int x, y, xt, yt, count;

            printf("Choose source and target locations and count (x y xt yt c): ");
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &xt);
            scanf("%d", &yt);
            scanf("%d", &count);

            ok = moveUnits(map, x, y, xt, yt, count, turn);
        }
        // spawn
        else if (cmd[0] == 's') 
        {
            int x, y, count;

            printf("Choose location and count (x y c): ");
            scanf("%d", &x);
            scanf("%d", &y);
            scanf("%d", &count);

            ok = addUnits(map, x, y, count, turn);
        }

        // To render
        if (ok) 
        {
            renderMap(map);
            turn = (turn + 1 == players)? 0 : turn + 1;
        }
        else
            printf("\n : greska! pokusajte ponovo.\n");
    }

    return 0;
}