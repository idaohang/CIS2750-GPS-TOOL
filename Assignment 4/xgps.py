#***********
#CIS*2750 Assignment 3 W10
#xgps.py -- requires header files "gputil.h" and "gpstool.h within the same directory
#Last Modified: 19/3/2010
#Author: Kwok Lau 0595198
#Email: kwok@uoguelph.ca
#Description; This is a Python GUI created for the last two part of the assignment. Before it can be ran it needs to have a shared library as well as the approprirate functions 
#		found in gputil.c to translate its content to Python
#***********


#!/usr/bin/python
import sys
import tkMessageBox
import tkFileDialog
import tkColorChooser
import Tix
import string
import math
from Tkinter import *
from GMapData import *
import os
import Gps
import MySQLdb
import urllib
#from Gps import readFile

startWebServer()
root = Tix.Tk()
root.title("XGPS")
root.config(bg="#81a6ca", width=600, height=600)

if len(sys.argv) > 3:
	hostname = sys.argv[3]
	
else:
	hostname = "db.cis.uoguelph.ca"

try:
	testconn = MySQLdb.connect(host=hostname, user=sys.argv[1], passwd=sys.argv[2], db=sys.argv[1])
	testconn.close()

except MySQLdb.Error, e:
	print "Error %d: %s" % (e.args[0], e.args[1])
	root.quit()


db = MySQLdb.connect(host=hostname, user=sys.argv[1], passwd=sys.argv[2], db=sys.argv[1])
cursor = db.cursor()

cursor.execute ("""CREATE TABLE IF NOT EXISTS HIKE
(
	hikeno	INT AUTO_INCREMENT, PRIMARY KEY(hikeno),
	name	VARCHAR(30),
	location	VARCHAR(30),
	comment	VARCHAR(80),
	rating	SMALLINT,
	note	TINYTEXT
)
""")

cursor.execute ("""CREATE TABLE IF NOT EXISTS WAYPTS
(
	fileno	INT,
	ptno	INT,
	PRIMARY KEY(fileno, ptno),
	id	VARCHAR(30),
	comment	VARCHAR(80),
	lat	DOUBLE,
	lon	DOUBLE
)
""")

cursor.execute ("""CREATE TABLE IF NOT EXISTS HIKEPTS
(
	hikeno	INT,
	fileno	INT, 
	ptno	INT, 
	leg	INT,
	FOREIGN KEY(hikeno) REFERENCES HIKE,
	FOREIGN KEY(fileno) REFERENCES WAYPTS,
	FOREIGN KEY(ptno) REFERENCES WAYPTS,
	PRIMARY KEY(hikeno, leg),
	distance	FLOAT
)
""")

#sql = """SELECT * FROM HIKE"""
#cursor.execute(sql)
#data = cursor.fetchone()

#print data

#Delete any old temporary folder and files
if os.path.exists("tempGPSout"):
		os.system("rm -r tempGPSout")

#Create a temporary folder where temporary files will be stored
os.system("mkdir tempGPSout")

wayptRadio = IntVar()
wayptRadio.set(1)

routeRadio = IntVar()
routeRadio.set(1)

trkptRadio = IntVar()
trkptRadio.set(1)

trackRadio = IntVar()
trackRadio.set(1)

wayptCheck = IntVar()
wayptCheck.set(0)

routeCheck = IntVar()
routeCheck.set(0)

trkptCheck = IntVar()
trkptCheck.set(0)

wayptColour = StringVar()
routeColour = StringVar()
trkptColour = StringVar()
trackColour = StringVar()

waypts=[]
routes=[]
trkpts=[]
tracks=[]

filename = StringVar()
filename.set("")

fileString = StringVar()
fileString.set("")

originalFile = StringVar()
originalFile.set("")

filenum = IntVar()
filenum.set(0)

index = IntVar()

ratestr = StringVar()
ratestr.set("0")

maxfileno = IntVar()
maxfileno.set(0)

querystr = StringVar()
querystr.set("Select a Preset Query")

queryCheck = IntVar()
queryCheck.set(0)

hikeArr=[]
locArr=[]
comArr=[]
ratArr=[]
noteArr=[]
curhikeArr=[]

fileArr=[]
ptArr=[]
latArr=[]
lonArr=[]
idArr=[]
waycomArr=[]
legArr=[]
distArr=[]
hikefileArr=[]


#Printing out the command line arguments
#print sys.argv[1]
#print sys.argv[2]
#print sys.argv[3]


#def points2distance(start,  end):
	#"""
	#Calculate distance (in kilometers) between two points given as (long, latt) pairs
	#based on Haversine formula (http://en.wikipedia.org/wiki/Haversine_formula).
	#Implementation inspired by JavaScript implementation from 
	#http://www.movable-type.co.uk/scripts/latlong.html
	#Accepts coordinates as tuples (deg, min, sec), but coordinates can be given 
	#in any form - e.g. can specify only minutes:
	#(0, 3133.9333, 0) 
	#is interpreted as 
	#(52.0, 13.0, 55.998000000008687)
	#which, not accidentally, is the lattitude of Warsaw, Poland.
	#"""
	#start_long = math.radians(recalculate_coordinate(start[0],  'deg'))
	#start_latt = math.radians(recalculate_coordinate(start[1],  'deg'))
	#end_long = math.radians(recalculate_coordinate(end[0],  'deg'))
	#end_latt = math.radians(recalculate_coordinate(end[1],  'deg'))
	#d_latt = end_latt - start_latt
	#d_long = end_long - start_long
	#a = math.sin(d_latt/2)**2 + math.cos(start_latt) * math.cos(end_latt) * math.sin(d_long/2)**2
	#c = 2 * math.atan2(math.sqrt(a),  math.sqrt(1-a))
	#return 6371 * c

 	
def points2distance(lat, lon):
	R = 6371
	dLat = math.radians(lat[1]-lat[0])
	dLon = math.radians(lon[1]-lon[0]) 
	a = math.sin(dLat/2) * math.sin(dLat/2) + math.cos(math.radians(lat[0])) * math.cos(math.radians(lat[1])) * math.sin(dLon/2) * math.sin(dLon/2)
	c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a)) 
	return (R * c)

