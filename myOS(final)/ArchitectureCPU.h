#ifndef __ArchitectureCPU_H__
#define __ArchitectureCPU_H__

#include <iostream>
using namespace std;

enum Instruction { 
	Mov, Add, Sub, Mul, Div, And, Or, Xor, Shlu, Shru, Shls, Shrs, Cmp, Je, 
	Jne, Jge, Jgt, Jle, Jlt, Int, Pushw, Pushc, Popw, Popc, Rmem, Wmem
};

const char * InstName[26] = {
	"Mov", "Add", "Sub", "Mul", "Div", "And", "Or", "Xor", "Shlu", "Shru",
	"Shls", "Shrs", "Cmp", "Je", "Jne", "Jge", "Jgt", "Jle", "Jlt", "Int",
	"Pushw", "Pushc", "Popw", "Popc", "Rmem", "Wmem"
};

// OK - нормальное выполнение программы
// Error - ошибка в программе, например нет ее завершения
// Empty - чтение пустого блока памяти 
// все программы на ассемблере должны завершаться прерыванием Exit

enum Interrupt {
	OK = 0, Error = -1, Empty = 2, Exit = 16, Sys = 11, Div_0 = 3, MyLanR=14, MyLanW=88,
	Lan = 32, Dev = 254, PrIn = 555, PrOut = 265,
};

const char * InterruptName[8] = {
	"OK", "Error", "Empty", "Exit", "Sys", "Div_0", "Lan", "Dev"
};
//Interrupt intername[8] = {OK, Error, Empty, Exit, Sys, Div_0, Lan, Dev}; 

const char * RegName[16] = {
	"r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10",
	"r11", "r12", "r13", "r14", "PC", "SP"
};

// формат инструкции соответствует размеру блока памяти
class Block {   
public:
    // state состояние блока : занят - false, свободен - true  
    Block() {
		state = true; 
		code = 0; 
		nregister = 0; 
		operand = 0;
	}

    void setState (bool state) { this->state = state;}
    bool  getState () { return state; }

    void setCode (int code) { this->code = code; }
    int  getCode () { return code; }
            
    void setNregister (int nregister) { this->nregister = nregister; } 
    int  getNregister () { return nregister; }
            
    void setOperand (int operand) { this->operand = operand; } 
    int  getOperand () { return operand; }
            
    void debugBlock (){
        cout << " state="      << getState()   
             << " code="       << getCode() 
             << " nregister="  << getNregister()
             << " operand="    << getOperand() << endl;   
    }

//формат инструкции [команда] r[номер регистра] [$,r][номер регистра или константа]
//	                     A            B              C 	

private:
    bool state;      // блок занят или свободен 
    _int8 code;      // A - байт, код команды 
    _int8 nregister; // B - байт, номер регистра
    int operand;     // C - int,  операнд [номер регистра или константа]
};  

#define NUMBER_OF_REGISTERS 16
#define SIZE_OF_REGISTER_NAMES 4  //один символ требуется для завершения 
								  //строки strcat_s 

enum Name {
  r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, PC, SP
};

// Определение постификсного инкриментного operator для SysReg.
inline Name operator++ (Name &rs, int) { return rs = (Name) (rs + 1); }

// PC программный счетчик указыаающий на содержащий адрес ячейки 
//    по которому нужно извлечь очередную команду 

class SysReg {
public: 
	SysReg() { 
		int j = 0, i = 1; // j регистр в массиве от 0, i - номер с 1  
		for (Name curName = r1; curName <= SP; curName++ ) {
			register_[j++] = new Register (i++, -1, curName);
			status = true; // регистры не используются процессом, для планирования  
		}
	}
	int getState (Name name) {
		for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
			if (register_[i]->getName() == name) {
				return (register_[i]->getState());  
			}
		}
		return -1; //some system Error 
	}
	void setState (_int8 nregister, int operand) {
		// данные заносятся в таком виде
		for ( int i = 0; i < NUMBER_OF_REGISTERS; i++) {
			if (register_[i]->getName() == nregister)  {
				register_[i]->setState(operand);
				//cout <<" register_[i]->getName() =  " 
				//     << register_[i]->getName()  <<endl;
				//cout <<" register_[i]->getState() = "
				//     << register_[i]->getState() << endl;
				return;
			}
		}
	}
	
	// очистить содержимое регистров после окончания программы
	void clearSysReg() {
		for (int i = 0; i < NUMBER_OF_REGISTERS; i++)	{
			register_[i]->setState(-1); 
		}
	}

	void Debug() {
		// информация о регистрах
		for( int i = 0; i < NUMBER_OF_REGISTERS; i++)
			cout << "register_["<< i <<"] "<<" numreg= "
			     << register_[i]->getNumReg()
				 << " state = "<< register_[i]->getState()
				 << " name  = "<< register_[i]->getName()
				 << " NameOf= "<< register_[i]->NameOf() 
				 << endl;
			cout << endl;
		}
