#include <iostream>
using namespace std;
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
//NOTES FOR TA:
//what do you mean random port(like the port given in ARGS?)
//what IP do i use???
//can server send malinformed packets? does client need to deal with retransmitsions for server?
struct ACK {
        uint16_t opCode;
        uint16_t seshNo;
        uint16_t block;
        uint8_t segment;
    };
struct WRITE {
    uint16_t opCode;
    uint16_t seshNo;
    char fileName[512];
};

struct READ{
    uint16_t opCode;
    uint16_t seshNo;
    char fileName[512];
};

struct ERR {
    uint16_t opCode;
    char message[512];
    uint8_t nullByte;
};

struct DATA {
    uint16_t opCode;
    uint16_t seshNo;
    uint16_t blockNo;
    uint8_t segment;
    char data[1024];
};
int Time1;
int Time2;
int Time3;
int Time4;
int Time5;
int Time6;
int Time7;
int Time8;
int MultiBlock = 1;
struct DATA d1;
struct DATA d2;
struct DATA d3;
struct DATA d4;
struct DATA d5;
struct DATA d6;
struct DATA d7;
struct DATA d8;
int check1 = 0;
int check2 = 0;
int check3 = 0;
int check4 = 0;
int check5 = 0;
int check6 = 0;
int check7 = 0;
int check8 = 0;
int size1 = 0;
int size2 = 0;
int size3 = 0;
int size4 = 0;
int size5 = 0;
int size6 = 0;
int size7 = 0;
int size8 = 0;
int tester =1;
int lastSegment = 0;
int segmentsInUse = 0;
int readyToWrite = 0;
int retransmitTries[8] = {0,0,0,0,0,0,0,0};
void sendAck(int16_t seshNo, int16_t block, int16_t segment, socklen_t length, int Socket, struct sockaddr *client){
    //build ack
    struct ACK a;
    a.opCode = 05;
    a.seshNo = seshNo;
    a.block = block;
    a.segment = segment;

    char ack[7];
    memcpy(ack+0, &a.opCode, 2);
    memcpy(ack+2, &a.seshNo, 2);
    memcpy(ack+4, &a.block, 2);
    memcpy(ack+6, &a.segment, 1);
    
    //string ACK = "05" + seshNo + block + segment;
    //const char * ack = ACK.c_str();
    int num_Bytes = sendto(Socket, ack, sizeof(ack), 0, client, length);
    if(num_Bytes ==-1){
        perror("Send Fail");
        printf("Unsuccessful send for ACK, Terminating...\n");
        exit(1);
    }
    else {
        printf("Sent ack... \n");
    }


}

void sendErr(string message, socklen_t length, int Socket, struct sockaddr *client){
    struct ERR err;
    err.opCode = 06;
    //err.message = message;
    strcpy(err.message, message.c_str()); 
    char error[2+strlen(err.message)+1];
    memcpy(error+0, &err.opCode, 2);
    memcpy(error+2, &err.message, strlen(err.message)+1);

    int num_Bytes = sendto(Socket, error, sizeof(error), 0, client, length);
    if(num_Bytes ==-1){
        perror("Send Fail");
        printf("Unsuccessful send for err, Terminating...\n");
        exit(1);
    }
    else {
        printf("Sent err... \n");
    }
}