def closeprog():
	def confirmExit():
		#Deleting the temporary folders along with any temporary files stored
		if os.path.exists("tempGPSout"):
			 os.system("rm -r tempGPSout")
			 
		root.quit()
	
	#Creating a top level box
	exitBox = Toplevel(height=200, width=200)
	exitBox.title("Exit?")
	exitMsg = Label(exitBox, text="Are you sure you want to exit?")
	exitMsg.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
	yesButton = Button(exitBox, text="Yes", command=confirmExit).grid(column=1, row=4, padx=5, pady=5)
	noButton = Button(exitBox, text="No", command=exitBox.destroy).grid(column=2, row=4, padx=5, pady=5)
	
	#Listening for a escape or enter key
	def destroyexitBox(event):
		if event.keysym == "Escape":
			exitBox.destroy()
		if event.keysym == "Return":
			root.quit()
			
	exitBox.bind_all('<Key>', destroyexitBox)

def helpinfo():
	helpBox = Toplevel(height=200, width=200)
	helpBox.title("Help")
	helpLabel = Label (helpBox, text="To Begin, please open a valid GPS file and load it into the program.").pack()
	helpButton = Button(helpBox, text="OK", command=helpBox.destroy).pack(padx=10, pady=5)
	
	#Listening for a escape or enter key
	def destroyHelpWin(event):
		if event.keysym == "Escape":
			helpBox.destroy()
		if event.keysym == "Return":
			helpBox.destroy()
			
	helpBox.bind_all('<Key>', destroyHelpWin)

def proginfo():
	aboutBox = Toplevel(height=200, width=200)
	aboutBox.title("About XGPS")
	aboutLabel = Label(aboutBox, text="XGPS Version 1.01\nKwok Lau 0595198\nCIS*2750 W10").pack(padx=10, pady=10)
	aboutButton = Button(aboutBox, text="OK", command=aboutBox.destroy).pack(padx=10, pady=5)
	
		
	#Listening for a escape or enter key
	def destroyAboutWin(event):
		if event.keysym == "Escape":
			aboutBox.destroy()
		if event.keysym == "Return":
			aboutBox.destroy()
			
	aboutBox.bind_all('<Key>', destroyAboutWin)
	
def updateMap():
	wayptColNum=0
	if wayptColour.get()=="red":
		wayptColNum=0
	if wayptColour.get()=="blue":
		wayptColNum=1
	if wayptColour.get()=="green":
		wayptColNum=2
		
	trkptColNum=0
	if trkptColour.get()=="red":
		trkptColNum=0
	if trkptColour.get()=="blue":
		trkptColNum=1
	if trkptColour.get()=="green":
		trkptColNum=2
		

	#Start a new GMapData session. Full source code found at GMapData.py
	gmd = GMapData( "XGPS Kelvin Lau 0595198", [waypts[0][0],waypts[0][1]], 12 )
	index=0
	#Setting the route and track color from global variables
	gmd.setColors(routeColour.get())
	gmd.setColors(trackColour.get())
	#If waypt is set to 'show'
	if wayptRadio.get()==1:
		for i in range(len(waypts)):
			gmd.addPoint([waypts[i][0], waypts[i][1]])
			gmd.addOverlay(i, 1, wayptColNum)
		index = int(len(waypts))

	#If route is set to 'show'
	if routeRadio.get()==1:
		for i in range(len(routes)):
			#if element i in the list is selected
			if routeList.selection_includes(i):
				#copy point information
				route = routes[i]
				for j in range(2, len(route)):
					gmd.addPoint([waypts[route[j]][0], waypts[route[j]][1]])
					gmd.addOverlay(index, len(route)-2, 3)
				index += len(route)-2
		
	#If trackpoint is set to 'show'	
	if trkptRadio.get()==1:
		for i in range(len(trkpts)):
			gmd.addPoint([trkpts[i][0], trkpts[i][1]])
			gmd.addOverlay(index+i, 1, trkptColNum)
		index = int(len(trkpts))
		
	#If trackpoint is set to 'show'	
	if trackRadio.get()==1:
		nextSeq=IntVar()
		for i in range(len(tracks)):
			#if element i in the list is selected
			if trackList.selection_includes(i):
				#copy point information
				track = tracks[i]
				#if it is the last element
				if i+1==int(len(tracks)):
					nextSeq = int(len(trkpts)) - track[0]
				else:
					nextSeq = tracks[i+1][0] - track[0]
				for j in range(nextSeq):
					gmd.addPoint([trkpts[track[0]-1+j][0], trkpts[track[0]-1+j][1]])
					gmd.addOverlay(index+j, nextSeq, 4)

	#generate HTML to serve
	gmd.serve( "public_html/index.html" );

	#launch browser
	launchBrowser( "http://localhost:45198/" )

def updateDisplays():
	
	#Calling readGpFile from Gputil.c through C wrapper functions
	status = Gps.readFile(filename.get())

	if status == "OK":

		del waypts[:]
		del routes[:]
		del trkpts[:]
		del tracks[:]
		
		#Pulling the data from the GpFile through C wrapper functions
		Gps.getData(waypts, routes, trkpts, tracks)
		tempfilestr = "'" + filename.get() + "'"

		#Executing gpstool -info command through shell call
		os.system("cp " + tempfilestr + " 'tempGPSout/GPStemp.gps'")
		os.system("./gpstool -info < " + tempfilestr + " > 'tempGPSout/tempoutput.txt' 2>> 'tempGPSout/GPSerror.txt'")

		#Opening output log from gpstool and error log
		logError = open("tempGPSout/GPSerror.txt", "r")
		tempoutput = open("tempGPSout/tempoutput.txt", "r")

		tempStr = ""
		
		#Enabling and clearing the file textbox
		fileTextbox.config(state=NORMAL)
		fileTextbox.delete(1.0, END)
		
		#Reading the file and filling in the file textbox
		for tempStr in tempoutput:
			fileTextbox.insert(END, tempStr)
		
		#Disabling the textbox against further modifications
		fileTextbox.config(state=DISABLED)
	
		errStr = ""
		
		logTextbox.config(state=NORMAL)
		logTextbox.delete(1.0, END)

		for errStr in logError:
			logTextbox.insert(END, errStr)
			
		logTextbox.config(state=DISABLED)
		
		routeList.delete(0, END)
		
		for i in range(len(routes)):
			routeStr = str(routes[i][0]) + " " + routes[i][1]
			routeList.insert(END, routeStr)
		
		trackList.delete(0, END)
		
		for i in range(len(tracks)):
			trackStr = str(tracks[i][0]) + " " + tracks[i][1]
			trackList.insert(END, trackStr)
		
		#Activating the menu bar
		filemenu.entryconfig(1, state=NORMAL)
		filemenu.entryconfig(2, state=NORMAL)
		filemenu.entryconfig(3, state=NORMAL) 
		editmenu.entryconfig(0, state=NORMAL)
		editmenu.entryconfig(1, state=NORMAL)	
		editmenu.entryconfig(2, state=NORMAL)
		hikesmenu.entryconfig(0, state=NORMAL)
		updateButton.config(state=NORMAL)
		
		#Check to see if there are any waypts
		if len(waypts) > 0:
			wayptCheck.set(1)
		else:
			editmenu.entryconfig(2, state=DISABLED)
		
		#Check to see if there are any routes
		if len(routes) > 0:
			routeCheck.set(1)
		
		#Check to see if there are any trackpoints
		if len(trkpts) > 0:
			trkptCheck.set(1)
			
		#Check to see if there are more than 1 type of points
		if wayptCheck.get() + routeCheck.get() + trkptCheck.get() <= 1:
			#If there are 1 or less than 1 type of points, disable the discard and keep option
			editmenu.entryconfig(0, state=DISABLED)
			editmenu.entryconfig(1, state=DISABLED)

		#Freeing the GpFile
		freeStatus = Gps.freeFile()
		
	else:
		logError = open("tempGPSout/GPSerror.txt", "r")
		
		errStr = ""
		
		logTextbox.config(state=NORMAL)
		logTextbox.delete(1.0, END)

		for errStr in logError:
			logTextbox.insert(END, errStr)
			
		logTextbox.config(state=DISABLED)
		
		errBox = Toplevel(height=200, width=200)
		errBox.title("Error")
		errLabel = Label(errBox, text=status).pack()
		okButton = Button(errBox, text="Cancel", command=errBox.destroy).pack(padx=5, pady=5)
		
		#Listening for either a escape or enter key command
		def destroyerrBox(event):
			if event.keysym == "Escape":
				errBox.destroy()
			if event.keysym == "Return":
				errBox.destroy()
			
		errBox.bind_all('<Key>', destroyerrBox)