/* ***************************************************************************** */
/**/ class Register {
/**/ public:
/**/   //Объявление двух конструкторов: по умолчанию, и конструктора, 
/**/   //который устанавливает состояние и значение имени регистра 
/**/ 	Register ();
/**/ 	Register (int numreg, int state, Name name){
/**/ 		this->numreg = numreg;
/**/ 		this->state = state;
/**/ 		this->name = name;
/**/ 	}
/**/ 
/**/ 	// get and set functions.
/**/ 	// numReg - внутреннeе представление номера регистра
/**/ 	// например, для r1 numReg = 1, согласовано с именем
/**/ 	int  getNumReg () { return numreg; }  // получить кардинальное число.
/**/ 	// установить кардинальное число :
/**/ 	void setNumReg (int numreg) { this->numreg = numreg; }
/**/ 
/**/ 	int  getState () { return state; }   // получить состояние регистра.
/**/ 	// установить состояние регистра :
/**/ 	void setState (int state) { this->state = state; }
/**/ 	
/**/ 	Name  getName () { return name; }    // получить code регистра в enum.
/**/ 	// установить code регистра в enum :
/**/ 	void  setName (Name name) { this->name = name; }  
//**
/**/ 	char *NameOf () {    // получить string представление регистра.
/**/ 		static char szName[SIZE_OF_REGISTER_NAMES];
/**/ 		static char *Numbers[] = { 
/**/ 			"1", "2", "3", "4", "5", "6", "7", "8", "9", 
/**/ 			"10", "11", "12", "13", "14"
/**/ 		};
/**/ 		if ( getName() <= 13) {
/**/ 			strcpy_s (szName, SIZE_OF_REGISTER_NAMES, "r");	
/**/ 			strcat_s (szName, SIZE_OF_REGISTER_NAMES, Numbers[ getName()]);       
/**/ 			return szName;
/**/ 		} 
/**/ 		switch (getName()) {
/**/ 		case PC:
/**/ 			strcpy_s (szName, SIZE_OF_REGISTER_NAMES , "PC");
/**/ 			break;
/**/ 		case SP:
/**/ 			strcpy_s (szName, SIZE_OF_REGISTER_NAMES , "SP");
/**/ 			break;
/**/ 		} 
/**/ 		return szName;
/**/ 	};
/**/ private:
/**/ 	int numreg; 
/**/ 	Name name;
/**/ 	int state;
/**/ }; 
/* ***************************************************************************** */

	bool getStatus() { return status; }
	void setStatus (bool status) { this->status = status; } 

private: 
	Register* register_[NUMBER_OF_REGISTERS];
	bool status;  // true  - регистры не используются процессом 
                  // false - регистры использует процесс
}; // end SysReg


// состояния процесса
#define NUMBER_OF_STATE 7
enum State {
	NotRunning  = 0, // Не выполняющийся
	Running     = 1, // Выполняющийся 
	Blocked     = 2, // Блокированный
	Swapped     = 3, // Выгруженный, готовый к выполнению
	ExitProcess = 4, // Завершающийся
	New         = 5, // Новый для выполнения программы создается новый процесс(н.и.)
	Ready       = 6  // Готовый  (не используется)
};

class Timer {
public:
    Timer() { time = 0; } 
    static void setTime () { time = 0; } 
    static int  getTime () { return time; } 
    static void tick () { time++; } 
private:
    // на операции диспетчирования затричивается время, на CPU 
    // на обслуживание - ожидание и блокирование.
    static int time;
};

int Timer::time;

// при выпонении функций класса Statistic, задается один из параметров расчета 
// TimeExec - предполагаемое время выполнения 
// TimeServ - предполагаемое время обслуживания 
// значения StatTime присваиваются в соответствии с индексами массива
// что применяется при просмотре вектора 

enum StatTime {
	TimeExecNotPreem  =  0,
	TimeExec          =  1, 
	TimeServ          =  2,  
	noParam           = -1
};

#define NUMBER_OF_PARAM 3

// предположительное время выполнения вычисляется методом getShortestRemainTime
// который вызывается объектом класса Dispatcher, после первого наблюдения  

class VectorParam { // вектор параметров расчета, применяется в Scheduler  
public:
    VectorParam () {
		for (int i = 0; i < NUMBER_OF_PARAM; i++) {
			param[i] = new Param ();
		}
	}  
    void setState (StatTime time) { param[time]->state = true; }
    bool getState (StatTime time) { return param[time]->state; }
    
    StatTime getStatTime() {
		for (int i = 0; i < NUMBER_OF_PARAM; i++) {
			if (param[i]->state) {
				if (i == 0) return TimeExecNotPreem;
				if (i == 1) return TimeExec;
				if (i == 2) return TimeServ;		  
			} 
		}
		return noParam;
	} 

    void resetState (StatTime time) { param[time]->state = false; }

    void setTthreshold (StatTime time, double Tthreshold) {
		param[time]->Tthreshold = Tthreshold;
    }  

    double getTthreshold (StatTime time) { 
		return param[time]->Tthreshold;
	}

    void clearVectorParam () {
		for (int i = 0; i < NUMBER_OF_PARAM; i++) {
			param[i]->state = false;
			param[i]->Tthreshold = -1;
		}
	}
private:
/* ******************************************************************* */
/**/ class Param {
/**/ public: 
/**/ 	Param() { 
/**/ 		Tthreshold = -1; 
/**/ 		state = false; 
/**/ 	}
/**/	bool state;        // индикатор включения накопления времени
/**/	double Tthreshold; // порог предположительного времени 
/**/ };
/* ******************************************************************* */
    Param * param[NUMBER_OF_PARAM];
};


#endif