#-*- coding: utf-8 -*-
#npc表

def exist(no): return no in sheet
def getName(no) : return sheet[no]["name"]
def getModelID(no): return sheet[no]["model"]

#npc编号从1001开始
sheet = {
    1:{"name":"游蓝海", "model": 1},
    1001:{"name":"法师", "model":1001 },
    1002:{"name":"传送阵", "model":1002},
    1003:{"name":"鼠标", "model":1003},
    1004:{"name":"恶魔法师", "model": 1001}
}