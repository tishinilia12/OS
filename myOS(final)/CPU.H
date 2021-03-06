#ifndef __CPU_H__
#define __CPU_H__

#include "ArchitectureCPU.h"
#include "Memory.h"

#include "Device.h"
#include "LAN.h"

//#include "File.h"
//#include "Protocol.h"
#include "myBuffer.h"

class CPU {
  public:
    CPU(Memory *memory,int MAX_PROCESS, LAN *lan, Device *dev){
      this -> memory = memory;
      this -> lan    = lan;
      this -> dev    = dev;
    } 
    ~CPU() {}

    // выполнить инструкцию - блок
  virtual Interrupt exeInstr(int addr, SysReg *sysreg) {
      Interrupt interrupt; 
      Block* block = fetch(sysreg);
      // пустой блок не декодируется
      if (block->getState()) return Error;   
      interrupt = decode(block, sysreg);   // прерывание
      switch (interrupt){
        case OK:  // изменить PC
          sysreg->setState(PC, memory-> getNextAddr(sysreg->getState(PC))); 
          break;
        case Exit: cout << "Exit" <<endl; 
          return Exit; 
        case Dev: // обращение к устройству печати 
          dev->printData(sysreg->
            getState((Name)block->getNregister()) );
          // изменить PC
          sysreg->setState(PC, 
            memory->getNextAddr(sysreg->getState(PC))); 
          break;
        case Lan:  // обращение к устройству сети
          lan->sendData(sysreg->getState((Name)block->getNregister()) ); 
          // изменить PC
          sysreg->setState(PC, memory->getNextAddr(sysreg->getState(PC))); 
          break;
        default : return interrupt;
      }
      return OK;
    } // end exeInstr

    void Debug(SysReg *sysreg){ // трассировка состояния регистров для одного блока  
                                // см. Scheduler Debug
      sysreg->Debug();
      // только один блок  
      Block* block = fetch(sysreg);
      block->debugBlock(); 
      decode(block, sysreg);
      sysreg->Debug(); 
    }
  private:
  protected:

  Memory *memory;
    int MAX_PROCESS;
    
    // функции алгоритма CPU 
    // выборка инструкции по состоянию PC
    Block* fetch(SysReg *sysreg) { 
      //cout <<" fetch, PC = "<< sysreg->getState(PC) << endl; 
        return memory->read(sysreg->getState(PC)); 
    }


    LAN *lan;
    Device *dev;
 
    virtual Interrupt decode(Block* block, SysReg *sysreg) { 
      //block->debugBlock();
      // доступ к выборке операнда access: sysreg[id]->getState(....)
		int temp=block->getCode();
		int mask=temp&128;
		temp=temp&127;
      Block* mblock = NULL; // используется при чтении и записи
	  switch( temp){
        case Mov: // Перемещение содержимого второго операнда в первый {регистр} 
          sysreg->setState(block->getNregister(),block->getOperand());
        break;
        case Add : // 1. взять содержимое регистра и операнда (тоже регистр)
                   // 2. результат записать в первый операнд
			if (mask) {
					sysreg->setState( (Name)block->getNregister(),
					sysreg->getState((Name)block->getNregister()) +
					sysreg->getState((Name)block->getOperand()) );
			}
			else {
					sysreg->setState((Name)block->getNregister(),    
                    sysreg->getState((Name)block->getNregister()) +
                    block->getOperand() );
			}
            break;
        case Sub : // 1. взять содержимое регистра и операнда (тоже регистр)
                   // 2. результат записать в первый операнд 
			if(mask){
				sysreg->setState( (Name)block->getNregister(),    
                sysreg->getState((Name)block->getNregister()) -
                sysreg->getState((Name)block->getOperand() ) );
			}
			else {
				sysreg->setState( (Name)block->getNregister(),    
				sysreg->getState((Name)block->getNregister()) -
				block->getOperand()  );
			}
            break;
        case Mul : // 1. взять содержимое регистра и операнда (тоже регистр)
                   // 2. результат записать в первый операнд
			if (mask) {
				sysreg->setState( (Name)block->getNregister(),    
                sysreg->getState((Name)block->getNregister()) *
                sysreg->getState((Name)block->getOperand()  ) );
			}
			else {
				sysreg->setState( (Name)block->getNregister(),    
				sysreg->getState((Name)block->getNregister()) *
				block->getOperand()  );
			}
            break;
		case Div : 
			if (mask) {
				if (sysreg->getState((Name)block->getOperand()) == 0) return Div_0; 
                // 1. взять содержимое регистра и операнда (тоже регистр)
                // 2. результат записать в первый операнд  
                sysreg->setState( (Name)block->getNregister(),    
                sysreg->getState((Name)block->getNregister()) /
                sysreg->getState((Name)block->getOperand()  ) );
			}
			else {
				if (block->getOperand() == 0) return Div_0; 
				sysreg->setState( (Name)block->getNregister(),    
				sysreg->getState((Name)block->getNregister()) /
				block->getOperand()  );
			}
            break;
        case Int : // прерывание   
            return (Interrupt) block->getOperand();
            break;
        case Wmem: // Сохранить содержимое региста в память
            mblock = memory->read(block->getOperand());     
            if (!mblock ->getState()) return Error; // чтение занятого блока
            mblock ->setOperand( sysreg->getState((Name)block-> getNregister()));
            mblock ->setState(false); // занят
            break;
        case Rmem:	// Чтение операнда из блока памяти в регистр
            mblock = memory->read(block->getOperand());    
            if (mblock ->getState()) return Empty; // чтение только занятого блока
            sysreg->setState((Name)block-> getNregister(), mblock->getOperand());  
            break;
        default : return Error;
    }	
    return OK;
  } // end decode
};

#endif


