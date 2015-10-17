#-*- coding: utf-8 -*-

DEFAULT_MUSIC = "res/music/desert.mp3"

def getMapName(no) : return sheet[no]["name"]
def getMapMusic(no) : return sheet[no].get("music", DEFAULT_MUSIC)
def getMapSky(no) : return sheet[no]["sky"]
def getAmbintColor(no) : return sheet[no]["ambient"]
def getLightDir(no) : return sheet[no]["ldir"]
def getLightColor(no) : return sheet[no]["lcolor"]
def getFogColor(no) : return sheet[no]["fog"]

def getMapPath(no) :
	name = getMapName(no)
	if not name :
		return ""
	else :
		return "res/map/%s/map.lzd" % name

#获得一个地图上的传送阵
def getMapTransport(no) :
	lst = []
	for k,v in transport.iteritems():
		if v["position"][0] == no :
			lst.append(k)
	return lst
		
#获得所有传送阵
def getTransports() : return transport.keys()
		
def getTransMapNO(no) :
	d = transport.get(no, None)
	if d : return d[0]
	else : return 0
		
def getTransPosition(no) :
	d = transport[no].get("position", None)
	if d : return d[1]
	else : return None
		
def getTransTask(no) : return transport[no].get("task", None)
def getTransLevel(no) : return transport[no].get("lv", 0)



MAP_NAME = {
	1: "test",
	1001: "kunqu",
	1002: "qingshan",
	1003: "jiuyuan",
	1004: "donghe",
	1005: "tumote",
	1006: "shiguai",
	1007: "daerhan",
	1008: "guyang",
	1009: "baiyunebo",
}

sheet = {
	1: {"name":"test", "music":"res/music/desert.mp3", "sky":"res/gui/sky/凌晨/%d.jpg", "ambient":0xffcccccc, "ldir":(-1, -1, 0), "lcolor":(1.0, 0.8, 0.8, 0.8), "fog":0xffe0e0e0},
	1001: {"name":"kunqu","music":"res/music/happy.mp3","sky":"res/gui/sky/太阳初升/%d.jpg", "ambient":0xffffffff, "ldir":(-0.1, -0.2, -1), "lcolor":(1.0, 1.0, 1.0, 1.0), "fog":0xfff1f1c3},
	1002: {"name":"qingshan","music":"res/music/p38.mp3","sky":"res/gui/sky/太阳初升/%d.jpg", "ambient":0xff8d8d8d, "ldir":(0.4, -0.6, 0.7), "lcolor":(1.0, 0.87, 0.87, 0.5), "fog":0xffbababa},
	1003: {"name":"jiuyuan","music":"res/music/p43.mp3","sky":"res/gui/sky/多云/%d.jpg", "ambient":0xffafafaf, "ldir":(0, -1, 0), "lcolor":(1.0, 0.7, 0.7, 0.7), "fog":0xffa0abbb},
	1004: {"name":"donghe","music":"res/music/23.mp3","sky":"res/gui/sky/多云/%d.jpg", "ambient":0xffafafaf, "ldir":(0, -1, 0), "lcolor":(1.0, 0.7, 0.7, 0.7), "fog":0xffa0abbb},
	1005: {"name":"tumote","music":"res/music/p20.mp3","sky":"res/gui/sky/雨天/%d.jpg", "ambient":0xff8b8b8b, "ldir":(0, -1, 0), "lcolor":(1.0, 0.51, 0.51, 0.51), "fog":0xff161616},
	1006: {"name":"shiguai","music":"res/music/desert.mp3","sky":"res/gui/sky/雨天/%d.jpg", "ambient":0xff8b8b8b, "ldir":(0, -1, 0), "lcolor":(1.0, 0.51, 0.51, 0.51), "fog":0xff161616},
	1007: {"name":"daerhan","music":"res/music/p47.mp3","sky":"res/gui/sky/傍晚/%d.jpg", "ambient":0xff9e8766, "ldir":(0, -1, 0), "lcolor":(1.0, 0.84, 0.71, 0.61), "fog":0xffffaf49},
	1008: {"name":"guyang","music":"res/music/p04.mp3","sky":"res/gui/sky/傍晚/%d.jpg", "ambient":0xff6c6c6c, "ldir":(0, -1, 0), "lcolor":(1.0, 1.0, 0.9, 0.8), "fog":0xff383838},
	1009: {"name":"baiyunebo","music":"res/music/P39.mp3","sky":"res/gui/sky/太空/%d.jpg", "ambient":0xff1b1b1b, "ldir":(0.1, -0.2, -1), "lcolor":(1.0, 0.78, 0.78, 0.78), "fog":0xff0f0f0f},
}


transport = {
	0 : {"position":(1, (0, 0, 0)), "lv": 1, "task":(4,) },
	10 : {"position":(1, (-5000, 0, 0)),  "task":(4,), "lv": 1},
	1 : {"position":(1001, (0, 0, 0)), "lv": 1},
	2 : {"position":(1002, (0, 0, 0)), "lv": 1},
	3 : {"position":(1003, (0, 0, 0)), "lv": 1},
	4 : {"position":(1004, (0, 0, 0)), "lv": 1},
	5 : {"position":(1005, (0, 0, 0)), "lv": 1},
	6 : {"position":(1006, (0, 0, 0)), "lv": 1},
	7 : {"position":(1007, (0, 0, 0)), "lv": 1},
	8 : {"position":(1008, (0, 0, 0)), "lv": 1},
	9 : {"position":(1009, (0, 0, 0)), "lv": 1},
	11 : {"position":(1001, (-5000, 0, 0)), "lv": 1},
	12 : {"position":(1002, (-5000, 0, 0)), "lv": 1},
	13 : {"position":(1003, (-5000, 0, 0)), "lv": 1},
	14 : {"position":(1004, (-5000, 0, 0)), "lv": 1},
	15 : {"position":(1005, (-5000, 0, 0)), "lv": 1},
	16 : {"position":(1006, (-5000, 0, 0)), "lv": 1},
	17 : {"position":(1007, (-5000, 0, 0)), "lv": 1},
	18 : {"position":(1008, (-5000, 0, 0)), "lv": 1},
	19 : {"position":(1009, (-5000, 0, 0)), "lv": 1},
}