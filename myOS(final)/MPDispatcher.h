#ifndef __MPDispatcher_H__
#define __MPDispatcher_H__

#include <iostream>
using namespace std;
#include "ArchitectureCPU.h"
#include "VirtualMemory.h"
#include "Scheduler.h"
#include "Process.h"

#include <math.h> // from version 8.0

/* MultiProcessor scheduling (MP) - многопроцессорное планирование
       метод Global queue многопроцессорное планирование  
   массив системных регистров для каждого процессора
   учитывается в dispatch */ 
class MPDispatcher: public Dispatcher {
  public:
    MPDispatcher (int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES,Scheduler * scheduler,MMU * mmu,
        int MAX_PROCESSOR):
      Dispatcher(SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES,scheduler,mmu) {
        this->MAX_PROCESSOR = MAX_PROCESSOR;
        vaScheduler = valarray <Scheduler *> (MAX_PROCESSOR); 
        vaStatus    = valarray <bool> (MAX_PROCESSOR); 
        for(int i=0; i < MAX_PROCESSOR; i++) {
          this->vaScheduler[i] = new Scheduler(scheduler);  
          this-> vaStatus[i]= true; // процессор свободен, false занят     
        }
    }
    
    // cформировать задание, замещение scheduleProcess класса Dispatcher   
    void scheduleProcess(MMU * mmu, bool priority){
      cout << " ***   scheduleProcess - overriding functions   *** " <<endl;
      // 1. распределить процессы по процессорам
      //    скольео процессов, может выполняться на однои процессоре
      int quotaProcess = (int)round((double)scheduler->getProcess()/MAX_PROCESSOR);
      scheduler->setQuotaProcess(quotaProcess);
      bool maxProcess = false;
      // определить есть ли процессы в очереди 
      for(int i=0; i < MAX_PROCESSOR; i++) {
        // 2. Сформировать задание из общей очереди
          maxProcess =  scheduler->scheduleJob(mmu,priority);
        // 3. Передать задание свободному процессору 
          vaScheduler[i]->setJob(scheduler->getJob());
        // 4. отметить, что процессор занят 
          vaStatus[i] = false;
       if (! maxProcess) break;  
       // maxProcess  == true превышение MAX_PROCESS, есть еще процессы	  
       // maxProcess  == false, все процессы вошли в задание  
      }
MPDebug();
mmu->DebugMemory();
    }
    //3. выполнить задания на процессорах
    Interrupt executeProcess (MMU * mmu ){
    cout << " ***   executeProcess - overriding functions  *** " <<endl;
      Interrupt interrupt;
      for(int i=0; i < MAX_PROCESSOR; i++) {
        if (! vaStatus[i]) {// занят  
          interrupt = vaScheduler[i]->еxecute(mmu);
          vaStatus[i]= true; // освободить процессор 
        }
      }
      return interrupt;
    } 

    void MPDebug(){
      for(int i=0; i < MAX_PROCESSOR; i++) {
        cout << "--- Processor --- " << i <<" state = " << vaStatus[i] << endl;
        //scheduler->DebugSPNQueue();
        //vaScheduler[i]->DebugSPNQueue();
        //char ch;
        //cin >> ch;
      }
    }
  private:
    // Symmetric Rounding
    double round(double value) {
      if (value < 0.0)
        return ceil( value );
      else
        return floor( value );
    }
	  
	int MAX_PROCESSOR;
    valarray <Scheduler *> vaScheduler;
    valarray <bool> vaStatus;
 
};
#endif