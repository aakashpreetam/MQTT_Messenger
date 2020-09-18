// TO COMPILE : g++ --std=c++14 code.cpp -l paho-mqtt3c -o code
// TO EXECUTE : ./code

// Implementation of Messenger Application using MQTT 
// Author: Aakash Preetam 
// Date: 24th February 2020 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"     //Includes Paho MQTT C Client - MQTT Client Library Header File
#include <bits/stdc++.h>
//Set Port Number 1884 in <HiveMQ_Install_Directory>/conf/config.xml
#define ADDRESS     "tcp://localhost:1884"  //Server to which the client will connect, using TCP Protocol
#define QOS         2     // Ensuring that Message is Delivered exactly once

volatile MQTTClient_deliveryToken deliveredtoken;

using namespace std;

int existing_user;
string username,password,otheruser;
string p2p("/chat/p2p/");       //Topic string to be used for User to User communication, User name appends to this string to form a Topic
string group("/chat/group/");   //Topic string to be used for User to Group communication, Group name appends to this string to form a Topic
string CLIENTID,TOPIC_S,TOPIC_P,PAYLOAD;
string dummy;
string groupname,TOPIC_G;
int group_flag=0;

//Create New User and Store User Name and Password in user.csv file
void create_user(string username,string password)
{
    std::ofstream file;
    file.open("user.csv", std::ios::out | std::ios::app); // Open user.csv file to store the user credential 
    file << username << "," << password << endl;
    file.close();
}

//Checks if the user is already registered, returns 1 if exists else 0 is returned
int check_existing_user(string username,string password)
{   
    fstream fin;
    vector<string> row;
    string word,temp,line;
    int count=0;

    fin.open("user.csv",ios::in);
    while(fin >> temp)
    {
        row.clear();
        //getline(fin,line,'\n') ;
        line=temp;
        stringstream s(line);
        while(getline(s,word,',')) //Gets each word from the user record
        {
            row.push_back(word);
        }
        if(row[0]==username && row[1]==password) //Compares username and password
        {
            count = 1;
            //cout << row[1] << endl;
            return 1;
        }
    }
    fin.close();
    
    return 0;
}

//Handles user registration
int user_auth()
{

    printf("Press 1, To Login | Press [2-9]: For New User Registration | Press [0,a-z,A-Z,Special Characters], To Quit : ");
    
    //cin >> existing_user;
    scanf("%d",&existing_user);
    if(existing_user==0)
    {
        cout << "Exiting\n";
        exit(0);
    }
    if(existing_user==1)//Checks If Existing User
    {
        printf("Existing User Login\nEnter Your Name(Unique & Space Not Allowed): ");
        cin >> username;
        printf("Password(Space Not Allowed): ");
        cin >> password;

        //Checks if the user is already registered
        int existing_yes=check_existing_user(username,password);

        if(existing_yes==true)  //If User Exists, Continue Chat Function
        {
            cout<<"User Exists\n";
            //Continue to CHAT FUNCTION in main
        }
        else                    //User doesn't exist, Ask to create new user and chat
        {
            cout<<"User Doesn't Exist\n";
            //Asks to Create New User
            cout<<"Do you want to create a New User ? Y/N: \n";
            char f;
            cin >> f;

            if(f=='Y' || f=='y')          //If yes, Create New User
            {
                create_user(username,password);
                cout<<"New User "<<username<<" is Created\n";
                //Continue to CHAT FUNCTION in main
            }
            else                //If No, Program Exits
            {
                cout << "Exiting\n";
                exit(0);
            }
        }
    }
    else                                // Register New User
    {
        printf("New User Registration\nEnter Your Name(Unique & Space Not Allowed): ");
        cin >> username;
        printf("Password: ");
        cin >> password;
        //Verifies if the user is already registered
        int existing_yes=check_existing_user(username,password);
        if(existing_yes==true)  //If User Exists, Continue Chat Function
        {
            cout << "User already exists\n";
            //Continue to CHAT FUNCTION in main
        }
        else
        {
            create_user(username,password);
            cout<<"New User "<<username<<" is Created\n";
            //Continue to CHAT FUNCTION in main
        }
        
    }
    return 0;
}

//Verifies message delivery
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    // printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

