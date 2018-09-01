#include <iostream>
using namespace std;

class LAN {
public: 
	LAN () {}
	virtual void sendData (int data) {
		cout << "LAN send:  data = " << data << endl;  
	}
	virtual int reciveData () {
		char ch;
		cout << "Please, input data: [0..9]" << endl;  
		cin >> ch;
		cout << "LAN recived:  data = " << ch << endl; 
		return ch;
	} 
private:
protected:
};

class MyLAN: public LAN {
public: 
	MyLAN(){}
    void Writetofile (int data){
		FILE * file=fopen("LAN.txt","w");
		fprintf(file,"%d\n",data);
		fclose(file);
        cout << "LAN wrote:  data = " << data << endl;  
	}
	int Readfromfile (){
		int data;
		FILE * file=fopen("LAN.txt","r");
		fscanf(file,"%d",&data);
		fclose(file);
		cout << "LAN recived:  data = " << data << endl; 
		return data;
  } 

  private:
  protected:
};