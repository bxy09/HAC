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
                if sim(i,j) > sim(i,k) and labelsim(i,j) < labelSim(i,k):
                Update Weights
"""

from readDataFile import file2lines as file2lines
from random import randrange as randrange
import time
import sys

data = []   #data as comes in the file.ugly.
feats = {}  #features and their weights.cool
dataLines = {}  #data, readable in this code.cool.
labels = {}
pilotData = {}  #randomly selected samlpes.only sample IDs.short.cool.
pilotLabel = {}    #labels ofrandomly selected samles.short.-->not used

def swap(array, i, j):
    z = array[i]
    array[i] = array[j]
    array[j] = z
##end of swap
    
def makeRandomArray(size, portion):
    ones = int(size * portion)
    res = [1] * ones + [0]*(size - ones)
    for i in range(size - 2):
        swap(res, i, randrange(i + 1,size))
    return res
##end of makeRandomArray

def getFeat(initW = 1):
    global feats
    global data
    for line in data:
        items = line.split()
        ID = items.pop(0)
        label = items.pop(0)
        for item in items:
            pair = item.split(":")
            if not feats.has_key(int(pair[0])):
                feats[int(pair[0])] = initW
    return len(feats)
##end of def getFeats

def randomlySelectData(portion = 0.01):
    global dataLines
    global pilotData
    global pilotLabel
    global data
    
    dataSize = len(dataLines)
    monitorArray = makeRandomArray(dataSize, portion)

    IDs = dataLines.keys()
    for i in range(dataSize):
        if randArray[i] == 1:
            pilotData[ IDs[i] ] = data[ IDs[i] ]
            pilotLabel[ IDs[i] ] = labels[ IDs[i] ]
##end of randomlySelectData

def reMakeData():
    global data
    global dataLines
    global labels
    for line in data:
        items = line.split()
        ID = items.pop(0)
        dataLines[ID] = {}
        label = items.pop(0)
        labels[ID] = label
        for item in items:
            pair = item.split(":")
            dataLines[ID][int(pair[0])] = float(pair[1])
##end of reMakeData

#--scalar product
def scalarProduct(a, b):
    res = 0.0
    for i in a.keys():
        if b.has_key(i):
            res += a[i] * b[i]
    return res
##end of scalarProduct(a,b)

def sim(x,y, power = 1):
    global dataLines
    global feats
    newx = {}
    newy = {}
    for key in dataLines[x].keys():
        newx[key] = dataLines[x][key] * feats[key]
    for key in dataLines[y].keys():
        newy[key] = dataLines[y][key] * feats[key]
    sp = scalarProduct(newx, newy)
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
    global dataLines
    for key in feats.keys():
        if (key in dataLines[X]) and (key in dataLines[Y]):
            feats[key] += feats[key] * nu * dataLines[X][key] * dataLines[Y][key]
##end of updateWeights

def learnWPilot(randomRun, nu = 0.015, thirdParty = -1, iterations = 1):
    countx = 0
    county = 0
    countz = 0
    updatecount = 0
    global pilotData
    global labels
    print 'Counting to:',len(pilotData),
    for iteration in range(iterations):
        print "\niter", iteration + 1,":",
        countx = 0
        for X in  pilotData:
            countx += 1
            current = time.time()
            print countx,
            county = 0
            for Y in pilotData:
                county += 1
                countz = 0
                for Z in pilotData:
                    countz += 1
                    if X==Z or X==Y or Z==Y:
                        continue
                  ##--use dataLines and "feats" to calculate sim and Lsim:
                    if sim(X,Y) > sim(X, Z) and LsimReuters(labels[X], labels[Y]) < LsimReuters(labels[X], labels[Z]):
                      ##--update weights: update "feats" variable.
                        updateWeights(X, Y, nu)
                        updateWeights(X, Z,  thirdParty * nu)
                        updatecount += 1
                countz = 0
            county = 0
    ##--end of for, X in pilotData
    return updatecount
##end of learnpWPilot

def saveWeights(weightFile, suffix):
    global feats
    ww = open(weightFile + "." + suffix , "w")
    for key in feats.keys():
        ww.write(str(key) + " " + str(feats[key]) + "\n")
    ww.close()
##end of saveWeights

def mainRun(params):
  ##--Setup:
    dataFileName =params['dataFileName']# "selectedSamples.txt"
    nu = params['nu']#0.015
    weightfile = params['weightfile']#"featureWeights"
    suffix = params['suffix']#"default"
    portion = params['portion']#0.1
    iterations = params['iterations']
    thirdParty = params['thirdParty']
    randomRuns = params['randomRuns']
  ##--end of Setup

  ##--get data:
    global data
    global pilotData
    data = file2lines(dataFileName)
  ##--make data good for this code:
    reMakeData() ##put the data from file into dictionary ADT

  ##--get Features and initialize weights
    getFeat()

  ##--get Features and initialize weights
    randomRun = 0
    dataSize = len(dataLines)
    IDs = dataLines.keys()
  ##randomRun:
    for randomRun in range(randomRuns):
        if randomRun > 9:
            suffix = 'p' + str(portion) + 'r' + str(randomRun)
        else:
            suffix = 'p' + str(portion) + 'r0' + str(randomRun)
        print suffix,
        pilotData = []
        randArray = makeRandomArray(dataSize , portion)
        for i in range(dataSize):
            if randArray[i] == 1:
                pilotData.append(IDs[i])

      ##--Learning:
        updatecount = learnWPilot(randomRun, nu, thirdParty, iterations)

      ##--record weights:
        saveWeights(weightfile, suffix)    
  ##end of randomRun
##end of main

def main(args):
#"nu" is learning rate.
#"thirdParty" is in  -1..+1 depending on what to do with the third sample.
#####       Must be -1 by default in the project.
    params = {} 
    params['dataFileName'] = "data1228labeled"#"dataFile" #"selectedSamples.txt"
    params['nu']           = 0.015
    params['weightfile']   = "featureWeightsByFour"
    params['suffix']       = "default"
    params['portion']      = 0.1
    params['thirdParty']   = -1
    params['iterations']   = 1#int(args[-1])
    params['randomRuns']   = 10
    t = time.time()
    for portion in [0.01,0.05,0.1]:
        params['portion'] = portion
        mainRun(params)
    print time.time() - t
##end of main


main(sys.argv)

