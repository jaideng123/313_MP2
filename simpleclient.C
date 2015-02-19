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

#include <stdio.h>
#include <time.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h> 

#include "reqchannel.H"

#if defined(sun)
    #define CLOCKNAME CLOCK_HIGHRES
        
#else
    #define CLOCKNAME CLOCK_PROCESS_CPUTIME_ID
#endif

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
/* Timing */
/*--------------------------------------------------------------------------*/

struct timespec diff(struct timespec start, struct timespec end)
 {
    struct timespec temp;
    //the if condition handles time stamp end being smaller than than 
    //time stamp start which could lead to negative time.

     if ((end.tv_nsec-start.tv_nsec)<0) {
          temp.tv_sec = end.tv_sec-start.tv_sec-1;
          temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
      } else {
          temp.tv_sec = end.tv_sec-start.tv_sec;
          temp.tv_nsec = end.tv_nsec-start.tv_nsec;
      }
  return temp;
 }

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
	unsigned long long int end_time1, end_time2, total_time1=0, total_time2=0;
	struct timespec timeDiff,timeres;
    struct timespec time1, time2, calibrationTime;
	
	clock_gettime(CLOCKNAME, &time1);
	clock_gettime(CLOCKNAME, &time2);
	calibrationTime = diff(time1,time2); //calibration for overhead of the function calls
    clock_getres(CLOCKNAME, &timeres);  // get the clock resolution data

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
	
/* ------------------------- INTER PROCESS COMMUNICATION ---------------------------------- */
  cout << "CLIENT STARTED:" << endl;

  cout << "Establishing control channel... " << flush;
  RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
  cout << "done." << endl;

  /* -- Start sending a sequence of requests */
  clock_gettime(CLOCKNAME, &time1);
  string reply1 = chan.send_request("hello");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;
  
  clock_gettime(CLOCKNAME, &time1);
  string reply2 = chan.send_request("data Joe Smith");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;
  
  clock_gettime(CLOCKNAME, &time1);
  string reply3 = chan.send_request("data Jane Smith");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;
  
  for(int i = 0; i < 100; i++) {
    string request_string("data TestPerson" + int2string(i));
	clock_gettime(CLOCKNAME, &time1);
    string reply_string = chan.send_request(request_string);
	clock_gettime(CLOCKNAME, &time2);
	timeDiff = diff(time1,time2);
	end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
	end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
	total_time1 += end_time1;
	total_time2 += end_time2;
	cout << "reply to request " << i << ":" << reply_string << endl;;
  }
  
  clock_gettime(CLOCKNAME, &time1);
  string reply4 = chan.send_request("quit");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  cout << "Reply to request 'quit' is '" << reply4 << endl;
  
/* ----------------------------------------------------------------------------------------- */
  cout << "Time for IPC : " << total_time1 << " seconds and " << total_time2 << " nanoseconds." << endl;
  total_time1 = 0;
  total_time2 = 0;
  
  usleep(1000000);
  
/* --------------------------- LOCAL PROCESS COMMUNICATION ------------------------------------------- */
  clock_gettime(CLOCKNAME, &time1);
  process_request("hello");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  
  clock_gettime(CLOCKNAME, &time1);
  process_request("data Joe Smith");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  
  clock_gettime(CLOCKNAME, &time1);
  process_request("data Jane Smith");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;
  
  for(int i = 0; i < 100; i++){
    clock_gettime(CLOCKNAME, &time1);
    process_request("data" + int2string(i));
	clock_gettime(CLOCKNAME, &time2);
    timeDiff = diff(time1,time2);
    end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds
    end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
    total_time1 += end_time1;
    total_time2 += end_time2;
  }
  
  clock_gettime(CLOCKNAME, &time1);
  process_request("quit");
  clock_gettime(CLOCKNAME, &time2);
  timeDiff = diff(time1,time2);
  end_time1 = timeDiff.tv_sec - calibrationTime.tv_sec;//time in seconds   
  end_time2 = timeDiff.tv_nsec - calibrationTime.tv_nsec;//time in nanoseconds
  total_time1 += end_time1;
  total_time2 += end_time2;

/* ----------------------------------------------------------------------------------------- */
  cout << "Time for Local Process Communication : " << total_time1 << " seconds and " << total_time2 << " nanoseconds." << endl;
  
  usleep(1000000);
}
