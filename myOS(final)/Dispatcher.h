#ifndef __Dispatcher_H__
#define __Dispatcher_H__

#include <iostream>
using namespace std;
#include "ArchitectureCPU.h"
#include "VirtualMemory.h"
#include "Scheduler.h"
#include "Process.h"

class Dispatcher { // управляет ресурсами, изменяет состояние процессов.
  public:
    Dispatcher (int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES, Scheduler *scheduler, MMU *mmu)
	{
      virtualMemory = new myVirtualMemory(SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES); 
      this -> scheduler = scheduler;
      this -> mmu = mmu;     // Model memory unit
      this -> cs  = new CS;  // critical section 
      this -> timeSlice = -1;  // квант времени для планирования по методу RR 
    }

    ProcessImage * getVirtualMemory(){
      // системные регистры не присвоены, процессор не выделен  
      // 1. выделить virtualMemory 
      int addr = virtualMemory->getAddrFreeImage();
      if (addr == -1) return NULL;
      virtualMemory->setAddrImage(addr);
      //  2.получить образ по виртуальному адресу
      ProcessImage * рrocessImage = virtualMemory->getImage();
      //  3. сохранить виртуальный адрес в образе
      рrocessImage->setVirtualAddr(addr);
      //  4. увеличить время обслуживания 
      рrocessImage->setTservice(рrocessImage->getTservice()+1);
      return рrocessImage; // по установленному адресу 
    } 

    void freeVirtualMemory(HANDLE *handle){
       ProcessImage * рrocessImage = (ProcessImage *)handle;
       // 0. только если процесс в очереди ExitProcess (выполнился)
       State state = рrocessImage->getState();
       int ID = рrocessImage->getID();
       // 1. очистить регистры
       scheduler->getSysReg(ID)->setStatus(true); 
       scheduler->getSysReg(ID)->clearSysReg(); 
       рrocessImage->setSysReg(NULL);
       // 2. удалить из очереди 
       scheduler->pop(state, рrocessImage);
       // 3. сохранить наблюдение для метода SPN;       
       if (scheduler->vParam.getStatTime() != noParam ) {
         scheduler->setObservation(рrocessImage);  
         if (scheduler->vParam.getState(TimeExecNotPreem)) // выполнения SPN
           scheduler->setTthreshold(TimeExec);
         // 4. установить  порог наименьшего ожидаемого времени
         if (scheduler->vParam.getState(TimeExec)) // выполнения SRT
           scheduler->setTthreshold(TimeExec);
         if (scheduler->vParam.getState(TimeServ))
           scheduler->setTthreshold(TimeServ);   // обслуживания HRRN
       }
       // 4. освободить virtualMemory, инициализировать PCB
       virtualMemory->clearImage(((PCB*)handle)->getVirtualAddr());
    }

    // 
    virtual void initProcessID(ProcessImage *processImage, Memory *code)
	{ 
      //1. определить ID   
      int ID = scheduler->getID();
      if (ID >= 0)  { // ID не -1, то есть массив регистров 
        processImage->setID(ID);  
      }
	  else { // в очередь как новый 
          cout << " init Process ID > MAX_PROCESS in system "<< endl;
      }
      //2. выделить регистры по ID
      scheduler->getSysReg(ID)->setStatus(false);
      processImage->setSysReg(scheduler->getSysReg(ID)); 
      //3. состояние процесса 
      processImage->setState(NotRunning);
      //4. разместить программу 
      processImage->setCode(code);
      //code = NULL; // к программе нет доступа из вне
      //5. установить время входа
      processImage->setTenter(Timer::getTime());
      //6. инициализировать время обслуживания 
      processImage->setTservice(0);
      //7. в очередь как NotRunning
        scheduler->push(NotRunning, processImage);
    }
    
