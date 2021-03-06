#ifndef __netConfig_H__
#define __netConfig_H__

#include "stdafx.h"

#include "myNetOS.h"
#include "myBuffer.h"

#include <iostream>
#include <strstream>
#include "string.h"

using namespace std;

class Config {
  public:
	Config(char *filename){ this->filename = filename;}
	~Config(){ cout << "~ Config: "<< endl; }

	int getValue ( string attr ) {  //
cout << " attr = " << attr << endl;
	  ifstream ifile(filename);
	  if(!ifile.is_open()){
		cout <<"Can not open file"<< endl;
	  }
      
	  string str;
	  int num_ = -1;
	  basic_string <char>::size_type index_1;

	  while(!ifile.eof())	{
        getline(ifile, line);
        index_1 = line.find(attr); 
		if (index_1 != string::npos ) {
//cout << " == "  << line.find(attr) << endl; 
//cout << " val "  << line.find("value") + 5 << endl;
		  str = line.substr(line.find("value") + 5,line.size());
//cout << "str = " << str << endl; 
		  str = str.substr(str.find("=")+1);  
//cout << "str = " << str << endl;
cout << " num "  << atoi(str.c_str()) << endl;
          return atoi(str.c_str()); 
	    }
	  } // end while 
	  return -1;

	}//end getValue

  private:
	string line;
	char *filename;
}; //end Config


class myNet {
  public:
	 myNet(){ 
       this->config = new Config("config.xml");
       SIZE_OF_MEMORY_IN_BLOCKS = config->getValue("SIZE_OF_MEMORY_IN_BLOCKS");
       // максимальное количество процессов
       MAX_PROCESS = config->getValue("MAX_PROCESS");
       SIZE_OF_MEMORY_IN_PAGE = config->getValue("SIZE_OF_MEMORY_IN_PAGE");
       //размер виртуальной памяти в образах процесса
       SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES = MAX_PROCESS;
  
       buffer_1 = new myBuffer(333);
       buffer_2 = new myBuffer(18);

       buffer_1->setBuffer(buffer_2); 
       buffer_2->setBuffer(buffer_1);

	    client_1 = new myNetOS(SIZE_OF_MEMORY_IN_BLOCKS,MAX_PROCESS,SIZE_OF_MEMORY_IN_PAGE,
        	buffer_1 );
	    client_2 = new myNetOS (SIZE_OF_MEMORY_IN_BLOCKS,MAX_PROCESS,SIZE_OF_MEMORY_IN_PAGE, 
	        buffer_2 );

	 } // end Net  
     
	 void dispatch_1(myMessage * message) {
       //supressBegin();
	   this->client_1->dispatch(message);
       //supressEnd();  
	 }
	 
	 void dispatch_2(myMessage * message) {
       //supressBegin();
	   this->client_2->dispatch(message);
       //supressEnd();
	 }

	 int getIP_1 (){ return buffer_1->getIP();}
	 myMessage * getMessage_1 (){ return buffer_1->getMessage();}

     int getIP_2 (){ return buffer_2->getIP();} 
	 myMessage * getMessage_2 () { return buffer_2->getMessage();}

  private:
    Config * config;
    
	// размер оперативной памяти в блоках
    int SIZE_OF_MEMORY_IN_BLOCKS;
    // максимальное количество процессов
    int MAX_PROCESS;
    int SIZE_OF_MEMORY_IN_PAGE;
    //размер виртуальной памяти в образах процесса
	int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES;
  
    myBuffer * buffer_1;
    myBuffer * buffer_2;

    myNetOS * client_1;
    myNetOS * client_2;

	strstream str;
    streambuf *x;

	void supressBegin(){
      //cout << " REDIRECT cout IN str " << endl;
      x = cout.rdbuf( str.rdbuf() );    
	}

	void supressEnd(){
	  str.clear();
	  cout.rdbuf(x);
      //cout << str.rdbuf() << endl;
	  //cout << str.rdbuf() << endl;
      //cout << " direct str IN cout " << endl; 
	}

};

/*
int main() {

  cout << " I am os" <<endl; // write in str
  
  net.dispatch_1(new Message);
  net.dispatch_2(new Message);

  return 11110;

} // end main
*/

#endif