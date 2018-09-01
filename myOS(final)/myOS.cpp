#include "stdafx.h"
#include <string>
#include <iostream>

#include "CPU.H"
#include "myCPU.H"

#include "MMU.H"
#include "Memory.h"
#include "Scheduler.h"
#include "Process.h"
#include "Kernel.h"
#include "HANDLE.h"
#include "Dispatcher.h"
#include "MPDispatcher.h"

#include "Loader.h"
#include "myBuffer.h"

using namespace std;

class myOS {

public:
	myOS(int SIZE_OF_MEMORY_IN_BLOCKS, int MAX_PROCESS, myBuffer *buffer){
		this-> SIZE_OF_MEMORY_IN_BLOCKS = SIZE_OF_MEMORY_IN_BLOCKS;
   		this-> MAX_PROCESS =  MAX_PROCESS; 
		this-> buffer = buffer;
		this-> memory = new myMemory(SIZE_OF_MEMORY_IN_BLOCKS);
		this-> mmu = new myMMU(*memory);
		//this-> file = new File (&lan);
		//this-> protocol = new Protocol (&lan);
		this-> dev = new Device;
		
		//this-> cpu = new myCPU(*memory,MAX_PROCESS, new LAN, new Device, new myLAN, buffer);
		this-> interrupt = OK; 
		//this-> scheduler = new myScheduler (cpu, MAX_PROCESS); 
		this-> job = new Job[MAX_PROCESS];
		this-> SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES = MAX_PROCESS;	
		this-> dispatcher = new myDispatcher(SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES, scheduler,mmu);
		this-> kernel = new myKernel(dispatcher);
		this-> code = NULL, code_1 = NULL, code_2 = NULL;
		this-> loader = new Loader(mmu,dispatcher,kernel); } //параметры
		
		void exeprog (char*name) {
			this->handle = loader->load(name);
			kernel->DebugProcessImage(handle);  // последний
			dispatcher->dispatch();
			kernel->TerminateProcess(handle); 
		}

		void debug(){
			memory->debugHeap();
		}
		void clearMemory(){
			memory->clearMemory();
		}

		int dispatch();

private:
	int SIZE_OF_MEMORY_IN_BLOCKS;   // размер оперативной памяти в блоках
	int MAX_PROCESS;				// максимальное количество процессов 
	myMMU *mmu;	
	myMemory *memory;	
	CPU *cpu;	
	Interrupt interrupt;
	Scheduler *scheduler; //myScheduler
	Job* job; // моделирование многозадачного режима 3
	int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES; // размер виртуальной памяти в образах процесса
	myDispatcher *dispatcher;
	myKernel *kernel;
	Code *code, *code_1, *code_2;
	Loader *loader;
	HANDLE *handle_1, * handle_2, * handle; // для трех процессов 
	myBuffer *buffer;
	LAN lan;
	Device *dev; //Добавил dev
	//File *file;
	//Protocol *protocol;
}; 


/////////////////////////////////////////////////////////////////////////////////////////

