import eye_test
#import os
import subprocess


def run():
	print "Beginning first eye test"

	call = subprocess.Popen('raspivid -n -o ../pupil_video.h264 -br 60 -rot 90 -t 7000' ,shell=True)
	eye_test.run()

	print "Test complete!"
