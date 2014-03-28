#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

struct user_description {
	string user;
	string ID;
	string testNum;
};

void* FSM1(void*);
void* FSM2(void*);

pthread_mutex_t queue_sync;

int main()
{

	pthread_t fsm1_thread, fsm2_thread;
	if(pthread_mutex_init(&queue_sync, NULL) != 0) cout << "Bad mutex result" << endl;
	if(pthread_create(&fsm1_thread, NULL, FSM1, NULL) != 0) cout << "Bad thread1 result" << endl;
	if(pthread_create(&fsm2_thread, NULL, FSM2, NULL) != 0) cout << "Bad thread2 result" << endl;

	while(1);
	return 0;
}

void* FSM1(void* arg)
{
	ifstream queue;
	


	return NULL;


}

//This FSM reads from the ID queue and communicates with the basestation for scp
void* FSM2(void* arg)
{
	ifstream queue;
	string user, ID, testNum;

	vector<user_description> queue_contents; 

	//Loop forever, checking the queue on each iteration
	while(1)
	{
		//Make sure nothing can be written during this time
		pthread_mutex_lock(&queue_sync);

		//Check if anything exists in the buffer
		queue.open("ID_queue.dat");

		//Nope, nothing written
		if(!queue.is_open())
		{
			queue.close();
			pthread_mutex_unlock(&queue_sync);
			cout << "nothing written this cycle" << endl;
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
		}

		//Open the queue back up for writing from the other FSM		
		queue.close(); remove("ID_queue.dat"); pthread_mutex_unlock(&queue_sync);

		for(int i = 0; i < queue_contents.size(); i++)
		{
	
			//Wait until the video corresponding to this user exists
			string video_filename = "Test"+testNum+"_"+user+".h264";
			cout << "Waiting on video file " << video_filename << endl;
			ifstream video(video_filename.c_str());
			while(!video.is_open())
				video.open(video_filename.c_str());
			video.close();

			
		}
	}

	return NULL;
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
