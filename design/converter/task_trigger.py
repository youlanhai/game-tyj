#-*- coding: utf-8 -*-

import csvop

def work() :
	return csvop.parseCSV(Parser(), "task_trigger.csv", "dd_task_trigger.py")
	
class Parser(csvop.BaseParser) :
	def parse(self, col, value) :
		if col == 0 :
			return self.eval(value)
		if col == 1 :
			return ("type", self.eval(value))
		if col == 2:
			if value : return ("new", self.dict(value))
		if col == 3:
			if value : return ("del", self.tuple(value))
		if col == 4:
			if value : return ("mov", self.dict(value))
		return None
