
########### install files ###############

install(FILES  readme 	clock-drop-shadow.svg	     	clock-face-shadow.svg	     	clock-face.svg		     	clock-frame.svg		     	clock-glass.svg		     	clock-hour-hand-shadow.svg	     	clock-hour-hand.svg		     	clock-marks.svg		     	clock-minute-hand-shadow.svg     	clock-minute-hand.svg	     	clock-second-hand-shadow.svg     	clock-second-hand.svg    	theme.conf DESTINATION ${clockdatadir}/themes/default)



#original Makefile.am contents follow:

#themedir = $(clockdatadir)/themes/default
#
#theme_DATA =	    \
#	readme\
#	clock-drop-shadow.svg	    \
#	clock-face-shadow.svg	    \
#	clock-face.svg		    \
#	clock-frame.svg		    \
#	clock-glass.svg		    \
#	clock-hour-hand-shadow.svg	    \
#	clock-hour-hand.svg		    \
#	clock-marks.svg		    \
#	clock-minute-hand-shadow.svg    \
#	clock-minute-hand.svg	    \
#	clock-second-hand-shadow.svg    \
#	clock-second-hand.svg   \
#	theme.conf
#
#EXTRA_DIST = $(theme_DATA)
