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

string test_request_file = "/home/pi/549/test_headers.txt";
string basestation_results_file = "/home/pi/549/results_file.txt";
string results_file_cache_directory = "/home/pi/549/OCV_Results/";

int T1_video_done = 0;

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

//This FSM waits for permission from the PiUi interface to add to the ID queue and take a video
void* FSM1(void* arg)
{
	ofstream queue;
	ifstream test_read;

	//Loop forever, checking if we have permission to do a test on each iteration
	while(1)
	{
		//Only do something when the user has requested a test
		test_read.open(test_request_file.c_str());
		while(!test_read.is_open())
		{	system("sleep 1"); test_read.open(test_request_file.c_str());	}	
	

		cout << "T2: received PiUi request" << endl;
	
		//Parse the PiUi test information
		string user, ID, testNum;
		getline(test_read, user);
		getline(test_read, ID);
		getline(test_read, testNum);		

		//Make sure FSM2 doesn't read while we write
		pthread_mutex_lock(&queue_sync);
						
		//Open up the queue and append the new information from PiUi
		queue.open("ID_queue.dat", ofstream::out | ofstream::app);
		queue << user << endl << ID << endl << testNum << endl;
		queue.close();

		test_read.close(); remove(test_request_file.c_str());
		pthread_mutex_unlock(&queue_sync);

		cout << "T2: test information for user " << user << " written to ID queue" << endl;

		//Now that the queue is updated, we want to take the video for the test

		//First, create the new filename for the video
		string video_command = string("sudo python /home/pi/549/PiUi_test_files/run_test.py ") + user + " " + testNum;
		system(video_command.c_str());

		string done_filename = "/home/pi/549/Test_" + testNum + "_done.txt";
		ofstream done_stream(done_filename.c_str());
		done_stream << "DONE WITH TEST" << endl;
		done_stream.close();
	
		cout << "T2: Video test complete for user " << user << endl;	
	}

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
			cout << "T1: nothing found this cycle in queue" << endl;
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

			cout << "T1: found test request for user " << user << " in queue" << endl;
		}

		//Open the queue back up for writing from the other FSM		
		queue.close(); remove("ID_queue.dat"); pthread_mutex_unlock(&queue_sync);

		for(int i = 0; i < queue_contents.size(); i++)
		{
			string done_filename = "/home/pi/549/Test_" + queue_contents[i].testNum+ "_done.txt";	

			//Wait until the video corresponding to this user exists
			string video_filename = "/home/pi/549/Temp_Videos/Test" + queue_contents[i].testNum + "_" + queue_contents[i].user +".h264";
			cout << "T1: Waiting on video file " << video_filename << endl;
			ifstream video(video_filename.c_str());
			ifstream done(done_filename.c_str());
			while(!done.is_open())
			{	system("sleep 1"); done.open(done_filename.c_str());	}
			
			string remove_done_file = "sudo rm " + done_filename;
			done.close(); system(remove_done_file.c_str());
	
			cout << "T1: removing done file " << done_filename << endl;

			

			//Create the metafile for this video
			ofstream metastream("request_header.txt");
			metastream << queue_contents[i].user << endl;
			metastream << queue_contents[i].ID << endl;
			metastream << queue_contents[i].testNum << endl;
			metastream.close();

			cout << "T1: Video file " << video_filename << " found " << endl;
			string scp_command = "scp -c blowfish -i /home/pi/.ssh/id_rsa " + video_filename + " request_header.txt apfeifer@192.168.1.6:/home/apfeifer/549/";
			system(scp_command.c_str());

			//Delete the video file once it's sent
			string delete_command = "sudo rm " + video_filename;
			system(delete_command.c_str());

			//Synchronize with the basestation - get the final results	
			video.open(basestation_results_file.c_str());
			cout << "T1: Waiting on basestation results file" << endl;
			while(!video.is_open())
			{	system("sleep 1"); video.open(basestation_results_file.c_str());	}
			video.close();			

			//Either continue with the next element in the queue, or start the cyclic executive again
			//But first, file away the results file

			//Format as: USERNAME_TESTNUM_BASELINE?.txt
			string cache_command = "sudo mv " + basestation_results_file + " " + results_file_cache_directory + queue_contents[i].user + "_" + queue_contents[i].testNum + queue_contents[i].ID + ".txt"; 
			system(cache_command.c_str());
		}

		//Empty the local queue cache
		queue_contents.clear();
	}

	return NULL;
}
