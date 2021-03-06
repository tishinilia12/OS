#ifndef __HANDLE_H__
#define __HANDLE_H__

class HANDLE {
  public:
    HANDLE(){  
      ID = -1; // идентификатор который должен быть присвоен системой
               // -1 - идентификатор не присвоен 
      // process scheduling
      Tenter = -1;     // Время входа
      Tbegin = -1;     // Время начала  
      Tservice = 0;    // Время обслуживания в очередях
      Texec    = 0;   // Время выполнения
      Tterminate = 0; // Время завершения
    }
    virtual ~HANDLE(){cout<< " object HANDLE deleted" << endl;}

    int getTenter(){return Tenter;}
    int getTbegin(){return Tbegin;}
    int getTservice(){return Tservice;}
    int getTterminate(){return Tterminate;}
    int getTexec(){return Texec;}
    int getTround(){return Texec + Tservice;} // Время оборота
    float getTnorm(){return (float)(Texec + Tservice) / Tservice;}

    void ProcessTime(){
      cout <<"      ProcessTime ID = "<< ID << endl;
      cout <<"Tenter:    "<< Tenter << endl;     // Время входа
      cout <<"Tbegin:    "<< Tbegin << endl;     // Время начала  
      cout <<"Tservice:  "<< Tservice  << endl; // Время обслуживания в очередях
      cout <<"Tterminate:"<< Tterminate << endl; // Время завершения
      cout <<"Texec:     "<< Texec      << endl; // Время завершения
      cout <<"Tround:    "<< getTround()<< endl; // Время оборота
      cout <<"Tnorm:     "<< getTnorm()<< endl;  // Нормализованное время
    }

	void setTservice(int Tservice){this ->Tservice = Tservice;}

  private:
    int ID; 
    // process scheduling
    int Tenter;     // Время входа
    int Tbegin;     // Время начала  
    int Tservice;   // Время обслуживания, только в очередях    
    int Tterminate; // Время завершения
    int Texec;      // Время выполнения
  protected: 
    int getID(){return ID;}
    void setID(int ID){this ->ID = ID;}
    // process scheduling
    void setTenter(int Tenter){this ->Tenter = Tenter;}
    void setTbegin(int Tbegin){this ->Tbegin = Tbegin;}
    //void setTservice(int Tservice){this ->Tservice = Tservice;}
    void setTterminate(int Tterminate){this ->Tterminate = Tterminate;}
    void setTeхес(int Texec){this ->Texec = Texec;}
    void clearTime(){ // применяется при завершении  
      Tenter = -1;    // Время входа
      Tbegin = -1;    // Время начала  
      Tservice = 0;   // Время обслуживания
      Texec    = 0;   // Время выполнения
      Tterminate = 0;// Время завершения
    }
};

class CS: HANDLE { // для CriticalSection - критической секции
  public:
    CS(){ 
      cs = false; // false - критическая секция не занята 
                  // true  - критическая секция  занята.
    }
    virtual ~CS(){cout<< " object CS deleted" << endl;}
    bool getCS(){return cs;}
    void setCS(bool cs){this->cs = cs;}    
  private:
    bool cs;
  protected: 
};
#endif