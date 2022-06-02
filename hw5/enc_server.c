//enc_server
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/limits.h>

#define MAX_ALPHABET 26

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 


void encryption(char *key_msg, char* text_msg){





}

//REFERENCED:
// EXPLORATION: PIPES & FIFO
//https://canvas.oregonstate.edu/courses/1883362/pages/exploration-pipes-and-fifo?module_item_id=21941932

//readfile(connectionSocket,buffer,sizeof(buffer));
void readfile_block(int socket_ptr, char* final_msg_buf, size_t buffersize){
  
  int bytesRead, num_bytes_sent = 0;

  //no need to have a large buffer since it will receive bytes in chunks
  char buffer[1024]; 
 // char * temp;
 // char* terminalLoc;

  memset(buffer, '\0', sizeof(buffer));
  
  //printf("CASE 0\n");
  while (strstr(final_msg_buf, "@@") == NULL){
  //while ( (terminalLoc = strstr(final_msg_buf, "\0")) == NULL){

    // Clear the read buffer before reading 
    memset(final_msg_buf, '\0',buffersize);
    
   // printf("TESTSEVER\n");

    // Read the next chunk of data from the pipe
    // We read one byte less than the size of readBuffer because the readBuffer is initalized to '\0' and this way the chunk we read becomes a null terminated string.
    //bytesRead = read(connectionSocket, buffer, sizeof(buffer) - 1);
    bytesRead = recv(socket_ptr, buffer, sizeof(buffer)-1, 0); 
  
 
    // Check for errors
    if (bytesRead > buffersize) { 
      // -1 indicates an error
      perror("final message buffer cant hold enough values");
      exit(1); 
    } 
    if (bytesRead == 0) {
      // 0 indicates an error or end-of-file
      printf("bytesRead == 0\n"); 
    break; 
    }


    // Add the chunk we read to what we have read up to now
    strcat(final_msg_buf, buffer); 
   // printf("Reader: Chunk received: \"%s\" \n", buffer);
   // printf("Reader: Total received till now: \"%s\"\n", final_msg_buf);
  

    num_bytes_sent += bytesRead;
  }

  // Find the location of the terminal indicator @@ and remove it from the message by replacing by the null character
  char* terminalLoc = strstr(final_msg_buf, "@@");
  *terminalLoc = '\0';
 // printf("Reader: Complete string: \"%s\"\n", final_msg_buf);


}

void readfile_nonblock(int socket_ptr, char * final_msg_buf, size_t buffersize){
  char const *verify = "received";
  char temp[strlen(verify)];

  readfile_block(socket_ptr, final_msg_buf, buffersize);
  send(socket_ptr, verify, strlen(verify), 0);
 // return charsRead;

}

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}



int main(int argc, char *argv[]){

  int connectionSocket, charsRead;
  char text_msg[100000];
  char key_msg[100000];
 // char completeMessage[100000];
 // char characters[100000];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  

  // spawnpid = fork();
  // switch (spawnpid){
  // 	case 0:

  // 		//write()
  // 	break;

  // 	default:

  // 	break;

  // }

  // Accept a connection, blocking if one is not available until one connects
  while(1){
    printf("wiating fort diuck\n");
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }
    else{
      printf("CONNECTED\n");
    }

    // printf("SERVER: Connected to client running at host %d port %d\n", 
    //                       ntohs(clientAddress.sin_addr.s_addr),
    //                       ntohs(clientAddress.sin_port));

/*NOTE!!!  
* only in the child server process will the actual encryption take place,
* and the ciphertext be written back: 
* the original server daemon process continues listening for new connections, not encrypting data.
*/

    //once connection is established, initiate the fork process
    int spawnpid = fork();
    int childstatus, bytesRead;
//switch function here

    char const *verify = "received";
    char temp[strlen(verify)];

    switch(spawnpid){
      
      case -1:
        perror("fork failed");
        exit(1);
        break;

      case 0:
        //read and encrypt message

        //printf("READfile before read\n");
        printf("PLAINTEXT msesgae is---------- \n");
        readfile_block(connectionSocket,text_msg,sizeof(text_msg)); 
        //readfile_nonblock(connectionSocket, text_msg, sizeof(text_msg));
       // printf("first readfile is done\n");
        send(connectionSocket, verify, strlen(verify),0);
        printf("plaintext: %s\n", text_msg);


        printf("KEY msesgae is---------- \n");
        readfile_block(connectionSocket, key_msg, sizeof(key_msg));
       // readfile_nonblock(connectionSocket, key_msg, sizeof(key_msg));
        send(connectionSocket, verify, strlen(verify),0);

    
        printf("key: %s\n", key_msg);
        exit(EXIT_SUCCESS);
        break;


      default:

        spawnpid = waitpid(spawnpid, &childstatus, 0);
        printf(" PARENT (%d): CHILD (%d) terminated. exiting\n", getpid(),spawnpid);      
        fflush(stdout);
        
        break;
    }








//*****************
  //   // Get the message from the client and display it
  //   memset(buffer, '\0', 100000);
    
  //   // Read the client's message from the socket
  //   charsRead = recv(connectionSocket, buffer, 100000, 0); 
  //   if (charsRead < 0){
  //     error("ERROR reading from socket");
  //   }
  // //  printf("SERVER: I received this from the client: \"%s\"\n", buffer);
  //     printf("SERVER: received buffer: \"%s\"\n",buffer);  


//start here for receive



    // Send a Success message back to the client
    // charsRead = send(connectionSocket, 
    //               "I am the server, and I got your message", 39, 0); 
    // if (charsRead < 0){
    //   error("ERROR writing to socket");
    // }
    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}