def discardpts():

	def cancelDiscard():
		discardBox.destroy()
		
	def okDiscard():
		def doDiscardPts():
			discardStr=""
			if routeCheck.get()==1:
				discardStr += "r"
			if wayptCheck.get()==1:
				discardStr += "w"
			if trkptCheck.get()==1:
				discardStr += "t"
				
			os.system("./gpstool -discard " + discardStr + " < 'tempGPSout/GPStemp.gps' > 'tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps' 2>> 'tempGPSout/GPSerror.txt'")
			
			filename.set("tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps")
			filenum.set(filenum.get()+1)
			updateDisplays()
			
		def discardCheck():
			if routeCheck.get()==1 and wayptCheck.get()==1 and trkptCheck.get()==1:
				discardError = Toplevel(height=200, width=200)
				discardError.title("Error Discarding")
				discardErrorMsg = Label(discardError, text="Error: Cannot Discard all Components.")
				discardErrorMsg.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
				okButton = Button(discardError, text="Cancel", command=discardError.destroy).grid(column=1, columnspan=2, row=4, padx=5, pady=5)
				
				#Listening for either a escape or enter key command
				def destroydiscardError(event):
					if event.keysym == "Escape":
						discardError.destroy()
					if event.keysym == "Return":
						discardError.destroy()  
									
				discardError.bind_all('<Key>', destroydiscardError)
			else:
				doDiscardPts()
				
		
				
		def discardRoutes():
			routeCheck.set(1)
			discardBoth.destroy()
			discardCheck()

		if routeCheck.get()==0 and wayptCheck.get()==1:
			discardBoth = Toplevel(height=200, width=200)
			discardBoth.title("Discard routes?")
			discardRoute = Label(discardBoth, text="Discarding Waypoints will also discard routes.")
			discardRoute.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
			okButton = Button(discardBoth, text="OK", command=discardRoutes).grid(column=1, row=4, padx=5, pady=5)
			canButton = Button(discardBoth, text="Cancel", command=discardBoth.destroy).grid(column=2, row=4, padx=5, pady=5)

			#Listening for either a escape or enter key command
			def destroydiscardBoth(event):
				if event.keysym == "Escape":
					discardBoth.destroy()
				if event.keysym == "Return":
					discardRoutes()
			
			discardBoth.bind_all('<Key>', destroydiscardBoth)

		else:
			discardCheck()
			
		discardBox.destroy()
		
	discardBox = Toplevel(height=200, width=200)
	discardBox.title("Discard...")
	if len(waypts) > 0:
		wayptCheck.set(0)
		wayptBut = Checkbutton(discardBox, text="  Waypoints", variable=wayptCheck)
		wayptBut.grid(column=1, columnspan=2, row=1, padx=3, pady=3)
	else:
		wayptCheck.set(1)
		
	if len(routes) > 0:
		routeCheck.set(0)
		routeBut = Checkbutton(discardBox, text="   Routes  ", variable=routeCheck)
		routeBut.grid(column=1, columnspan=2, row=2, padx=3, pady=3)	
	else:
		routeCheck.set(1)
		
	if len(trkpts) > 0:
		trkptCheck.set(0)
		trkptBut = Checkbutton(discardBox, text="Trackpoints", variable=trkptCheck)
		trkptBut.grid(column=1, columnspan=2, row=3, padx=3, pady=3)
	else:
		trkptCheck.set(1)
	okButton = Button(discardBox, text="OK", command=okDiscard).grid(column=1, row=4, padx=5, pady=5)
	canButton = Button(discardBox, text="Cancel", command=cancelDiscard).grid(column=2, row=4, padx=5, pady=5)
	
	#Listening for either a escape or enter key command
	def destroydiscardBox(event):
		if event.keysym == "Escape":
			cancelDiscard()
		if event.keysym == "Return":
			okDiscard()
			
	discardBox.bind_all('<Key>', destroydiscardBox)
	
def sortpts():
	
	def okSort():
		
		os.system("./gpstool -sortwp < '" + filename.get() + "' > 'tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps' 2>> 'tempGPSout/GPSerror.txt'")
			
		filename.set("tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps")
		filenum.set(filenum.get()+1)
			
		sortBox.destroy()
		updateDisplays()

	def cancelSort():
		sortBox.destroy()
		
	sortBox = Toplevel(height=200, width=200)
	sortBox.title("Sort...")
	sortLabel = Label(sortBox, text="Are you sure you want to sort the waypoints?")
	sortLabel.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
	okButton = Button(sortBox, text="OK", command=okSort).grid(column=1, row=2, padx=5, pady=5)
	canButton = Button(sortBox, text="Cancel", command=cancelSort).grid(column=2, row=2, padx=5, pady=5)
	
	#Listening for either a escape or enter key command
	def destroysortBox(event):
		if event.keysym == "Escape":
			cancelSort()
		if event.keysym == "Return":
			okSort()
			
	sortBox.bind_all('<Key>', destroysortBox)
	
