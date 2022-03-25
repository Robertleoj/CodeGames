import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.
# ---
# Hint: You can use the debug stream to print initialTX and initialTY, if Thor seems not follow your orders.

# light_x: the X position of the light of power
# light_y: the Y position of the light of power
# initial_tx: Thor's starting X position
# initial_ty: Thor's starting Y position
light_x, light_y, initial_tx, initial_ty = [int(i) for i in input().split()]

curr_x = initial_tx
curr_y = initial_ty

dirs = []
while light_x != curr_x or light_y != curr_y:
    if light_x > curr_x:
        if light_y > curr_y:
            dirs.append('SE')
            curr_x += 1
            curr_y += 1
        elif light_y < curr_y:
            dirs.append('NE')
            curr_x += 1
            curr_y -= 1
        else:
            dirs.append('E')
            curr_x += 1
    elif light_x < curr_x:
        if light_y > curr_y:
            dirs.append('SW')
            curr_x -= 1
            curr_y += 1
        elif light_y < curr_y:
            dirs.append('NW')
            curr_x -= 1
            curr_y -= 1
        else:
            dirs.append('W')
            curr_x -= 1



# game loop
idx = 0
while True:
    remaining_turns = int(input())  # The remaining amount of turns Thor can move. Do not remove this line.
    print(dirs[idx])
    idx += 1




    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)


    # A single line providing the move to be made: N NE E SE S SW W or NW
