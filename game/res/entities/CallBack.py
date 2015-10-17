#-*- coding: utf-8 -*-
import Lazy
import traceback

g_callBackPool = None

class CallObj(Lazy.iCallObj) :
	def __init__(self, time, call, args) :
		super(CallObj, self).__init__()
		self.init(time)
		self.call = call
		self.args = args
			
	def onDead(self) :
		if self.call:
			try :
				self.call(*self.args)
			except :
				print traceback.print_exc()
		g_callBackPool.remove(self)

			
class CallBackPool(object) :
	def __init__(self) :
		global g_callBackPool
		g_callBackPool = self
		
		self.pool = []
		
	def add(self, obj) :
		if not obj or obj in self.pool :
			return
		self.pool.append(obj)
		
	def remove(self, obj) :
		try :
			self.pool.remove(obj)
		except ValueError:
			return
		
CallBackPool()

def callBack(time, call, *args) :
	global g_callBackPool
	obj = CallObj(time, call, args)
	g_callBackPool.add(obj)
	return obj

class Functor(object) :
	def __init__(self, func, *args) :
		print "Functor", func, args
		self.func = func
		self.args = args
		
	def __call__(self) :
		self.func(*self.args)