#-*- coding: utf-8 -*-

import csvop

def work() :
	return csvop.parseCSV(Parser(), 
		"monster_center.csv",
		"dd_monster_center.py")

class Parser(csvop.BaseParser) :
	def parse(self, col, value) :
		if col == 0 :
			return self.eval(value)
		if col == 1 :
			return ("map", self.eval(value))
		if col == 2:
			return ("position", self.tuple(value))
		if col == 3:
			return ("monster", self.eval(value))
		if col == 4:
			return ("amount", self.eval(value))
		if col == 5:
			return ("level", self.eval(value))
		if col == 6:
			return ("friend", self.eval(value))
		return None
		
