/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

    Simple client main program for MP2 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h> 

#include "reqchannel.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

void process_hello(const string & _request) {
  cout<<"hello to you too"<<endl;
}

void process_data(const string &  _request) {
  cout<<int2string(rand() % 100)<<endl;
}

void process_request(const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(_request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(_request);
  }
  else if (_request.compare("quit") == 0) {
      cout<<"bye"<<endl;
      return;
   }
  else {
    cout<<"unknown request"<<endl;
  }
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	pid_t parent = getpid();
	pid_t pid = fork();

	if (pid == -1)
	{
		// error, failed to fork()
	} 
  else if(pid == 0){
		// we are the child
		 char *arg[12];
		if(execv("./dataserver",argv) == -1)
      cout<<"Error, fork failed"<<endl;
    return -1;
	}


  cout << "CLIENT STARTED:" << endl;

  cout << "Establishing control channel... " << flush;
  RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
  cout << "done." << endl;

  /* -- Start sending a sequence of requests */

  string reply1 = chan.send_request("hello");
  cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

  string reply2 = chan.send_request("data Joe Smith");
  cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

  string reply3 = chan.send_request("data Jane Smith");
  cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

  for(int i = 0; i < 100; i++) {
    string request_string("data TestPerson" + int2string(i));
    string reply_string = chan.send_request(request_string);
	cout << "reply to request " << i << ":" << reply_string << endl;;
  }
 
  string reply4 = chan.send_request("quit");
  cout << "Reply to request 'quit' is '" << reply4 << endl;

  usleep(1000000);
}
