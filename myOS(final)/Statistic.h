#ifndef __Statistic_H__
#define __Statistic_H__

#include "ArchitectureCPU.h"
#include "Process.h"
#include <queue>
#include <hash_map>
#include <math.h> // from version 8.0

using namespace stdext; // for hash_map  
#define E 2.71828182845904523536

class Table {  // сохранение наблюдений по двум параметрам  
  public: 
    Table(int tExec, int tServ){
      this->tExec = tExec;
      this->tServ = tServ;
    }
    int getTime(StatTime time) {
      if (time == TimeExec) return tExec;
      if (time == TimeServ) return tServ;
      return -1; // Error 
    } 
  private:  
    int tExec; // время выполнения
    int tServ; // время обслуживания
};

class Statistic {
  public: 
    Statistic( ){}

    void setObservation(string user, int tExec, int tServ){
      hmIter = ovserv.find(user);
      if ( hmIter == ovserv.end() ){
        //If no match is found, end() is returned
        vector <Table> tb;
        tb.push_back(Table(tExec,tServ));
        ovserv.insert(pairObserv (user, tb));  
      } 
      else { // tServ для метода HRRN
        hmIter ->second.push_back(Table(tExec,tServ));
      } 
    } 
    
    // взвешенное среднее от 0 < alfa < 1, 
    double getTpredict(string user, StatTime time){ 
      hmIter = ovserv.find(user);
      if ( hmIter == ovserv.end() ) return -1.0;
      unsigned n = hmIter ->second.size(); // число наблюдениий 
      if ( n == 0 ) return -1;
      double alfa = 0.8; // установлен взвешенный коэффициент  
      // для последнего наблюдения, т.к. с 0, то n-1
      double Tpredict = alfa * hmIter->second[n-1].getTime(time);
      // просматриваем с предпоследнего наблюдения до второго
      for (int i = n - 1; i > 0; i--) { // i > 0 все кроме последнего
        Tpredict = Tpredict + 
	    pow((1-alfa),i) * alfa * hmIter->second[i-1].getTime(time);
      }
      return Tpredict;
    }

    double getTpredictSimple(string user, StatTime time){ 
      // обычное среднее не используется
      hmIter = ovserv.find(user);
      if ( hmIter == ovserv.end() ) return -1.0;
      unsigned n = hmIter ->second.size(); // число наблюдениий 
      double Tpredict = 0;
      // просматриваем наблюдения
      for (unsigned i = 0; i <n; i++) {
        Tpredict = Tpredict + hmIter ->second[i].getTime(time);   
      }
      return Tpredict/n; // делим на число наблюдений   
    }

    void clearTpredict(){
      ovserv.erase(ovserv.begin(), ovserv.end()); // очистить наблюдаемые данные
    }
    
    // порог срабатывания, с минимально предполагаемым временем
    // в соответствии с параметром наблюдения и числом наблюдений 
    double getTimeThreshold (StatTime time){
      int size_ = ovserv.size();
      //коэффициент, использующий сигмоидную функцию
      double beta = 1/(1 + pow(E, -size_));
      hmIter = ovserv.begin();
      double TpredictMin = getTpredict(hmIter->first,time);
      //1. просматриваем все наблюдения процессов
      //  просмотр массива, начиная со второго элемента   
      for ( hmIter= hmIter++; hmIter != ovserv.end(); hmIter++ ){
        //выбираем процесс с min предполагаемым временем 
        if ( TpredictMin > getTpredict(hmIter->first,time)) {
          TpredictMin = getTpredict(hmIter->first,time);
        } 
      }
      return TpredictMin*beta; // порог активации
    }

  private:
    // хранение наблюдений для вычисления предполагаемое времени
    // по методу SPN 

    hash_map <string, vector<Table>> ovserv;  
    hash_map <string, vector<Table>> :: iterator hmIter;
    typedef pair <string, vector<Table>> pairObserv;
};

#endif