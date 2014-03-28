#Copy the data over the wifi link to the receiving laptop - obviously, this is for my laptop :)
echo "Sending video file for image processing..."
scp -c blowfish -i /home/pi/.ssh/id_rsa $1 done_file_upstream.txt apfeifer@192.168.1.4:/home/apfeifer/549/

#Wait for the text results from the OpenCV processing
echo "Waiting for OpenCV results..."
while [ ! -e "done_file_downstream.txt" ]; do

	if [ -e "OCV_pupil_measurements.txt" ]; then
		recv_size_new=`stat -c%s "OCV_pupil_measurements.txt"`
		echo -en "\rReceived $recv_size_new bytes"
	fi

	sleep 1
done

echo -en "\rReceived $recv_size_new bytes\n"
rm done_file_downstream.txt

#Once we have the results file, it can be fed to PiUi, to be sent back to the Android controller
echo "Results file received"
