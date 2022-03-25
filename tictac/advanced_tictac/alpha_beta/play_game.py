import os
import sys

executable1 = sys.argv[1]

# get current path
path = os.getcwd()


cd = lambda x: os.chdir(x)
run = lambda x: os.system(x)

cd("second_try")

run("g++ *.cpp -o player")
run("cp player ../")
cd("..")

executable2 = "player"

executable_path_1 = path + "/" + executable1
executable_path_2 = path + "/" + executable2


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

# os.system(f"kill `ps aux | grep {executable1} | grep -v game_player |awk '{{ print $2 }}'`")
# os.system(f"kill `ps aux | grep {executable2} | grep -v game_player |awk '{{ print $2 }}'`")



