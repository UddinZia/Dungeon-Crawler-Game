#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**

 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */

char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) 
{
    ifstream file;
    file.open(fileName);

    if (!file.is_open()) // if file couldn't open
    {
        return nullptr;
    }
    
    else
    {
        int charNum = 0;
        file >> maxRow >> maxCol;
        file >> player.row >> player.col;
        int maxChar = maxRow * maxCol;
        string line;
        while (getline(file, line))
        {
            for (unsigned int i = 0; i < line.length(); ++i)
            {
                if (line.at(i) != ' ')
                {
                    charNum++;
                }
            }
        }
        if (charNum != maxChar)
        {
            return nullptr;
        }
        file.close();
    }
    file.open(fileName);
    
    file >> maxRow >> maxCol; 
    if (file.fail()) // invalid map dimensions
    {
        return nullptr;
    }
    file >> player.row >> player.col;
    if (file.fail()) // invalid player location
    {
        return nullptr;
    }

    if ((player.row > maxRow) || (player.col > maxCol) ||  (player.row < 0) || (player.col < 0)) // player location out of bounds
    {
        return nullptr;
    }
    
    if ((maxRow <= 0) || (maxCol <= 0)) // dimensions are <= 0 
    {
        return nullptr;
    }
    

    char** map = new char*[maxRow];
    for (int i = 0; i < maxRow; ++i)
    {
        map[i] = new char[maxCol];
    }

    char tile;
    bool door = false;
    bool exitDoor = false;
    for (int j = 0; j < maxRow; ++j)
    {
        for (int k = 0; k < maxCol; ++k)
        {
            file >> tile;
            if (tile == '?')
            {
                door = true;    
            }
            if (tile == '!')
            {
                exitDoor = true;
            }
            if (tile == 'o')
            {
                map[j][k] = '-';
            }
            if ((tile == '-') || (tile == '$') ||(tile == '@') || (tile == 'M') || (tile == '+') || (tile == '?') || (tile == '!')) // if correct tile
            {
                map[j][k] = tile;
            }
            else
            {
                deleteMap(map,maxRow);
                return nullptr; //return nullptr if empty or bad tile
            }
        }
    }
    if ((door == true && exitDoor == true) || (door == false && exitDoor == false)) // if map has both door and an exit or neither
    {
       deleteMap(map,maxRow);
       return nullptr;
    }
    map[player.row][player.col] = 'o';
    file.close();
    return map;
}

/**
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) 
{
    if (input == 'w') // move up a row
    {
        nextRow--;
    }
    else if (input == 'a') // move one col to left
    {
        nextCol--;
    }
    else if (input == 's') // move down a row
    {
        nextRow++;
    }
    else if (input == 'd') // move one col to right
    {
        nextCol++;
    }
}

/**

 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */

/*
char** createMap(int maxRow, int maxCol) {
    return nullptr;
}

*/
/**
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */

void deleteMap(char**& map, int& maxRow) 
{
 if (map != nullptr)
    {
        for (int i = 0; i < maxRow; ++i)
        {
            delete[] map[i];
        }
        delete[] map;
        map = nullptr;
        maxRow = 0;
    }
}

/**
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) 
{
    
    
    if (map == nullptr || maxRow == 0 || maxCol == 0)
    {
        return nullptr;
    }

    int oldRow = maxRow;
    int oldCol = maxCol;
    maxRow *= 2;
    maxCol *= 2;
    

    char** newMap = new char*[maxRow];
    for (int i = 0; i < maxRow; ++i)
    {
        newMap[i] = new char[maxCol];
    }

    for (int currentRow = 0; currentRow < maxRow; currentRow++)
    {
        for (int currentCol = 0; currentCol < maxCol; ++currentCol)
        {
            if ((currentRow >= oldRow || currentCol >= oldCol) && map[currentRow % oldRow][currentCol % oldCol] == 'o')
            {
                map[currentRow % oldRow][currentCol % oldCol] = '-';    
            }
            newMap[currentRow][currentCol] = map[currentRow % oldRow][currentCol % oldCol];
        }

    }
    deleteMap(map,oldRow);    
    return newMap;
}

/**
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) 
{
    if (nextCol > maxCol || nextCol < 0) // out of bounds for column
    {
        return 0;
    }
    else if (nextRow > maxRow || nextRow < 0) // out of bounds for row
    {
        return 0;
    }
    else if (map[nextRow][nextCol] == '+' || map[nextRow][nextCol] == 'M') // encounter pillar or monster
    {
        return 0;
    }
    else if (map[nextRow][nextCol] == '-')
    {
        map[player.row][player.col] = '-'; // set current position to TILE_OPEN
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = 'o'; //set next position to TILE_PLAYER
        return 1;
    }
    else if (map[nextRow][nextCol] == '$')
    {
        player.treasure++;
        map[player.row][player.col] = '-'; 
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = 'o';
        return 2;
    }
    else if (map[nextRow][nextCol] == '@') // encounter amulet
    {
        map[player.row][player.col] = '-'; 
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = 'o';
        return 3;
    }
    else if (map[nextRow][nextCol] == '?') // encounter door
    {
        map[player.row][player.col] = '-'; 
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = 'o';
        return 4;
    }
    else if (map[nextRow][nextCol] == '!') // encounter exit
    {
        if (player.treasure > 0) // if player has enough treasure
        {
            map[player.row][player.col] = '-'; 
            player.row = nextRow;
            player.col = nextCol;
            map[player.row][player.col] = 'o';
            return 5;
        }
        return 0;
    }
    return 0; 
}

/**
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) 
{
    bool monster = false;
    for (int i = player.row; i >= 0; --i) // checking above player
    {
        if (map[i][player.col] == '+') //if monster DNE or obstacle in line of sight
        {
            break; 
        }
        else if (map[i][player.col] == 'M') // set previous tile to open and next to monster
        {
            map[i][player.col] = '-';
            map[i + 1][player.col] = 'M';
            if ((i + 1) == player.row) // if monster is on player tile
            {
                monster = true;
            }
        }
    }

    for (int j = player.row; j <= maxRow -1; ++j) // checking below player
    {
        if (map[j][player.col] == '+') 
        {
             break; 
        }
        else if (map[j][player.col] == 'M') 
        {
            map[j][player.col] = '-';
            map[j - 1][player.col] = 'M';
            if ((j-1) == player.row)
            {
                monster = true;
            }
        }
    }

    for (int k = player.col; k <= maxCol - 1; ++k) // check to right of player
    {
        if (map[player.row][k] == '+')
        {
             break;
        }
        else if (map[player.row][k] == 'M')
        {
            map[player.row][k] = '-';
            map[player.row][k - 1] = 'M';
            if ((k-1) == player.col)
            {
                monster = true;
            }
        }
    }

    for (int z = player.col; z >= 0; --z) // check to left of player
    {
        if (map[player.row][z] == '+')
        {
            break;
        }
        else if (map[player.row][z] == 'M')
        {
            map[player.row][z] = '-';
            map[player.row][z + 1] = 'M';
            if ((z+1) == player.col)
            {
               monster = true;
            }
        }
    }
    return monster;
}
