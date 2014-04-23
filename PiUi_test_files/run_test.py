import LED_Control
import os
import sys
import subprocess


def run():
	user_name = sys.argv[1]
	test_number = sys.argv[2]
	video_name = 'Test' + test_number+'_'+user_name+'.h264'
	#Set dynamic time for each individual test
	if int(test_number) in [1,4]:
		time = '6050'
	else:
		time = '8000'
	#Double check location of video
	raspivid_com = 'raspivid -n -o /home/pi/549/Temp_Videos/'+video_name+' -br 60 -rot 90 -t '+time

	print "Beginning eye test "+test_number

	call = subprocess.Popen(raspivid_com ,shell=True)
	os.setuid(0)
	LED_Control.main(int(test_number))


	print "Test "+test_number+" complete!"


run()
