#ifndef __BUFFER_H__
#define __BUFFER_H__

class myBuffer {
public:
	void push (int data) {
		cout << "Push into bufer : " << data << endl;
		this -> save_data = data; 
	}

	int pop() {
		cout << "Pop from buffer : " << save_data << endl;
		return save_data;
	}
private:
	int save_data;
	friend class myOS;
	friend class myCPU;
};

#endif