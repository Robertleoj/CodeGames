import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.

# w: width of the building.
# h: height of the building.
w, h = [int(i) for i in input().split()]
n = int(input())  # maximum number of turns before game over.
x0, y0 = [int(i) for i in input().split()]

min_x = 0

max_x = w -1 

min_y = 0

max_y = h - 1

curr_x = x0
curr_y = y0

# game loop
while True:
    bomb_dir = input()  # the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)
    jmp_x = 0
    jmp_y = 0




    if "R" in bomb_dir:
        min_x = curr_x + 1
        curr_x = (max_x + min_x) // 2
       
    if "U" in bomb_dir:
        max_y = curr_y - 1
        curr_y = (max_y + min_y) // 2

    if "D" in bomb_dir:
        min_y = curr_y + 1
        curr_y = (max_y + min_y) // 2

    if "L" in bomb_dir:
        max_x = curr_x - 1
        curr_x = (max_x + min_x) // 2      


    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)


    # the location of the next window Batman should jump to.
    print(f"{curr_x} {curr_y}")
