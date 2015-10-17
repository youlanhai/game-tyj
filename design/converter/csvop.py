#-*- coding: utf-8 -*-

import csv
import traceback

##################################################
#csv转换器
##################################################
class Converter(object) :
	def __init__(self, parser) :
		self.dataStr = ""
		self.parser = parser
		self.header = ()
		
	#工作。同时转换和保存
	def work(self, inCsvfile, outPyfile) :
		if not self.convert(inCsvfile) :
			print "---convert failed", inCsvfile
			return False
		if not self.save(outPyfile) :
			print "---save failed", outPyfile
			return False
		print "success convert %s to %s" % (inCsvfile, outPyfile)
		return True
		
	#转换
	def convert(self, csvfile) :
		fin = None
		try :
			fin = open(csvfile, "rb")
		except IOError:
			print "file %s not found" % csvfile
			return False
		try :
			self.parse(fin)
		except :
			fin.close()
			print traceback.print_exc()
			return False
		fin.close()
		return True
		
	#解析
	def parse(self, fin) :
		self.dataStr = "sheet = {\n"
		fcsv = csv.reader(fin)
		for idx, data in enumerate(fcsv) :
#			print "data:", data
			if idx == 0 : #0行为表头
				self.parseHeader(data)
			else:
				self.parseRow(idx, data)
		self.dataStr += "}\n"
		
	def parseHeader(self, header):
		self.header = tuple(header)
		
	def parseRow(self, row, data) :
		tempStr = "{"
		key = None
		for col, d in enumerate(data) :
			result = self.parseCol(col, d)
			if result is None: #忽略项
				continue
			if not isinstance(result, tuple) : #主键
				key = result
			else : #正常项
				k, v = result
				tempStr += "'%s': %s, " % (k, v)
		tempStr += "}"
		if key is not None :
			self.dataStr += "\t%s: %s,\n" % (key, tempStr)
		
	def parseCol(self, col, value) :
		if hasattr(self.parser, "convDict"):
			key = self.header[col]
			t, k, fun = self.parser.convDict[key]
			return k, fun(key, value)
			
		if hasattr(self.parser, "parseByKey"):
			key = self.header[col]
			return self.parser.parseByKey(key, value)
		
		return self.parser.parse(col, value)
	
	#保存
	def save(self, pyfile) :
		try :
			fin = open(pyfile, "w")
		except IOError :
			print "create file %s failed" % pyfile
			return False
		try :
			self.saveData(fin)
		except :
			fin.close()
			traceback.print_exc()
			return False
		fin.close()
		return True
		
	def saveData(self, handle) :
		self.saveHead(handle)
		self.saveSheet(handle)
		
	def saveHead(self, handle) :
		handle.write("#-*- coding: utf-8 -*-\n\n")
		
	def saveSheet(self, handle) :
		handle.write(self.dataStr)
		handle.write("\n\nsize = %d\n" % len(self.dataStr))

##################################################
#表数据分析器基类
##################################################
class BaseParser(object) :
	def parse(self, c, v) :
		return c, v
		
	def eval(self, st) :
		return st.strip()
		
	def str(self, st) :
		v = "'%s'" % st.strip()
		return v
	
	def dict(self, st) :
		v = st.strip()
		if v[0] != "{" :
			v = "{" + v
		if v[-1] != "}" :
			v += "}"
		return v
		
	def list(self, st) :
		v = st.strip()
		if v[0] != "[" :
			v = "[" + v
		if v[-1] != "]" :
			v += "]"
		return v
		
	def tuple(self, st) :
		v = st.strip()
		if v[0] != "(" :
			v = "(" + v
		if v[-1] != ")" :
			if v[-1] != "," :
				v += ","
			v += ")"
		return v

##################################################
#转换csv文件
##################################################
MYGAME_CSV_PATH = "../csv/"
MYGAME_DATA_PATH = "../../res/entities/data/"

def parseCSV(parser, csvFile, pyFile):
	return Converter(parser).work(MYGAME_CSV_PATH+csvFile, MYGAME_DATA_PATH+pyFile)
