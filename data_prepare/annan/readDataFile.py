# Do not print anything here...
# Run (F5) this code before importing any of its functions to other codes.
import math

def writeData(data, labels, filename):
    ff= open(filename, "w")
    for index in data:
        ff.write(str(index) + "\t")
        ff.write(str(labels[index]) + "\t")
        for item in data[index]:
            ff.write(str(item) + ":" + str(data[index][item]) + " ")
        ff.write("\n")
    ff.close()
##end of writeData

def writeDataAndLabelsSeparately(data, labels, dataFileName, writeIDs = True):
    #return [len(data), maxID, itemcount ]
    maxID = 0
    itemcount = 0
    ff= open(dataFileName, "w")
    ll = open(dataFileName + ".rclass", "w")
    for index in data:
        if writeIDs:
            ff.write(str(index) + " ")
            ll.write(str(index) + " ")
        ll.write(str(labels[index]) + "\n")
        for item in data[index]:
            ff.write(str(item) + ":" + str(data[index][item]) + " ")
            if maxID < int(item):
                maxID = int(item)
            itemcount += 1
        ff.write("\n")
    ff.close()
    ll.close()
    lines = file2lines(dataFileName)
    ff = open(dataFileName, "w")
    ff.write(str(len(data)) + " " + str(maxID) + " " + str(itemcount) + "\n")
    for line in lines:
        ff.write(line)
        ff.write("\n")
    ff.close()
##end of writeDataAndLabelsSeparately

def file2lines(filename):
    ff = open(filename, "r")
    lines = ff.readlines()
    ff.close()
    return lines
##end of file2lines

def readFromMyFileWithTfidf(myFileName, labelFileName):
    ff= open(myFileName, "r")
    lines = ff.readlines()
    ff.close()
    data = []
    paras = lines[0].split()
    docNum = int(paras[0])
    termNum = int(paras[1])
    docFrequency = {}
    for lineIndex in range(0,docNum):
        items = lines[lineIndex+1].split()
        dataForLine = {}
        itemNum = len(items)
        for itemIndex in range(0,itemNum,2):
            key = int(items[itemIndex])
            value = float(items[itemIndex+1])
            dataForLine[key] = value
            if docFrequency.has_key(key):
                docFrequency[key] = docFrequency.get(key)+1
            else:
                docFrequency[key] = 1
        data.append(dataForLine)
    docNumF = float(docNum)
    for docIndex in range(docNum):
        for key in data[docIndex].keys():
            data[docIndex][key] = data[docIndex][key]*math.log(docNumF/docFrequency[key])

    ##read labels

    ff = open(labelFileName,"r")
    lines = ff.readlines()
    ff.close()
    levelNum = int(lines[0])
    labels = {}
    sampleNum = len(lines)-2

    for lineIndex in range(sampleNum):
        line=lines[lineIndex+2]
        items = line.split()
        docIndex = int(items[0])
        value = items[1:]
        value.reverse()
        labels[docIndex] = value
    return [data, labels, termNum, levelNum]
##end of readFromMyfile


