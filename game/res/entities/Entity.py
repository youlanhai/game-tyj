#-*- coding: utf-8 -*-

import random

import iData
import Lazy
import Define
import Physics
import Topboard

from data import d_model
from data import d_npc
from data import d_action


from GData import GData

##################################################
#实体基类
##################################################
class Entity(Lazy.iEntity, iData.iData) :
	@staticmethod
	def isEntity() : return True
	
	@staticmethod
	def isNpc() : return False
	
	@staticmethod
	def isPlayer() : return False
		
	@staticmethod
	def isTransport() : return False
		
	@staticmethod
	def isMonster() : return False
	
	#设置物理
	def setPhysics(self, p) :
		super(Entity, self).setPhysics(p)
		self.__physics = p
	def getPhysics(self): return self.__physics
	physics = property(getPhysics, setPhysics)
	
	#设置头顶板
	def setTopboard(self, board) :
		super(Entity, self).setTopboard(board)
		self.__topboard = board
	def getTopboard(self) : return self.__topboard
	topboard = property(getTopboard, setTopboard)
	
	
	#构造方法
	def __init__(self) :
		super(Entity, self).__init__()
		iData.iData.__init__(self)
		
		#python管理的对象，需要python持一个引用计数
		self.__physics = None
		self.__topboard = None
		
		self.enableLockHeight(True)
		self.setLockHeight(2)	
		self.showDistance = 3000 #显示距离
		self.eno = 0
		self.name = "测试呀"
		self.createTopBoard()
		self.createPhysics()
	
	#设置实体编号
	def setENO(self, eno) :
		self.eno = eno
		self.updateData()
	
	#设置任务名称
	def setName(self, name) :
		self.name = name
		self.refreshTopBoard()
	
	#创建物理
	def createPhysics(self) :
		self.physics = Physics.Physics(self)
	
	#显示实体
	def show(self, show_) :
		super(Entity, self).show(show_)
#		self.refreshTopBoard()

	def enterWorld(self) :
		pass
		
	#离开世界
	def leaveWorld(self) :
		if GData.targetLocked == self :
			GData.player.lockTarget(None)
		self.show(False)
		self.physics = None
		self.topboard = None
		self.model = None
		
	
	#获得随机位置
	def genRandomPos(self) :
		return Lazy.Vector3(random.uniform(-500, 500), 0, random.uniform(-500, 500))
	
	#根据eno更新数据。
	def updateData(self) :
		self.position = self.genRandomPos()
		self.setName(d_npc.getName(self.eno))
		self.loadModel(d_npc.getModelID(self.eno))
		self.updateShow()
	
	def setPosition(self, x, y, z) :
		self.position = Lazy.Vector3(x, y, z)
	#########################################################
	#玩家与NPC交互
	#########################################################
	
	#【引擎回调】是否可鼠标选择。返回false，将不可接受鼠标消息
	def canSelect(self) :
		return self.isAlive()
	
	def canShow(self):
		dist = self.distToPlayer()
		
		if dist > self.showDistance + 100: return False
		if dist <= self.showDistance: return True
		
		return True
	
	def updateShow(self):
		show = self.canShow()
		self.show(show)
		
		if not show and self == GData.targetLocked:
			GData.player.lockTarget(None)
		
	#【引擎回调】收到鼠标消息。
	def onFocusCursor(self, msg) :
		if msg == Define.CM_LUP :
			self.targetSelect()
		elif msg == Define.CM_RUP :
			self.targetActive()

	#选择目标
	def targetSelect(self) :
		GData.player.lockTarget(self)
		self.onTargetSelect()
	
	#激活目标
	def targetActive(self) :
		GData.player.lockTarget(self)
		if self.isInRange() :
			self.onTargetActive()
		else :
			self.onTargetTooFar()
	
	#目标被选择
	def onTargetSelect(self) :
		Lazy.debug("i was selected!")
		
	#是否在玩家活动范围
	def isInRange(self) :
		return self.distToPlayer() <= Lazy.getMaxDistToNpc()
	
	#目标过远
	def onTargetTooFar(self) :
		GData.player.moveToTarget()
	
	#目标被激活
	def onTargetActive(self) :
		Lazy.debug("i was hited!")
		
	#########################################################
	#头顶板
	#########################################################
	#创建头顶板
	def createTopBoard(self) :
		self.topboard = Topboard.Topboard(self)
		if not self.topboard.create(self.name, "font#22#1", 0xff0000ff, 10) :
			print "error create topboard failed."
		self.topboard.setMaxShowDistance(5000)
		
	#重置头顶板
	def resetTopboard(self) :
		if not self.topboard :
			return
		if self.model :
			r = self.model.getBBRadius()
			self.topboard.setBiasHeight(2*r)
	
	#刷新头顶板内容
	def refreshTopBoard(self) :
		if not self.topboard :
			return
		self.topboard.text = self.name
		self.topboard.show(self.visible())
	
	#加载模型
	def loadModel(self, id) :
		if not d_model.exist(id):
			return
		self.modelID = id
		self.model = Lazy.loadModel(d_model.getPath(id), d_model.getType(id))
		if not self.model:
			print "model not found", id, self
			return
		self.model.scale = d_model.getScale(id)
		self.model.yaw = d_model.getYaw(id)
		self.model.pitch = d_model.getPitch(id)
		self.model.roll = d_model.getRoll(id)
		self.onModelLoaded()
	
	#模型被加载
	def onModelLoaded(self):
		self.resetTopboard()
		if d_action.exist(self.modelID) :
			self.model.playAction(d_action.free(self.modelID), True)
	
	#播放动作
	def playAction(self, actionID):
		if self.model and d_action.exist(self.modelID) :
			self.model.playAction(d_action.action(self.modelID, actionID), d_action.needLoop(actionID))
	
	#切换显示
	def toggle(self) :
		self.show(not self.visible())
	
	#与玩家距离
	def distToPlayer(self) :
		if not GData.player: return 0
		return self.position.distTo(GData.player.position)
		
	#########################################################
	#升级
	#########################################################
	def onLevelUp(self, old) :
		print "onLevelUp", old, self.level
	
	#死亡回调
	def onDead(self) :
		if self.physics :
			self.physics.onAITrigger()
		