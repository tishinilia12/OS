#include "stdafx.h"

#include "CPU.H"
#include "ArchitectureCPU.h"

class Decoder {
  public:
	  Decoder( ){}  
/*  void decode() {
	bool l = locked();
	m_lock = false;
	dcmd.reset();
	if(cmd.getCode()) {
		int c = cmd.getCode();
		if((cmd.isReg() && regFile.locked(cmd.getAddr())) ||
		   regFile.locked(cmd.reg()) || regFile.locked(cp)) {
			m_lock = true;
			dcmd.reset();
			cmd.reset();
			return ;
		}
		dcmd.setCode(cmd.getCode());
		dcmd.setOp2((cmd.isReg()) ? regFile.get(cmd.getAddr()) : cmd.getAddr() );
		dcmd.setOp1(regFile.get(cmd.reg()));
		dcmd.setReg(cmd.reg());
		if(c < Cmp) {
			dcmd.setMemAct(false);
			dcmd.setRegAct(true);
			dcmd.setStAct(false);
		}
		else if(c == Cmp) {
			dcmd.setMemAct(false);
			dcmd.setRegAct(false);
			dcmd.setStAct(false);
		}
		else if(c < Int) {
			dcmd.setMemAct(false);
			dcmd.setRegAct(true);
			dcmd.setStAct(false);
			dcmd.setReg(cp);
			dcmd.setOp1(dcmd.getOp2());
			m_lock = true;
		}
		else if(c == Int) { // not any flag
		}
		else if(c <= Popc) {
			dcmd.setStAct(true);
			if(c%2 == Pushw%2) dcmd.setStSize(true), dcmd.setMemSize(true);
			else dcmd.setStSize(false), dcmd.setMemSize(false);
			if(c < Popw)
				dcmd.setStDir(true),
				dcmd.setRegAct(false),
				dcmd.setMemAct(true),
				dcmd.setMemRW(true);
			else
				dcmd.setStDir(false),
				dcmd.setRegAct(true),
				dcmd.setMemAct(false),
				dcmd.setMemRW(false);
		}
		else if(c <= Shorc) {
			dcmd.setStAct(false);
			dcmd.setMemAct(true);
			if(c%2 == Loadw%2)
				dcmd.setMemSize(true);
			else dcmd.setMemSize(false);
			if(c < Shorw)
				dcmd.setMemRW(false),
				dcmd.setRegAct(true);
			else
				dcmd.setMemRW(true),
				dcmd.setRegAct(false);
		}else dcmd.reset();

	} else dcmd.reset();
	if(dcmd.getRegAct()) regFile.lock(dcmd.getReg());
	if(!(l || locked())) fetchAddr += CMD_LEN, dcmd.setFetched(true);
	cmd.reset();
  } */
} 