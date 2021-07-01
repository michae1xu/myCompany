#!/usr/bin/python2

import sys
import os
import shutil

pathsource = sys.argv[1]
pathdest = sys.argv[2]
#pathsource = r"/home/rpudebug/rpudesignersolution/tmp"
#pathdest = r"/home/rpudebug/rpudesignersolution/var/[DSN]cmdword"


with open(os.path.join(pathsource,"cmd_index.h"), "r") as f:
    hlines = f.readlines()

with open(os.path.join(pathsource,"rpu","sboxGroup.h"), "r") as f:
    hlines.extend(f.readlines())

with open(os.path.join(pathdest,"include","renux","renux_const.h"), "w") as f:
    f.writelines(hlines)



with open(os.path.join(pathsource,"rpu","sboxGroup.c"), "r") as f:
    clines = f.readlines()

with open(os.path.join(pathdest,"drivers","rpu","renux_const.c"), "w") as f:
    f.writelines(clines)

shutil.copy(os.path.join(pathsource, "rpu_cfg.bin"), os.path.join(pathdest, "drivers", "rpu"))
shutil.copy(os.path.join(pathsource, "sboxData.bin"), os.path.join(pathdest, "drivers", "rpu"))

exit(0)
