#ifndef __Memory_H__
#define __Memory_H__

#include <iostream>
using namespace std;
#include "ArchitectureCPU.h"

class Memory {
public:
	Memory (int SIZE_OF_MEMORY_IN_BLOCKS) { 
		currBlock = 0; 
		this->SIZE_OF_MEMORY_IN_BLOCKS = SIZE_OF_MEMORY_IN_BLOCKS;
		heap = new Block[SIZE_OF_MEMORY_IN_BLOCKS];
    }

	// применяется при считывании инструкций из файла 
	// функция устанавливает начальный адрес для последовательной записи  
	void setAddrBlock (int addr) { currBlock = addr; }

	// применяется при считывании инструкций из файла 
	void setCmd (int cmd, int op1, int op2) {  
		int addr = currBlock++;  
		if (addr == -1) { 
			cout << "No free blocks" << endl;
			return;
		}
		heap[addr].setState (false);
		heap[addr].setCode (cmd); 
		heap[addr].setNregister (op1);  
		heap[addr].setOperand (op2);
	}
		 
	void setCmd (int cmd, int op1, Name op2) {
		int temp = 1;
		temp = temp << 7;
		cmd = cmd | temp;
		int addr = currBlock++;  
		if (addr == -1) { 
			cout << "No free blocks" << endl;
			return;
		}
		Block * pb = Memory::read(addr);		
		pb->setState (false);
		pb->setCode (cmd); 
		pb->setNregister (op1);  
		pb->setOperand (op2);
	}
	/* read применяется при считывании инструкций CPU и  
     при организации свопинга с виртуальной памятью */
	Block* read(int addr) { return &heap[addr]; }
	// получить индекс следующего блока, если нет блока то Error  
  
	// спользовать функции  для управления памятьюю
	int getAddrFreeBlock() { // выбрать первый свободный блок при просмотре
		for (int i = 0; i < SIZE_OF_MEMORY_IN_BLOCKS; i++)	
			if (heap[i].getState()) return i;      
		return -1;  // нет свободных блоков
	}

	/*  для кучи алрес вычисляется как следуюший по индексу
		для страничной и сегментной организации метод должен быть замещен.
		Используется в CPU, для содержимого регистра PC */
	int getNextAddr (int currAddr) { return currAddr + 1; } 

	void clearMemory () {  // очистка всей памяти 
		for (int i = 0; i < SIZE_OF_MEMORY_IN_BLOCKS; i++) {	
			clearBlock(i);
		}
		currBlock = 0;
	}

	void clearMemory (int AddrFrom, int AddrTo) { // очистка части памяти 
		// текущий блок не меняется  
		for (int i = AddrFrom; i < SIZE_OF_MEMORY_IN_BLOCKS; i++) {
			if (i > AddrTo) return; 
				clearBlock(i);
		}	  
	}
 
	int getSizeMemory () { return SIZE_OF_MEMORY_IN_BLOCKS; } 

	// можно посмотреть содержимое памяти   
	void debugHeap() {
		for (int i = 0;i < SIZE_OF_MEMORY_IN_BLOCKS; i++){
			cout << "heap[" << i << "] = " <<  " state=" << heap[i].getState()   
				 << " code=" << heap[i].getCode() 
				 << " nregister=" << heap[i].getNregister()
				 << " operand=" << heap[i].getOperand() << endl;   
		}
	}  

//private :
void clearBlock(int addr) {
		heap[addr].setState(true);
		heap[addr].setCode(0); 
		heap[addr].setNregister(0);  
		heap[addr].setOperand(0);
	}
	int SIZE_OF_MEMORY_IN_BLOCKS;
	int currBlock;
	Block *heap;
}; 

class Code : public Memory {
public :
	Code (int SIZE_OF_PROGRAMM) : Memory(SIZE_OF_PROGRAMM) {}
	Code (Memory *memory) : Memory (memory->getSizeMemory()) {
		Block *heap; 
		for (int i = 0; i < memory->getSizeMemory(); i++) {
			heap = memory->read(i); 
			setCmd(heap->getCode(), heap->getNregister(), heap->getOperand() );     
		}
	}
	
	virtual ~Code() { cout << " object Code deleted" << endl; }
};

class Page {
public:
	Page(){
		for(int i=0;i<5;i++)
			bloks[i]=NULL;
		size=0;
	}
	void Save(Block * block){
		if(size!=5)	{
			this->bloks[size]=block;
			size++;
		}
	}
	Block * Read(int numblock) {
		if( numblock < 5 ) {
			return  bloks[numblock];
		}
		return NULL;
	}
private :
	int size;
	Block * bloks[5];
};



/////////////////////////////////////////////////////////////////////////////////////////////////



class myMemory:public Memory {
public:
	myMemory (int SIZE_OF_MEMORY_IN_PAGES) : Memory(5*SIZE_OF_MEMORY_IN_PAGES) {
		this -> SIZE_OF_MEMORY_IN_PAGES = SIZE_OF_MEMORY_IN_PAGES;
	}

	virtual int getAddrFreeBlock() { // выбрать первый свободную  страницу при просмотре
		for (int i = 0; i < SIZE_OF_MEMORY_IN_PAGES; i++)
			if (heap[5*i].getState()) 
				return 5*i;      
		return -1;  // нет свободных страниц
	}

	int getAddrFreePage() { // выбрать первый свободную  страницу при просмотре
		for (int i = 0; i < SIZE_OF_MEMORY_IN_PAGES; i++)
			if (heap[5*i].getState()) 
				return 5*i;      
		return -1;  // нет свободных страниц
	}
	
	virtual void clearMemory() {  // очистка всей памяти 
		for (int i = 0; i < SIZE_OF_MEMORY_IN_PAGES; i++){
			clearBlock(5*i);
			clearBlock(5*i+1);
			clearBlock(5*i+2);
			clearBlock(5*i+3);
			clearBlock(5*i+4);
		}
		currBlock = 0;
	}
//В странице содержится 5 блока, сохраняет в память поблочно
    void setCmdfromPage(Page * page) {
		int i=0;
		Block * buffer;
		while( (buffer=page->Read(i)) != NULL ) {
			Memory::setCmd(buffer->getCode(), buffer->getNregister(), buffer->getOperand() );
			i++;
		}
	} 

private:
	int SIZE_OF_MEMORY_IN_PAGES;
};

#endif