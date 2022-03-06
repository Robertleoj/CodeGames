import sys
import math

# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.

# checcx = 0
# checcy = 0


def thrust(distance, angle):
    dist_p =  max(-math.exp(-distance * 0.002) + 1, 0)
    # if abs(angle) > 90:
    #     angle_p = 0
    # else:
    angle_p = (180**0.8 - abs(angle)**0.8) / (180**0.8)

    return dist_p * angle_p * 100

# Cell

# Cell

checc_x = 0
checc_y = 0
boost_used = False
using_boost = True


# game loop
while True:
    # next_checkpoint_x: x position of the next check point
    # next_checkpoint_y: y position of the next check point
    # next_checkpoint_dist: distance to the next checkpoint
    # next_checkpoint_angle: angle between your pod orientation and the direction of the next checkpoint
    x, y, next_checkpoint_x, next_checkpoint_y, next_checkpoint_dist, next_checkpoint_angle = [int(i) for i in input().split()]
    opponent_x, opponent_y = [int(i) for i in input().split()]


    
    if using_boost and abs(next_checkpoint_angle) < 5:
        thrust_amount = "BOOST"
        using_boost = False
        boost_used = True
    else:
        thrust_amount = str(int(thrust(next_checkpoint_dist, next_checkpoint_angle)))
    # thrust_amount 
    

    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)


    # You have to output the target position
    # followed by the power (0 <= thrust <= 100)
    # i.e.: "x y thrust"
    print(str(next_checkpoint_x) + " " + str(next_checkpoint_y) + " " + thrust_amount)
