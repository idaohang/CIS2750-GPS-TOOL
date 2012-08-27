#!/usr/bin/env python

# Import GUI classes
from Tkinter import *


from GMapData import *

def main():
	
	#1. start CGI/HTTP server
	startWebServer()

	#2. create GMapData object and populate with data
	gmd = GMapData( "Univ. of Guelph", [43.530318,-80.223241], 14 )	# use default values
	gmd.addPoint( [43.530318,-80.223241] )	# s.b. center of map
	gmd.addOverlay( 0, 1, 2 )	# single point, blue icon

	#3. generate HTML to serve
	gmd.serve( "public_html/index.html" );

	#4. launch browser
	launchBrowser( "http://localhost:8080/" )


	raw_input( "Press enter:" )		# pause
	
	gmd.addPoint( [43.530318,-80.223241] )	# 3-point red line
	gmd.addPoint( [43.535538,-80.223461] )
	gmd.addPoint( [43.520758,-80.220681] )
	gmd.addOverlay( 1, 3, 0 )
	
	gmd.serve( "public_html/index.html" );
	launchBrowser( "http://localhost:8080/" )


	raw_input( "Press enter:" )
	
	gmd.addPoint( [43.530318,-80.223241] )	# 3-point blue line
	gmd.addPoint( [43.515538,-80.220461] )
	gmd.addPoint( [43.520758,-80.225681] )
	gmd.addOverlay( 4, 3, 2 )
	
	gmd.serve( "public_html/index.html" );
	launchBrowser( "http://localhost:8080/" )


	print "Close the Tk window to proceed with server shutdown..."
	root = Tk()
	root.mainloop()				# open a Tk window

	#5. kill servers
	killServers()
	
	
if __name__ == "__main__":
    main()
