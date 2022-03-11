import os
import sys

executable = sys.argv[1]

# get current path
path = os.getcwd()

executable_path = path + "/" + executable

cd = lambda x: os.chdir(x)
run = lambda x: os.system(x)


while "game_player" not in os.listdir():
    cd("..")

cd("game_player")

os.system(f"rm {executable}1")
os.system(f"rm {executable}2")

os.system(f"cp {executable_path} ./{executable}1")
os.system(f"cp {executable_path} ./{executable}2")

os.system("make")

os.system(f"./game_player {executable}1 {executable}2")
os.system(f"rm {executable}1")
os.system(f"rm {executable}2")



