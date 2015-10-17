#-*- coding: utf-8 -*-

import csvop

def work() :
	return csvop.parseCSV(Parser(), "task_submit.csv", "dd_task_submit.py")

class Parser(csvop.BaseParser) :
	def parseByKey(self, key, value) :
		if key == "id" :
			return self.eval(value)
		
		elif key == "npc":
			return ("npc", self.eval(value))
		
		elif key == "talk":
			if value : return ("talk", self.dict(value))
		
		return None
		
