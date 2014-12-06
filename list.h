#ifndef LIST_H
#define LIST_H

/**************************/
/* SLG List Class - V1.0  */
/* Last Update - 13/12/29 */
/**************************/

#define DOLIST(LIST_NAME, POINTER_NAME, LIST_TYPE, LIST_CODE) \
          do{\
          LIST_NAME->setPointer(0); \
          if(LIST_NAME->getLength() > 0) do \
          { \
            POINTER_NAME = (LIST_TYPE*)LIST_NAME->getPointer(); \
 \
            LIST_CODE \
 \
          } while(!LIST_NAME->incPointer()); \
          } while(0)



class lnode
{
public:
  lnode* next;
  lnode* prev;
  void* data;
};

class List
{
public:
  List();
  void add(void* data);
  void add(List* data);
  long getLength();
  void* get(long position);
  void* get(long pos, long offset);
  void del(long position);

  int incPointer();
  int decPointer();
  void* getPointer();
  int setPointer(long position);
  int delPointer();
  void addPointer(void* data);

void print();

private:
  lnode* start;
  lnode* end;
  lnode* pointer;
  long length;
};

#endif
