#include "gc_pointer.h"
#include "LeakTester.h"


int main()
{
    Pointer<int> p = new int(19);
    Pointer<int> q = new int(15);
    Pointer<int> r = new int(14);


    p = new int(21);


    p = new int(28);

    p=q;
    r=q;
    p.showlist();   

    Pointer<char> c = new char('b');
    
  	int *arr_ptr =  new int[4];  // 4 elements in array (all 0)
    for( int i = 0; i<4; i++) {
        arr_ptr[i]=i;
      	std::cout << arr_ptr[i] << std::endl;
    }
  	
 	std::cout << arr_ptr << std::endl;

  
  	Pointer<int,4> d(arr_ptr);

    p.showlist();   

    return 0;
}

