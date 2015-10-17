#-*- coding: utf-8 -*-
#模型表

def exist(no) : return no in sheet
def getPath(no) : return sheet[no]["path"]
def getType(no) : return sheet[no]["type"]
def getScale(no) : return sheet[no].get("scale", 1.0)
def getYaw(no) : return sheet[no].get("yaw", 0)
def getPitch(no) : return sheet[no].get("pitch", 0)
def getRoll(no) : return sheet[no].get("roll", 0)

#玩家从1开始。其他：从1001开始
#注意在d_action表中添加动作。
sheet = {
	1: {"path":"res/model/jingtian/jingtian.x", "type":"skinmesh", "scale" : 1.0, "yaw":3.14},
	1001: {"path":"res/model/法师.x", "type":"skinmesh", "scale" : 1.0,"yaw":-1.57 },
	1002: {"path":"res/model/传送门.X", "type":"mesh", "scale" : 0.8,"yaw":0.0 },
	1003: {"path":"res/model/箭头.x", "type":"mesh", "scale":50.0, "yaw":0.0, "pitch":-1.57},
	1004: {"path":"res/model/恶魔猎手.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1005: {"path":"res/model/弓箭手.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1006: {"path":"res/model/狼.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1007: {"path":"res/model/农民.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1008: {"path":"res/model/兽王.x", "type":"skinmesh", "scale" : 2.0, "yaw":-1.57},
	1009: {"path":"res/model/巫妖.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1010: {"path":"res/model/小强.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1011: {"path":"res/model/月女.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1012: {"path":"res/model/憎恶.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1013: {"path":"res/model/蜘蛛.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
	1014: {"path":"res/model/小强.x", "type":"skinmesh", "scale" : 1.0, "yaw":-1.57},
}
