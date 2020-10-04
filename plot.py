import matplotlib.pyplot as plt

small = "small"
medium = "medium"
large = "large"
txt = ".txt"

smallR = []
mediumR = []
largeR = []

contextS = []
contextM = []
contextL = []

for i in range(1, 31):
    f = open("output_" + small + str(i) + txt, 'r')
    for line in f:
        s = line.split()
        if(s[0] == "Numero"):
            contextS.append(int(s[-1]))
        else:
            ultrapassou = int(s[1]) - int(s[2])
            smallR.append(ultrapassou)

for i in range(1, 31):
    f = open("output_" + medium + str(i) + txt, 'r')
    for line in f:
        s = line.split()
        if(s[0] == "Numero"):
            contextM.append(int(s[-1]))
        else:
            ultrapassou = int(s[1]) - int(s[2])
            mediumR.append(ultrapassou)

for i in range(1, 31):
    f = open("output_" + large + str(i) + txt, 'r')
    for line in f:
        s = line.split()
        if(s[0] == "Numero"):
            contextL.append(int(s[-1]))
        else:
            ultrapassou = int(s[1]) - int(s[2])
            largeR.append(ultrapassou)

totalS = 0
totalM = 0
totalL = 0
for num in smallR:
    if num > 0:
        totalS += 1

for num in mediumR:
    if num > 0:
        totalM += 1

for num in largeR:
    if num > 0:
        totalL += 1


fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_title("Porcetagem de processos atrasados")
ax.set_ylabel("Porcentagem")
ax.set_xlabel("Tamanho da entrada")
valueS = 100*totalS/len(smallR)
valueM = 100*totalM/len(mediumR)
valueL = 100*totalL/len(largeR)
values = [valueS, valueM, valueL]
classes = [small, medium, large]
ax.bar(classes, values)
plt.show()