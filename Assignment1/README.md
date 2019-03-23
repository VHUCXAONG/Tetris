# CMPT361 Assignment1 Tetris on 2D plane
## Overview
Tetris game on 2D plane. All assignment requirements and steps are completed.

## Additional functions

#### int updatetile()
1. A tile is tested whether it can drop down. If not, function **settile()** will be called.

2. An boolean variable called **ending flag** is set at the beginning of the function. If the value is True, then the game will stop. And once the game is over, all the keyboard inputs will be unavailable and the screen won't change except	*q*, *ESC* and *r*.

#### vec3 GetTop()

The function gets top y axis value of a tile, which prevents new tiles to get out of the screen.

#### bool CheckSameColor(int x1, int y1, int x2, int y2)

Check if two blocks at (x1,y1) and (x2,y2) have the same color.

#### void RemoveCol(int x, int y)

Remove three consecutive tiles of the same color in the same column.

#### void Remove(int x, int y)
Remove three consecutive tiles of the same color in the other two directions: horizontal and diagonal.

#### void CheckRemove()

Check if the condition that three tiles in the same column, row or diagonal is satisfied. This functions is called recursively until the condition is no longer satisfied.

## Some clearifications of the game:

1. The game stops when the entire board is covered. And in this case, a newly generated board will have overlap with existing boards. That's the symbol of losing the game.

2. After game ends, the screen will stay unchanged until you press "r" to restart or "esc" or "q" to quit.

3. After removing some tiles, some remaining tiles might "hang in the air", which is really weird but doesn't violate any requirement of this assignment.
