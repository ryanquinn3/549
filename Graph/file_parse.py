# Parses the data given from the ocv output function
# Requires the filename of the text function given
# Returns a 3-tuple that consists of the frame of the video, the area and
# the pupil
def parse_ocv_output(filename):
    f = open(filename, 'r')
    
    frame = []
    area  = []
    ratio = []

    for line in f:
        # Remove newline and split by ;
        newstr = line[:-1]
        split = newstr.split(';')
        frame.append(split[0])
        area.append(split[1])
        ratio.append(split[2])

    data = [frame, area, ratio]

    return data
