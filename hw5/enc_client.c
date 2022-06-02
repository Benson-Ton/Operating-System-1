//enc_client
//Sources:

// ASCII TABLE
//https://www.cs.cmu.edu/~pattis/15-1XX/common/handouts/ascii.html

//Perror and fopen
// https://www.tutorialspoint.com/c_standard_library/c_function_perror.htm

//PIPES & FIFO Module week 8
//https://canvas.oregonstate.edu/courses/1883362/pages/exploration-pipes-and-fifo?module_item_id=21941932

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <fcntl.h> //open()
/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 


//checks the characters of the plaintext and keygen files
void verify_characters(char* input, char* key)
{

  //run through the input file
  for(int i = 0; i < strlen(input)-1; i++){
    
    //makes sure all of the characters are Capitalized
    if( (int)input[i] > 90 || (int)input[i] < 65 ){
 
      //ignore the space
      if(input[i] != ' '){
        fprintf(stderr, "Input includes an invalid character\n");
        exit(EXIT_FAILURE);
      }
    }
  }

//run through the key file
  for(int i = 0; i < strlen(key)-1; i++){ // -1 ignores the null 
    
    //makes sure all of the characters are Capitalized
    if( (int)key[i] > 90 || (int)key[i] < 65 ){
     
      //ignore the space
      if(key[i] != ' '){
        fprintf(stderr, "Key includes an invalid character\n");
        exit(EXIT_FAILURE);
      }
    } 
  }  
} 

//Stackoverflow: sending sockets 
//https://stackoverflow.com/questions/57740245/what-is-the-correct-way-to-use-send-on-sockets-when-the-full-message-has-not-b
//void sendfile(char *file, int socket_ptr, int data_size){
void sendfile(char *file, int socket_ptr, int data_size){

  int num_bytes_sent = 0, file_desc, remaining_bytes = 0;
  //printf("num bytes: %d\n",num_bytes_sent );
  char filtered_msg[100000];

  file_desc = open(file, O_RDONLY);

  //Clear out the message buffer with NULL terminator 
memset(filtered_msg, '\0', sizeof(filtered_msg));

  //read in the desired file and put it into filtered_msg buffer
  read(file_desc, filtered_msg, sizeof(filtered_msg));

  //remove the trailing \n in filtered_msg
  filtered_msg[strcspn(filtered_msg, "\n")] = '\0';
//printf("%s\n",filtered_msg );
 strcat(filtered_msg, "@@");

//stop the loop when number of sent is greated than the message
  while(num_bytes_sent < strlen(filtered_msg))
  {

    //printf("HELLO\n");

      //send msg to server and keep sending if it has not sent the full file bytes
      remaining_bytes = send(socket_ptr, &filtered_msg[num_bytes_sent], strlen(filtered_msg) - num_bytes_sent, 0);
      // printf("remaining bytes : %d\n", remaining_bytes);
      if (remaining_bytes == -1)
      {
          // Handle error
          printf("HANDLED ERROR TRIGGERED\n");
          error("CLIENT: ERROR reading from socket");
          break;
      }
      //add the counter for the total number messages sent
      num_bytes_sent += remaining_bytes;
     // printf("number of bytes sent : %d\n", num_bytes_sent);

    if (remaining_bytes < 0){
      error("CLIENT: ERROR writing to socket");
    }


  //printf("%s\n",filtered_msg );
  } 
  //printf("%s\n",final_msg );
  //printf("while loop ended\n");
  close(file_desc);
}



void sendfile_nonblock(char * file, int socket_ptr){
  char const *verify = "received";
  char buf[strlen(verify)];
  sendfile(file, socket_ptr, 0);
  //int charsWritten = socket_send_nonblock(message_buf, socketfd);
  recv(socket_ptr, buf, sizeof(buf), 0);
 // return charsWritten;

}


// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;

  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}



int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[100000];
//printf("ASDSADAS\n");
  // Check usage & args
  if (argc != 4) { 
    fprintf(stderr,"USAGE: %s hostname port is not connected\n", argv[0]); 
    exit(0); 
  } 


int text_file = open(argv[1], O_RDONLY);


if( text_file == -1 ){
  perror("Error: ");
  exit(EXIT_FAILURE);
}

int keygen_file = open(argv[2], O_RDONLY);

if(keygen_file == -1){
  perror("Error: ");
  exit(EXIT_FAILURE);
}


char * input_msg = (char *) calloc(100000,sizeof(char));
char * key_msg = (char *) calloc(100000,sizeof(char));
read(text_file, input_msg, sizeof(buffer));
read(keygen_file, key_msg, sizeof(buffer));

//printf("length of key %d\n", strlen(key_msg) );
//printf("length of key12312:  %d\n", strlen(key_msg)-1 );
if(strlen(key_msg)-1 <  strlen(input_msg)-1){
  printf("Error: '%s' is too short\n", argv[2]);
  exit(EXIT_FAILURE);
}

//checks for characters
//argv[1] and argv[2], check for key and plain text
//verify the characters to be capitalize and spaces
verify_characters(input_msg, key_msg);

free(input_msg);
free(key_msg);




  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  int port_number = atoi(argv[3]);
  if(port_number == 0){error("invalid port number");}
  setupAddressStruct(&serverAddress, port_number, "localhost");




  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }


//****************************>>>>>>
  // Send message to server
char const * verify = "received";
char temp[strlen(verify)];

//sendfile_nonblock(argv[1], socketFD);
sendfile(argv[1], socketFD, sizeof(text_file));

//need to receive junk message in between sending to prevent blocking
recv(socketFD,temp, sizeof(temp),0);


//sendfile_nonblock(argv[2], socketFD);
sendfile(argv[2],socketFD, sizeof(keygen_file));

//need to receive junk message in between sending to prevent blocking
recv(socketFD,temp, sizeof(temp),0);

close(text_file);
close(keygen_file);



//RECEIVES ENCRIPTED DATA

  // Get return message from server
  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
 //printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
printf("%s\n", buffer);
  // Close the socket
  close(socketFD); 
  return 0;
}



