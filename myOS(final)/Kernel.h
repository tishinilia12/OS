#ifndef __Kernel_H__
#define __Kernel_H__

#include "Dispatcher.h"
#include "Scheduler.h"

class Kernel {
public:
    Kernel(Dispatcher *dispatcher){
      this ->dispatcher = dispatcher;
    }

    // создание процесса 
    HANDLE * CreateProcess(string user, Memory *code){
      // обратиться к блоку управления VirtualMemory
      // для выделнения памяти под образ процесса
      ProcessImage *processImage = dispatcher -> getVirtualMemory();
      if (processImage == NULL) return NULL; // нет памяти
      processImage->setUser(user); 
      dispatcher->initProcessID(processImage,code); // инициализировать процесс
      return (HANDLE *)processImage;
    } // end CreateProcess   
    
    // удаление процесса
    void TerminateProcess(HANDLE* handle){
      // 1. очистить VirtualMemory и очистить sysreg процесса 
      dispatcher -> freeVirtualMemory(handle);
    }
    // методы критической секции 
    HANDLE* CreateCriticalSection(){ return dispatcher->getHandleCS(); }

    void EnterCriticalSection(HANDLE* handle){
      dispatcher -> EnterCriticalSection(handle); 
    }
    void LeaveCriticalSection(HANDLE* handle){
      dispatcher -> LeaveCriticalSection(handle);
    }

    void DebugProcessImage(HANDLE *handle){
      ((ProcessImage *)handle)->Debug();}
private:
    Dispatcher *dispatcher;
protected:
};



///////////////////////////////////////////////////////////////////////////////////////////////////



class myKernel: public Kernel{
	public:
		myKernel(myDispatcher *dispatcher):Kernel(dispatcher) { //в базовом классе хранится ссылка на диспетчер как на объект базового класса
			this->dispatcher=dispatcher; //в производном классе также сохраняется ссылка на диспетчер как на объект производного класса
		}
		// методы блокирующей переменной
		// функция, которая помещает процесс в состояние ожидание
		void wait(HANDLE *handle){
			dispatcher->wait(handle);
		}
		//функция, которая выполняет процесс использующий нужный ресурс
		void past(HANDLE *handle){
			dispatcher->past(handle);
		}
	protected:
		myDispatcher *dispatcher;
};


#endif