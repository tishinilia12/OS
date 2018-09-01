#include<fstream>

const char * Instnames[26] = 
	{ "Mov", "Add", "Sub", "Mul", "Div", "And", "Or", "Xor", "Shlu", 
	  "Shru", "Shls", "Shrs","Cmp", "Je", "Jne", "Jge", "Jgt", "Jle", 
	  "Jlt", "Int", "Pushw", "Pushc", "Popw", "Popc", "Rmem", "Wmem" };

Instruction sample[26] = 
	{ Mov, Add, Sub, Mul, Div, And, Or, Xor, Shlu, Shru, Shls, Shrs, Cmp, Je, 
	  Jne, Jge, Jgt, Jle, Jlt, Int, Pushw, Pushc, Popw, Popc, Rmem, Wmem };

const char * Regnames[16] = 
	{ "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", 
	  "r10", "r11", "r12", "r13", "r14", "PC", "SP" };

Name regsample[16] = 
	{ r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, PC, SP };

class Loader {
public:
	Loader (myMMU * mmu, myDispatcher * dispatcher, myKernel *kernel) {
		this->mmu = mmu;
		this->dispatcher = dispatcher; 
		this->kernel = kernel;
	}

	HANDLE * load (char *filename) {
		Code *prog; //объ€вл€ем программу, то есть указатель на нее
		ifstream ifile (filename); //открытие файла
		if (ifile.is_open() == false) {
			cout << "Can not open file" << endl;
		}
		int size_prog;
		ifile >> size_prog; //считываем размер программы в эту переменную
		prog = new Code (size_prog); //создаем объект этого размера, пока пустой, 
									 //далее будем в него загружать по одной команде
		while (ifile.eof() == false) {
			string line; //объ€вл€ем строку, с нее будет читатьс€ по строке из файла
			getline (ifile, line); //получение строки из файла последовательно
			string command = line.substr (0,3); //выделение первых трех символов
			
			if (command == "Rme") {
				string regname = line.substr(5,2);//с п€того индекса по две цифры берем
				for (int t = 0;t < 16; t++) {
					if(regname == Regnames[regsample[t]]) {
						prog->setCmd (Rmem, regsample[t], 
							atoi(line.substr(8,line.size()-8).c_str()));
							//устанавливаем команду, а атои преобразует строку в число
					}
				}
			}
			else if (command == "Wme") {
				string regname = line.substr(5,2);
				for (int t = 0;t < 16; t++) {
					if (regname == Regnames[regsample[t]]) {
						prog->setCmd (Wmem, regsample[t], 
							atoi(line.substr(8,line.size()-8).c_str()));
					}
				}
			}
			else if (command == "Int") {
				string regname=line.substr(4,2);//считываем из строки с 4 индекса по два символа
				if (regname == "0,") {
					prog->setCmd (Int, 0, Exit);
				} else {
					for (int t = 0;t < 16; t++) {
						if(regname == Regnames[regsample[t]]) {


							string NameInt;
							NameInt = line.substr(7,line.size()-7);
							//от размера отнимаем 7
							
							if(NameInt == "Lan") {
								prog->setCmd (Int, regsample[t], Lan);
							} else if (NameInt == "Dev") {
								prog->setCmd (Int, regsample[t], Dev);
							} else if (NameInt == "Sys") {
								prog->setCmd(Int,regsample[t],Sys);
							
							} else if (NameInt == "MyLAN") {
								prog->setCmd (Int, regsample[t], Lan);
							} else if (NameInt == "PrOut") {
								prog->setCmd (Int, regsample[t], PrOut);
							} else if (NameInt == "PrIn") {
								prog->setCmd (Int, regsample[t], PrIn);
							}
						}
					}
				}
			}
			else if (command == "Div") {
				string regname = line.substr(4,2);
				for (int t = 0;t < 16; t++) {
					if(regname == Regnames[regsample[t]]) {
						string regname = line.substr(7,2);
						for (int m = 0; m < 16; m++) {
							if(regname == Regnames[regsample[m]]) {	
								prog->setCmd (Div, regsample[t], regsample[m]);
							}
						}
					}
				}	
			}
			else if (command == "Mul") {
				string regname = line.substr(4,2);
				for (int t = 0;t < 16; t++) {
					if(regname == Regnames[regsample[t]]) {
						string regname=line.substr(7,2);
						for (int m = 0; m < 16; m++) {
							if (regname == Regnames[regsample[m]]) {
								prog->setCmd (Mul, regsample[t], regsample[m]);
							}
						}
					}
				}
			}
			else {
				for (int i = 0; i < 26; i++) {
					if (command == Instnames[sample[i]]) {	
						string regname=line.substr(4,2);
						for (int t = 0;t < 16; t++) {
							if(regname == Regnames[regsample[t]]) {
								prog->setCmd (sample[i], regsample[t],
									atoi(line.substr(7,line.size()-7).c_str()));
							}
						}
					}
				}
			}
		} //end while()

		return kernel->CreateProcess("P2",prog);
	}

private:
	Dispatcher *dispatcher;
	Kernel *kernel;
	myMMU * mmu;	
};


//#endif