#-*- coding: utf-8 -*-

import traceback

moduleName = (
	"task", 
	"task_npc", 
	"task_submit",
	"task_trigger",
	"monster",
	"monster_center",
)

def workAll() :
	for name in moduleName :
		print "\nstart work:", name
		try:
			mod = __import__(name)
			mod.work()
		except :
			print "convert %s found excption" % name
			print traceback.print_exc()

if __name__ == "__main__" :
	workAll()
