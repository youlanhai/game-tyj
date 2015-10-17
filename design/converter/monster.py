#-*- coding: utf-8 -*-

import csvop

def work() :
	return csvop.parseCSV(Parser(), "monster.csv", "dd_monster.py")

class Parser(csvop.BaseParser) :
	def parseByKey(self, key, value) :
		
		if key == "id" :
			return self.eval(value)
		
		elif key == "name" :
			return ("name", self.str(value))
		
		elif key == "model":
			return ("model", self.eval(value))
		
		elif key == "level":
			return ("level", self.eval(value))
		
		elif key == "taskObj":
			if value : return ("taskObj", self.tuple(value))
		
		elif key == "friend":
			return ("friend", self.eval(value))
		
		elif key == "talk":
			if value : return ("talk", self.dict(value))
		
		elif key == "canshow" :
			if value : return ("canshow", self.eval(value))
		
		elif key == "notshow" :
			if value : return ("notshow", self.eval(value))
		
		elif key == "position" :
			if value : return ("position", self.tuple(value))
		
		return None