def keeppts():


	def cancelKeep():
		keepBox.destroy()
		
	def okKeep():
		
		def keepCheck():
			if routeCheck.get()==0 and wayptCheck.get()==0 and trkptCheck.get()==0:
				keepError = Toplevel(height=200, width=200)
				keepError.title("Error Discarding")
				keepErrorMsg = Label(keepError, text="Error: Cannot Discard all Components.")
				keepErrorMsg.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
				okButton = Button(keepError, text="Cancel", command=keepError.destroy).grid(column=1, columnspan=2, row=4, padx=5, pady=5)
				
				def destroykeepError(event):
					if event.keysym == "Escape":
						keepError.destroy()
					if event.keysym == "Return":
						keepError.destroy()
				keepError.bind_all('<Key>', destroykeepError)
			else:	
				keepStr=""
				if routeCheck.get()==1:
					keepStr += "r"
				if wayptCheck.get()==1:
					keepStr += "w"
				if trkptCheck.get()==1:
					keepStr += "t"

				os.system("./gpstool -keep " + keepStr + " < 'tempGPSout/GPStemp.gps' > 'tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps' 2>> 'tempGPSout/GPSerror.txt'")
				
				filename.set("tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps")
				filenum.set(filenum.get()+1)
				
				updateDisplays()
			
		def keepWaypts():
			wayptCheck.set(1)
			keepBoth.destroy()
			keepCheck()
			
			
		if routeCheck.get()==1 and wayptCheck.get()==0:
			keepBoth = Toplevel(height=200, width=200)
			keepBoth.title("Keep Waypoints?")
			keepRoute = Label(keepBoth, text="Keeping routes will also keep Waypoints.")
			keepRoute.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
			okButton = Button(keepBoth, text="OK", command=keepWaypts).grid(column=1, row=4, padx=5, pady=5)
			canButton = Button(keepBoth, text="Cancel", command=keepBoth.destroy).grid(column=2, row=4, padx=5, pady=5)

			#Listening for either a escape or enter key command
			def destroykeepBoth(event):
				if event.keysym == "Escape":
					keepBoth.destroy()
				if event.keysym == "Return":
					keepWaypts()
			
			keepBoth.bind_all('<Key>', destroykeepBoth)
		else:
			keepCheck()
			
		keepBox.destroy()
		
		
	keepBox = Toplevel(height=200, width=200)
	keepBox.title("Keep...")
	if len(waypts) > 0:
		wayptCheck.set(1)
		wayptBut = Checkbutton(keepBox, text="  Waypoints", variable=wayptCheck)
		wayptBut.grid(column=1, columnspan=2, row=1, padx=3, pady=3)
	else:
		wayptCheck.set(0)
		
	if len(routes) > 0:
		routeCheck.set(1)
		routeBut = Checkbutton(keepBox, text="   Routes  ", variable=routeCheck)
		routeBut.grid(column=1, columnspan=2, row=2, padx=3, pady=3)
	else:
		routeCheck.set(0)
		
	if len(trkpts) > 0:
		trkptCheck.set(1)
		trkptBut = Checkbutton(keepBox, text="Trackpoints", variable=trkptCheck)
		trkptBut.grid(column=1, columnspan=2, row=3, padx=3, pady=3)
	else:
		trkptCheck.set(0)
		
	okButton = Button(keepBox, text="OK", command=okKeep).grid(column=1, row=4, padx=5, pady=5)
	canButton = Button(keepBox, text="Cancel", command=cancelKeep).grid(column=2, row=4, padx=5, pady=5)
	
	#Listening for either enter or escape and executing appropriate functions
	def destroykeepBox(event):
		if event.keysym == "Escape":
			cancelKeep()
		if event.keysym == "Return":
			okKeep()
			
	keepBox.bind_all('<Key>', destroykeepBox)
	

#Opening a file window
def openFile():
	openfilestr = tkFileDialog.askopenfilename(title="Open File...", parent = root, filetypes=[("GPS Files", "*.gps"), ("All Files", "*")])
	
	#If no file is selected
	if openfilestr=="":
		return
		
	if openfilestr:
		filename.set(openfilestr)
		originalFile.set(openfilestr)
		tempString = openfilestr.split("/")
		fileString.set(tempString[len(tempString)-1])
		maxfileno.set(cursor.execute("""SELECT MAX(fileno) FROM WAYPTS"""))
		curfileno = cursor.fetchone()[0]
		
		if type(curfileno) == NoneType:
			maxfileno.set(1);
		else: 
			maxfileno.set(maxfileno.get()+1)
			
		root.title("XGPS - " + fileString.get())
		updateDisplays()
	#status = Gps.readFile(filename.get())

	
  
def saveFile():
	tempfilename = filename.get()
	
	os.system("cp " + "'" + tempfilename + "'" + " '" + originalFile.get() + "'")
	
	filename.set(originalFile.get())
	root.title("XGPS - " + fileString.get())
  
def saveasFile():
	savefilestr = tkFileDialog.asksaveasfilename(title="Save File...", parent = root, filetypes=[("GPS Files", "*.gps"), ("All Files", "*")])
	
	tempfilename = filename.get()
	
	os.system("cp " + "'" + tempfilename + "'" + " " + savefilestr);
	
	filename.set(savefilestr)
	originalFile.set(savefilestr)
	tempString = savefilestr.split("/")
	fileString.set(tempString[len(tempString)-1])
	root.title("XGPS - " + fileString.get())

def mergeFile():
	secondFile = tkFileDialog.askopenfilename(title="Merge File...", parent = root, filetypes=[("GPS Files", "*.gps"), ("All Files", "*")])

	if secondFile=="":
		return

	secondFile = "'" + str(secondFile) + "'"
	firstFile = filename.get()
	
	os.system("./gpstool -merge " + secondFile + " < 'tempGPSout/GPStemp.gps' > 'tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps' 2>> tempGPSout/GPSerror.txt")
	
	filename.set("tempGPSout/modifiedTemp" + "(" + str(filenum.get()) + ")" + ".gps")
	filenum.set(filenum.get()+1)

	updateDisplays()


