import time
#Takes in a test_number and shoots the lights for that specified test
def main(test_number):
	if test_number == 1 or test_number == 4:
		print 'Main pupil stimulation'
		time.sleep(7)
	elif test_number == 2 or test_number == 5:
		print 'Tracking pupil stimulation'
		time.sleep(7)
	elif test_number == 3 or test_number == 6:
		print 'Speed pupil stimulation'
		time.sleep(7)