int myOS::dispatch() {

	Interrupt interrupt = OK;
	
	myBuffer *buffer = new myBuffer;

	myCPU *cpu = new myCPU (memory,MAX_PROCESS, &lan, new Device, new MyLAN,buffer);

		//new File(&lan)
//							 new Protocol(&lan)

	/* планирование расписания для выполнения программ
       для однопроцессорных систем */ 
	myScheduler scheduler(cpu, MAX_PROCESS);
    Job* job = new Job[MAX_PROCESS]; // моделирование многозадачного режима 3

	char ch = 'Y'; 

	/* демонстрация создания образа процесса
	   при  создании объектов класса Code указыается точный
	   размер кода программы в блоках, т.к. количество строк */
	Code *code = NULL, *code_1 = NULL, *code_2 = NULL, *prog = NULL;

    HANDLE *handle_1 = NULL, * handle_2 = NULL; // для двух процессов 
    // размер виртуальной памяти в образах процесса
	int SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES = MAX_PROCESS; 

	myDispatcher dispatcher (SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES, &scheduler, mmu);
	myKernel kernel (&dispatcher);
	Loader loader (mmu, &dispatcher, &kernel);

	while (ch == 'Y' || ch == 'y' || ch == 'N'  ) {
		cout << endl;
		cout << "dialog mode: \n"
		"lab 1. Computer architecture. execute programm 1,2 . . . . . . . enter 1\n"
		"lab 2. OS architecture. MultiTasking execute programm 1 and 2. . enter 2\n"
		"lab 3. Model process. Create Process for programm 1 and 2. . . . enter 3\n"
		"lab 4. Synchronize process.CriticalSection for programm 1 and 2. enter 4\n"
		"lab 5. Main memory and Management Memory Unit  . . . . . . . . . enter 5\n"
		"lab 6. Virtual memory and process control block - PCB. . . . . . enter 6\n"
		"lab 7. Scheduling for one processor system. Methods. . . . . . . enter 7\n"
		"lab 8. MultiProcessor scheduling. Methods. . . . . . . . . . . . enter 8"<< endl;
		cin >> ch;
  
switch (ch) {



case '1':  

	cout << " execute P1: Y/N-any "<< endl;
	cin >> ch;
	if (ch == 'Y' || ch == 'y') {
	/*  P2: вычисление арифметического выражения
		70-(X+3)*5+10/2 = 5(12-X)	
		X - данные из файла (считываеются с устройства LAN)  */
		cout << endl;
		cout << "dump SysReg & first block execution: Y/N-any "<< endl;
		cin >> ch;
		if (ch == 'Y' || ch == 'y') { // присвоить адрес в PС, выполняемого блока 
			// для программы по умолчанию выделен пул регистров с индесом 0
			scheduler.getSysReg(0) -> setState (PC, 0);
			scheduler.DebugBlock (0, cpu);	//трассировка состояния регистров
		}										//и блока,его выполнение

/* Programm 1: вычисление арифметического выражения
 (15+5)/(7+3)+(6/2)*(9/3)=11 и вывод результата на устойство (Dev) печати 
   Деление и умножение в модели CPU определены только между регистрами.
   Программа в начале работы размещается по нулевому адресу */
//(15+5)/(7+3)+(6/2)*(9/3)=11
	memory->setCmd(Mov,r1,15);
	memory->setCmd(Mov,r2,5);
	memory->setCmd(Add,r1,r2);
	memory->setCmd(Mov,r2,7);
	memory->setCmd(Mov,r3,3);
	memory->setCmd(Add,r2,r3);
	memory->setCmd(Div,r1,r2);
	memory->setCmd(Mov,r4,6);
	memory->setCmd(Mov,r5,2);
	memory->setCmd(Div,r4,r5);
	memory->setCmd(Mov,r5,9);
	memory->setCmd(Mov,r6,3);
	memory->setCmd(Div,r5,r6);
	memory->setCmd(Mul,r4,r5);
	memory->setCmd(Add,r1,r4);
	memory->setCmd(Int,r1,MyLanW);
    memory->setCmd(Int,r1,Lan);
	memory->setCmd(Wmem,r1,70);
	memory->setCmd(Int,0,Exit);

		cout << endl;
		cout << " P1 in memory: dump memory Y/N-any "<< endl;
		cin >> ch;
		if (ch == 'Y' || ch == 'y')	//просмотреть содержимое памяти,
			memory -> debugHeap();	//после загрузки программы 	
		cout << " dump SysReg & execute P1: Y/N-any "<< endl;
		cin >> ch;
		if (ch == 'Y' || ch == 'y') {
			cout << " begin P1 cpu.execute "<< endl; 
			scheduler.DebugSysReg(0);   // трассировка состояния регистров до выполнения
			/*  выполнение программы расположенной по адресу 0
				идентификатор  процесса  id = 0 для P2 */

			interrupt = scheduler.execute (0, 0, cpu); 
			
			scheduler.DebugSysReg (0); // трассировка состояния регистров после выполнения
			cout << " end   P1 cpu.execute interrupt = " << interrupt << endl;
			cout << endl;
		} 
	} // end execute programm 2 

	cout << " execute P2: Y/N-any "<< endl;
	cin >> ch;
	if (ch == 'Y' || ch == 'y') {
/* Programm 2: умножение двух чисел  (11+3)*2-(2+6)*10=-52 
    mmu.setAlloc(0); -- запись на место, программы в памяти
	память не очищается, запись производится в свободную память */
	mmu->setAlloc(20);
	// (11+3)*2-(2+6)*10=-52
	memory->setCmd(Int,r1,MyLanR);
	memory->setCmd(Rmem,r1,70);
	memory->setCmd(Mov,r2,3);
	memory->setCmd(Add,r1,r2);
	memory->setCmd(Mul,r1,2);
	memory->setCmd(Mov,r3,2);
	memory->setCmd(Mov,r4,6);
	memory->setCmd(Add,r3,r4);
	memory->setCmd(Mul,r3,10);
	memory->setCmd(Sub,r1,r3);
	memory->setCmd(Int,r1,Dev);
	memory->setCmd(Int,0,Exit);
    
		cout << endl;
		cout << " programm 2 in memory: dump memory Y/-any "<< endl;
		cin >> ch;
		//просмотреть содержимое памяти, после загрузки программы :
		if (ch == 'Y' || ch == 'y') 
			memory -> debugHeap();
		cout << " SysReg & execute P2: Y/-any "<< endl;
		cin >> ch;
		if (ch == 'Y' || ch == 'y') {
			cout << " begin P2 cpu.execute "<< endl;                  
			scheduler.DebugSysReg (1); // трассировка состояния регистров
			/*  выполнение программы расположенной по адресу 15
				идентификатор  процесса  id = 1 для P1 */
			interrupt = scheduler.execute (15, 1, cpu); 
			scheduler.DebugSysReg (1); // трассировка состояния регистров
			cout << " end   P2 cpu.execute interrupt = "<< interrupt << endl;
			cout << endl;
		}
		
		cout << " dump memory & SysReg Y/-any "<< endl;
		cin >> ch;
		if (ch == 'Y' || ch == 'y') { 
			memory -> debugHeap();
			cout << endl;
			scheduler.DebugSysReg (1);
		}
	}
	memory -> clearMemory();
	break;


case '2':

	cout << endl;
	cout << " clear memory & load P1: addr=10 & P2 addr=40" << endl;   
    memory->clearMemory();

    mmu->setAlloc(10);
/* Programm 1: вычисление арифметического выражения
   (15+5)/(7+3)+(6/2)*(9/3)=11 и вывод результата на устойство (Dev) печати 
   Деление и умножение в модели CPU определены только между регистрами.
   Программа в начале работы размещается по нулевому адресу */
	mmu->setAlloc(10);
    memory->setCmd(Mov,r1,15);
	memory->setCmd(Mov,r2,5);
	memory->setCmd(Add,r1,r2);
	memory->setCmd(Mov,r2,7);
	memory->setCmd(Mov,r3,3);
	memory->setCmd(Add,r2,r3);
	memory->setCmd(Div,r1,r2);
	memory->setCmd(Mov,r4,6);
	memory->setCmd(Mov,r5,2);
	memory->setCmd(Div,r4,r5);
	memory->setCmd(Mov,r5,9);
	memory->setCmd(Mov,r6,3);
	memory->setCmd(Div,r5,r6);
	memory->setCmd(Mul,r4,r5);
	memory->setCmd(Add,r1,r4);
	memory->setCmd(Wmem,r1, 69);  // запись содержимое r1 в память по адресу 69     
	memory->setCmd(Int, r1, Dev); // обращение к устройству для печати содержимого r1   
	memory->setCmd(Int, r1, MyLanW); // посыл содержимого r1 по сети   
	memory->setCmd(Int, 0, Exit); // все программы должны заканчиваться прервыванием Exit 
	mmu->setAlloc(40);
/* Programm 2: 
    mmu.setAlloc(0); -- запись на место, программы в памяти
	память не очищается, запись производится в свободную память */
		mmu->setAlloc(40);
	memory->setCmd(Mov,r1,11);
	memory->setCmd(Mov,r2,3);
	memory->setCmd(Add,r1,r2);
	memory->setCmd(Mul,r1,2);
	memory->setCmd(Mov,r3,2);
	memory->setCmd(Mov,r4,6);
	memory->setCmd(Add,r3,r4);
	memory->setCmd(Mul,r3,10);
	memory->setCmd(Sub,r1,r3);  // регистр * регистр
	memory->setCmd(Wmem,r1, 70);  // запись содержимое r1 в память по адресу 70   
	memory->setCmd(Rmem,r1, 70);  // чтение содержимого r1 из памяти по адресу 70   
	memory->setCmd(Int, r1, Dev); // обращение к устройству для печати содержимого r1   
	memory->setCmd(Int, 0, Exit);
	cout << " dump memory: Y/N-any "<< endl;
    cin >> ch;
	if (ch == 'Y') 
      memory->debugHeap();

	// в job: первый элемент начальный адрес процесса в памяти 
	//        второй элемент отведен под id процесса 
	//        третий элемент - индикатор выполнения задания 
	job[0].set(10,0,true);
	job[1].set(40,1,true);
	
	cout << " Multitasking execute p1,p2: Y/N-any "<< endl;
    cin >> ch;
	if (ch == 'Y') {
	  cout << " begin P1,P2 cpu.execute "<< endl; 
	  // многозадачное выполнение программы: 
	  // адреса задаются в массиве job (работа)
	  // по прерыванию таймера, программа выполняет одну инструкцию 
	  scheduler.setSliceCPU(0);
      interrupt = scheduler.еxecute(job, NULL);  
      cout << " end   P1,P2 cpu.execute interrupt = "<< interrupt << endl;
	}
	cout << " dump memory: Y/N-any "<< endl;
    cin >> ch;
	if (ch == 'Y')
      memory->debugHeap();
	
	cout << " clear memory: Y/N-any "<< endl;
    cin >> ch;
	if (ch == 'Y'){
      memory->clearMemory();		
      memory->debugHeap();
	  scheduler.getSysReg(1)->clearSysReg(); // очистить регистры
	}
    break;



case '3':

	cout << endl;
	cout << " create Process P1 and P2 from code: " << endl;
	// 1. для каждой программы должен создаваться свой объект 
	// 2. код в примерах записывается с 0 адреса 
	 code = new Code(17);
	 code->setCmd(Mov,r1,15);
	code->setCmd(Mov,r2,5);
	code->setCmd(Add,r1,r2);
	code->setCmd(Mov,r2,7);
	code->setCmd(Mov,r3,3);
	code->setCmd(Add,r2,r3);
	code->setCmd(Div,r1,r2);
	code->setCmd(Mov,r4,6);
	code->setCmd(Mov,r5,2);
	code->setCmd(Div,r4,r5);
	code->setCmd(Mov,r5,9);
	code->setCmd(Mov,r6,3);
	code->setCmd(Div,r5,r6);
	code->setCmd(Mul,r4,r5);
	code->setCmd(Add,r1,r4);
	code->setCmd(Wmem,r1,71);
	code->setCmd(Int,r1,Dev);
	code->setCmd(Int, 0, Exit);
    
	cout << " programm 1 in code: dump code Y/N-any "<< endl;
	cin >> ch;
	if (ch == 'Y' || ch == 'y')
		code->debugHeap(); //просмотреть содержимое памяти в которой программа     

	handle_1 = kernel.CreateProcess("P1",code);  // создать процесс

 code = new Code(11);
	code->setCmd(Rmem,r1,71);
	code->setCmd(Mov,r2,3);
	code->setCmd(Add,r1,r2);
	code->setCmd(Mul,r1,2);
	code->setCmd(Mov,r3,2);
	code->setCmd(Mov,r4,6);
	code->setCmd(Add,r3,r4);
	code->setCmd(Mul,r3,10);
	code->setCmd(Sub,r1,r3);
	code->setCmd(Int,r1,Dev);
	code->setCmd(Int,0,Exit);
	handle_2 = kernel.CreateProcess("P2",code);  // создать процесс
	    
	kernel.DebugProcessImage(handle_2);  // последний

	//cout << " dump queue NotRunning Y/N-any "<< endl;
	cout << " dump queue New Y/N-any "<< endl;
	cin >> ch;
	if (ch == 'Y' || ch == 'y') 
		//dispatcher.DebugQueue(NotRunning);
		dispatcher.DebugQueue(New);

	cout << " dispatcher queue & execute Y/N-any "<< endl;
	cin >> ch;
	dispatcher.dispatch(); //  изменяет состояние системы 
							//  так как ядро не под управлением CPU
							//  для модели из трех очередей 2 вызова 

	cout << " dump memory: Y/N-any "<< endl;
	cin >> ch;
	if (ch == 'Y' || ch == 'y') 
		memory->debugHeap();

	cout << " dump queue NotRunning & Running Y/N-any "<< endl;
	cin >> ch;
	if (ch == 'Y' || ch == 'y') { 
		dispatcher.DebugQueue(NotRunning);
		dispatcher.DebugQueue(Running); 
	}

	cout << " dispatch queue & dump queue ExitProcess Y/N-any "<< endl;
	cin >> ch;
	dispatcher.dispatch(); // изменяет состояние системы 
	if (ch == 'Y' || ch == 'y') 
		dispatcher.DebugQueue(ExitProcess); 
	
	cout << " TerminateProcess & dump ProcessImage Y/N-any "<< endl;
	cin >> ch;
	kernel.TerminateProcess(handle_1);  // завершить процесс  
	kernel.TerminateProcess(handle_2);  // завершить процесс  
	if (ch == 'Y' || ch == 'y') {
		kernel.DebugProcessImage(handle_1);//проверка:виртуальная память освобождена
		kernel.DebugProcessImage(handle_2);//проверка:виртуальная память освобождена
	}

	memory->clearMemory();		
	break;


case '4':
	 cout << endl;
     cout <<"create unsynchronized Process P1 and P2,"
		    "execute,look Error:"<< endl;
	
	
	/* Programm 1: вычисление арифметического выражения
   (30-3)/3+1=10 и вывод результата на устойство (Dev) печати 
   Деление и умножение в модели CPU определены только между регистрами.
   Программа в начале работы размещается по нулевому адресу */
	
 code_1 = new Code(17);
	 code_1->setCmd(Mov,r1,15);
	code_1->setCmd(Mov,r2,5);
	code_1->setCmd(Add,r1,r2);
	code_1->setCmd(Mov,r2,7);
	code_1->setCmd(Mov,r3,3);
	code_1->setCmd(Add,r2,r3);
	code_1->setCmd(Div,r1,r2);
	code_1->setCmd(Mov,r4,6);
	code_1->setCmd(Mov,r5,2);
	code_1->setCmd(Div,r4,r5);
	code_1->setCmd(Mov,r5,9);
	code_1->setCmd(Mov,r6,3);
	code_1->setCmd(Div,r5,r6);
	code_1->setCmd(Mul,r4,r5);
	code_1->setCmd(Add,r1,r4);
	code_1->setCmd(Wmem,r1,71);
	code_1->setCmd(Int,r1,Dev);
	code_1->setCmd(Int, 0, Exit);

	 code_2 = new Code(11);
	code_2->setCmd(Rmem,r1,71);
	code_2->setCmd(Mov,r2,3);
	code_2->setCmd(Add,r1,r2);
	code_2->setCmd(Mul,r1,2);
	code_2->setCmd(Mov,r3,2);
	code_2->setCmd(Mov,r4,6);
	code_2->setCmd(Add,r3,r4);
	code_2->setCmd(Mul,r3,10);
	code_2->setCmd(Sub,r1,r3);
	code_2->setCmd(Int,r1,Dev);
	code_2->setCmd(Int,0,Exit);

     handle_1 = kernel.CreateProcess("P2",code_1); // создать процесс
	 handle_2 = kernel.CreateProcess("P1",code_2);  // создать процесс

	 dispatcher.dispatch(); // изменяет состояние системы 2 вызова                 
  	 
	 handle_1->ProcessTime(); 
	 handle_2->ProcessTime();

   /* программы работают асинхронно, поэтому P1 не успевает осцществить 
      запись содержимое r1 в память по адресу 70, строка 2
      программа P2 читает содержимого r1 из памяти по адресу 70,
	  но память пуста */        

   cout << " Dump memory Y/N-any "<< endl;
   cin >> ch;
   if (ch == 'Y') {
     memory->debugHeap();
   }
   kernel.TerminateProcess(handle_1); // завершить процесс
   kernel.TerminateProcess(handle_2); // завершить процесс  
   
   memory->clearMemory();

   cout << endl;
   cout << " create synchronized Process P1 and P2" << endl;
   cout << " using CriticalSection Y/N-any" << endl;
   cin >> ch;
   if (ch == 'Y') {

     handle_1 = kernel.CreateProcess("P2",code_1);  // создать процесс
	 handle_2 = kernel.CreateProcess("P1",code_2);  // создать процесс

	 kernel.wait(handle_2);	     
	 kernel.past(handle_1);
       
     // выпополнение программы P1
     cout << " --------- P2 dispatch ----------" <<endl;
     dispatcher.dispatch(); // изменяет состояние системы 1-й шаг                         
     kernel.DebugProcessImage(handle_1); // состояние процессов
     kernel.DebugProcessImage(handle_2); // состояние процессов

	 kernel.past(handle_2);
     // выпополнение программы P2
     cout << " --------- P2  dispatch ---------" <<endl;
     dispatcher.dispatch(); // изменяет состояние системы 2-й шаг
       kernel.DebugProcessImage(handle_2); //проверка: виртуальная память освобождена
       memory->debugHeap();
     cout << " dispatch" <<endl;
       dispatcher.dispatch(); // изменяет состояние системы 3-й шаг

	 handle_1->ProcessTime(); 
	 handle_2->ProcessTime();

	 kernel.TerminateProcess(handle_1); // завершить процесс  
	 kernel.TerminateProcess(handle_2); // завершить процесс
   }
   memory->clearMemory();
   break;
   case '5':
	cout << endl;
	
 code_1 = new Code(17);
	 code_1->setCmd(Mov,r1,15);
	code_1->setCmd(Mov,r2,5);
	code_1->setCmd(Add,r1,r2);
	code_1->setCmd(Mov,r2,7);
	code_1->setCmd(Mov,r3,3);
	code_1->setCmd(Add,r2,r3);
	code_1->setCmd(Div,r1,r2);
	code_1->setCmd(Mov,r4,6);
	code_1->setCmd(Mov,r5,2);
	code_1->setCmd(Div,r4,r5);
	code_1->setCmd(Mov,r5,9);
	code_1->setCmd(Mov,r6,3);
	code_1->setCmd(Div,r5,r6);
	code_1->setCmd(Mul,r4,r5);
	code_1->setCmd(Add,r1,r4);
	code_1->setCmd(Wmem,r1,71);
	code_1->setCmd(Int,r1,Dev);
	code_1->setCmd(Int, 0, Exit);
    

    handle_1 = kernel.CreateProcess("P2",code_1); // создать процесс
	handle_2 = loader.load("lab5.txt");
	kernel.DebugProcessImage(handle_1);
    kernel.wait(handle_2);
	kernel.past(handle_1);
	dispatcher.dispatch();
    kernel.past(handle_2);
     
     dispatcher.dispatch(); 
	 cout<<"Dump memory:Y/N-any"<<endl;
	 cin>>ch;
	 if(ch=='Y')
	 memory->debugHeap();
     handle_1->ProcessTime(); 
	 handle_2->ProcessTime();
     kernel.TerminateProcess(handle_1); // завершить процесс
     kernel.TerminateProcess(handle_2); // завершить процесс  
	 memory->clearMemory();
     break;
   case '6':
	 cout << endl;
     cout <<"Virtual memory  Y/N-any"<< endl;
      /* Пример иллюстрирует выполнение основных функций
         Virtual memory и рrocess сontrol block - PCB, Process Image*/
      code_1 = new Code(10);
	//Перемещение содержимого второго операнда в первый (регистр)
	code_1 = new Code(17);
	 code_1->setCmd(Mov,r1,15);
	code_1->setCmd(Mov,r2,5);
	code_1->setCmd(Add,r1,r2);
	code_1->setCmd(Mov,r2,7);
	code_1->setCmd(Mov,r3,3);
	code_1->setCmd(Add,r2,r3);
	code_1->setCmd(Div,r1,r2);
	code_1->setCmd(Mov,r4,6);
	code_1->setCmd(Mov,r5,2);
	code_1->setCmd(Div,r4,r5);
	code_1->setCmd(Mov,r5,9);
	code_1->setCmd(Mov,r6,3);
	code_1->setCmd(Div,r5,r6);
	code_1->setCmd(Mul,r4,r5);
	code_1->setCmd(Add,r1,r4);
	code_1->setCmd(Wmem,r1,71);
	code_1->setCmd(Int,r1,Dev);
	code_1->setCmd(Int, 0, Exit);
     cin >> ch;
     if (ch == 'Y') {
       dispatcher.DebugVirtualMemory(); //структура виртуальной памяти
       //2. выделение виртуальной памяти в образах
        handle_1 = kernel.CreateProcess("P1",code_1); // создать процесс
        cout << " Process created " << endl;
        dispatcher.DebugVirtualMemory();
       // изменяет состояние системы и выполнение программы      
        dispatcher.dispatch(); 
       //dispatcher.dispatch();  
        kernel.TerminateProcess(handle_1); // завершить процесс 
       //3. освобождение виртуальной памяти в образах
        dispatcher.DebugVirtualMemory();
        //4. очистить основную память
        memory->clearMemory();
      }
      cout <<endl;
     cout <<"dump Process control block (PCB) and process image Y/N-any"<< endl;
      cin >> ch;
     if (ch == 'Y') {
       handle_1 = kernel.CreateProcess("P1",code_1); // создать процесс
        // 1. Состояние блока PCB
        dispatcher.DebugPCB(handle_1);      
        dispatcher.dispatch(); // изменяет состояние системы 2 вызова
       // 2. Состояние блока PCB после выполнения программы.
        //  см. состояние PC 
        dispatcher.DebugPCB(handle_1);
        dispatcher.dispatch(); 
        kernel.TerminateProcess(handle_1);  // завершить процесс
        memory->clearMemory();
      }     
     break;
   case '7':  
	/*1. для программы "P1" и "P2" создаются объекты процессы
       сравнение методов планирования
	   Tenter - Время входа
       Tbegin - Время начала  
       Tservice - Время обслуживания в очередях
       Texec    - Время выполнения
       Tterminate - Время завершения
       Tround = Tservice + Texec  -- Время оборота
       Tround/ Tservice - Нормализованное время */
	
	code_1 = new Code(11);
	//Перемещение содержимого второго операнда в первый (регистр)
	code_1->setCmd(Mov, r1, 2); 
	code_1->setCmd(Mov, r2, 10);
	code_1->setCmd(Mul, r1, r2);  //регистр * регистр
	code_1->setCmd(Add, r1, 3);     
	code_1->setCmd(Mov, r2, 2);   
	code_1->setCmd(Div, r1, r2); 
	code_1->setCmd(Sub, r1, 3);
	code_1->setCmd(Wmem,r1, 69);  // запись содержимое r1 в память по адресу 69  
	code_1->setCmd(Int, r1, Dev); // обращение к устройству для печати содержимого r1    
	code_1->setCmd(Int, r1, Lan); // посыл содержимого r1 по сети   
	code_1->setCmd(Int, 0, Exit); 

    code_2 = new Code(7);
	code_2->setCmd(Mov, r1, 15);
	code_2->setCmd(Mov, r2, 2);
	code_2->setCmd(Mul, r1, r2);  
	code_2->setCmd(Wmem,r1, 70);  
	code_2->setCmd(Rmem,r1, 70);     
	code_2->setCmd(Int, r1, Dev);   
	code_2->setCmd(Int, 0, Exit);

	cout << endl;
    cout << " scheduler FCFS Y/N-any "<< endl;
    cin >> ch;
    if (ch == 'Y') {
	  Timer::setTime(); // устанавливаем таймер в нулевое значение	 
      handle_1 = kernel.CreateProcess("P1",code_1);  // создать процесс
      handle_2 = kernel.CreateProcess("P2",code_2);  // создать процесс
	  dispatcher.dispatch(); // изменяет состояние системы 
	                         // для модели из трех очерелей 2 вызова
	  dispatcher.dispatch();  
      handle_1->ProcessTime(); 
	  handle_2->ProcessTime();

	  kernel.TerminateProcess(handle_1);   // завершить процесс  
	  kernel.TerminateProcess(handle_2);   // завершить процесс  
      //очереди должны быть пусты после эксперимнта
	  //scheduler.DebugSPNQueue(); 
	  memory->clearMemory();	
	}
	cout << endl;
    cout << " scheduler RR Y/N-any "<< endl;
    cin >> ch;
    if (ch == 'Y') {
      int slice = 5;
      Timer::setTime(); // устанавливаем таймер в нулевое значение
	  /* в зависимости от размера парограммы и квоты, изменяется 
	     число строк dispatcher.dispatch(); */
      dispatcher.setTimeSlice(slice);
      handle_1 = kernel.CreateProcess("P1",code_1);  // создать процесс
      handle_2 = kernel.CreateProcess("P2",code_2);  // создать процесс
	  int disp = 4; // подобрано по диаграмме slice и организации очередей   
	  for (int i =0; i < disp; i++){
	    dispatcher.dispatch(); // изменяет состояние системы 
	  }  
	  
	  handle_1->ProcessTime(); 
	  handle_2->ProcessTime();

	  kernel.TerminateProcess(handle_1);   // завершить процесс  
	  kernel.TerminateProcess(handle_2);   // завершить процесс  
	  memory->clearMemory();	
      //очереди должны быть пусты после эксперимнта 
      //scheduler.DebugSPNQueue(); 
      dispatcher.resetTimeSlice();
	}
	cout << endl;
    cout << " scheduler SPN (shortest process next)  Y/N-any "<< endl;
    cin >> ch;
    if (ch == 'Y') {
      int numProcess = 5; /* количество объектов одного процесса 
	    для расчета его предсказуемого времени выполнения  */
      Timer::setTime(); // устанавливаем таймер в нулевое значение
	  // включить регистрацию наблюдений
	  dispatcher.setTpredict(TimeExecNotPreem); 
	  /* для не вытесняющего метода 
	     alfa = 0.8; установлен взвешенный коэффициент  0 < alfa < 1
	     в ShortestProcessNext(), открыт отладчик. */
	  for (int i =0; i < numProcess; i++) { 
        cout << endl;  
        cout << " ---------------------- experiment i =  " << i <<endl;
        handle_1 = kernel.CreateProcess("user_1",code_1);
		handle_2 = kernel.CreateProcess("user_2",code_2); 
	    dispatcher.dispatch(); // изменяет состояние системы 
        dispatcher.dispatch(); 
		if (i < numProcess-1 ){ /* -1. для cохранения информации
	                              о последнем эксперименте */
	      kernel.TerminateProcess(handle_1); // завершить процесс  
	      kernel.TerminateProcess(handle_2); // завершить процесс  
	    }
		/* наблюдения сохраняются, только после завершения процесса
		   для первого процесса нет наблюдений */
        memory->clearMemory();	
	  }  
	  handle_1->ProcessTime(); 
	  handle_2->ProcessTime();
      kernel.TerminateProcess(handle_1); // завершить процесс   
      kernel.TerminateProcess(handle_2); // завершить процесс  

      //scheduler.DebugSPNQueue();
	  dispatcher.resetTpredict(); // выключить регистрацию наблюдений
	}
	cout << endl;
	cout << " scheduler SRT (shortest remaining time)  Y/N-any "<< endl;
    cin >> ch;
    if (ch == 'Y') { // представляет собой вытесняющую версию SPN
      int numProcess = 3; /* количество объектов одного процесса 
	            для расчета его предсказуемого времени выполнения  */
      Timer::setTime(); // устанавливаем таймер в нулевое значение
	  // включить регистрацию наблюдений процессов
	  dispatcher.setTpredict(TimeExec);  
	  // и вытеснение 
	  for (int i =0; i < numProcess; i++) {
        cout << endl;  
        cout << " ---------------------- experiment i =  " << i <<endl;
		// параметр 0, включает наименьшее оставшееся время процесса 
        handle_1 = kernel.CreateProcess("user_1",code_1);
		handle_2 = kernel.CreateProcess("user_2",code_2); 

	    dispatcher.dispatch(); // изменяет состояние системы 
	    dispatcher.dispatch(); // вытеснение user_1 и выполнение 
        dispatcher.dispatch(); // длинный процесс не в swapOut, не успевает   
        
		if (i < numProcess-1 ){/* -1. для cохранения информации 
                                 о последнем эксперименте */
	      kernel.TerminateProcess(handle_1);  // завершить процесс  
	      kernel.TerminateProcess(handle_2);   // завершить процесс  
		}
		/* наблюдения сохраняются, только после завершения процесса
		   для первого экземпляра процесса нет наблюдений */
        memory->clearMemory();	
	  }  
	  handle_1->ProcessTime(); 
	  handle_2->ProcessTime();

	  kernel.TerminateProcess(handle_1);   // завершить процесс  
	  kernel.TerminateProcess(handle_2);   // завершить процесс  
      //scheduler.DebugSPNQueue();
	  dispatcher.resetTpredict(); // выключить регистрацию наблюдений
	}
	
	cout << endl;
	cout << " scheduler HRRN (hightst response ratio next)  Y/N-any "<< endl;
    cin >> ch;
    if (ch == 'Y') { // представляет собой вытесняющую версию SPN
      int numProcess = 3; /* количество объектов одного процесса 
	          для расчета его предсказуемого времени выполнения  */
      Timer::setTime(); // устанавливаем таймер в нулевое значение
	  // включить регистрацию наблюдений процессов 
	  dispatcher.setTpredict(TimeServ);
	  // в ShortestProcessNext(), открыт отладчик.
	  for (int i =0; i < numProcess; i++) {
        cout << endl;  
        cout << " ---------------------- experiment i =  " << i <<endl;
		// параметр 0, включает наименьшее оставшееся время процесса 
        handle_1 = kernel.CreateProcess("user_1",code_1);
		handle_2 = kernel.CreateProcess("user_2",code_2); 

	    dispatcher.dispatch(); // изменяет состояние системы 
	    dispatcher.dispatch(); // вытеснение user_1 и выполнение 
        dispatcher.dispatch(); // процесс swapOut   
		if (i < numProcess-1 ){ // -1. для cохранения информации 
        // о последнем эксперименте
	      kernel.TerminateProcess(handle_1); // завершить процесс  
	      kernel.TerminateProcess(handle_2); // завершить процесс  
		  /* наблюдения сохраняются, только после завершения процесса
		     для первого экземпляра процесса нет наблюдений */
		}
        memory->clearMemory();	
	  }  
	  handle_1->ProcessTime(); 
	  handle_2->ProcessTime();

	  kernel.TerminateProcess(handle_1);  // завершить процесс  
	  kernel.TerminateProcess(handle_2);  //
      //scheduler.DebugSPNQueue();
	  dispatcher.resetTpredict();  // выключить регистрацию наблюдений
	}

    cout << endl;
    cout << " scheduler dynamic priority DP Y/N-any "<< endl;
    cin >> ch;
    if (ch == 'Y') {
      int slice = 5;
      // priority 0..3 индекс массива. TimeSlice - значение элемента в массиве.
	  int * prSlice = new int [3];
      prSlice[0] = 5;  
	  prSlice[1] = 3;
	  prSlice[2] = 2;
      Timer::setTime(); // устанавливаем таймер в нулевое значение
	  /* в зависимости от размера парограммы и TimeSlice, изменяется 
	     число строк dispatcher.dispatch(); */
      dispatcher.setTimeSlice(3,prSlice);
         handle_1 = kernel.CreateProcess("P1",code_1);  // создать процесс
	  dispatcher.dispatch(); // изменяет состояние системы 
         handle_2 = kernel.CreateProcess("P2",code_2);  // создать процесс	  

	  int disp = 5; // где
	  for (int i =0; i < disp; i++){
	    dispatcher.dispatch(); // изменяет состояние системы 
	  } 
	  
	  handle_1->ProcessTime(); 
	  handle_2->ProcessTime();

	  kernel.TerminateProcess(handle_1);          // завершить процесс  
	  kernel.TerminateProcess(handle_2);          // завершить процесс  
	  memory->clearMemory();

      //scheduler.DebugSPNQueue(); 
      dispatcher.resetTimeSlice();
	}
	break;
   case '8':  
	/*1. сравнение методов планирования много пироцессорной системы
       для программ "P1" и "P2" создаются объекты процессы
         
	   Tenter - Время входа
       Tbegin - Время начала  
       Tservice - Время обслуживания в очередях
       Texec    - Время выполнения
       Tterminate - Время завершения
       Tround = Tservice + Texec  -- Время оборота
       Tround/ Tservice - Нормализованное время */
{ // block begin
	code = NULL;
    code = new Code(10);
	code->setCmd(Mov, r1, 5); //арифмитические действия над целыми числами
	code->setCmd(Mov, r2, 4);
	code->setCmd(Mov, r3, 2);     
	code->setCmd(Div, r2, r3);   
	code->setCmd(Sub, r1, r2); 
	code->setCmd(Add, r1, 3);
	code->setCmd(Mov, r2, 6);
	code->setCmd(Mul, r1, r2);
	code->setCmd(Wmem, r1, 69);  // запись содержимое r1 в память по адресу 69
	code->setCmd(Int, r1, Dev);
	code->setCmd(Int, 0, Exit); 
	cout << endl;
    cout << " Assignment of Processes to Processors and FCFS Y/N-any "<< endl;
	/* MultiProcessor scheduling (MP) - многопроцессорное планирование
	   метод Global queue многопроцессорное планирование  */ 

	int MAX_PROCESSOR = 2;  // максимальное количество процессоров	
    Scheduler mpScheduler(cpu, MAX_PROCESS); 
	MPDispatcher mpDispatcher(SIZE_OF_VIRTUAL_MEMORY_IN_IMAGES, 
		                      &mpScheduler,mmu,MAX_PROCESSOR);
	Kernel mpKernel(&mpDispatcher);
	valarray <HANDLE *> vaHANDLE (MAX_PROCESS);

    cin >> ch;
    if (ch == 'Y') {
	  Timer::setTime(); // устанавливаем таймер в нулевое значение	 
	  // 1. Состояние процессоров
	  mpDispatcher.MPDebug();
      for(int i=0; i < MAX_PROCESS; i++) {  
		vaHANDLE[i] = mpKernel.CreateProcess("P",code); // создать процесс
	  }
	  mpDispatcher.dispatch(); // изменяет состояние системы 
	  mpDispatcher.dispatch(); // для модели из трех очерелей 2 вызова
	  mpDispatcher.dispatch();

	  for(int i=0; i < MAX_PROCESS; i++) {      
        vaHANDLE[i]->ProcessTime(); // создать процесс
		mpKernel.TerminateProcess(vaHANDLE[i]);  // завершить процесс  
	  }
	  memory->clearMemory();	
      // mpScheduler.DebugSPNQueue();
	}
} // block end
    break;

default: ;

}	// end case

	if (code != NULL) {
		delete code;   
		code = NULL; 
	}
	if (code_1 != NULL) {
		delete code_1; 
		code_1 = NULL; 
	}
	if (code_2 != NULL) {
		delete code_2; 
		code_2 = NULL; 
	}

}// end while
	
	return 0;

} // end myOS::dispatch() 

int main() {
	int SIZE_OF_MEMORY_IN_BLOCKS = 15;  // размер оперативной памяти в блоках
	int MAX_PROCESS = 3;                // максимальное количество процессов
	int SIZE_OF_MEMORY_IN_PAGE = 0;     // размер памяти в страницах 
	myBuffer *buffer = new myBuffer;

	myOS myos ( SIZE_OF_MEMORY_IN_BLOCKS, MAX_PROCESS, buffer);
	return myos.dispatch();
}