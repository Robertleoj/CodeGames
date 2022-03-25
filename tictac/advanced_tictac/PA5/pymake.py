import os



def get_files(path):
    files = []
    for f in os.listdir(path):
        if os.path.isdir(os.path.join(path, f)):
            files.extend(get_files(os.path.join(path, f)))
        else:
            if(f.endswith(".cpp")):
                files.append(os.path.join(path, f))
    return files


LINKER_FLAGS = "-lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf"

# For my stupid desktop
# LINKER_FLAGS += " -ltiff"

COMPILER_FLAGS = ""
CC = "g++"
OBJ_NAME  = "out"
files = get_files("./")

file_str = " ".join(files)

os.system(f"{CC}  {COMPILER_FLAGS} {file_str} {LINKER_FLAGS} -o {OBJ_NAME}")