def doStore(index):

	def hikeStore():
		
		#Filling out the HIKE table
		hikeName = nameText.get(0.0, 30.0)
		
		if countryName == "Canada" or countryName == "United States":		#if the country is CA or US, include the prov/state
			location = countryName + ", " + stateName
		else:
			location = countryName
			
		comment = routeText.get(0.0, 80.0)
		rating = ratestr.get()
		note = noteText.get(0.0, 255.0)
		
		#Trimming the newline character from strings
		hikeArr.append(hikeName.rstrip("\n"))
		locArr.append(location.rstrip("\n"))
		comArr.append(comment.rstrip("\n"))
		ratArr.append(rating.rstrip("\n"))
		noteArr.append(note.rstrip("\n"))
		
		#cursor.execute("""SELECT * FROM HIKE""")
		#testprint = cursor.fetchone()
		#print testprint
		counter = 0
		#Filling out the WAYPTS table
		for j in range(2, len(route)):
			
			fileArr.append(maxfileno.get())
			legArr.append(j-1)
			idval = waypts[route[j]][2]
			comval = waypts[route[j]][3]
			latArr.append(waypts[route[j]][0])
			lonArr.append(waypts[route[j]][1])
			idArr.append(idval.rstrip("\n"))
			waycomArr.append(comval.rstrip("\n"))
			ptArr.append(route[j])
			curhikeArr.append((cursor.execute ("""SELECT MAX(hikeno) FROM HIKE;""") + index))
			
			if counter==0:
				distArr.append(0.0)
				counter = counter + 1
				
			else:
				lat = (waypts[route[j-1]][0], waypts[route[j]][0])
				lon = (waypts[route[j-1]][1], waypts[route[j]][1])
				distArr.append(points2distance(lat,  lon))
		
		if index+1 < len(routes):
			doStore(index+1)
		else:	
			for k in range(index+1):
				cursor.execute ("""INSERT IGNORE INTO HIKE (name, location, comment, rating, note) 
						VALUES (%s, %s, %s, %s, %s)""", (hikeArr[k], locArr[k], comArr[k], ratArr[k], noteArr[k]))
						
			for h in range(len(fileArr)):
				cursor.execute ("""INSERT IGNORE INTO WAYPTS (fileno, ptno, id, comment, lat, lon) 
					VALUES (%s, %s, %s, %s, %s, %s)""", 
					(fileArr[h], ptArr[h], idArr[h], waycomArr[h], latArr[h], lonArr[h]))
						
				cursor.execute ("""INSERT IGNORE INTO HIKEPTS (hikeno, fileno, ptno, leg, distance) 
					VALUES (%s, %s, %s, %s, %s)""", (curhikeArr[h], fileArr[h], ptArr[h], legArr[h], distArr[h]))

				
		storeBox.destroy()
	
	def omitStore():
		if index+1 < len(routes):
			doStore(index+1)
			
		else:	
			for k in range(index):
				cursor.execute ("""INSERT IGNORE INTO HIKE (name, location, comment, rating, note) 
						VALUES (%s, %s, %s, %s, %s)""", (hikeArr[k], locArr[k], comArr[k], ratArr[k], noteArr[k]))
						
			for h in range(len(fileArr)):
				cursor.execute ("""INSERT IGNORE INTO WAYPTS (fileno, ptno, id, comment, lat, lon) 
					VALUES (%s, %s, %s, %s, %s, %s)""", 
					(fileArr[h], ptArr[h], idArr[h], waycomArr[h], latArr[h], lonArr[h]))
						
				cursor.execute ("""INSERT IGNORE INTO HIKEPTS (hikeno, fileno, ptno, leg, distance) 
					VALUES (%s, %s, %s, %s, %s)""", (curhikeArr[h], fileArr[h], ptArr[h], legArr[h], distArr[h]))
						
		storeBox.destroy()
	
	def cancelStore():
		storeBox.destroy()
	
	if len(routes) < 1:
		routeError = Toplevel(height=200, width=200)
		routeError.title("No Routes!")
		routeErrorMsg = Label(routeError, text="Error: There are no route to store.")
		routeErrorMsg.grid(column=1, columnspan=2, row=1, padx=5, pady=5)
		cancelButton = Button(routeError, text="Cancel", command=routeError.destroy).grid(column=1, columnspan=2, row=4, padx=5, pady=5)
		
	else:
		route = routes[index]
			
		storeBox = Toplevel(bg="#b8dcf1")
		storeBox.title("Add Hike to Database (%d of %d)" % (index+1, len(routes)))
		nameLabel = Label(storeBox, text="Name of the hike:", bg="#b8dcf1").grid(column=0, row=0, pady=5)
		nameText =  Text(storeBox, height=1, width = 30, bg="white", wrap=NONE)
		nameText.grid(column=1, columnspan=2, row=0, pady=5, padx=5)
		nameText.insert(END, routes[index][1])
		
		routeLabel = Label(storeBox, text="Comment:", bg="#b8dcf1").grid(column=0, row=1, pady=5)
		routeText = Text(storeBox, height=1, width = 30, bg="grey", wrap=NONE)
		routeText.grid(column=1, columnspan=2, row=1, pady=5, padx=5)
		routeText.insert(END, routes[index][1])
		routeText.config(state=DISABLED)
		
		legLabel = Label(storeBox, text="Number of Legs:", bg="#b8dcf1").grid(column=0, row=2, pady=5)
		legText = Text(storeBox, height=1, width = 30, bg="grey", wrap=NONE)
		legText.grid(column=1, columnspan=2, row=2, pady=5, padx=5)
		legText.insert(END, len(route)-2)
		legText.config(state=DISABLED)
		
	
		for j in range(2, len(route)):
			url = "http://maps.google.com/maps/api/geocode/xml?latlng=%s,%s&sensor=true" % (waypts[route[j]][0], waypts[route[j]][1])
			xml = urllib.urlopen(url).read()
			
			if "<status>ZERO_RESULTS</status>" in xml:
				countryName = "Not on Landmass"
			
			else:
				countryTuples = xml.split("<address_component>")
				for segment in countryTuples:
					if "<type>country</type>" in segment:
						break
				nameStart = segment.find("<long_name>")
				nameEnd = segment.find("</long_name>")
				
				countryName = segment[nameStart+len("<long_name>"):nameEnd]
				
				if countryName == "Canada" or countryName == "United States":
					provTuple = xml.split("<address_component>")
					for segment in countryTuples:
						if " <type>administrative_area_level_1</type>" in segment:
							break
					nameStart = segment.find("<short_name>")
					nameEnd = segment.find("</short_name>")
				
					stateName = segment[nameStart+len("<short_name>"):nameEnd]
				break
	
		locLabel = Label(storeBox, text="Location:", bg="#b8dcf1").grid(column=0, row=3, pady=5)
		locText = Text(storeBox, width=30, height=1, state=NORMAL)
		locText.grid(column=1, columnspan=2, row=3, pady=5, padx=5)
		locText.insert(END, countryName)
		locText.config(state=DISABLED)	
		
		if countryName == "Canada" or countryName == "United States":
			locText.config(state=NORMAL)
			locText.insert(END,", " + stateName)
					
		ratestr.set("0")
		rateLabel = Label(storeBox, text="Rating:", bg="#b8dcf1").grid(column=0, row=4, pady=5)
		rateCombo = OptionMenu(storeBox, ratestr, "0", "1", "2", "3", "4", "5")
		rateCombo.grid(column=1, row=4, pady=5, padx=5)
		
		noteLabel = Label(storeBox, text="Note:", bg="#b8dcf1").grid(column=0, row=5, pady=5)
		noteText = Text(storeBox, height= 5, width = 30, bg="white")
		noteText.grid(column=1, columnspan=2, row=5, pady=5, padx=5)
		storeButton = Button(storeBox, text="Store Hike", command=hikeStore).grid(column=0, row=6, padx=5, pady=5)
		omitButton = Button(storeBox, text="Omit Hike", command=omitStore).grid(column=1, row=6, padx=5, pady=5)
		canButton = Button(storeBox, text="Cancel", command=cancelStore).grid(column=2, row=6, padx=5, pady=5)
		
		#Listening for either a escape or enter key command
		def destroystoreBox(event):
			if event.keysym == "Escape":
				cancelStore()
			#if event.keysym == "Return":
				#okStore()
				
		storeBox.bind_all('<Key>', destroystoreBox)
	
	
