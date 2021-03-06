#include "Memory.h"
#include "Process.h"
#include <fstream>
class MMU {
  public:
      MMU(Memory &memory) {this ->memory = &memory;} //Меняю на myMemory

      // загрузка программыы из образа процесса в основную память
      virtual void swapIn(ProcessImage * processImage){
        // получить программу из образа процесса
        Memory * code = processImage->getCode();
        // получить адрес размещения программы в оперативной памяти
        // если он был 
        int addrPCB  = processImage->getAddr();  
        int addrReal = getRealAddr(); // адрес размещения программы 
        if (addrPCB == -1)  // не присвоен
        // если PC = -1, то PC не проинициализирован присвоить начало программы    
        //if (processImage->getSysReg()->getState(PC) == -1)
          processImage->getSysReg()->setState(PC,addrReal);
        else {  // размещение програмы в памяти 
          // 1. определить смещение  PC - addrPCB и изменить адрес PC 
          int addrOffPC = processImage->getSysReg()->getState(PC) - addrPCB; 
          processImage->getSysReg()->setState(PC,addrOffPC + addrReal);  
        }
        processImage->setAddr(addrReal); // запомнить адрес в блоке PCB 
        // установить адрес размещения программы в оперативной памяти
        setAlloc(addrReal); 

        // прочитать блок кода, начало программы в образе
        Block * block = NULL;
        for(int i=0; i < code->getSizeMemory(); i++){
          block = code->read(i); // код программы читантся с 0 адреса 
          // записать  блок в память только последовательно, не занятые блоки 
          if (! block->getState()) {  
            memory->setCmd(block->getCode(),
                           block->getNregister(),
                           block->getOperand() );
          }
        } 
      } 
      
      // выгрузка программы из основной памяти,
      // применяется в алгоритмах вытеснения  
      virtual void swapOut(ProcessImage * processImage){
        // получить адрес размещения программы в оперативной памяти из PCB  
        int AddrFrom = processImage->getAddr(); //
        int AddrTo   = AddrFrom + processImage->getCode()->getSizeMemory() -1; //
        memory -> clearMemory(AddrFrom,AddrTo);
        //cout << " MMU  swapOut AddrFrom = "<< AddrFrom << " AddrTo= "<< AddrTo<< endl;
        //memory->debugHeap();
      }

      // определить свободный адрес для загрузки программы
      // ограничения, проверяется тольео один блок,
      // остальные считаются свободными - это не всегда так.  
      virtual int getRealAddr(){return this->memory->getAddrFreeBlock();}

      //virtual int getVertAddr(int areal){return areal;}; 
      
      // установить адрес размешения программы в памяти     
      virtual void setAlloc(int addr){memory->setAddrBlock(addr);}

      // показать выполнение swapIn 
      virtual void Debug(HANDLE * handle){
        // получить программу из образа процесса
        ProcessImage * processImage = (ProcessImage *)handle;
        //см. void swapIn(ProcessImage * processImage) 
        Memory * code = processImage->getCode();
        // получить адрес размещения программы в оперативной памяти
        // если он был 
          cout << endl;  
          cout << "-- get address of loading programm in memory and set PC --"<<endl;  
        int addrPCB = processImage->getAddr();  
          cout << "addrPCB = " << addrPCB <<" address of loading programm from PCB" <<endl;
        int addrReal = getRealAddr(); // адрес размещения программы 
          cout << "addrReal= " << addrReal <<" address of loading programm Real"<<endl;

        if (addrPCB == -1)  // не присвоен
        // если PC = -1, то PC не проинициализирован присвоить начало программы    
          cout << " if address in PCB == -1 set addrReal PC = " <<addrReal<< endl;
        else {  // размещение програмы в памяти 
          // 1. определить смещение  PC - addrPCB и изменить адрес PC 
        cout <<"PC = "<<processImage->getSysReg()->getState(PC)<<" address in PC "<<endl; 
          int addrOffPC = processImage->getSysReg()->getState(PC) - addrPCB; 
        cout <<"addrOffPC = PC - addrPCB = "<< addrOffPC<<" address offset"<<endl; 
        cout <<"PC = addrOffPC + addrReal = "<<  addrOffPC + addrReal <<endl; 
        }
        // прочитать блок кода, начало программы в образе
        Block * block = NULL;
        cout << endl; 
        cout << " ---- read block from process image code to memory ---"<<endl; 
        cout << " size code = "<< code->getSizeMemory()<<endl;
        for(int i=0; i < code->getSizeMemory(); i++){
          block = code->read(i); // код программы читается с 0 адреса           
          // записать  блок в память только последовательно, не занятые блоки 
          if (! block->getState()) {  
              block->debugBlock();
          }
        } 
      }

      virtual void DebugMemory(){ memory->debugHeap();}

  private:
    Memory *memory;
};



///////////////////////////////////////////////////////////////////////////////////////////////////




class myMMU: public MMU {
public:
	myMMU(myMemory &memory):MMU(memory) 
	{
		this ->memory = &memory;
	}
      // загрузка программыы из образа процесса в основную память
      virtual void swapIn(ProcessImage * processImage) {
        // получить программу из образа процесса
        Memory * code = processImage->getCode();
        // получить адрес размещения программы в оперативной памяти
        // если он был 
        int addrPCB  = processImage->getAddr();  
        int addrReal = getRealAddr(); // адрес размещения программы 
        if (addrPCB == -1)  // не присвоен
        // если PC = -1, то PC не проинициализирован присвоить начало программы    
        //if (processImage->getSysReg()->getState(PC) == -1)
          processImage->getSysReg()->setState(PC,addrReal);
        else {  // размещение програмы в памяти 
          // 1. определить смещение  PC - addrPCB и изменить адрес PC 
          int addrOffPC = processImage->getSysReg()->getState(PC) - addrPCB; 
          processImage->getSysReg()->setState(PC,addrOffPC + addrReal);  
        }
        processImage->setAddr(addrReal); // запомнить адрес в блоке PCB 
        // установить адрес размещения программы в оперативной памяти
        setAlloc(addrReal); 

        // прочитать блок кода, начало программы в образе
        Block * block = NULL;
		Page * page;
		//чтение блоков из кода по 5
        for(int i=0; i < code->getSizeMemory();i= i+5){
		  page=new Page;
		  for(int t=i;t<i+5;t++)
		  {
				 block = code->read(t);
				if (! block->getState()) 
					 page->Save(block);
		  }
		  //Все команды из страниц загружаются в память
		  memory->setCmdfromPage(page);
		  delete page;
        } 
      } 
     virtual int getRealAddr(){return this->memory->getAddrFreePage();}
protected:
	myMemory *memory;
};