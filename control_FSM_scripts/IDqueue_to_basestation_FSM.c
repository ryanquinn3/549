#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct user_description {
	string user;
	string ID;
	string testNum;
};

int main()
{
	ifstream queue;
	char cur_buffer = 'A';	
	string user, ID, testNum;

	vector<user_description> queue_contents; 

	//Loop forever, checking the queue on each iteration
	while(1)
	{	
		//Check if anything exists in the buffer
		queue.open("ID_queue.dat");

		//Nope, nothing written
		if(!queue.is_open())
		{
			cout << "nothing written this cycle" << endl;
			queue.close();
			system("sleep 2");
			continue;
		}

		//If we've found a user value, then there must be some work to do!
		//Get all the currently available information
		while(getline(queue, user)) 
		{
			getline(queue, ID);
			getline(queue, testNum);
		
			user_description temp_desc;
			temp_desc.user = user; temp_desc.ID = ID; temp_desc.testNum = testNum;
			queue_contents.push_back(temp_desc);

			//Open the queue back up for writing from the other FSM
			queue.close(); remove("ID_queue.dat");
		}

		for(int i = 0; i < queue_contents.length(); i++)
		{
	
			//Wait until the video corresponding to this user exists
			string video_filename = "Test"+testNum+"_"+user+".h264";
			ifstream video(video_filename.c_str());
			while(!video.is_open())
				video.open(video_filename.c_str());
			video.close();

			
		}
	}

	return 0;
}
/*
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
*/
