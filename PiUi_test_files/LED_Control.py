import RPi.GPIO as GPIO, time

pins = [14,15,8,11,18,23,9,10,24,22,25,27]
def set_leds(leds,state):
	for light in leds:
		GPIO.output(pins[light],state)




def run_test_1(test_num):
	if test_num == 1:
		seq = [8]
	else:
		seq = [9]
	time.sleep(3)
	set_leds(seq,True)
	time.sleep(3)
	set_leds(seq,False)


#Takes in a test_number and shoots the lights for that specified test
def main(test_number):
	GPIO.setmode(GPIO.BCM)
	GPIO.setwarnings(False)
	for pin in pins:
		GPIO.setup(pin,GPIO.OUT)
	
	set_leds(range(12),False)
	
	if test_number == 1 or test_number == 4:
		run_test_1(test_number)
	elif test_number == 2 or test_number == 5:
		print 'Tracking pupil stimulation'
		time.sleep(7)
	elif test_number == 3 or test_number == 6:
		print 'Speed pupil stimulation'
		time.sleep(7)


	set_leds(range(12),False)