    // номера пунктов приведены в соответствии с порядком изучения
    virtual void dispatch(){  
      ProcessImage * рrocessImage = NULL; 
      Timer::tick(); // увеличить время
      //1. просмотреть всю очередь Running, если есть завершившиеся, 
      // убрать только из очереди
      // в виртуальной памяти объект образа удаляется только методом ядра
      int size_ = scheduler->size(Running);
      for (int i = 0; i < size_; i++){
        рrocessImage = (ProcessImage *)(scheduler->front(Running));
        // увеличить время обслуживания 
        рrocessImage->setTservice(рrocessImage->getTservice()+1);
        if  (рrocessImage->getState() == ExitProcess ) { // процесс завершен
          scheduler->pop(Running); // удаляем из очереди 
          scheduler->push(ExitProcess, рrocessImage); //ставим в очередь выполненых
        }
      }
      // 3. просмотреть очередь Blocked
      size_ = scheduler->size(Blocked);
      for (int i = 0; i < size_; i++){
        рrocessImage = (ProcessImage *)(scheduler->front(Blocked));
        // увеличить время обслуживания 
        рrocessImage->setTservice(рrocessImage->getTservice()+1);
        if (!cs->getCS()) { // критическая секция свободна 
          scheduler->pop(Blocked); // удаляем из очереди 
          рrocessImage->setState(Running); // изменяем состояние процесса
          рrocessImage->setFlag(1);
          // не изменяем приоритет процесса
          scheduler->push(Running, рrocessImage); // ставим в очередь выполняемых
          cs->setCS(true); // закрыть секцию
          break; 
        }
      }
      //2. просмотреть очередь NotRunning и, 
      // если для образа определены все ресурсы (регистры и т.д.) 
      // поставить на выполнение

      size_ = scheduler->size(NotRunning);
      for (int i = 0; i < size_; i++){
        рrocessImage =  (ProcessImage *)(scheduler->front(NotRunning));
        //увеличить время обслуживания 
        рrocessImage->setTservice(рrocessImage->getTservice()+1); 
        // выделены ли под образ регистры системы и процессор 
        if (рrocessImage->getSysReg() == NULL) {
          // выделить регистры, если нет свободных, оставить в очереди   
          SysReg *sysReg = scheduler->getSysReg(scheduler->getID());
          if (sysReg == NULL) {
            scheduler->pop(NotRunning); // удаляем из очереди
            scheduler->push(NotRunning, рrocessImage); // ставим в эту же очередь
            continue;
          };
          рrocessImage->setSysReg(sysReg);
        }
        scheduler->pop(NotRunning); // удаляем из очереди
        //2.1. Code загрузить в память, процессор выделен
        // обратиться к блоку управления памятью, если есть ресурс  
        if (рrocessImage->getFlag() == 1) {
          рrocessImage->setState(Running); //изменяем состояние процесса
          // устанотвить квант времени и приоритет, в зависимости от метода   
          рrocessImage->setTimeSlice(this->timeSlice); // метод RR
          if ( prSlice.getPrioritySlice()){ // метод DP
            int pr = рrocessImage->getPriority();
            рrocessImage->setTimeSlice(prSlice.getTimeSlice(pr));
            рrocessImage->setPriority(prSlice.getPriority(pr));
          }
          scheduler->push(Running, рrocessImage); // ставим в очередь выполняемых
        } else {
         рrocessImage->setState(Blocked); // изменяем состояние процесса
         scheduler->push(Blocked, рrocessImage); // ставим в очередь блокированных
        }
      } // end for
      //3. просмотреть очередь Swapped и применить метод DP
      // выполняется только для метода DP
      bool priority = prSlice.getPrioritySlice(); 
      if ( priority ){ // метод DP
        size_ = scheduler->size(Swapped);
        for (int i = 0; i < size_; i++){
          рrocessImage =  (ProcessImage *)(scheduler->front(Swapped));
          // 3.1. изменить приоритет процесса и TimeSlice
          int pr = рrocessImage->getPriority();
          рrocessImage->setTimeSlice(prSlice.getTimeSlice(pr));
          рrocessImage->setPriority( prSlice.getPriority(pr)); 
          scheduler->pop(Swapped); // удаляем из очереди
          scheduler->push(Swapped, рrocessImage); // ставим в эту же очередь
        }
      }
      // подготовить работу
      if ( scheduler->empty(Running) && scheduler->empty(Swapped) ) {
          cout << "dispatch: empty Running && Swapped" <<endl;
        return;//нет заданий на выполнение
      }
      scheduleProcess(mmu, priority);

      // выполнить процессы в соответствии со сформированным заданием
      Interrupt interrupt = executeProcess(mmu); 
    }

    // выполнить процессы в соответствии со cформированным заданием
    virtual void scheduleProcess(MMU * mmu, bool priority){
        bool maxProcess = scheduler->scheduleJob(mmu,priority);
      // задание сформировано  
      // maxProcess  == true превышение MAX_PROCES, есть еще процессы
      // maxProcess  == false, все процессы вошли в задание
    }

    // выполнить процессы в в соответствии со сформированным заданием
    virtual Interrupt executeProcess (MMU * mmu){return scheduler->еxecute(mmu);} 

    // MultiProcessor scheduling (MP) - многопроцессорное планирование
//	void dispatchMP(){
//	  this->dispatch();
/*	  Scheduler* scheduler[5];
      scheduler[0] = new Scheduler(this->scheduler);
      // конструктор прототипирования
      if (scheduler[0]) {            // процессор свободен
          scheduler[0]->execute(mmu, priority); 
      } */ 
//	}
    // для управления критической секцией
    HANDLE * getHandleCS(){ return (HANDLE *)cs; }
    
