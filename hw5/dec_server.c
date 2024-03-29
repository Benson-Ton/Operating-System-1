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

struct encrpytion_map{
  int values;
  char letters;
};


//How to check for alphabetic characters
//https://stackoverflow.com/questions/31010152/function-to-check-for-alphabetic-characters
void de_encryption(char *key_msg, char* text_msg){

  char encypted_letter;
  int index = 0;

//note the message is comming in with no newline and with a null terminator 

  do{

    if(text_msg[index] == ' '){

      //replace with value of 26 since A-Z plus space so 26 is space
      text_msg[index] = MAX_ALPHABET; 
    }
    else{

      //need to subtract the ASCII value from the beginning which is A
      text_msg[index] = text_msg[index] - 65;
    }

   if(key_msg[index] == ' '){

      //replace with value of 26 since A-Z plus space so 26 is space
      key_msg[index] = MAX_ALPHABET;
    }
    else{
      
       //need to subtract the ASCII value from the beginning which is A
      key_msg[index] = key_msg[index] - 65;
    }


    //add them together to for de-encrypting 
    encypted_letter = text_msg[index] - key_msg[index];

    if(encypted_letter < 0){
    	encypted_letter = encypted_letter + MAX_ALPHABET;
    }
    // if(encypted_letter == 32){
    // 	encypted_letter = 32;
    // }

    //mod 26 since the space is accounted for seperatly in the if statement 
    //this process is to revert back to 'A'
    encypted_letter = encypted_letter % (MAX_ALPHABET );

    if(encypted_letter != MAX_ALPHABET){

      //add the ASCII values back to get the correct character
      text_msg[index] = encypted_letter + 65;
    }
    // else if(text_msg[index] == ' '){
    // 	text_msg[index] = ' ';
    // }
    else{
      //add the space if it is not within the alphabet range 
      text_msg[index] = ' ';
    }
    index++;
  }
  //trigger the do while loop until it reaches the null terminator
while(text_msg[index] != '\0'); 

//re-add to the end of the message
text_msg[index] = '\0';

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

  //memset(buffer, '\0', sizeof(buffer));
  memset(final_msg_buf, '\0',buffersize);
  //printf("CASE 0\n");
  while (strstr(final_msg_buf, "@@") == NULL){
  //while ( (terminalLoc = strstr(final_msg_buf, "\0")) == NULL){

    // Clear the read buffer before reading 
    memset(buffer, '\0', sizeof(buffer));
    
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
  

  // Accept a connection, blocking if one is not available until one connects
  while(1){
    //printf("wiating fort diuck\n");
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }
    else{
      //printf("CONNECTED\n");
    }

/*NOTE!!!  
* only in the child server process will the actual encryption take place,
* and the ciphertext be written back: 
* the original server daemon process continues listening for new connections, not encrypting data.
*/

    //once connection is established, initiate the fork process
    int spawnpid = fork();
    int childstatus, bytesRead;

    //junk sending variables to prevent blocking
    char const *verify = "received";
    char temp[strlen(verify)];


    switch(spawnpid){
      
      case -1:
        perror("fork failed");
        exit(1);
        break;

      case 0:
        //read and encrypt message
        memset(text_msg, '\0', sizeof(text_msg));
        memset(key_msg, '\0', sizeof(key_msg));

        //printf("READfile before read\n");
       // printf("PLAINTEXT msesgae is---------- \n");
        readfile_block(connectionSocket,text_msg,sizeof(text_msg)); 
        //readfile_nonblock(connectionSocket, text_msg, sizeof(text_msg));
       // printf("first readfile is done\n");

        // need to send junk mesasge in between reading to prevent blocking 
        send(connectionSocket, verify, strlen(verify),0);
       // printf("plaintext: %s\n", text_msg);
       // printf("length of plaintext: %d \n", strlen(text_msg));


      //  printf("KEY msesgae is---------- \n");
        readfile_block(connectionSocket, key_msg, sizeof(key_msg));
       // readfile_nonblock(connectionSocket, key_msg, sizeof(key_msg));

        // need to send junk mesasge in between reading to prevent blocking 
        send(connectionSocket, verify, strlen(verify),0);

    
        //printf("key: %s\n", key_msg);
      //  printf("length of key: %d \n", strlen(key_msg));

        //testing 
        char temp_text[15] = "HELLO";
        char temp_key[15] = "XMCKL";

        de_encryption(key_msg, text_msg);


      //  printf("AFTER ENCRYPTION\n");
   //     printf("%s\n", text_msg );

        //sending the encryption back
        send(connectionSocket, text_msg, strlen(text_msg),0);
 

        exit(EXIT_SUCCESS);
        break;


      default:

        //wait for child process to finish
        spawnpid = waitpid(spawnpid, &childstatus, 0); 
       // printf(" PARENT (%d): CHILD (%d) terminated. exiting\n", getpid(),spawnpid);      
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