int checkAck(int Ack1, int Ack2, int Ack3, int Ack4, int Ack5, int Ack6, int Ack7, int Ack8, int Socket, struct sockaddr *server, socklen_t length ){
    //printf("CHECKING ACK\n");
    printf("%d,%d,%d,%d,%d,%d,%d,%d\n", Ack1,Ack2,Ack3,Ack4,Ack5,Ack6,Ack7,Ack8);
    if ((Ack1 == 0)&& (Ack2 == 0) && (Ack3 == 0) && (Ack4 ==0) && (Ack5==0) && (Ack6==0) && (Ack7==0) && (Ack8==0)){
        printf("ALL ACKS RECIEVED\n");
        for (int i =0; i < 8; i++ ){
            retransmitTries[i] = 0;
        }
        return 1;
    }
    else{
        //check for retransmit
        if (Ack1 == 1){
            int timeNow = time(NULL);
            if((timeNow - Time1) >=5){
                printf("Packet1 lost, attempting retransmit\n");
                if(retransmitTries[0] < 4){
                    
                    char sendData[7+size1];
                    memcpy(sendData+0, &d1.opCode, 2);
                    memcpy(sendData+2, &d1.seshNo, 2);
                    memcpy(sendData+4, &d1.blockNo, 2);
                    memcpy(sendData+6, &d1.segment, 1);
                    memcpy(sendData+7, &d1.data, size1);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time1 = time(NULL);
                    }
                    retransmitTries[0] = retransmitTries[0]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[0] = 0;
        }
        if (Ack2 ==1 ){
            int timeNow = time(NULL);
            //printf("TIMENOW: %d\n", timeNow);
            //printf("TIME2: %d\n", Time2);
            //printf("STRLEN of d1%d\n", strlen(d1.data))
            if((timeNow - Time2) >=5){
                printf("Packet2 lost, attempting retransmit\n");
                if(retransmitTries[1] < 4){
                   char sendData[7+size2];
                    memcpy(sendData+0, &d2.opCode, 2);
                    memcpy(sendData+2, &d2.seshNo, 2);
                    memcpy(sendData+4, &d2.blockNo, 2);
                    memcpy(sendData+6, &d2.segment, 1);
                    memcpy(sendData+7, &d2.data, size2);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time2 = time(NULL);
                    }
                    retransmitTries[1] = retransmitTries[1]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else {
            retransmitTries[1] = 0;
        }
        if (Ack3 ==1 ){
            int timeNow = time(NULL);
            if((timeNow - Time3) >=5){
                printf("Packet3 lost, attempting retransmit\n");
                if(retransmitTries[2] < 4){
                   char sendData[7+size3];
                    memcpy(sendData+0, &d3.opCode, 2);
                    memcpy(sendData+2, &d3.seshNo, 2);
                    memcpy(sendData+4, &d3.blockNo, 2);
                    memcpy(sendData+6, &d3.segment, 1);
                    memcpy(sendData+7, &d3.data, size3);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time3 = time(NULL);
                    }
                    retransmitTries[2] = retransmitTries[2]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[2] = 0;
        }
        if (Ack4 ==1 ){
            int timeNow = time(NULL);
            if((timeNow - Time4) >=5){
                printf("Packet4 lost, attempting retransmit\n");
                if(retransmitTries[3] < 4){
                   char sendData[7+size4];
                    memcpy(sendData+0, &d4.opCode, 2);
                    memcpy(sendData+2, &d4.seshNo, 2);
                    memcpy(sendData+4, &d4.blockNo, 2);
                    memcpy(sendData+6, &d4.segment, 1);
                    memcpy(sendData+7, &d4.data, size4);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time4 = time(NULL);
                    }
                    retransmitTries[3] = retransmitTries[3]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[3] = 0;
        }
        if (Ack5 ==1 ){
            int timeNow = time(NULL);
            if((timeNow - Time5) >=5){
                printf("Packet5 lost, attempting retransmit\n");
                if(retransmitTries[4] < 4){
                   char sendData[7+size5];
                    memcpy(sendData+0, &d5.opCode, 2);
                    memcpy(sendData+2, &d5.seshNo, 2);
                    memcpy(sendData+4, &d5.blockNo, 2);
                    memcpy(sendData+6, &d5.segment, 1);
                    memcpy(sendData+7, &d5.data, size5);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time5 = time(NULL);
                    }
                    retransmitTries[4] = retransmitTries[4]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[4] = 0;
        }
        if (Ack6 ==1 ){
            int timeNow = time(NULL);
            if((timeNow - Time6) >=5){
                printf("Packet6 lost, attempting retransmit\n");
                if(retransmitTries[5] < 4){
                   char sendData[7+size6];
                    memcpy(sendData+0, &d6.opCode, 2);
                    memcpy(sendData+2, &d6.seshNo, 2);
                    memcpy(sendData+4, &d6.blockNo, 2);
                    memcpy(sendData+6, &d6.segment, 1);
                    memcpy(sendData+7, &d6.data, size6);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time6 = time(NULL);
                    }
                    retransmitTries[5] = retransmitTries[5]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[5] = 0;
        }
        if (Ack7 ==1 ){
            int timeNow = time(NULL);
            if((timeNow - Time7) >=5){
                printf("Packet7 lost, attempting retransmit\n");
                if(retransmitTries[6] < 4){
                   char sendData[7+size7];
                    memcpy(sendData+0, &d7.opCode, 2);
                    memcpy(sendData+2, &d7.seshNo, 2);
                    memcpy(sendData+4, &d7.blockNo, 2);
                    memcpy(sendData+6, &d7.segment, 1);
                    memcpy(sendData+7, &d7.data, size7);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time7 = time(NULL);
                    }
                    retransmitTries[6] = retransmitTries[6]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[6] = 0;
        }
        if (Ack8 ==1 ){
            int timeNow = time(NULL);
            if((timeNow - Time8) >=5){
                printf("Packet8 lost, attempting retransmit\n");
                if(retransmitTries[7] < 4){
                   char sendData[7+size8];
                    memcpy(sendData+0, &d8.opCode, 2);
                    memcpy(sendData+2, &d8.seshNo, 2);
                    memcpy(sendData+4, &d8.blockNo, 2);
                    memcpy(sendData+6, &d8.segment, 1);
                    memcpy(sendData+7, &d8.data, size8);

                    int num_Bytes = sendto(Socket, sendData, sizeof(sendData), 0, server, length);
                    if(num_Bytes ==-1){
                        perror("Send Fail");
                        printf("Unsuccessful send from Client, Terminating...\n");
                        exit(1);
                    }
                    else {
                        printf("Sent Data packet... \n");
                        Time8 = time(NULL);
                    }
                    retransmitTries[7] = retransmitTries[7]+1;
                }
                else{
                    printf("Tried 3 times... Packet lost, file transfer terminating..");
                    string message = "Tried 3 times... Packet lost, file transfer terminating..";
                    sendErr(message, length, Socket, server);
                    exit(1);
                }
            }
        }
        else{
            retransmitTries[7] = 0;
        }
        return 0;
    }
}
void sendWriteRequest(uint16_t seshNo, string filename, socklen_t length, int Socket, struct sockaddr *server){
    //build writeReq
    struct WRITE w;
    w.opCode = 03;
    w.seshNo = seshNo;
    strcpy(w.fileName, filename.c_str());

    char write[4+strlen(w.fileName)+1];
    memcpy(write+0, &w.opCode, 2);
    memcpy(write+2, &w.seshNo, 2);
    memcpy(write+4, &w.fileName, strlen(w.fileName)+1);

    int num_Bytes = sendto(Socket, write, sizeof(write), 0, server, length);
        if(num_Bytes ==-1){
            perror("Send Fail");
            printf("Unsuccessful send from Client, Terminating...\n");
            exit(1);
        }
        else {
            printf("Sent write request packet... \n");
        }

}

void sendReadRequest(uint16_t seshNo, string filename, socklen_t length, int Socket, struct sockaddr *server){
    //build writeReq
    struct READ r;
    r.opCode = 02;
    r.seshNo = seshNo;
    strcpy(r.fileName, filename.c_str());

    char read[4+strlen(r.fileName)+1];
    memcpy(read+0, &r.opCode, 2);
    memcpy(read+2, &r.seshNo, 2);
    memcpy(read+4, &r.fileName, strlen(r.fileName)+1);

    int num_Bytes = sendto(Socket, read, sizeof(read), 0, server, length);
        if(num_Bytes ==-1){
            perror("Send Fail");
            printf("Unsuccessful send from Client, Terminating...\n");
            exit(1);
        }
        else {
            printf("Sent read request packet... \n");
        }

}
extern int errno;
void perror(const char *s);
int main(int argc, char* argv[]){
    //check for arguments later
        if (argc !=4){
            cout << "NOT ENOUGH ARGUMENTS TERMINATING..." << endl;
            exit(1);
        }

    //parse everything
    char *arg2 = argv[1];
    char * split;
    char * UN;
    char * PW;
    char * IP;
    char * PORT;
    int count = 0;
    split = strtok (arg2, "@:");
    while (split != NULL){
        if (count ==0){
            UN = split;
        }
        if (count == 1){
            PW = split;
        }
        if (count ==2){
            PORT = split;
        }
        if (count == 3){
            IP = split;
        }
        split = strtok(NULL, "@:");
        count = count +1;
    }
    //printf("PW: %s\n", PW);

    char * type = argv[2];

    char * fileName = argv[3];

    stringstream st;
    st << PORT;

    int port;
    st >> port;

    //set up EFTP packet structures:
    string user = UN;
    string pass = PW;
    //NEED TO CHANGE AUTH CALL
    string auth = "01" + user + "0" + pass + "0";
    //cout << auth;
    const char * a =auth.c_str();
    
    int mySocket;
    mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(mySocket == -1){
        printf("Socket not set up properly, termintating...");
        exit(1);
    }

    int status;
    printf("PORT PORT PORT:%d\n", port);
    struct sockaddr_in ip_server;
    struct sockaddr *server;
    memset ((char*) &ip_server, 0, sizeof(ip_server));
    ip_server.sin_family = AF_INET;
    ip_server.sin_port = htons(port);
    ip_server.sin_addr.s_addr = inet_addr(IP);

    server = (struct sockaddr *) &ip_server;

    //send authentication:

    //create authentication packet:
    
    //printf("Auth: %s\n", a);
    socklen_t length;
    length = sizeof(ip_server);
    int num_Bytes = sendto(mySocket, (char *)a, strlen(a)+1, 0, server, length);
    if(num_Bytes ==-1){
        perror("Send Fail");
        printf("Unsuccessful send for AUTH, Terminating...\n");
        exit(1);
    }
    else {
        printf("Sent authentication packet... \n");
    }
    //wait for either error or ACK
    char buffer[1024];
    int recieved_Bytes = recvfrom(mySocket, (char *)buffer, sizeof(buffer), 0, server, &length);
    if(recieved_Bytes == -1){
        printf("Unsuccessful recv, Terminating... \n");
        exit(1);
    }
    else{
        printf("Recieved response from server...");
        printf("Data: %s\n", buffer);

    }
    int16_t opCodeBeforeLoop;
    memcpy(&opCodeBeforeLoop, buffer, 2);
    printf("opCode: %d \n", opCodeBeforeLoop);
    int16_t seshNo;
    if (opCodeBeforeLoop == 5){
         printf("Received ACK from server...\n");
         struct ACK a;
         memcpy(&a, buffer, sizeof(a));
         cout << a.seshNo << endl;
         seshNo = a.seshNo;
    }
    if(opCodeBeforeLoop == 6){
        printf("Error:");
        struct ERR e;
        memcpy(&e, buffer, sizeof(e));
        cout << e.message << endl;
        exit(1);
    }
    

    //start read or write phase
    string request = type;
    string file = fileName;
    int RSeg1 = 0;
    int RSeg2 = 0;
    int RSeg3 = 0;
    int RSeg4 = 0;
    int RSeg5 = 0;
    int RSeg6 = 0;
    int RSeg7 = 0;
    int RSeg8 = 0;
    long fileCounter = 0;
    char binary[1024];
    FILE * f;
    long fileSize;
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 100000;
    setsockopt(mySocket,SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
    if (request == "upload"){
        //build packet
        //string writeRequest = "03" + seshNo + file + "0";
        //const char * WR = writeRequest.c_str();
        sendWriteRequest(seshNo, file, length, mySocket, server);
        f = fopen(fileName, "rb");
    }
    else if (request == "download"){
       sendReadRequest(seshNo, file, length, mySocket, server);
       f = fopen(fileName, "wb");
    }
    else {
        printf("request is not valid, terminating...");
        exit(1);
    }

    //client now listens while true
    while(1){
        //printf("Waiting for response from server....\n");
        int check = checkAck(RSeg1,RSeg2,RSeg3,RSeg4,RSeg5,RSeg6,RSeg7,RSeg8, mySocket,server,length);
        //buffer needs to be 1024 + 7 = 1031
        char buffer[1032];
        bzero(buffer, 1032);
        int recieved_Bytes = recvfrom(mySocket, (char *)buffer, sizeof(buffer), 0, server, &length);
        if(recieved_Bytes == -1){
            //just keep waiting
        }
        else{
            printf("Recieved %d bytes from server...\n", recieved_Bytes);
            //printf("Data: %d\n", buffer);

            //parse the data
            char codeBuff[2];
            int16_t opCode;
            memcpy(&opCode, buffer, 2);
            printf("opCode: %d \n", opCode);

            if (opCode == 5){
                printf("Ack packet recieved\n");
                //start data transfer if write request
                struct ACK a;
                memcpy(&a, buffer, sizeof(a));

                
                uint8_t loopCounter = 1;
                
                if (a.segment ==0){
                    //we know this ack is to acknowledge a write requst and not a segment
                    printf("Write Request Recieved.. Sending Data");
                    //determine file size
                    fseek(f, 0, SEEK_END);
                    fileSize = ftell(f);
                    rewind (f);
                }
                else if (a.segment ==1){
                    //received segment 1
                   RSeg1 = 0;
                }
                else if (a.segment ==2){
                    RSeg2 = 0;
                }
                else if (a.segment == 3){
                    RSeg3 = 0;
                }
                else if (a.segment ==4){
                    RSeg4 =0;
                }
                else if (a.segment == 5){
                    RSeg5 =0;
                }
                else if (a.segment ==6){
                    RSeg6=0;
                }
                else if (a.segment ==7){
                    RSeg7 = 0;
                }
                else if (a.segment ==8){
                    RSeg8 =0;
                   
                }
                else{
                    printf("Something went wrong...");
                }

                //int check = checkAck(RSeg1,RSeg2,RSeg3,RSeg4,RSeg5,RSeg6,RSeg7,RSeg8, mySocket,server,length);
                loopCounter = 1;
                check = checkAck(RSeg1,RSeg2,RSeg3,RSeg4,RSeg5,RSeg6,RSeg7,RSeg8, mySocket,server,length);
                 if ((MultiBlock !=0) && (check ==1)){
                 if (f){
                        bzero(binary, 1024);
                        while (fileCounter <= fileSize){
                             if(loopCounter ==1){
                                    RSeg1 = 1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time1 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d1.data, 1024);
                                        d1.opCode = 04;
                                        d1.seshNo = seshNo;
                                        d1.blockNo = MultiBlock;
                                        d1.segment = loopCounter;
                                        memcpy(d1.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d1.opCode, 2);
                                        memcpy(sendData+2, &d1.seshNo, 2);
                                        memcpy(sendData+4, &d1.blockNo, 2);
                                        memcpy(sendData+6, &d1.segment, 1);
                                        memcpy(sendData+7, &d1.data, (fileSize -fileCounter));
                                        cout <<d1.data<<endl;
                                        size1 = (fileSize-fileCounter);
                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d1.opCode = 04;
                                        d1.seshNo = seshNo;
                                        d1.blockNo = MultiBlock;
                                        d1.segment = loopCounter;
                                        memcpy(d1.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d1.opCode, 2);
                                        memcpy(sendData+2, &d1.seshNo, 2);
                                        memcpy(sendData+4, &d1.blockNo, 2);
                                        memcpy(sendData+6, &d1.segment, 1);
                                        memcpy(sendData+7, &d1.data, 1024);
                                         size1 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                    
                                }
                                else if (loopCounter ==2){
                                    RSeg2 = 1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time2= time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d2.data, 1024);
                                        d2.opCode = 04;
                                        d2.seshNo = seshNo;
                                        d2.blockNo = MultiBlock;
                                        d2.segment = loopCounter;
                                        memcpy(d2.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d2.opCode, 2);
                                        memcpy(sendData+2, &d2.seshNo, 2);
                                        memcpy(sendData+4, &d2.blockNo, 2);
                                        memcpy(sendData+6, &d2.segment, 1);
                                        memcpy(sendData+7, &d2.data, (fileSize -fileCounter));
                                        cout <<d2.data<<endl;
                                        size2 = (fileSize-fileCounter);

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d2.opCode = 04;
                                        d2.seshNo = seshNo;
                                        d2.blockNo = MultiBlock;
                                        d2.segment = loopCounter;
                                        memcpy(d2.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d2.opCode, 2);
                                        memcpy(sendData+2, &d2.seshNo, 2);
                                        memcpy(sendData+4, &d2.blockNo, 2);
                                        memcpy(sendData+6, &d2.segment, 1);
                                        memcpy(sendData+7, &d2.data, 1024);
                                         size2 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else if (loopCounter ==3){
                                    RSeg3 = 1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time3 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d3.data, 1024);
                                        d3.opCode = 04;
                                        d3.seshNo = seshNo;
                                        d3.blockNo = MultiBlock;
                                        d3.segment = loopCounter;
                                        memcpy(d3.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d3.opCode, 2);
                                        memcpy(sendData+2, &d3.seshNo, 2);
                                        memcpy(sendData+4, &d3.blockNo, 2);
                                        memcpy(sendData+6, &d3.segment, 1);
                                        memcpy(sendData+7, &d3.data, (fileSize -fileCounter));
                                        size3 = (fileSize - fileCounter);
                                        cout <<d3.data<<endl;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d3.opCode = 04;
                                        d3.seshNo = seshNo;
                                        d3.blockNo = MultiBlock;
                                        d3.segment = loopCounter;
                                        memcpy(d3.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d3.opCode, 2);
                                        memcpy(sendData+2, &d3.seshNo, 2);
                                        memcpy(sendData+4, &d3.blockNo, 2);
                                        memcpy(sendData+6, &d3.segment, 1);
                                        memcpy(sendData+7, &d3.data, 1024);
                                        size3 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else if (loopCounter ==4){
                                    RSeg4 = 1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time4 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d4.data, 1024);
                                        d4.opCode = 04;
                                        d4.seshNo = seshNo;
                                        d4.blockNo = MultiBlock;
                                        d4.segment = loopCounter;
                                        memcpy(d4.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d4.opCode, 2);
                                        memcpy(sendData+2, &d4.seshNo, 2);
                                        memcpy(sendData+4, &d4.blockNo, 2);
                                        memcpy(sendData+6, &d4.segment, 1);
                                        memcpy(sendData+7, &d4.data, (fileSize -fileCounter));
                                        size4 = (fileSize-fileCounter);
                                        cout <<d4.data<<endl;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d4.opCode = 04;
                                        d4.seshNo = seshNo;
                                        d4.blockNo = MultiBlock;
                                        d4.segment = loopCounter;
                                        memcpy(d4.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d4.opCode, 2);
                                        memcpy(sendData+2, &d4.seshNo, 2);
                                        memcpy(sendData+4, &d4.blockNo, 2);
                                        memcpy(sendData+6, &d4.segment, 1);
                                        memcpy(sendData+7, &d4.data, 1024);
                                        size4 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else if (loopCounter ==5){
                                    RSeg5 = 1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time5 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d5.data, 1024);
                                        d5.opCode = 04;
                                        d5.seshNo = seshNo;
                                        d5.blockNo = MultiBlock;
                                        d5.segment = loopCounter;
                                        memcpy(d5.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d5.opCode, 2);
                                        memcpy(sendData+2, &d5.seshNo, 2);
                                        memcpy(sendData+4, &d5.blockNo, 2);
                                        memcpy(sendData+6, &d5.segment, 1);
                                        memcpy(sendData+7, &d5.data, (fileSize -fileCounter));
                                        size5 = (fileSize - fileCounter);
                                        cout <<d5.data<<endl;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d5.opCode = 04;
                                        d5.seshNo = seshNo;
                                        d5.blockNo = MultiBlock;
                                        d5.segment = loopCounter;
                                        memcpy(d5.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d5.opCode, 2);
                                        memcpy(sendData+2, &d5.seshNo, 2);
                                        memcpy(sendData+4, &d5.blockNo, 2);
                                        memcpy(sendData+6, &d5.segment, 1);
                                        memcpy(sendData+7, &d5.data, 1024);
                                        size5 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else if (loopCounter ==6){
                                    RSeg6 = 1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time6 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d6.data, 1024);
                                        d6.opCode = 04;
                                        d6.seshNo = seshNo;
                                        d6.blockNo = MultiBlock;
                                        d6.segment = loopCounter;
                                        memcpy(d6.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d6.opCode, 2);
                                        memcpy(sendData+2, &d6.seshNo, 2);
                                        memcpy(sendData+4, &d6.blockNo, 2);
                                        memcpy(sendData+6, &d6.segment, 1);
                                        memcpy(sendData+7, &d6.data, (fileSize -fileCounter));
                                        size6 = (fileSize-fileCounter);
                                        cout <<d6.data<<endl;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d6.opCode = 04;
                                        d6.seshNo = seshNo;
                                        d6.blockNo = MultiBlock+1;
                                        d6.segment = loopCounter;
                                        memcpy(d6.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d6.opCode, 2);
                                        memcpy(sendData+2, &d6.seshNo, 2);
                                        memcpy(sendData+4, &d6.blockNo, 2);
                                        memcpy(sendData+6, &d6.segment, 1);
                                        memcpy(sendData+7, &d6.data, 1024);
                                        size6 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else if (loopCounter ==7){
                                    RSeg7 ==1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time7 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d7.data, 1024);
                                        d7.opCode = 04;
                                        d7.seshNo = seshNo;
                                        d7.blockNo = MultiBlock;
                                        d7.segment = loopCounter;
                                        memcpy(d7.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d7.opCode, 2);
                                        memcpy(sendData+2, &d7.seshNo, 2);
                                        memcpy(sendData+4, &d7.blockNo, 2);
                                        memcpy(sendData+6, &d7.segment, 1);
                                        memcpy(sendData+7, &d7.data, (fileSize -fileCounter));
                                        size7 = (fileSize-fileCounter);
                                        cout <<d7.data<<endl;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d7.opCode = 04;
                                        d7.seshNo = seshNo;
                                        d7.blockNo = MultiBlock;
                                        d7.segment = loopCounter;
                                        memcpy(d7.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d7.opCode, 2);
                                        memcpy(sendData+2, &d7.seshNo, 2);
                                        memcpy(sendData+4, &d7.blockNo, 2);
                                        memcpy(sendData+6, &d7.segment, 1);
                                        memcpy(sendData+7, &d7.data, 1024);
                                        size7 =1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else if (loopCounter ==8){
                                    RSeg8 ==1;
                                    bzero(binary, 1024);
                                    printf("filesize: %d", fileSize);
                                    //set time 
                                    Time8 = time(NULL);
                                    if ((fileSize -fileCounter) < 1024){
                                        //last segment
                                        fread(binary, 1, (fileSize -fileCounter), f);
                                        cout <<binary << endl;
                                        //send segment
                                        printf("Sending segment less than 1024\n");
                                        //build
                                        bzero(d8.data, 1024);
                                        d8.opCode = 04;
                                        d8.seshNo = seshNo;
                                        d8.blockNo = MultiBlock;
                                        d8.segment = loopCounter;
                                        memcpy(d8.data, binary, (fileSize -fileCounter));

                                        char sendData[7+(fileSize-fileCounter)];
                                        memcpy(sendData+0, &d8.opCode, 2);
                                        memcpy(sendData+2, &d8.seshNo, 2);
                                        memcpy(sendData+4, &d8.blockNo, 2);
                                        memcpy(sendData+6, &d8.segment, 1);
                                        memcpy(sendData+7, &d8.data, (fileSize -fileCounter));
                                        size8 = (fileSize - fileCounter);
                                        cout <<d8.data<<endl;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }

                                    
                                        MultiBlock = 0;
                                        break;

                                    }
                                    else{
                                        printf("sending full segment..\n");
                                        fread(binary, 1, 1024, f);

                                        fileCounter = fileCounter + 1024;
                                        
                                        fseek(f, fileCounter, SEEK_SET);
                                        d8.opCode = 04;
                                        d8.seshNo = seshNo;
                                        d8.blockNo = MultiBlock;
                                        d8.segment = loopCounter;
                                        memcpy(d8.data, binary, 1024);
                                        //printf("Segment: %d",sizeof(d.segment));

                                        char sendData[7+1024];
                                        memcpy(sendData+0, &d8.opCode, 2);
                                        memcpy(sendData+2, &d8.seshNo, 2);
                                        memcpy(sendData+4, &d8.blockNo, 2);
                                        memcpy(sendData+6, &d8.segment, 1);
                                        memcpy(sendData+7, &d8.data, 1024);
                                        size8 = 1024;

                                        int num_Bytes = sendto(mySocket, sendData, sizeof(sendData), 0, server, length);
                                            if(num_Bytes ==-1){
                                                perror("Send Fail");
                                                printf("Unsuccessful send from Client, Terminating...\n");
                                                exit(1);
                                            }
                                            else {
                                                printf("Sent Data packet... \n");
                                            }
                                        }

                                }
                                else {
                                    //over limit
                                    MultiBlock = MultiBlock+1;
                                    break;

                                }
                            
                            loopCounter = loopCounter +1;
                                
                                
                        }

                    }
                    }
              
                

            }
            else if (opCode == 6){
                struct ERR e;
                memcpy(&e, buffer, sizeof(e));
                cout << e.message << endl;
                exit(1);
                //terminate server
            }
            else if (opCode == 4){
                
                //data transfer
                printf("Recieved %d Bytes of Data...\n", recieved_Bytes);
                //write data 
                    struct DATA d;
                    bzero(d.data,1024);
                    memcpy(&d, buffer, sizeof(d));

                if(d.segment == 1){
                    //demo retransmit
                    if (tester ==0){
                        check1 =0;
                        
                    }
                    else{
                        bzero(d1.data,1024);
                        memcpy(&d1, buffer, sizeof(d1));
                        printf("OpCode: %d\n", d1.opCode);
                        printf("SeshNo: %d\n", d1.seshNo);
                        printf("Block: %d\n", d1.blockNo);
                        printf("Segment: %d\n", d1.segment);
                        printf("Data: %s\n", d1.data);
                        printf("SIZE OF DATA: %d\n", strlen(d1.data));
                        size1 = recieved_Bytes-7;
                        check1 = 1;
                        
                        if (size1 < 1024){
                            lastSegment = 1;
                            readyToWrite = 1;
                        }
                        
                    }
                    
                    
                }
                else if (d.segment == 2){
                    bzero(d2.data,1024);
                    memcpy(&d2, buffer, sizeof(d2));
                    printf("OpCode: %d\n", d2.opCode);
                    printf("SeshNo: %d\n", d2.seshNo);
                    printf("Block: %d\n", d2.blockNo);
                    printf("Segment: %d\n", d2.segment);
                    printf("Data: %s\n", d2.data);
                    printf("SIZE OF DATA: %d\n", strlen(d2.data));
                    size2 = recieved_Bytes-7;
                    check2 =1;
                    
                    if (size2 < 1024){
                            lastSegment = 1;
                            readyToWrite =1;
                        }
                }
                else if (d.segment ==3){
                    bzero(d3.data,1024);
                    memcpy(&d3, buffer, sizeof(d3));
                    size3 = recieved_Bytes-7;
                    check3 =1;
                    
                    if (size3 < 1024){
                            lastSegment = 1;
                            readyToWrite = 1;
                        }
                   
                }
                else if(d.segment ==4){
                    bzero(d4.data,1024);
                    memcpy(&d4, buffer, sizeof(d4));
                    size4 = recieved_Bytes-7;
                    check4 =1;
                    
                    if (size4 < 1024){
                            lastSegment = 1;
                            readyToWrite = 1;
                        }
                    
                }
                else if(d.segment ==5){
                    bzero(d5.data,1024);
                    memcpy(&d5, buffer, sizeof(d5));
                    size5 = recieved_Bytes-7;
                    check5 =1;
                    
                    if (size5 < 1024){
                            lastSegment = 1;
                            readyToWrite = 1;
                        }
                    
                }
                else if(d.segment ==6){
                    bzero(d6.data,1024);
                    memcpy(&d6, buffer, sizeof(d6));
                    size6 = recieved_Bytes-7;
                    check6 =1;
                    
                    if (size6 < 1024){
                            lastSegment = 1;
                            readyToWrite =1;
                        }
                    
                }
                else if(d.segment ==7){
                    bzero(d7.data,1024);
                    memcpy(&d7, buffer, sizeof(d7));
                    size7= recieved_Bytes-7;
                    check7 =1;
                    
                    if (size7 < 1024){
                            lastSegment = 1;
                            readyToWrite = 1;
                        }
                   
                }
                else if(d.segment ==8){
                    bzero(d8.data,1024);
                    memcpy(&d8, buffer, sizeof(d8));
                    size8 = recieved_Bytes-7;
                    check8 =1;
                    readyToWrite = 1;
                    
                    if (size8 < 1024){
                            lastSegment = 1;
                        }
                    
                }
                if (segmentsInUse < d.segment){
                    segmentsInUse = d.segment;
                }
               
               
                //fseek(f, sizeof(d.data), SEEK_SET);
                //send ack back
                if (readyToWrite){
                    int checkSeg =1;
                    if((check1 ==0) && ((segmentsInUse - 1) >=0 )){
                        checkSeg = 0;
                    }
                    if((check2 ==0) && ((segmentsInUse - 2) >=0 )){
                        checkSeg = 0;
                    }
                    if((check3 ==0) && ((segmentsInUse - 3) >=0 )){
                        checkSeg = 0;
                    }
                     if((check4 ==0) && ((segmentsInUse - 4) >=0 )){
                        checkSeg = 0;
                    }
                     if((check5 ==0) && ((segmentsInUse - 5) >=0 )){
                        checkSeg = 0;
                    }
                     if((check6 ==0) && ((segmentsInUse - 6) >=0 )){
                        checkSeg = 0;
                    }   
                     if((check7 ==0) && ((segmentsInUse - 7) >=0 )){
                        checkSeg = 0;
                    }
                     if((check8 ==0) && ((segmentsInUse - 8) >=0 )){
                        checkSeg = 0;
                    }
                    //check to see if all segments are here
                    if (checkSeg ==1){
                        printf("WRITING>>>>>>>\n");
                        if (((segmentsInUse - 1) >=0 )){
                            printf("writing segment 1");
                            fwrite(d1.data, 1, size1, f);
                        }
                        if (((segmentsInUse - 2) >=0 )){
                            fwrite(d2.data, 1, size2, f);
                        }
                        if (((segmentsInUse - 3) >=0 )){
                            fwrite(d3.data, 1, size3, f);
                        }
                        if (((segmentsInUse - 4) >=0 )){
                            fwrite(d4.data, 1, size4, f);
                        }
                        if (((segmentsInUse - 5) >=0 )){
                            fwrite(d5.data, 1, size5, f);
                        }
                        if (((segmentsInUse - 6) >=0 )){
                            fwrite(d6.data, 1, size6, f);
                        }
                        if (((segmentsInUse - 7) >=0 )){
                            fwrite(d7.data, 1, size7, f);
                        }
                        if (((segmentsInUse - 8) >=0 )){
                            fwrite(d8.data, 1, size8, f);
                        }

                        check1 = 0;
                        check2 = 0;
                        check3 = 0;
                        check4 = 0;
                        check5 = 0;
                        check6 = 0;
                        check7 = 0;
                        check8 = 0;
                        segmentsInUse=0;
                        readyToWrite = 0;
                        if (lastSegment){
                            printf("CLOSING");
                            fclose(f);
                        }
                        
                    }
                }
                    

                    //reset
                    
                if (tester ==0){
                    tester =1;
                    //testing retransmit, dont send ack to fake packet loss
                }
                else{
                    sendAck(0,d.blockNo,d.segment,length,mySocket,server);

                }
                
            }

        }


    }



}