#!/usr/bin/python2

import sys
import os
import shutil
import glob

#a = input()
def printConsole(fuc):
    def wrapper(*args,**kwargs):
        if(len(args) == 1):
            print("Remove file: ", args[0])
        if(len(args) == 2):
            print("Updata file: ", args[0])
        fuc(*args,**kwargs)
    return wrapper

pathsource = sys.argv[1]
pathdest = sys.argv[2]
#pathsource = r"/home/rpudebug/rpudesignersolution/tmp"
#pathdest = r"/home/rpudebug/rpudesignersolution/var/[DSN]cmdword"

os.chdir(pathdest)
os.makedirs("drivers/rpu")
os.makedirs("flash")
os.makedirs("include/renux")
os.makedirs("lib/rpu")

rmfiles = glob.glob(os.path.join(pathdest,"lib","rpu","*.o"))
rmfiles.extend(glob.glob(os.path.join(pathdest,"lib","rpu","*.c")))
rmfiles.extend(glob.glob(os.path.join(pathdest,"lib","rpu","*.h")))

for file in rmfiles:
    printConsole(os.remove)(file)


myCopy = printConsole(shutil.copy)
myCopy(os.path.join(pathsource, "cmd_index.h"), os.path.join(pathdest, "include"))
myCopy(os.path.join(pathsource, "rpu_cfg.bin"), os.path.join(pathdest, "flash"))
myCopy(os.path.join(pathsource, "sboxData.bin"), os.path.join(pathdest, "flash"))

cpfiles = os.listdir(os.path.join(pathsource,"rpu"))

for file in cpfiles:
    myCopy(os.path.join(pathsource, "rpu", file), os.path.join(pathdest, "lib", "rpu"))

print(("Copy Data from \t \"%s\" \t to \"%s\" \n ") % (pathsource,pathdest))

exit(0)
#a = input()
