#pragma once
#include "CPU.h"







class myCPU: public CPU {
  public:

	  myCPU(Memory *memory,int MAX_PROCESS, LAN *lan, Device *dev, MyLAN *mylan,myBuffer *buffer):
		CPU(memory,MAX_PROCESS, lan, dev){
			 cout<<"my CPU enabled"<<endl;
			this->mylan=mylan;
			this -> buffer = buffer;
		}



	Interrupt exeInstr(int addr, SysReg *sysreg) { 
int num=70;
	Interrupt interrupt; 
	Block* block = fetch(sysreg);
      // ������ ���� �� ������������
	if (block->getState()) return Error;   
	interrupt = CPU::exeInstr(addr,sysreg);
      switch (interrupt){
		case MyLanR:

			sysreg -> setState (block -> getNregister(), mylan -> Readfromfile());
			sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );

			break;
		case MyLanW:
			mylan->Writetofile(sysreg->getState((Name)block->getNregister()) ); 
          sysreg->setState(PC, memory->getNextAddr(sysreg->getState(PC))); 
			break;
		/*case It1:
			sysreg->setState( (Name)block->getNregister(), num); 
		   // �������� PC
          sysreg->setState(PC,memory->getNextAddr(sysreg->getState(PC))); 

		  case It2:
			sysreg->setState( (Name)block->getNregister(), num); 
		   // �������� PC
          sysreg->setState(PC,memory->getNextAddr(sysreg->getState(PC)));

		break;
*/
		default : return interrupt;
      }
      return OK;
    } // end exeInstr

	MyLAN *mylan;
	myBuffer *buffer;
  private:
	 
  protected:

};



/*
class myCPU: public CPU {
public:
	myCPU (Memory *memory, int MAX_PROCESS, myLAN *lan, Device *dev, File *file, //������� Device *dev //�����
	Protocol *protocol, myBuffer *buffer) : CPU (memory, MAX_PROCESS, lan, dev) { //������ NULL dev
		this -> dev = dev;
		this -> lan = lan;
		this -> file = file;
		this -> protocol = protocol; 
		this -> buffer = buffer;
    } 
		
	Interrupt exeInstr (int addr, SysReg *sysreg) {
		MemBlock = NULL;
		Interrupt interrupt = CPU::exeInstr (addr, sysreg);		
		Block * block = fetch (sysreg);

		if (block -> getState()) 
			return Error;  // ������ ���� �� ������������

		switch(interrupt){

			case readFile: // ������ ��  �����

				cout << "interrupt = " << interrupt << endl;
				//��������� � ������� ������ �� ����� :
				file -> set_Data ();
				sysreg -> setState (block -> getNregister(), file -> get_Data());
				// �������� PC :
				sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );
			  break;

			case writeProtocol:
				cout << "interrupt = " << interrupt << endl;
				//������ � �������� :
				protocol -> set_Result (sysreg -> getState ((Name)block -> getNregister()) );
				// �������� PC
				sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );
			break;

			case readProtocol:
				cout << "interrupt = " << interrupt << endl;
				//������ �� ��������� :
				protocol -> set_Data ();
				sysreg -> setState (block -> getNregister(), protocol -> get_Data());     
				// �������� PC :
				sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );
			break;

					
		   default:
			 return interrupt;
      }
	  return interrupt;
    } // end exeInstr
		/*if (interrupt == PrIn) {  
			cout << "interrupt = " << interrupt << endl;
			//������ � �������� :
			protocol -> set_Result (sysreg -> getState ((Name)block -> getNregister()) );
            // �������� PC
			sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );
		}
		if (interrupt == PrOut) {   
			cout << "interrupt = " << interrupt << endl;
			//������ �� ��������� :
			protocol -> set_Data ();
			sysreg -> setState (block -> getNregister(), protocol -> get_Data());     
			// �������� PC :
			sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );
		}
		if (interrupt == F) { 
			cout << "interrupt = " << interrupt << endl;
			//��������� � ������� ������ �� ����� :
			file -> set_Data ();
			sysreg -> setState (block -> getNregister(), file -> get_Data());
			// �������� PC :
			sysreg -> setState (PC, memory -> getNextAddr (sysreg -> getState(PC)) );
		}
		return interrupt;
	}

private :
	Device *dev;
	myLAN *lan; 
	Block* MemBlock;
	File *file;
	Protocol *protocol;
	myBuffer *buffer;
};
*/