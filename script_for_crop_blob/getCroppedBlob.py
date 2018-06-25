#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Mon Jun 25 21:17:24 2018

@author: spratik
"""
import os

path = "/home/spratik/Computer-Vision/inputImage/licence_sample_image"
output = "./output1/"

for root, dir, files in os.walk(path):
    #print root
    #print dir
    for name in files:
        imageName = name.split('.');
        croppedImageOutputPath = os.path.join(output,imageName[0])
        inputImagePath = os.path.join(root,name)
        #print croppedImageOutputPath
        cmd = './BlobDetection ' + inputImagePath + ' ' + croppedImageOutputPath
        #print cmd
        os.system(cmd)
    