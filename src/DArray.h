#ifndef _DARRAY_H_
#define	_DARRAY_H_


template <class T>
class DArray{
	unsigned long _count;
	T* mas;
public:
	
	
	DArray(){_count=0; mas=0;}
	//---------------------------
	void add(T& newitem){
		T* tmp=0;
		if (_count){
			tmp=new T[_count];
			for (unsigned long i=0;i<_count;i++){
				tmp[i]=mas[i];
			}
			delete []mas;
		}

		_count++;
		mas=new T[_count];

		if (_count>1){
			for (unsigned long i=0;i<_count-1;i++){
				mas[i]=tmp[i];
			}
			delete []tmp;
		}
		
		mas[_count-1]=newitem;
		
	}
	//---------------------------
	bool remove(unsigned long index){
		if ((index<0)||(index>_count))
			return false;

		if (_count>1){
			T* tmp=new T[_count-1];
			for (unsigned long i=0;i<index;i++){
				tmp[i]=mas[i];
			}
			for (unsigned long i=index+1;i<_count;i++){
				tmp[i-1]=mas[i];
			}

			delete []mas;

			_count--;

			mas=new T[_count];

			for (unsigned long i=0;i<_count;i++){
				mas[i]=tmp[i];
			}
			delete []tmp;
		}

		else 
			destroy();
		return true;
	}
	//---------------------------
	void destroy(){
		if (mas){
			delete []mas;
			mas=0;
			_count=0;
		}
	}
	//---------------------------
	unsigned long count(){return _count;}

	//---------------------------
	T& operator[](unsigned long index) { 

		return mas[index]; 
	}

	//----------------------------
	//~DArray(){
	//	destroy();
	//}


};





#endif //_DARRAY_H_