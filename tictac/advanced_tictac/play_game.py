import os
import sys

executable1 = sys.argv[1]
executable2 = sys.argv[2]

# get current path
path = os.getcwd()

executable_path_1 = path + "/" + executable1
executable_path_2 = path + "/" + executable2

cd = lambda x: os.chdir(x)
run = lambda x: os.system(x)


while "game_player" not in os.listdir():
    cd("..")

cd("game_player")

os.system(f"rm {executable1}")
os.system(f"rm {executable2}")

os.system(f"cp {executable_path_1} ./{executable1}")
os.system(f"cp {executable_path_2} ./{executable2}")

os.system("make")

os.system(f"./game_player {executable1} {executable2}")
os.system(f"rm {executable1}")
os.system(f"rm {executable2}")



