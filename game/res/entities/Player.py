#-*- coding: utf-8 -*-

import Lazy

import CallBack
import Define
import Entity
#import Helper
import TaskMgr
import Physics

from CallBack import Functor

#from data import d_action
from data import d_map
from data import d_task

from GData import GData

class Player(Entity.Entity) :
	@staticmethod
	def isPlayer() : return True
	
	def __init__(self) :
		print "player init"
		super(Player, self).__init__()
		self.speed.set(400, 400, 400)
		self.setENO(1)
		self.taskMgr = TaskMgr.TaskMgr()
		GData.guiMgr.playerInfo.updateInfo(self)
		#self.addExp(1000)
		#self.setLevel(50)
		
	def canShow(self): return True
		
	#创建物理
	def createPhysics(self) :
		self.physics = Physics.PlayerPhysics(self)
		
	def load(self, data):
		self.taskMgr.load(data.get("task"))
		self.loadData(data.get("idata"))
		map = data.get("map", 1)
		position = data.get("position", (0, 0, 0))
		
		self.doTeleport(map, position)
	
	def save(self) :
		task = self.taskMgr.save()
		idata = self.saveData()
		return {
			"task": task,
			"idata": idata, 
			"position": (self.position.x, self.position.y, self.position.z),
			"map" : GData.mapMgr.mapNO,
		}
		
	def onGameStart(self):
		GData.guiMgr.taskTrace.updateData()
		
	def doTeleport(self, mapNo, position) :
		self.lockTarget(None)
		GData.gameState = Define.GS_TANSITION
		GData.guiMgr.fadeDoor.closeDoor(Functor(self._onTransDoorClose, mapNo, position))
		
	#进行传送
	def onTransport(self, mapNO, transNO) :
		print "onTransport", mapNO, transNO
		if self.canTransport():
			pos = d_map.getTransPosition(transNO)
			self.doTeleport(mapNO, pos)
	
	#关门特效完毕
	def _onTransDoorClose(self, mapNO, pos) :
		print "_onTransDoorClose", mapNO, pos
		GData.mapMgr.loadMap(mapNO, False)
		self.position = Lazy.Vector3(*pos)
		
	#开门特效完毕
	def _onTransMapLoaded(self) :
		GData.gameState = Define.GS_RUNNING
		
	def onMapChange(self, mapNO):
		GData.guiMgr.fadeDoor.openDoor(self._onTransMapLoaded)
		
	def onModelLoaded(self) :
		super(Player, self).onModelLoaded()
		print "---onModelLoaded", self
		self.resetCamera()
		if self.model :
			self.model.setAnimSpeed(1.5)
		
	def resetCamera(self) :
		print "---resetCamera", self
		if self.model:
			GData.camera.setHeight(self.model.getBBRadius())
		
	def canSelect(self):
		return False
		
	def chatClientMsg(self, msg) :
		print "chatClientMsg", msg
		GData.guiMgr.chat.chatMsg(msg)
		self.chartWordMsg(msg)
		
	def chartWordMsg(self, msg) :
		print "chartWordMsg", msg
		GData.guiMgr.wordMsg.addMsg("系统 " + msg)
		
	def canTransport(self) :
		return True
		if not GData.targetLocked or not GData.targetLocked.isTransport():
			self.chatClientMsg("没有传送阵，不能进行传送")
			return False
		else :
			return True
			

	def lockTarget(self, target) :
		print "lockTarget", target
		if GData.targetLocked == target :
			return
		GData.setTargetLocked(target)
		self.onTargetChange(target)
		
	def onTargetChange(self, target) :
		GData.guiMgr.targetInfo.updateInfo(target)
		GData.guiMgr.talkForm.clear()
	
	def handleKeyEvent(self, key, isDown) :
		if not isDown :
			if key == Define.VK_ESCAPE :
				if GData.targetLocked :
					self.lockTarget(None)
					return True
		return False
		
	def moveToTarget(self) :
		if GData.targetLocked :
			self.physics.searchPath(GData.targetLocked.position)
				
	def onMoveToEntityFinish(self) :
		if GData.targetLocked :
			if GData.targetLocked.distToPlayer() > Lazy.getMaxDistToNpc() :
				self.chatClientMsg("距离过远")
			else :
				CallBack.callBack(0.1, GData.targetLocked.onTargetActive)
			
	###############################################
	#任务相关
	###############################################
	def acceptTask(self, no) :
		print "acceptTask", no
		self.taskMgr.acceptTask(no)
		self.onTaskAccept(no)
		
		if not d_task.isNeedSubmit(no) :
			self.competeTask(no)
		
	def abandonTask(self, no) :
		print "abandonTask", no
		self.taskMgr.abandonTask(no)
		self.onTaskAbandon(no)
		
	def competeTask(self, no) :
		print "competeTask", no
		self.taskMgr.competeTask(no)
		self.addExp(self.needExpToLevel())
		self.onTaskCompete(no)
	
	def isTaskAccepted(self, no) :
		return self.taskMgr.isAccepted(no)
		
	def isTaskCompeted(self, no):
		return self.taskMgr.isCompeted(no)
		
	def canTaskAccept(self, no) :
		return self.taskMgr.canAccept(no)
		
	def canTaskCompete(self, no) :
		return self.taskMgr.canCompete(no)
		
	#是否是任务物品
	def isTaskObject(self, objNo) :
		return self.taskMgr.isTaskObject(objNo)
		
	#添加任务物品
	def addTaskObject(self, objNo, amount) :
		self.chatClientMsg("获得任务物品[%d]%d个" % (objNo, amount))
		self.taskMgr.addObjectStock(objNo, amount)
		
		GData.guiMgr.taskTrace.updateData()
		
	def onLevelUp(self, old) :
		print "onLevelUp", old, self.level
		GData.guiMgr.playerInfo.updateInfo(self)
		GData.monsterMgr.updateLevel(self.level + 1)
		self.chatClientMsg("恭喜您，已经升到%d级啦" % self.level)
		
	def onTaskAccept(self, taskNo) :
		self.chatClientMsg("接受任务%s" % d_task.getTaskName(taskNo))
		#self.execTaskTragger(taskNo, d_task.TRIGGER_ACCEPT)
		
		GData.guiMgr.taskTrace.updateData()
		
	def onTaskAbandon(self, taskNo) :
		self.chatClientMsg("放弃任务%s" % d_task.getTaskName(taskNo))
		
		GData.guiMgr.taskTrace.updateData()
		
	def onTaskCompete(self, taskNo) :
		self.chatClientMsg("完成任务%s" % d_task.getTaskName(taskNo))
		#self.execTaskTragger(taskNo, d_task.TRIGGER_COMPETE)
		
		GData.guiMgr.taskTrace.updateData()
		
	###############################################
	#攻击
	###############################################
	def canAttack(self, target, showMsg=True) :
		if not target :
			showMsg and self.chatClientMsg("目标无效")
			return False
		if not target.isAlive() :
			showMsg and self.chatClientMsg("目标已死亡")
			return False
		if target.isFriend() :
			showMsg and self.chatClientMsg("目标非敌方")
			return False
		if target.distToPlayer()>Lazy.getMaxDistToNpc():
			showMsg and self.chatClientMsg("目标距离过远")
			return False
		return True
		
	def attack(self) :
		print "player attack", GData.targetLocked
		if not GData.targetLocked :
			return
		self.physics.attack(GData.targetLocked.id)
			
	def onDead(self) :
		pass
		#GData.gameState = Define.GS_GAME_OVER
		
	def onHit(self, otherID, demage) :
		#print "player was hit", otherID, demage, self.hp
		GData.guiMgr.playerInfo.updateInfo(self)
		target = GData.entityMgr.entity(otherID)
		if target :
			self.chartWordMsg("%s对你攻击造成%d点伤害" % (target.name, demage))
		
	#攻击
	def onAttack(self, otherID, demage) :
		#print "onAttack", otherID, demage
		target = GData.entityMgr.entity(otherID)
		if target :
			self.chartWordMsg("你对%s攻击造成%d点伤害" % (target.name, demage))
			
	def onExpChange(self, exp) :
		self.chatClientMsg("获得经验%d" % exp )
		