import sys
import math


# Don't let the machines win. You are humanity's last hope...

width = int(input())  # the number of cells on the X axis
height = int(input())  # the number of cells on the Y axis

coords = set()

for i in range(height):
    line = input()  # width characters, each either 0 or .
    for j in range(width):
        if line[j] == '0':
            coords.add((j, i))

outcoords = []

for c in coords:
    x = c[0]
    y = c[1]
    outline = [x, y]
    horiz_x = x + 1
    while horiz_x < width:
        if (horiz_x, y) in coords:
            outline.extend([horiz_x, y])
            break

        horiz_x += 1
    else:
        outline.extend([-1, -1])

    horiz_y = y + 1
    while horiz_y < height:
        if (x, horiz_y) in coords:
            outline.extend([x, horiz_y])
            break
        horiz_y += 1
    else:
        outline.extend([-1, -1])
    
    # if (x, y + 1) in coords:
    #     outline.extend([x, y + 1])
    # else:
    #     outline.extend([-1, -1])

    outcoords.append(outline)

# Write an action using print
# To debug: print("Debug messages...", file=sys.stderr, flush=True)


# Three coordinates: a node, its right neighbor, its bottom neighbor
for out in outcoords:
    print(' '.join(map(str, out)))
