# python code to parse the CO2 and temperature data in the logs of the
# CO2 sensor in a freezer experiment

path = "...\\PH250\\Final CO2 project\\Freezer Test\\LOGS\\LOG"
logMax = 342

temp = []
time = []
freezCO2 = []
start = 0
CO2err = []

l = 0
t = 0
for l in range(logMax):
    if(l<10):
        num = "00"
    elif(l<100):
        num = "0"
    else:
        num = ""
    num = num + str(l) + ".txt"
    file = open(path+num,'r').readlines()
    for line in file:
        part = line.split(' ')[1] # this is specific for the file's format
        dat = float(part.split(',')[2])
        if (dat < 1500 and dat > 0): # exclude data glitches
            freezCO2.append(dat)
            CO2err.append((dat * 0.05)+50) # the CO2 sensor's stated error is 5% of the reading plus 50ppm
            temp.append(float(part.split(',')[1]))
            if(start == 0):
                start = convertTime(part.split(',')[0])
            time.append((t+start)/3600) #convert time to hours format
            t = t + 0.5
    l = l+1

plt.plot(time, temp)
plt.title("Temperature of CO2 sensor in standard freezer")
plt.xlabel("Hour of July 6")
plt.ylabel("Temperature ($\degree$C)")
plt.show()