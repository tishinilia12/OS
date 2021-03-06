#ifndef __Process_H__
#define __Process_H__

#include "HANDLE.h"
#include "ArchitectureCPU.h"

// состояния процесса см.
//State {New, Ready, Running, Blocked, ExitProcess, NotRunning};
// New – Новый для выполнения программы создается новый процесс.  
// Ready – Готовый  
// Running – Выполняющийся   
// Blocked – Блокированный
// ExitProcess – Завершающийся 
// NotRunning - Не выполняющийся

class Process: public HANDLE { // реализация модели из двух состояний 
  public: 
    Process(){ 
      this->user = "";
      state = NotRunning;
    }
    virtual ~Process(){cout<< " object Process deleted" << endl;}

    string getUser(){return user;}
    void setUser(string user){this->user = user;}

    int getID(){return HANDLE::getID();}
    void setID(int ID){HANDLE::setID(ID);}

      // cocтояние процесса
    void setState(State state){this->state=state;} 
    State getState(){ return state;}
  private:
    State state;  // модель состояния процесса 
  protected:
    string user;
};

class PCB: public Process { // Process Control Block
  public: 
    PCB(){
      sysReg = NULL;   
      addr = -1;        //адрес размещения в оперативной памяти начала программы         
      virtualAddr = -1; //адрес размещения в виртуальной памяти
      // Scheduling 
      timeSlice = -1; // квота времени, выделенная процессу для метода RR
      priority  = -1; // приоритет процесса может быть изменен системой DP
    }
    virtual ~PCB(){cout<< " object PCB deleted" << endl;}

    void setSysReg(SysReg *sysReg){	this->sysReg=sysReg;}
    SysReg * getSysReg(){ return sysReg;}
 
    // адрес размещения в оперативной памяти
    int getAddr(){return addr;}
    void setAddr(int addr){this ->addr = addr;}

    // адрес размещения в виртуальной памяти
    int getVirtualAddr(){return virtualAddr;}
    void setVirtualAddr(int virtualAddr){this ->virtualAddr = virtualAddr;}

    // приоритет процесса
    int getPriority(){return priority;}
    void setPriority(int priority){this ->priority = priority;}
    
    // квант непрерывного времени для процесса
    int getTimeSlice(){return  timeSlice;}
    void setTimeSlice(int  timeSlice){this -> timeSlice =  timeSlice;}

    char *NameOf(State state){  // получить string представление состояния.
      const int SIZE_OF_STATE_NAME = 15; 
      static char szName[SIZE_OF_STATE_NAME];
      switch(state) {
        case NotRunning:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "NotRunning" );
          break;
        case Running:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "Running" );
          break;
        case Swapped:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "Swapped" );
          break;
        case New:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "New" );
          break;
        case Ready:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "Ready" );
          break;
        case Blocked:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "Blocked" );
          break;
        case ExitProcess:
          strcpy_s( szName, SIZE_OF_STATE_NAME, "ExitProcess" );
          break;		  
      } 
      return szName;
    };
  private:
    int addr; 
    int virtualAddr; 
    SysReg *sysReg;  
    int timeSlice; // квота времени, выделенная процессу для метода RR
    int priority;  // приоритет процесса
  protected:
};

class ProcessImage: public PCB  {
  public:  
    ProcessImage(){ 		
        status = true; // ProcessImage в VirtualMemory свободен  
        this->memory=NULL;
        flag = 1; // ресурс свободен
    }  
    virtual ~ProcessImage(){cout<< " object ProcessImage deleted" << endl; }
    void Debug () {
      cout << "  ----- begin Debug ProcessImage ----- " << endl;
      cout << "user: "<< user << endl;
      cout << "ID:   "<< getID()  << endl;
      cout << "addr: "<< getAddr() << endl;
      cout << "VirtualAddr: "<< getVirtualAddr() << endl;
      cout << "State:    "<< NameOf(getState())  << endl;
      cout << "priority: "<< getPriority()<< endl;
      cout << "timeSlice:"<< getTimeSlice()<< endl;
      cout << "status:   "<< status<< endl;
      cout << "flag:     "<< flag<< endl;
      cout << "SysReg:   "<< endl;
        SysReg * sysReg = getSysReg();
        if ( sysReg == NULL ) 
          cout << "SysReg: NULL"<< endl;
        else {
          sysReg->Debug();
        }
      cout << "Code:   "<< endl;
        if ( memory==NULL ) 
          cout << "Code (memory): NULL"<< endl;
        else memory->debugHeap();
      cout << " ----- end Debug ProcessImage ----- "  <<endl;
    }

    void DebugTime () {
      cout << " -- ProcessImage: DebugTime ----------"  << endl;
      cout << " ID = "  <<  getID()<< endl;
      cout << " Timer tick = "  << Timer::getTime() << endl;
      cout << " TimeSlice  = "  << getTimeSlice()<< endl;
      cout << " Texe       = "  << getTexec()<< endl;
      cout << " Priority   = "  << getPriority()<< endl;
    }

    Memory* getCode(){return memory;}
    void setCode(Memory* memory){this ->memory = new Code(memory);} 
	void setCodeNULL(){this ->memory = NULL;} 

    // cтатус образа процесса, используется только в классе VirtualMemory
    // для выделения памяти: true ProcessImage своболен
    void setStatus(bool status){this->status=status;} 
    bool getStatus(){ return status;}

    // флаг, для моделирования доступа и ожидания (блокирования) ресурса
    // требуемому процессу для выполнения
    void setFlag(int flag){this->flag=flag;} 
    int getFlag(){ return flag;}

    void clearTime(){HANDLE::clearTime();}
  private:
    
    Memory* memory; 
    bool status; // для выделения памяти VirtualMemory
    int flag; // доступ к ресурсу, например 1, ресурс свободен
                                         // 0 - занят
  protected:
    friend class Dispatcher;
    friend class Scheduler;
};
#endif