def doQuery():
	
	def closeQuery():
		queryCheck.set(0)
		queryBox.destroy()
		
	def clearwindow():
		resultText.config(state=NORMAL)	#enabling result box for clearing of data
		resultText.delete(1.0, END)
		resultText.config(state=DISABLED) #disabling result textbox
	
	def helpQuery():
		
		def closeHelp():
			helpBox.destroy()
		
		helpBox = Toplevel(bg="#b8dcf1")
		helpBox.title("Help")
		hikeLabel = Label(helpBox, text="HIKE - represents each route in your current gps file", bg="#b8dcf1").grid(column=0, row=0, padx=5, pady=2)
		wayptLabel = Label(helpBox, text="WAYPTS - represents each waypoint on the hike", bg="#b8dcf1").grid(column=0, row=1, padx=5, pady=2)	
		hikeptsLabel = Label(helpBox, text="HIKEPTS - represents the individual points on the hike", bg="#b8dcf1").grid(column=0, row=2, padx=5, pady=2)
		cancelButton = Button(helpBox, text="Cancel", command=closeHelp). grid(column=0, row=3, padx=5, pady=2)
		
		def destroyHelp(event):
			if event.keysym == "Escape":
				closeHelp()
		
		helpBox.bind_all('<Key>', destroyHelp)
	
	if queryCheck.get()==0:
		
		queryCheck.set(1)
		queryBox = Toplevel(bg="#b8dcf1")
		queryBox.title("Query for %s at %s" % (sys.argv[1], hostname))
		hikeFrame = LabelFrame(queryBox, bg="#b8dcf1",padx=10, pady=10, text="Current Hike", width=250, height=600)
		hikeFrame.grid(column=0, columnspan=3, row=0, padx=15, pady=15)
		hikeFrame.grid_propagate()
	
		hikeScroll = Scrollbar(hikeFrame, orient=VERTICAL)
		hikeScroll.grid(column=4, row=1, rowspan=2, stick=N+S)
		hikeList = Listbox(hikeFrame, height=20, width = 20, selectmode=SINGLE, yscrollcommand=hikeScroll.set, bg="white",exportselection=0)
		hikeScroll.config(command = hikeList.yview)
		hikeList.grid(column=0, columnspan=3, row=1, rowspan=2, pady=10)
		
		hikeText = Text(hikeFrame, height=15, width = 40, state=DISABLED, bg="white")
		hikeText.grid(column=5, row=1, padx=5, pady=5)
	
		mapitButton = Button(hikeFrame, text="Map it")
		mapitButton.grid(column=5, row = 2, padx=5, pady=5)
		
		queryFrame = LabelFrame(queryBox, bg="#b8dcf1",padx=10, pady=10, text="Query", width=250, height=600)
		queryFrame.grid(column=0, columnspan=3, row=1)
		queryFrame.grid_propagate()
		
		presetLabel = Label(queryFrame, text="Presets:", bg="#b8dcf1").grid(column=0, row=0, pady=5)
		presetCombo = OptionMenu(queryFrame, querystr, 
					"Find the name of the shortest hike in ___(% is wild card)",
					"How many hikes have more than ___ legs?",
					"How many hikes have the distance greater than ___ between them?",
					"How many hikes have been rated ___ or higher?")
		presetCombo.grid(column=0, columnspan=3, row=0, pady=5, padx=5) 		
		
		queryLabel = Label(queryFrame, text="Query:", bg="#b8dcf1").grid(column=0, row=1, pady=5)
		queryText = Text(queryFrame, height=1, width = 55, bg="white")
		queryText.grid(column=1, row=1, padx=5, pady=5)
		queryText.insert(0.0, "SELECT")
		
		resultFrame  = LabelFrame(queryBox, bg="#b8dcf1",padx=10, pady=10, text="Results", width=250, height=600)
		resultFrame.grid(column=0, columnspan=3, row=2, padx=15, pady=15)
		resultFrame.grid_propagate()
	
		resultScroll = Scrollbar(resultFrame, orient=VERTICAL)
		resultScroll.grid(column=1, row=0, stick=N+S)
		resultText = Text(resultFrame, height=25, width = 65, state=DISABLED, bg="white", yscrollcommand=resultScroll.set)
		resultScroll.config(command = resultText.yview)
		resultText.grid(column=0, row=0, padx=5, pady=5)
		
		resultClear = Button(resultFrame, text="Clear", command=clearwindow).grid(column=1, row=1, pady=2, padx=2)

		submit = Button(queryBox, text="Submit").grid(column=0, row=3, padx=5, pady=5)
		closeButton = Button(queryBox, text="Close", command = closeQuery).grid(column=2, row=3, padx=5, pady=5)
		helpButton = Button(queryBox, text="Help", command = helpQuery).grid(column=1, row=3, padx=5, pady=5)
			
		
		cursor.execute("""SELECT name FROM HIKE ORDER BY name""")
		results = cursor.fetchall()

		
		for i in range(len(results)):
			hikeList.insert(END, results[i])

		#On window "X" close, run closeQuery Script
		queryBox.protocol("WM_DELETE_WINDOW", closeQuery)
	
		#Detecting an escape key
		def destroyQuery(event):
			if event.keysym == "Escape":
				closeQuery()
		
		queryBox.bind_all('<Key>', destroyQuery)

