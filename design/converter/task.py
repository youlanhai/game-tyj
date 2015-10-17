#-*- coding: utf-8 -*-

import csvop

def work() :
	return csvop.parseCSV(Parser(), "task.csv", "dd_task.py")

class Parser(csvop.BaseParser) :
	def parseByKey(self, key, value) :
		
		if key == "id" :
			return self.eval(value)
		
		elif key == "name" :
			return ("name", self.str(value))
		
		elif key == "npc":
			return ("npc", self.eval(value))
		
		elif key == "cond":
			if value : return ("cond", self.tuple(value))
		
		elif key == "work":
			if value : return ("work", self.dict(value))
		
		elif key == "position" :
			if value : return ("position", self.str(value))
		
		elif key == "describe":
			if value : return ("describe", self.str(value))
		
		elif key == "trigger":
			if value : return ("trigger", self.tuple(value))
		
		elif key == "talk":
			if value : return ("talk", self.dict(value))
		
		return None
