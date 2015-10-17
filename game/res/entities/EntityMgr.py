#-*- coding: utf-8 -*-

import copy
import traceback
import Lazy
import CallBack

from data import d_task
from data import d_monster
from data import d_map
from GData import GData

def loadEntityClass() :
	module =[ "Boss", "CursorEntity", "Transport", "Monster", "Npc", "Player", "TaskNpc"]
	for name in module :
		m = __import__(name)
		GData.regEntityClass(name, getattr(m, name))

##################################################
#
##################################################
class EntityMgr(Lazy.cEntityMgr) :
	
	@property
	def player(self) :
		return self._player
		
	def __init__(self) :
		super(EntityMgr, self).__init__()
		self._player = None
		self.entityPool = []
		self.entityCache = {} #用于记录被动态改变的entity。
		self.mapNo = 0
		self.activeEntity = None
		self.cursorInTerrainPos = None
		loadEntityClass()
		
		self._doUpdateShow()
		
	def add(self, entity) :
		print "add endity", entity
		if entity not in self.entityPool :
			self.entityPool.append(entity)
			super(EntityMgr, self).add(entity)
			entity.enterWorld()
		else:
			 print "error the entity has found in pool", entity
			
	def remove(self, entity) :
		print "remove endity", entity
		if not entity :
			return
		super(EntityMgr, self).remove(entity)
		try :
			self.entityPool.remove(entity)
		except ValueError:
			pass
		entity.leaveWorld()
		GData.monsterMgr.onRemove(entity)
	
	def clear(self) :
		self.activeEntity = None
		self.cursorInTerrainPos = None
		tempPool = copy.copy(self.entityPool)
		for ent in tempPool :
			if ent != self.player :
				self.remove(ent)

	def setPlayer(self, player) :
		if player == self.player :
			return
		super(EntityMgr, self).setPlayer(player)
		GData.setPlayer(player)
		if player :
			self._player = player
			self.add(player)
		else :
			self.remove(self.player)
			self._player = None
		
	def createEntity(self, classname, eno) :
		entityClass = GData.getEntityClass(classname)
		try:
			ent = entityClass()
		except :
			print traceback.print_exc()
			return 0
		ent.setENO(eno)
		self.add(ent)
		return ent.id
		
	def createEntities(self, mapNo) :
		print "createEntities start", mapNo
		self.mapNo = mapNo
		self.createAllNpc(mapNo)
		self.createAllTransport(mapNo)
		self.createAllBoss(mapNo)
		GData.monsterMgr.loadMonsterCenter(mapNo)
		GData.cursorEntID = self.createEntity("CursorEntity", 1003)
		print "createEntities over"
		
	def createAllNpc(self, mapNo) :
		npcs = d_task.getMapNpc(mapNo)
		for no in npcs :
			self.createEntity("TaskNpc", no)
	
	def createAllTransport(self, mapNo) :
		npcs = d_map.getMapTransport(mapNo)
		for no in npcs :
			self.createEntity("Transport", no)
			
	def createAllBoss(self, mapNo) :
		boss = d_monster.getMapBoss(mapNo)
		for no in boss :
			self.createEntity("Boss", no)
	
	def clearEntities(self) :
		self.clear()
	
	def findEntityByEno(self, eno) :
		for ent in self.entityPool :
			if ent.eno == eno : return ent
		
		return None
	
	def updateShowState(self):
		for ent in self.entityPool :
			ent.updateShow()
	
	def _doUpdateShow(self):
		
		try:
			self.updateShowState()
		except:
			print traceback.print_exc()
		
		CallBack.callBack(1, self._doUpdateShow)
