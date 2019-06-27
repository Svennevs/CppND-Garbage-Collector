#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise. 
    */
    bool isArray; 
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr); // Return an iterator to pointer details in refContainer.
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer(){
        Pointer(NULL);
    }
    Pointer(T*); //normal constructor
    
    Pointer(const Pointer &);// Copy constructor.
    
    ~Pointer(); // Destructor for Pointer.
    // Collect garbage. Returns true if at least
    // one object was freed->SvK: has to do so if memory not in use

    static bool collect();
    
    T *operator=(T *t);     // Overload assignment of pointer to Pointer.
    
    Pointer &operator=(Pointer &rv);   // Overload assignment of Pointer to Pointer.
    
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }

    
    T *operator->() { return addr; }   // Return the address being pointed to.
     
    T &operator[](int i){ return addr[i];} // Return a reference to the object at the
    // index specified by i.

   
    operator T *() { return addr; }  // Conversion function to T *.
    
    Iter<T> begin(){ // Return an Iter to the start of the allocated memory.
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }

    
    Iter<T> end(){  // Return an Iter to one past the end of an allocated array.
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    
    static int refContainerSize() { return refContainer.size(); } // Return the size of refContainer for this type of Pointer.
    
    static void showlist(); // A utility function that displays refContainer.
    
    static void shutdown(); // Clear refContainer when program exits.
};


// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;

template <class T, int size>
bool Pointer<T, size>::first = true;


// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size> Pointer<T,size>::Pointer(T *t){
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown); // atexit registers the function pointed to by func to be called on normal program termination
    first = false;

    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab

    //give it an address, isArray, arraySize
    addr = t;
    if(size>0){
        isArray=true;
        arraySize=size;
    }else{
        isArray=false;
    }

    //check if address exists in refContainer
    //if yes: increase refcount
    //if no: add to refContainer
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(addr);
    if (p != refContainer.end()){ //memory block allocated already
        p->refcount++;
    } else {
        if (size>0){ //if pointing to array
            refContainer.push_back( PtrDetails<T[size]>(addr,arraySize) ); 
        } else {
            refContainer.push_back( PtrDetails<T>(addr)); 
        }       
    }

}

template< class T, int size> // Copy constructor.
Pointer<T,size>::Pointer(const Pointer &ob){  //input is reference to object
    //this is a new object, just with all attributes equal to ob?

    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(ob.addr);

    p->refcount++;        // increment ref count of PtrDetails element
    addr = ob.addr;       // set address equal to object
    isArray = ob.isArray; 
    if (isArray){
        arraySize = p->arraySize; //set arraySize
    }
}


template <class T, int size> // Destructor for Pointer.
Pointer<T, size>::~Pointer(){
    
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(addr);


    // TODO: Finalize Pointer destructor
    // decrement ref count
    // Collect garbage when a pointer goes out of scope.

    // For real use, you might want to collect unused memory less frequently,
    // such as after refContainer has reached a certain size, after a certain number of Pointers have gone out of scope,
    // or when memory is low.

    p->refcount--;
    memfreed = collect();
    

}


// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){
    bool memfreed = false;
    typename std::list<PtrDetails<T> >::iterator p;
    
    do{
        // Scan refContainer looking for unreferenced pointers.
        for (p = refContainer.begin(); p != refContainer.end(); p++){
            // TODO: Implement collect()
            // If in-use, skip.
            if(p->refcount>0){
                continue;
            }
            // Remove unused entry from refContainer.
            refContainer.remove(*p);  //works because of the == overloading
            // Free memory unless the Pointer is null.
            if(p->memPtr){
                memfreed=true;
                if(p->isArray){
                    delete[] p->memPtr;
                }else{
                    delete p->memPtr;
                }
            }
            // Restart the search.
            break;
        }
    } while (p != refContainer.end());
    return memfreed;

    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor

}

template <class T, int size>
T *Pointer<T, size>::operator=(T *t){  // Overload assignment of pointer to Pointer.

    typename std::list<PtrDetails<T> >::iterator p;
    p=findPtrInfo(addr); 
    p->refcount--;
    p=findPtrInfo(t); //check for t in the refContainer
    if (pnew!= refContainer.end()){
        pnew->refcount++;
    }else{ //it could be that t was not in the refContainer yet
        if (size>0){ 
            refContainer.push_back( PtrDetails<T[size]>(addr,arraySize) ); 
        } else {
            refContainer.push_back( PtrDetails<T>(addr)); 
        }      
    }
    //set Pointer attributes
    addr=t;
    arraySize=size;
    if (arraySize>0){isArray=true;}else{isArray=false};
}

template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){ // Overload assignment of Pointer to Pointer.
    //should return reference to pointer. takes reference to pointer.

    // TODO: Implement assignment
    typename std::list<PtrDetails<T> >::iterator pold;
    typename std::list<PtrDetails<T> >::iterator pnew;

    pold = findPtrInfo(addr);
    pnew = findPtrInfo(rv.addr);
    // First, decrement the reference count
    // for the memory currently being pointed to.
    // Then, increment the reference count of
    // the new address.
    pold->refcount--;
    pnew->refcount++;

    // why do I store the address if I return rv anyway?
    addr = rv.addr;
    arraySize = rv.arraySize;
    isArray = rv.isArray;

    return rv;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}

// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}