import eye_test
#import os
import subprocess



print "Beginning first eye test"

call = subprocess.Popen('raspistill -n -t 10000 -tl 2500 -o myimage_%04d.jpg',shell=True)
eye_test.run()

print "Test complete!"
