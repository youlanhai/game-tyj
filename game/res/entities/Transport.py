#-*- coding: utf-8 -*-

import Lazy
#import Entity
import Npc

from data import d_map
from data import d_npc
from data import d_task

from GData import GData

class Transport(Npc.Npc) :
	
	@staticmethod
	def isTransport() : return True
	
	def __init__(self) :
		super(Transport, self).__init__()
		self.transNo = 0
		
	#是否可移动
	def canMove(self) : return False

	def onTargetActive(self) :
		lv = d_map.getTransLevel(self.eno)
		if GData.player.level < lv :
			GData.player.chatClientMsg("需要等级%d级" % lv)
			return
		task = d_map.getTransTask(self.eno)
		if task :
			for no in task :
				if not GData.player.isTaskCompeted(no) :
					GData.player.chatClientMsg("未完成任务[%s]" % d_task.getTaskName(no))
					return
		GData.guiMgr.transForm.show(True)
		
	#根据eno更新数据。
	def updateData(self) :
		pos = d_map.getTransPosition(self.eno)
		if pos :
			self.position = Lazy.Vector3(*pos)
		self.setName(d_npc.getName(1002))
		self.loadModel(d_npc.getModelID(1002))
		