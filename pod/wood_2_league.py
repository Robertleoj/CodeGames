import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.

# checcx = 0
# checcy = 0


def thrust(distance, angle):
    dist_p =  max(-math.exp(-distance * 0.002) + 1, 0)
    if abs(angle) > 90:
        angle_p = 0
    else:
        angle_p = (90 - abs(angle)) / 90

    return dist_p * angle_p * 100

# Cell

# Cell


# game loop
while True:
    # next_checkpoint_x: x position of the next check point
    # next_checkpoint_y: y position of the next check point
    # next_checkpoint_dist: distance to the next checkpoint
    # next_checkpoint_angle: angle between your pod orientation and the direction of the next checkpoint
    x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle = [int(i) for i in input().split()]
    opponent_x, opponent_y = [int(i) for i in input().split()]


    thrust_amount = thrust(next_checkpoint_dist, next_checkpoint_angle)
    # thrust_amount -= abs(next_checkpoint_angle * 0.5) / 180
    # thrust_amount 
    

    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)


    # You have to output the target position
    # followed by the power (0 <= thrust <= 100)
    # i.e.: "x y thrust"
    print(str(next_checkpoint_x) + " " + str(next_checkpoint_y) + " " + str(int(thrust_amount)))
