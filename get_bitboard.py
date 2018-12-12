#!/usr/bin/env python3

import sys
import math

def index_to_hex(index_list):
    args = [2**int(i) for i in index_list]
    return hex(sum(args))

def knight_collisions():
    square_list = []
    for i in range(64):
        index_list = []
        #(1,2)
        if ((i+17) <= 63 and ((i+17)%8) - (i%8) == 1):
            index_list.append(i+17)
        #(2,1)
        if ((i+10) <= 63 and ((i+10)%8) - (i%8) == 2):
            index_list.append(i+10)
        #(2,-1)
        if ((i-6) >= 0 and ((i-6)%8) - (i%8) == 2):
            index_list.append(i-6)
        #(1,-2)
        if ((i-15) >= 0 and ((i-15)%8) - (i%8) == 1):
            index_list.append(i-15)
        #(-1,-2)
        if ((i-17) >= 0 and (i%8) - ((i-17)%8) == 1):
            index_list.append(i-17)
        #(-2,-1)
        if ((i-10) >= 0 and (i%8) - ((i-10)%8) == 2):
            index_list.append(i-10)
        #(-2,1)
        if ((i+6) <= 63 and (i%8) - ((i+6)%8) == 2):
            index_list.append(i+6)
        #(-1,2)
        if ((i+15) <= 63 and (i%8) - ((i+15)%8) == 1):
            index_list.append(i+15)
        square_list.append(index_to_hex(index_list))
    return square_list

def bishop_collisions():
    square_list = []
    for i in range(64):
        index_list = []
        #Northeast
        x = 1
        y = 8
        while ((i+x)%8 > i%8 and math.floor((i+y)/8) <= 7):
            index_list.append(i+x+y)
            x += 1
            y += 8

        #Southeast
        x = 1
        y = -8
        while ((i+x)%8 > i%8 and math.floor((i+y)/8) >= 0):
            index_list.append(i+x+y)
            x += 1
            y -= 8

        #Southwest
        x = -1
        y = -8
        while ((i+x)%8 < i%8 and math.floor((i+y)/8) >= 0):
            index_list.append(i+x+y)
            x -= 1
            y -= 8

        #Northwest
        x = -1
        y = 8
        while ((i+x)%8 < i%8 and math.floor((i+y)/8) <= 7):
            index_list.append(i+x+y)
            x -= 1
            y += 8
        square_list.append(index_to_hex(index_list))
    return square_list

def rook_collisions():
    square_list = []
    for i in range(64):
        index_list = []
        #North
        y = 8
        while (math.floor((i+y)/8) <= 7):
            index_list.append(i+y)
            y += 8
        #South
        y = -8
        while (math.floor((i+y)/8) >= 0):
            index_list.append(i+y)
            y -= 8
        #East
        x = 1
        while (((i+x)%8) > i%8):
            index_list.append(i+x)
            x += 1
        #West
        x = -1
        while (((i+x)%8) < i%8):
            index_list.append(i+x)
            x -= 1
        square_list.append(index_to_hex(index_list))
    return square_list

def print_array(array):
    for a in array:
        print(a)