//msgarrvd function will be called whenever an MQTT publish message arrives
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    //cout << "Topic Name: " <<topicName << endl;
    // cout << "Topic P" <<TOPIC_P ;
    if(group_flag==1)       //ensures that the user sending the messeage to the group doesn't receive the message
    {
        group_flag=0;
        return 1;
    }

    //Prints the message    
    if(strcmp(topicName,TOPIC_P.c_str())!=0)
    {
        printf("\nIncoming Message ");
        //printf("     topic: %s\n", topicName);
        payloadptr = (char *)message->payload;
	    for(i=0; i<(int)message->payloadlen; i++)
	    {
	        putchar(*payloadptr++);
    	}
	    putchar('\n');
        //printf("Sending: ");
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);
        
        return 1;
    }
    return 1;
}

//connlost function will be called whenever an MQTT publish exchange fails
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause); //Prints the cause of exchange failure
}

int main(int argc, char* argv[])
{   
    //User Registration
    user_auth();
    CLIENTID=username; //Assign User Name as Client Id 

    //Creates CLient Object
    MQTTClient client;
    MQTTClient_deliveryToken token;

    //To define settings for Connection between MQTT Client and Server
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 0;  // Don't want keepalive processing
    conn_opts.cleansession = 0; //Setting false keeps state information

    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    //Creates Client Object
    MQTTClient_create(&client, ADDRESS, CLIENTID.c_str(),MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
    int rc;
    int ch;
    
    //msgarrvd callback will be called whenever an MQTT publish message arrives
    //delivered callback will be called whenever an MQTT publish exchange finishes
    //connlost callback will be called whenever an MQTT publish exchange fails
    //Context Information is set to NULL
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    MQTTClient_subscribe(client, (p2p+CLIENTID).c_str(), QOS);
    while(1)
    {
        cout<<"\nPress 1: To Join Group\n"<<"Press 2: To Leave Group\n"<<"Press 3: Enter Chat\n"<<"Press 4: Logout\nPress 5: Exit Chat App\nChoose: ";
        int x;
        cin >> x;
        if(x==1)
        {
            //Subscribes to the Group
            //string groupname,TOPIC_G;
            cout << "Enter Group Name(Space Not Allowed):" ;
            cin >> groupname;   //gets the name of the group which is user wants to join
            TOPIC_G=group+groupname; // Group Topic string

            printf("Subscribing to Group/Topic %s \nFor Client: %s\n\n", TOPIC_G.c_str(), CLIENTID.c_str());
            
            //Subscribes to the group
            MQTTClient_subscribe(client, TOPIC_G.c_str(), QOS);

            PAYLOAD=": User "+CLIENTID+" has joined the Group "+groupname;
                    //cout << "\nPAYLOAD: " << PAYLOAD;
                    pubmsg.payload = (void*)(PAYLOAD.c_str());
                    pubmsg.payloadlen = strlen(PAYLOAD.c_str());
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    deliveredtoken = 0;
                    group_flag=1;
            
            //Notifies to other members of the group that a new user has joined
            MQTTClient_publishMessage(client, TOPIC_G.c_str(), &pubmsg, &token);
        }
        else if(x==2)
        {
            //Unsubscribe from the Group
            string groupname,TOPIC_G;
            cout << "Enter Group Name(Space Not Allowed):" ;
            cin >> groupname;   //gets the name of the group which the user wants to unsubscribe from
            TOPIC_G=group+groupname;  // Group Topic string

            printf("Unsubscribing from Group/Topic %s \nFor Client: %s\n\n", TOPIC_G.c_str(), CLIENTID.c_str());
            
            PAYLOAD=": User "+CLIENTID+" has left the Group "+groupname;
                    //cout << "\nPAYLOAD: " << PAYLOAD;
                    pubmsg.payload = (void*)(PAYLOAD.c_str());
                    pubmsg.payloadlen = strlen(PAYLOAD.c_str());
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    deliveredtoken = 0;
                    group_flag=1;
            
            //Notifies to other members of the group that user has left
            MQTTClient_publishMessage(client, TOPIC_G.c_str(), &pubmsg, &token);
            
            //User unsubscribe from the group
            MQTTClient_unsubscribe(client, TOPIC_G.c_str());
        }
        else if(x==3)
        {
            //Enter Chat
            cout << "\nSend To: Press 1 for User or Press 2 for Group:\nChoose:";
            int x;
            cin >> x;
            if(x==1)
            {
                //User Chat
                while(1)
                {
                    cout << "Username: ";
                    cin >> otheruser ;
                    TOPIC_P=p2p+otheruser;
                    getline(cin,PAYLOAD);   // Reads NewLine
                    cout << "Message: ";
                    getline(cin,PAYLOAD);
                    //cout << "Topic P: " << TOPIC_P;
                    PAYLOAD="from "+CLIENTID+" : "+PAYLOAD;
                    //cout << "\nPAYLOAD: " << PAYLOAD;
                    pubmsg.payload = (void*)(PAYLOAD.c_str());
                    pubmsg.payloadlen = strlen(PAYLOAD.c_str());
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    deliveredtoken = 0;
                    //Sends message to another user
                    MQTTClient_publishMessage(client, TOPIC_P.c_str(), &pubmsg, &token);
                    cout << "\nPress Q to Come out of Chat, To continue Press X: ";
                    int x;
                    x=getchar();// Reads New Line
                    //x=getchar();
                    if(x=='Q')
                        break;
                }
            }
            else
            {
                //Group Chat
                while(1)
                {
                    cout << "Group Name(Space Not Allowed): ";
                    cin >> groupname;
                    TOPIC_G=group+groupname;
                    getline(cin,PAYLOAD);   // Reads NewLine
                    cout << "Message: ";
                    getline(cin,PAYLOAD);
                    //cout << "Topic G: " << TOPIC_G;
                    PAYLOAD="in Group "+groupname+" from "+CLIENTID+":"+PAYLOAD;
                    //cout << "\nPAYLOAD: " << PAYLOAD;
                    pubmsg.payload = (void*)(PAYLOAD.c_str());
                    pubmsg.payloadlen = strlen(PAYLOAD.c_str());
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    deliveredtoken = 0;
                    group_flag=1;
                    MQTTClient_publishMessage(client, TOPIC_G.c_str(), &pubmsg, &token);
                    cout << "\nPress Q to Come out of Chat, To continue Press X: ";
                    int x;
                    x=getchar();// Reads New Line
                    //x=getchar();
                    if(x=='Q')
                        break;
                }
            }

        }
        else if(x==4)           // Logs out of current user and continue to normal flow of the program, where a new user can continue to login/register and chat
        {   
            MQTTClient_disconnect(client, 10000); // Disconnect MQTT Client from MQTT Server after a timeout period of 10000 milliseconds (10 Seconds)
            MQTTClient_destroy(&client);    // Frees the memory allocated to the Client
            user_auth();    // Resume User Login/Registration
            
            CLIENTID=username;  //Assign User Name as Client Id 
            
            MQTTClient client;
            MQTTClient_deliveryToken token;
            
            //To define settings for Connection between MQTT Client and Server
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            conn_opts.keepAliveInterval = 0;  // Don't want keepalive processing
            conn_opts.cleansession = 0; //Setting false keeps state information

            MQTTClient_message pubmsg = MQTTClient_message_initializer;
            
            //Creates Client Object
            MQTTClient_create(&client, ADDRESS, CLIENTID.c_str(),MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
            int rc;
            int ch;

            //msgarrvd callback will be called whenever an MQTT publish message arrives
            //delivered callback will be called whenever an MQTT publish exchange finishes
            //connlost callback will be called whenever an MQTT publish exchange fails
            //Context Information is set to NULL
            MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
            if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
            {
                printf("Failed to connect, return code %d\n", rc);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            cout << "Exiting\n";
            MQTTClient_disconnect(client, 10000);   // Disconnect MQTT Client from MQTT Server after a timeout period of 10000 milliseconds (10 Seconds)
            MQTTClient_destroy(&client);    // Frees the memory allocated to the Client
            exit(0);    // Exits from the program
        }
    }

    MQTTClient_disconnect(client, 10000);   // Disconnect MQTT Client from MQTT Server after a timeout period of 10000 milliseconds (10 Seconds)
    MQTTClient_destroy(&client);            // Frees the memory allocated to the Client
    return rc;
}