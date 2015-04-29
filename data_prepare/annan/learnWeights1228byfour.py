"""
load data file
make feature array (with weights, implemented as dictionary: {int:str)
#---load feature file
#---load label file
get number of features (M)
assign weights to all features: initialize W = {featID:weight}

for each sample i:
    for each sample j:
        for each sample k:
            for each sample t:
                if sim(i,j) > sim(k,t) and labelsim(i,j) < labelSim(k,t):
                Update Weights
"""

from readDataFile import readFromMyFileWithTfidf as readFromMyFileWithTfidf
from random import randrange as randrange
import time
import math
import profile
import sys

data = []   #data as comes in the file.ugly.
feats = {}  #features and their weights.cool
labels = {}
pilotData = {}  #randomly selected samlpes.only sample IDs.short.cool.
pilotLabel = {}    #labels ofrandomly selected samles.short.-->not used

def swap(array, i, j):
    z = array[i]
    array[i] = array[j]
    array[j] = z
##end of swap
   
#--scalar product
def scalarProduct(a, b):
    res = 0.0
    for i in a.keys():
        if b.has_key(i):
            res += a[i] * b[i]
    return res
##end of scalarProduct(a,b)

def sim(x,y, power = 1):
    global data
    global feats
    newx = {}
    newy = {}
    xnorm = 0.0
    ynorm = 0.0
    for key in data[x].keys():
        newx[key] = data[x][key] * feats[key]
        xnorm += newx[key]*newx[key]
    for key in data[y].keys():
        newy[key] = data[y][key] * feats[key]
        ynorm = newy[key]*newy[key]
    sp = scalarProduct(newx, newy)/math.pow(xnorm*ynorm,0.5)
    return sp
##end of sim(x,y)

def Lsim(label1,label2):
    lx = label1.split(".")
    ly = label2.split(".")
    res = 0
    for i in range(min(len(ly),len(lx))):
        if lx[i] == ly[i]:
            res +=1
        else:
            return res
    return res
##end of Lsim

def LsimReuters(label1,label2):
    lx = len(label1)
    ly = len(label2)
    res = 0
    for i in range(min(lx, ly)):
        if label1[i] == label2[i]:
            res +=1
        else:
            return res
    return res
##end of LsimReuters


def updateWeights(X, Y, nu = 0.015):
    global feats
    global data
    for key in data[X].keys():
        if key in data[Y]:
            feats[key] += feats[key] * nu * data[X][key] * data[Y][key]
            
##end of updateWeights

def learnWPilot(nu = 0.015, thirdParty = -1, iterations = 1):
    countx = 0
    county = 0
    countz = 0
    countt = 0
    updatecount = 0
    global pilotData
    global labels
    print 'Counting to:',len(pilotData),
    for iteration in range(iterations):
        print "\niter", iteration + 1,":","\n"
        countx = 0
        for X in  pilotData:
            countx += 1
            #current = time.time()
            print countx,"x:\n"
            county = 0
            for Y in pilotData:
                county += 1
                countz = 0
                for Z in pilotData:
                    countz += 1
                    for T in pilotData:
                        countt += 1
                        if X==Z or X==Y or Z==Y or X==T or Y==T or Z==T:
                            continue
                    ##--use dataLines and "feats" to calculate sim and Lsim:
                        if LsimReuters(labels[X], labels[Y]) < LsimReuters(labels[Z], labels[T]) and sim(X,Y) > sim(Z, T):
                        ##--update weights: update "feats" variable.
                            updateWeights(X, Y, nu)
                            updateWeights(Z, T,  thirdParty * nu)
                            updatecount += 1
                countz = 0
            county = 0
    ##--end of for, X in pilotData
    return updatecount
##end of learnpWPilot

def saveWeights(weightFile, levelNum):
    global feats
    ww = open(weightFile, "w")
    featureNum = len(feats)
    ww.write(str(featureNum-1)+" "+str(levelNum) + "\n")
    for level in range(levelNum):
        ww.write("\n")
    
    for key in range(1,featureNum):
        ww.write(str(feats[key]) + " ")
    ww.write("\n")
    ww.close()
##end of saveWeights

def mainRun(params):
  ##--Setup:
    dataFileName =params['dataFileName']# "selectedSamples.txt"
    labelFileName=params['labelFileName']
    nu = params['nu']#0.015
    weightfile = params['weightfile']#"featureWeights"
    iterations = params['iterations']
    thirdParty = params['thirdParty']
  ##--end of Setup

  ##--get data:
    global data
    global pilotData
    global labels
    global feats
    [data,labels,termNum,levelNum] = readFromMyFileWithTfidf(dataFileName,labelFileName)

  ##--get Features and initialize weights
    feats = []
    feats.append(1.0)#blank feat for zero
    for i in range(termNum):
        feats.append(1.0)

  ##--get Features and initialize weights
    pilotData = labels.keys()
    
  ##--Learning:
    updatecount = learnWPilot(nu, thirdParty, iterations)

  ##--record weights:
    saveWeights(weightfile,levelNum)
##end of main

def main():
#"nu" is learning rate.
#"thirdParty" is in  -1..+1 depending on what to do with the third sample.
#####       Must be -1 by default in the project.
    args= sys.argv
    print args[0]+" "+args[1]+" "+args[2]+" "+args[3]+"\n"
    if(len(args)!=4):
        print "learnWeight <dataFile> <labelMarked> <weightPosition>"
    params = {} 
    params['dataFileName'] = args[1]
    params['labelFileName'] = args[2]
    params['nu']           = 0.015
    params['weightfile']   = args[3]
    params['thirdParty']   = -1
    params['iterations']   = 1#int(args[-1])
    t = time.time()    
    mainRun(params)
    print time.time() - t
##end of main


#main()
profile.run('main()')