#Create a menu bar	
menubar = Menu(root);

#Add in cascade "file" with options
filemenu = Menu(menubar, tearoff=0, bg="#b8dcf1")
filemenu.add_command(label="Open", command=openFile)
filemenu.add_command(label="Save", command=saveFile, state=DISABLED)
filemenu.add_command(label="Save As...", command=saveasFile, state=DISABLED)
filemenu.add_command(label="Merge...", state=DISABLED, command=mergeFile)
filemenu.add_command(label="Exit", command = closeprog)
menubar.add_cascade(label="File", menu=filemenu)

#Add in cascade "edit" with options
editmenu = Menu(menubar, tearoff=0, bg="#b8dcf1")
editmenu.add_command(label="Discard...", command=discardpts, state=DISABLED)
editmenu.add_command(label="Keep...", command=keeppts, state=DISABLED)
editmenu.add_command(label="Sort...", command=sortpts, state=DISABLED)
menubar.add_cascade(label="Edit", menu=editmenu)

hikesmenu = Menu(menubar, tearoff=0, bg="#b8dcf1")
hikesmenu.add_command(label="Store...", command=lambda:doStore(0), state=DISABLED)
hikesmenu.add_command(label="Query...", command=doQuery)
menubar.add_cascade(label="Hikes", menu=hikesmenu)

#Add in cascade "help" with options
helpmenu = Menu(menubar, tearoff=0, bg="#b8dcf1")
helpmenu.add_command(label="Help", command = helpinfo)
helpmenu.add_command(label="About", command = proginfo)
menubar.add_cascade(label="Help", menu=helpmenu)

root.config(menu=menubar)

#Create a label frame with a "File Information" Label
fileFrame = LabelFrame(root, bg="#81a6ca", padx=10, pady=10, text="File Information", width=500, height=200)
fileFrame.grid(column=0, columnspan=2, padx=20, pady=20)
fileFrame.grid_propagate()

fileTextbox = Text(fileFrame, spacing1=3, height=6, bg="white", state=DISABLED)
fileTextbox.grid(padx=3, pady=3)

#def clearfileText():
	#fileTextbox.config(state=NORMAL)
	#fileTextbox.delete(1.0, END)
	#fileTextbox.config(state=DISABLED)

#clearfileButton = Button(fileFrame, text="Clear", command=clearfileText)
#clearfileButton.grid(sticky=E, padx=2, pady=2)

mapFrame = LabelFrame(root, bg="#81a6ca",padx=10, pady=10, text="Map Control", width=500, height=200)
mapFrame.grid(column=0, columnspan=2)
fileFrame.grid_propagate()

wayptColour.set("red")

wayptRadioOn = Radiobutton(mapFrame, text="Show", variable=wayptRadio, value=1, bg="#81a6ca")
wayptRadioOn.grid(column=1, row =2)
wayptRadioOff = Radiobutton(mapFrame, text="Hide", variable=wayptRadio, value=0, bg="#81a6ca")
wayptRadioOff.grid(column=1, row=3)

def getWayptColour():	
	tempColour=wayptColour.get()
	
	def okWaypt():
		wayptButton.config(bg=wayptColour.get())
		wayptColWin.destroy()

	def cancelWaypt():
		wayptButton.config(bg=tempColour)
		wayptColWin.destroy()
	
	wayptColWin = Toplevel(height=200, width=200)
	wayptColWin.title("Waypoint Colour")
	wayptColButton1 = Button(wayptColWin, text="Select", command=okWaypt).grid(column=1, row=4, padx=5, pady=5)
	wayptColButton2 = Button(wayptColWin, text="Cancel", command=cancelWaypt).grid(column=2, row=4, padx=5, pady=5)
	wayptColRadio = Radiobutton(wayptColWin, text="Red", variable=wayptColour, value="red").grid(column=1, columnspan=2, row=1, padx=5, pady=5)
	wayptColRadio = Radiobutton(wayptColWin, text="Blue", variable=wayptColour, value="blue").grid(column=1, columnspan=2, row=2, padx=5, pady=5)
	wayptColRadio = Radiobutton(wayptColWin, text="Green", variable=wayptColour, value="green").grid(column=1, columnspan=2, row=3, padx=5, pady=5)

	#Listening for either a escape or enter key command
	def destroyWayptWin(event):
		if event.keysym == "Escape":
			cancelWaypt()
		if event.keysym == "Return":
			okWaypt()
			
	wayptColWin.bind_all('<Key>', destroyWayptWin)
	
wayptButton = Button(mapFrame, text=" Waypoint ", bg=wayptColour.get(), command=getWayptColour)
wayptButton.grid(column=1, row=1, padx=16, pady=8)

routeColour.set("#4474f8")

routeRadioOn = Radiobutton(mapFrame, text="Show", variable=routeRadio, value=1, bg="#81a6ca")
routeRadioOn.grid(column=2, row =2)
routeRadioOff = Radiobutton(mapFrame, text="Hide", variable=routeRadio, value=0, bg="#81a6ca")
routeRadioOff.grid(column=2, row=3)

def getRouteColour():	
	(triple, hexstr) = tkColorChooser.askcolor(color=routeColour.get(), title="Route Colour")
	if hexstr:
		routeColour.set(hexstr)
		routeButton.config(bg=hexstr)
	
routeButton = Button(mapFrame, text="   Route   ", bg=routeColour.get(), command=getRouteColour)
routeButton.grid(column=2, row=1, padx=16, pady=8)

trkptColour.set("green")

trkptLabel = Label(mapFrame, text="Trackpoints", bg="#81a6ca")
trkptLabel.grid(column=3, row=1,  padx=16, pady=8)
trkptRadioOn = Radiobutton(mapFrame, text="Show", variable=trkptRadio, value=1, bg="#81a6ca")
trkptRadioOn.grid(column=3, row =2)
trkptRadioOff = Radiobutton(mapFrame, text="Hide", variable=trkptRadio, value=0, bg="#81a6ca")
trkptRadioOff.grid(column=3, row=3)

