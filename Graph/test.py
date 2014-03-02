import file_parse as fp
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

print "Reading File"

data = fp.parse_ocv_output('OCV_pupil_measurements.txt')
x, y, z = data[0], data[1], data[2]

plt.figure(1)
plt.subplot(211)

plt.plot(x, y)
plt.title('Area of pupil over time')
plt.ylabel('Area')
plt.xlabel('Frame (1/25th second)')

plt.subplot(212)
plt.title('Ratio of pupil over time')
plt.plot(x, z)
plt.ylabel('Ratio')
plt.xlabel('Frame (1/25th second)')
plt.show()

print "Test complete!"
