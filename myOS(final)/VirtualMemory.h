#ifndef __VirtualMemory_H__
#define __VirtualMemory_H__

#include <iostream>
using namespace std;
#include "ArchitectureCPU.h"
#include "Process.h"

class VirtualMemory {
  public:
    VirtualMemory(int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES){
      this->SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES = SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES;
      image = new ProcessImage[SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES]; 
      currImage = 0;
    }  
  
  /* применяется при считывании инструкций
     функция устанавливает начальный адрес для последовательной записи  */
  virtual void setAddrImage( int addr) {currImage = addr; }
  // получить по установленному адресу образ процесса
  ProcessImage * getImage() {return &image[currImage];}

  /* применяется при инициализации образа
     по установленному адресу, записать в виртуальную память программу   */
  virtual void setMemory(Memory * memory) {  
      image[currImage].setStatus(false); // образ занят
      image[currImage].setCode(memory); 
  }
  
  /* read применяется при организации свопинга:
     из виртуальной памяти в оперативную */
  virtual Memory* read(int addr) { return image[addr].getCode();}

  /* получить индекс следующего образа, если нет образа то Error, -1  
     спользовать функции  для управления памятью */
  virtual int getAddrFreeImage(){ // выбирается первый свободный образ при просмотре 
    for (int i = 0; i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++)	
      if (image[i].getStatus()) {
          image[i].setStatus(false); // true VirtualMemory  образ свободен  
          return i;                 // false занят
        }
      
    return -1; // нет свободных образов
  }
  
  /* при удалении программы из системы: 
     освободить virtualMemory, инициализировать PCB */
  virtual void clearImage(int addr) {
    image[addr].setStatus(true);
	delete (Code *)image[addr].getCode(); //image[addr].setCode(NULL); 

    image[addr].setAddr(-1); 
    image[addr].setID(-1); 
    image[addr].setPriority(-1); 
    image[addr].setTimeSlice(-1); 
    image[addr].setState(NotRunning);
    image[addr].setSysReg(NULL);
    image[addr].setUser("");
    image[addr].setVirtualAddr(-1);
    image[addr].clearTime();
    image[addr].setTimeSlice(-1);
  }

  virtual void clearMemory() {
    for (int i = 0; i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++)	
       clearImage(i);
    currImage = 0;
  }

  // можно посмотреть содержимое образа процесса в виртуальной памяти   
  virtual void Debug() {
    for (int i = 0;i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++){
      cout << " VertualAddress = "<<i<<" status = "<<image[i].getStatus()
           << "  ID = "<< image[i].getID()<< endl; 
    }
  }  
  protected:
    int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES;
    int currImage;
    ProcessImage *image; 
}; 
class VirtualPage{
public:
	VirtualPage(){
		for(int i=0;i<5;i++)
			images[i]=NULL;
		size=0;
	}
	void Save(ProcessImage * image){
		if(size!=5){
			this->images[size]=image;
			size++;
		}
	}
	ProcessImage *Read(int numimage){
		if( numimage < 5 ){
			return  images[numimage];
		}
	}
protected:
	int size;
	ProcessImage *images[5];
};

class myVirtualMemory:public VirtualMemory{
public:
	myVirtualMemory(int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES):VirtualMemory(5*SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES){
		this->SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES=SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES;
	}
	   virtual void Debug() {
			for (int i = 0;i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++){
				cout<<"-------------"<<"Page VirtualMemory:"<<i+1<<"-------------"<<endl;
				int vpage_per=5*i;
				cout<<"1 ProcessImage"<<endl;
				cout << " VertualAddress = "<<i
					 <<" status = "<<image[i].getStatus()
                     << "  ID = "<< image[i].getID()<< endl;    
				vpage_per++;
				cout<<"2 ProcessImage"<<endl;
				cout << " VertualAddress = "<<i
					 <<" status = "<<image[i].getStatus()
                     << "  ID = "<< image[i].getID()<< endl;   
				vpage_per++;
				cout<<"3 ProcessImage"<<endl;
				cout << " VertualAddress = "<<i
					 <<" status = "<<image[i].getStatus()
                     << "  ID = "<< image[i].getID()<< endl;     
				vpage_per++;
				cout<<"4 ProcessImage"<<endl;
				cout << " VertualAddress = "<<i
					 <<" status = "<<image[i].getStatus()
                     << "  ID = "<< image[i].getID()<< endl;   
				vpage_per++;
				cout<<"5 ProcessImage"<<endl;
				cout << " VertualAddress = "<<i
					 <<" status = "<<image[i].getStatus()
                     << "  ID = "<< image[i].getID()<< endl;   
				vpage_per++;
			}
		}  

 virtual int getAddrFreeImage(){  // выбирается первый свободный образ при просмотре 
    for (int i = 0; i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++)	
      if (image[5*i].getStatus()) {
          image[5*i].setStatus(false); // true VirtualMemory  образ свободен  
          return 5*i;                 // false занят
        }
    return -1; // нет свободных образов
 }

int getAddrFreeVirtualPage(){ // выбрать первый свободную  страницу при просмотре
	 for (int i = 0; i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++)	
      if (image[5*i].getStatus()) {
          image[5*i].setStatus(false); // true VirtualMemory  образ свободен  
          return 5*i;                 // false занят
        }
    return -1; // нет свободных образов
 }

  virtual void clearMemory() {  
    for (int i = 0; i < SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; i++){
		clearImage(5*i);
		clearImage(5*i+1);
		clearImage(5*i+2);
		clearImage(5*i+3);
		clearImage(5*i+4);
	}
	currImage = 0;
  }
  virtual void setAddrImage( int addr) {currImage = addr; }
	protected:
		int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES;
};
#endif