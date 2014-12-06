#include <iostream>
#include "list.h"

using namespace std;

void List::print()
{
cout << length << "\t" << start << "\t" << end << "\t" << pointer << endl;
}

List::List()
{
  length = 0;
  start = 0;
  end = 0;
  pointer = 0;
}

void List::add(void* data)
{
  lnode* newnode = new lnode;
  newnode->data = data;

  ++length;
  
  if(start == 0)
  {
    newnode->next = 0;
    newnode->prev = 0;
    start = newnode;
    end = newnode;
    return;
  }
  
  end->next = newnode;
  newnode->prev = end;
  newnode->next = 0;
  end = newnode;
  
}

void List::add(List* data)
{
  data->setPointer(0);
  while(data->getLength())
  {
    add(data->getPointer());
    data->delPointer();
  }
}

long List::getLength()
{
  return length;
}

void* List::get(long position)
{
  if(position >= length)
    return 0;
  
  if(position == -1)
    return start->data;
    
  if(position == -2)
    return end->data;

  long i = 0;
  lnode* temp;
  temp = start;
  
  while(i < position)
  {
    temp = temp->next;
    ++i;
  }
  
  return temp->data;
  
}

void* List::get(long pos, long offset)
{
  if(pos > 0)
    return 0;
  if(offset >= length)
    return 0;
  
  long i = 0;
  lnode* temp;
  
  if(pos == -1)
  {
    temp = start;
    while(i < offset)
    {
      temp = temp->next;
      ++i;
    }
    return temp->data;
  }
    
  if(pos == -2)
  {
    temp = end;
    while(i < offset)
    {
      temp = temp->prev;
      ++i;
    }
    return temp->data;
  }
  
}

void List::del(long position)
{
  if(position >= length)
    return;

  --length;

  if(position == -1 || position == 0)
  {
    if(length == 0)
    {
      delete start;
      start = 0;
      end = 0;
      pointer = 0;
      return;
    }

    start = start->next;
    delete start->prev;
    start->prev = 0;
    return;
  }

  if(position == -2 || position == length)
  {
    if(pointer == end)
      pointer = end->prev;
    end = end->prev;
    delete end->next;
    end->next = 0;
    return;
  }

  long i = 0;
  lnode* temp;
  temp = start;

  while(i < position)
  {
    temp = temp->next;
    ++i;
  }
// cout << "MID" << endl;
  if(pointer == temp) pointer = temp->prev;

  temp->next->prev = temp->prev;

  temp->prev->next = temp->next;

  delete temp;

}
int List::incPointer()
{
  if(pointer == 0 || length == 0)
    return -1;

  if(pointer->next != 0)
  {
    pointer = pointer->next;
    return 0;
  }
  return -1;

}
int List::decPointer()
{
  if(pointer == 0)
    return -1;

  if(pointer->prev != 0)
  {
    pointer = pointer->prev;
    return 0;
  }
  return -1;
}
void* List::getPointer()
{
  if(pointer == 0)
    return 0;

  return pointer->data;
}
int List::setPointer(long position)
{
  if(position == 0)
    pointer = start;
  else if(position == 1)
    pointer = end;
  else
    return -1;
  return 0;
}
int List::delPointer()
{
  --length;
  if(pointer == 0)
    return -1;

  if(pointer == start)
  {
    if(length == 0)
    {
      delete start;
      start = 0;
      end = 0;
      pointer = 0;
      return 0;
    }
    start = start->next;
    delete start->prev;
    start->prev = 0;
    pointer = start;
    return 0;
  }

  if(pointer == end)
  {
    end = end->prev;
    delete end->next;
    end->next = 0;
    pointer = end;
    return 0;
  }

  lnode* temp;

  pointer->next->prev = pointer->prev;
  temp = pointer->prev;
  pointer->prev->next = pointer->next;

  delete pointer;
  pointer = temp;

  return 0;

}

void List::addPointer(void* data)
{
  if(!pointer)
  {
    add(data);
    return;
  }

  lnode* newnode = new lnode;
  newnode->data = data;
  
  ++length;
  
  if(start == 0)
  {
    newnode->next = 0;
    newnode->prev = 0;
    start = newnode;
    end = newnode;
    return;
  }
  
  newnode->next = pointer;
  if(pointer->prev)
  {
    newnode->prev = pointer->prev;
    pointer->prev->next = newnode;
  }
  else
  {
    newnode->prev = 0;
    start = newnode;
  }
  pointer->prev = newnode;
}










