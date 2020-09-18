Implementation of Messenger Application using MQTT
Author: Aakash Preetam
Date: 24th February 2020

About:
Messenger Application using MQTT. It uses publish-subscribe model of MQTT.
Application supports the following features:
1.	User Registration on the application
2.	One on One chat between two users.
3.	Group chat where the participants can join and leave at any time. Only the current participants of the group will be able to receive the messages sent by any of the current participants of the group.

Design:
•	First Step is User Registration, User is asked to login or to register if user is new.
For a New User, user chooses the option for new user registration.
User provides the username and password, if the username and password already exist in the program record, user is notified of the same and the user continues to the chat function.
If the username and password is new, user is registered, and the user continues to the chat function.
Existing user chooses the first option to login, User provides the username and password. If the username and password exist in the record, user continues to chat function else user is asked if the user wants to create a new username. If the user chooses yes option, a new username is created, and user continues to chat function.
If the user chooses No option, program is terminated.
•	After User Registration, User is presented with multiple options to choose from
o	Press 1: To Join Group
o	Press 2: To Leave Group 
o	Press 3: Enter Chat :                     //Select this option for One to One Chat or Group Chat
o	Press 4: Logout 
o	Press 5: Exit Chat App
•	For One to One Chat between two users
User enters chat and select the option to chat with a user.
User must input the username of the other user and the message to be sent to that user.
Recipient of the message will receive the message on their terminal immediately if logged in or if the other user is offline, message will be delivered to the user once the user is logged in.
•	For Group Chat
User enters chat and select the option to chat in a group.
User must input the group name of the group and the message to be sent to the group.
Group members will receive the message on their terminal immediately if logged in or if the group member is offline, message will be delivered to the group member once the group member is logged in.
•	Joining Group
User is asked to provide a group name; user enters group name and is subscribed to the group.
Group Members are notified by a message that a new user has joined the group. Group members will receive the message on their terminal immediately if logged in or if the group member is offline, message will be delivered to the group member once the group member is logged in.
•	Leaving Group
User is asked to provide a group name; user enters group name and is unsubscribed from the group.
Group Members are notified by a message that a user has left the group. Group members will receive the message on their terminal immediately if logged in or if the group member is offline, message will be delivered to the group member once the group member is logged in.
•	Logout
User can logout from chat application by selecting logout option. Once the user logs out of chat.
User can login again or a new register can register.
•	Exit Chat Application
User can close the chat application by selecting Exit Chat application option.

Files:
•	”code.cpp” : Source Code 
•	“user.csv” : user registration records are saved in user.csv file. This file is automatically created if not available.




How to Run:
•	Run HiveMQ Broker, make sure that TCP Listener Port is 1884(and is available). 
Verify that HiveMQ is up and running and listening on port 1884.
If Port# 1884 is used by some other process, we can choose some other available port. We need to update this Port number in the program source code also.
By default, HiveMQ binds to port 1883, which is the default MQTT port.
HiveMQ Default Config file is located at: <hivemq_install_directory>/conf/config.xml
Update the port number and other configuration settings in Config file.
•	Install and Build Paho C Client Library for MQTT for Linux
•	Compile the code by running below command in terminal: 
g++ --std=c++14 code.cpp -l paho-mqtt3c -o code
•	To Execute, run this command in terminal: ./code

Environment/Tools:
Operating System: Linux 5.3.0-40-generic #32~18.04.1-Ubuntu x86_64 GNU/Linux
Compiler: GNU C++ Compiler C++14
Broker: HiveMQ 4.2.2
Paho C Client Library for MQTT for Linux
