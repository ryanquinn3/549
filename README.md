Rapid Occular Concussion Testing
===
Repository for 18-549 Embedded Systems Engineering Project

ROCT is a simple occular headset, outfitted with a raspberry pi, that allows medical staff to perform occular tests
on athletes suspected of being concussed. 

Physical Components:
* __Headset__ - Pair of goggles, raspberry pi, lithium battery, 10 LED's, camera
* __Base Station__ - Laptop that uses OpenCV to process camera feed from headset
* __Tablet__ - Used as the remote control for ROCT. Accepts users information, initiates tests, displays results

Software Components:
* __PiUi Web Application__ - Uses nginx server running off raspi to deliver controlling interface to medical staff via our tablet. Used to register athletes, initiate occular tests, and display results. Web app written in python.
* __Base Station__ - Utilizing OpenCV, base station accepts video feeds over secure wifi connection and processes images in order to track size and position of users pupils over time.
* __Control FSM__ - Multi-threaded C++ program that queues up video files from pi's camera to be sent over the to the base station for processing. Accepts response back from base station and stores results file to be graphed by webapp. Allows for medical staff to queue up multiple tests in a row without the need to wait for response from base station (which takes some time to process video).




