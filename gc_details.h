// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails
{
//  this class takes care of the memory blocks itself
  public:
    unsigned refcount; // current reference count
    T *memPtr;         // raw pointer to allocated memory
    /* isArray is true if memPtr points
to an allocated array. It is false
otherwise. */
    bool isArray; 
    /* If memPtr is pointing to an allocated
array, then arraySize contains its size */
    // Here, ptr points to the allocated memory.
    // If this is an array, then size specifies
    // the size of the array.
    PtrDetails(T *ptr, bool bArr) {
        memPtr=ptr;
        refcount=1;
        isArray=bArr;
    }
};

// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template <class T>
bool operator==(const PtrDetails<T> &ob1,
                const PtrDetails<T> &ob2)
{
    return (ob1.memPtr == ob2.memPtr);
}