    void EnterCriticalSection(HANDLE* handle){
      if (!cs->getCS()) {   // ресурс не занят
        // для этого процесса, критическая секция открыта
        ((ProcessImage *) handle)->setFlag(1);
        cs->setCS(true); // закрать критическую секцию для других  
      }
      else { 
          // закрать критическую секцию для этого процесса
        ((ProcessImage *) handle)->setFlag(0); 
      }  
    }
    void LeaveCriticalSection(HANDLE* handle){
      cs->setCS(false); // открыть критическую секцию не занят, false 
    } 

    // для планирования по методу RR
    void setTimeSlice(int timeSlice){this ->timeSlice = timeSlice;}
    void  resetTimeSlice(){ 
      this ->timeSlice = -1;
      this->prSlice.setPrioritySlice(-1,NULL); 
    }

    // для планирования по методу DP (динамических приоритетов)  
    void setTimeSlice(int size, int * prSlice){ 
      this->prSlice.setPrioritySlice(size,prSlice);
    }
    
    // для метода SRT и HRRN
    void setTpredict(StatTime time){ scheduler->vParam.setState(time);}
    void resetTpredict() {scheduler->vParam.clearVectorParam();}
    double getTpredict(string user,StatTime time){return scheduler->getTpredict(user,time);}
   
    void DebugQueue(State state){scheduler->DebugQueue(state);}
    void DebugVirtualMemory(){this->virtualMemory->Debug();}
    void DebugPCB(HANDLE* handle){handle->ProcessTime();
      ((ProcessImage *)handle)->Debug();
    }

    class PrioritySlice {
      public:
        PrioritySlice(){
          this-> size = -1;
          this-> prSlice = NULL;
        }  
        void setPrioritySlice(int size, int * prSlice){
          this-> size = size;  
          this-> prSlice = prSlice;
        }  
        bool getPrioritySlice () {
          if (prSlice == NULL) return false;
          return true;
        }

        int getTimeSlice(int priority) {
          if (size == -1) return -1; // timeSlice не установлен
          if (priority == -1)  return prSlice[0];
          // оставляем последний приоритет 
          if (priority == size-1) return prSlice[size-1]; 
          return prSlice[priority+1];
        }
        int getPriority(int priority) {
          if (priority == -1)  return 0;
          if (priority == size-1) return size-1;
          return priority+1;
        }

      private: 
        int size;
        int * prSlice; // массив приоритетов и квантов времени 
                       // для планирования по методу DP 
    };

  protected:
    VirtualMemory * virtualMemory;
    Scheduler* scheduler;
    MMU* mmu;   
    CS* cs;        // critical section 
    int timeSlice; // квант времени для планирования по методу RR  
    PrioritySlice prSlice; // для планирования по методу DP  
};



/////////////////////////////////////////////////////////////////////////////////////////




class myDispatcher : public Dispatcher {
public:
	myDispatcher (int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES, Scheduler * scheduler, MMU * mmu) : 
							Dispatcher (SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES, scheduler, mmu)
	{
		cout << "myDispatcher enabled" << endl;
	}
		
	virtual void initProcessIDc(ProcessImage *processImage, Memory *code) { 
		processImage->setID(-10);  
		//2. выделить регистры по ID 
		//3. состояние процесса 
		processImage->setState(New);
		//4. разместить программу 
		processImage->setCode(code);
		//code = NULL; // к программе нет доступа из вне
		//6. инициализировать время обслуживания 
		processImage->setTservice(0);
		//7. в очередь как New
		scheduler->push(New, processImage);
	}

	virtual void dispatch () {  
		ProcessImage * рrocessImage = NULL; 
		//1. просмотреть всю очередь New, поставить в очередь NotRunnig
		// в виртуальной памяти объект образа удаляется только методом ядра
		int size_ = scheduler->size(New);
		for (int i = 0; i < size_; i++){
			рrocessImage = (ProcessImage *)(scheduler->front(New));
			Dispatcher::initProcessID(рrocessImage,рrocessImage->getCode());
			// увеличить время обслуживания 
			рrocessImage->setTservice(рrocessImage->getTservice()+1);
			scheduler->pop(New);
        }
		Dispatcher::dispatch();
	}
	
	void wait(HANDLE* handle){
		((ProcessImage*)handle)->setFlag(0);
		cs->setCS(false);
	}
	
	//функция, которая выполняет процесс использующий нужный ресурс
	void past(HANDLE* handle){
		((ProcessImage*)handle)->setFlag(1);
		cs->setCS(true);
	}

};


#endif