#-*- coding: utf-8 -*-

import packagepy
import packagepath

def pack() :
	packagepy.extractFiles("package\\python27\\", packagepath.paths, packagepath.syspath)

if __name__ == "__main__" :
	pack()
	