def getTrkptColour():
	tempColour=trkptColour.get()
	
	def okTrkpt():
		trkptButton.config(bg=trkptColour.get())
		trkptColWin.destroy()

	def cancelTrkpt():
		trkptButton.config(bg=tempColour)
		trkptColWin.destroy()
	
	#Creating a Toplevel box with the color selections
	trkptColWin = Toplevel(height=200, width=200)
	trkptColWin.title("Trackpoint Colour")
	trkptColButton1 = Button(trkptColWin, text="Select", command=okTrkpt).grid(column=1, row=4, padx=5, pady=5)
	trkptColButton2 = Button(trkptColWin, text="Cancel", command=cancelTrkpt).grid(column=2, row=4, padx=5, pady=5)
	trkptColRadio = Radiobutton(trkptColWin, text="Red", variable=trkptColour, value="red").grid(column=1, columnspan=2, row=1, padx=5, pady=5)	#Red Colour
	trkptColRadio = Radiobutton(trkptColWin, text="Blue", variable=trkptColour, value="blue").grid(column=1, columnspan=2, row=2, padx=5, pady=5)	#Blue Colour
	trkptColRadio = Radiobutton(trkptColWin, text="Green", variable=trkptColour, value="green").grid(column=1, columnspan=2, row=3, padx=5, pady=5)	#Green Colour
	
	#Listening for either a escape or enter key command
	def destroyTrkptWin(event):
		if event.keysym == "Escape":
			cancelTrkpt()
		if event.keysym == "Return":
			okTrkpt()
			
	trkptColWin.bind_all('<Key>', destroyTrkptWin)
	
trkptButton = Button(mapFrame, text="Trackpoints", bg=trkptColour.get(), command=getTrkptColour)
trkptButton.grid(column=3, row=1, padx=16, pady=8)

trackColour.set("#ffb41e")

trackLabel = Label(mapFrame, text="Tracks", bg="#81a6ca")
trackLabel.grid(column=4, row=1,  padx=16, pady=8)
trackRadioOn = Radiobutton(mapFrame, text="Show", variable=trackRadio, value=1, bg="#81a6ca")
trackRadioOn.grid(column=4, row =2)
trackRadioOff = Radiobutton(mapFrame, text="Hide", variable=trackRadio, value=0, bg="#81a6ca")
trackRadioOff.grid(column=4, row=3)

#Changing the track colour, default 'cyan'
def getTrackColour():	
	(triple, hexstr) = tkColorChooser.askcolor(color=trackColour.get(), title="Track Colour")
	if hexstr:
		trackColour.set(hexstr)
		trackButton.config(bg=hexstr)
	
trackButton = Button(mapFrame, text="  Tracks  ", bg=trackColour.get(), command=getTrackColour)
trackButton.grid(column=4, row=1, padx=16, pady=8)

updateButton = Button(mapFrame, text="Update Map", command = updateMap, state=DISABLED)
updateButton.grid(column=1, columnspan = 4, row=5, padx=5, pady=10)

routeFrame = Frame(root, bg="#81a6ca", pady=10, height=200, width=400)
routeFrame.grid(column=0, row=3)
routeFrame.grid_propagate()

trackFrame = Frame(root, bg="#81a6ca", pady=10, height=200, width=400)
trackFrame.grid(column=1, row=3)
trackFrame.grid_propagate() 

#Highlight all routes
def selectAllRoutes():
	routeList.selection_set(0, END)
	
#De-select all routes
def selectNoRoutes():
	routeList.selection_clear(0,END)

#Create a listbox with a "Routes" label on top and a scroll bar
routeListLabel = Label(routeFrame, text="Routes", bg="#81a6ca")
routeListLabel.grid(column=0, columnspan=3, row=0, padx=5)
routeScroll = Scrollbar(routeFrame, orient=VERTICAL)
routeScroll.grid(column=4, row=1, stick=N+S)
routeList = Listbox(routeFrame, height=10, width = 35, selectmode=MULTIPLE, yscrollcommand=routeScroll.set, bg="white", exportselection=0)
routeScroll.config(command = routeList.yview)
routeList.grid(column=0, columnspan=3, row=1, pady=10)
routeAll = Button(routeFrame, text="Select All", command = selectAllRoutes)	#Select All button to select all the items in the list
routeAll.grid(column=0, columnspan=2, row=2, pady=5)
routeNone = Button(routeFrame, text="De-Select All", command = selectNoRoutes)	#De-Select All button to de-select all the items in the list
routeNone.grid(column=2, columnspan=2, row=2, pady=5)

def selectAllTracks():
	trackList.selection_set(0, END)
	
def selectNoTracks():
	trackList.selection_clear(0,END)
	
trackListLabel = Label(trackFrame, text="Tracks", bg="#81a6ca")
trackListLabel.grid(column=0, columnspan=3, row=0, padx=5)
trackScroll = Scrollbar(trackFrame, orient=VERTICAL)
trackScroll.grid(column=4, row=1, stick=N+S)
trackList = Listbox(trackFrame, height=10, width = 35, selectmode=MULTIPLE, yscrollcommand=trackScroll.set, bg="white",exportselection=0)
trackScroll.config(command = trackList.yview)
trackList.grid(column=0, columnspan=3, row=1, pady=10)
trackAll = Button(trackFrame, text="Select All", command=selectAllTracks)	#Select All button to select all the items in the list
trackAll.grid(column=0, columnspan=2, row=2, pady=5)
trackNone = Button(trackFrame, text="De-Select All", command=selectNoTracks)	#De-Select All button to de-select all the items in the list
trackNone.grid(column=2, columnspan=2, row=2, pady=5)

logFrame = LabelFrame(root, bg="#81a6ca", padx=10, pady=10, text="Log Display", height=200, width=600)
logFrame.grid(column=0, columnspan=2, padx=20, pady=20)
logFrame.grid_propagate()

#Setting the textbox scroll bar and the textbox
logScroll = Scrollbar(logFrame, orient=VERTICAL)
logScroll.grid(row=1, rowspan=3, column=9, stick=N+S)
logTextbox = Text(logFrame, spacing1=0, height=8, yscrollcommand=logScroll.set, bg="white", state=DISABLED)
logScroll.config(command = logTextbox.yview)
logTextbox.grid(padx=3, pady=3, row=1, rowspan=3, column=1, columnspan=8, sticky=E)

#Clearing the log text
def clearlogText():
	logTextbox.config(state=NORMAL)	#Enabling log textbox
	logTextbox.delete(1.0, END)
	logTextbox.config(state=DISABLED) #disabling Log textbox
	
clearlogButton = Button(logFrame, text="Clear", command=clearlogText)
clearlogButton.grid(sticky=E, padx=5, pady=5, row=4, column=8)


root.mainloop()
if os.path.exists("tempGPSout"):
	 os.system("rm -r tempGPSout")
db.close()	 
killServers()
