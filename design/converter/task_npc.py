#-*- coding: utf-8 -*-

import csvop

def work() :
	return csvop.parseCSV(Parser(), "task_npc.csv", "dd_task_npc.py")

class Parser(csvop.BaseParser) :
	def parseByKey(self, key, value) :
		if key == "id" :
			return self.eval(value)
		
		elif key == "name" :
			return ("name", self.str(value))
		
		elif key == "model":
			return ("model", self.eval(value))
		
		elif key == "position":
			return ("position", self.tuple(value))
		
		elif key == "visible":
			if value : return ("visible", self.eval(value))
		
		elif key == "talk":
			if value : return ("talk", self.dict(value))
		
		elif key == "canshow":
			if value : return ("canshow", self.eval(value))
		
		elif key == "notshow":
			if value : return ("notshow", self.eval(value))
		
		elif key == "notshow2":
			if value : return ("notshow2", self.eval(value))
			
		return None

