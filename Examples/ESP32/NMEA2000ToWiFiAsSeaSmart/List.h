/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

  A big thanks to NorthWestern University ~riesbeck on implementing iterators.
  Could not have done it without you!
  http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html
*/

#pragma once

//#define _USE_STD_LIST_FOR_LINKED_LIST_

#ifdef _USE_STD_LIST_FOR_LINKED_LIST_
#include <list>

template <class T> using LinkedList = std::list<T>;
#else

template <class T> class LinkedListIterator;
template <class T> class LinkedListConstIterator;
template <class T> class LinkedListCircularIterator;

template <class T> class LinkedList {
  private:
    class node {
      public:
        T value;
        node *next;

        node(const T &v) : value(v), next(0) {};
    };

    node *head;

    friend class LinkedListIterator<T>;
    friend class LinkedListConstIterator<T>;
    friend class LinkedListCircularIterator<T>;

  public:
    typedef LinkedListIterator<T> iterator;
    typedef LinkedListConstIterator<T> const_iterator;
    using constIterator=const_iterator;
    typedef LinkedListCircularIterator<T> circularIterator;

    LinkedList() : head(0) {};

    LinkedList(const LinkedList &l) : head(0) {
      for (constIterator it = l.begin(); it != l.end(); it++) {
        add(*it);
      }
    };

    ~LinkedList() {
      clear();
    };

    LinkedList<T>& operator=(const LinkedList<T> &l) {
      clear();
      for (constIterator it = l.begin(); it != l.end(); it++) {
        add(*it);
      }
      return *this;
    };

    void push_back(const T &v) {
      if (head == 0) {
        head = new node(v);
      }
      else {
        node *n = head;
        while (n->next != 0) {
          n = n->next;
        }
        n->next = new node(v);
      }
    };

    inline void add(const T &v) { push_back(v); }

    iterator insert(iterator pos, const T& v) {
      if ( head==0 || pos._current==head ) {
        head=new node(v);
        head->next=pos._current;
        pos._current=head;
      } else {
        node *n;
        for ( n=head;n->next!=pos._current; n=n->next );
        n->next=new node(v);
        n->next->next=pos._current;
        pos._current=n->next;
      }

      return pos;
    }

    T& front() {
      return head->value;
    }

    T& back() {
      node *n = head;
      while ( n!=0 && n->next!=0 ) {
        n=n->next;
      }
      return n->value;
    }

    void remove(const T &v) {
      node *n = head;
      // remove from beginning
      while ( n!=0 && n->value==v ) {
        head=n->next;
        delete(n);
        n=head;
      }

      while ( n!=0 ) {
        node *del=n->next;
        if ( del!=0 && del->value==v ) {
          n->next=del->next;
          delete(del);
        }
        n=n->next;
      }
    }

    void clear() {
      node *n = head;
      while (n != 0) {
        node *next = n->next;
        delete(n);
        n = next;
      }
      head = 0;
    };

    iterator erase(LinkedListIterator<T> l) {
      if ( l==end() ) return l;
      node *dn = l._current;
      l++;
      if ( dn==head ) {
        head=dn->next;
      } else {
        node *n=head;
        for ( ;n->next!=dn;n=n->next );
        n->next=dn->next;
      }
      delete dn;
      return l;
    }

    void removeFirst() {
      node *n = head;
      if (head) {
        head = head->next;
        delete(n);
      }
    };

    iterator begin() {
      return iterator(head);
    };

    constIterator begin() const {
      return constIterator(head);
    };

    iterator end() {
      return iterator(0);
    };

    constIterator end() const {
      return constIterator(0);
    };

    circularIterator circularBegin() {
      return circularIterator(head);
    };

    circularIterator circularEnd() {
      return circularIterator(0);
    };

    int size() const {
      LinkedList<T>::node *n = head;
      int sz = 0;
      while (n != 0) {
        n = n->next;
        sz++;
      }
      return sz;
    };

    bool operator==(const LinkedList<T> &l) const {
      auto it=begin();
      auto oit=l.begin();
      for (;it!=end() && oit!=l.end() && (*it)==(*oit); it++, oit++);
      return ( (*it)==(*oit) );
    };

    bool operator!=(const LinkedList<T> &l) const {
      return !(*this==l);
    };

    template< class Compare >
    void sort( Compare comp ) {
      if ( head==0 || head->next==0 ) return; // Empty list or 1 element list
      bool sortDone;
      do {
        sortDone=true;
        node *pa=head;
        node *a=head;
        node *b=a->next;
        while ( b!=0 ) {
          if ( !(*comp)(b->value,a->value) ) {
            pa=a;
          } else { // sort required
            sortDone=false;
            a->next=b->next;
            b->next=a;
            if ( a==head ) {
              head=b;
            } else {
              pa->next=b;
            }
          }
          a=pa->next;
          b=a->next;
        }

      } while ( !sortDone );
    }
protected:
  template<class L> friend void MoveListItemForward(LinkedList<L> &list, const L &v);
  template<class L> friend void MoveListItemBack(LinkedList<L> &list, const L &v);
  void moveBack(const T &v) {
    node *n = head;

    if ( n==0 || n->value==v ) return;  // can not move

    node *p=n;
    n=n->next;
    if ( n!=0 && n->value==v ) {  // move second
      head->next=n->next;
      head=n;
      n->next=p;
    }

    node *pp=p;
    for (p=n,n=n->next;n!=0 && n->value!=v; pp=p, p=n, n=n->next );
    if ( n==0 ) return;

    p->next=n->next;
    pp->next=n;
    n->next=p;
  }

  void moveForward(const T &v) {
    node *n = head;

    if ( n==0 || n->next==0 ) return;  // can not move, only one value

    node *p=n;
    n=n->next;
    if ( p->value==v ) {  // move first
      head=n;
      p->next=n->next;
      n->next=p;
    }

    node *pp=p;
    for (p=n,n=n->next;n!=0 && p->value!=v; pp=p, p=n, n=n->next );
    if ( n==0 ) return;
    pp->next=n;
    p->next=n->next;
    n->next=p;
  }
};

template <class T> class LinkedListIterator {
  friend class LinkedList<T>;
  protected:
    typename LinkedList<T>::node *_current;

  public:
    LinkedListIterator(typename LinkedList<T>::node *node=0) : _current(node) {};

    LinkedListIterator<T>& operator=(const LinkedListIterator<T> &l) {
      _current = l._current;
      return *this;
    };

    LinkedListIterator<T>& operator=(const LinkedListConstIterator<T> &l) {
      _current = l._current;
      return *this;
    };

    bool operator==(LinkedListIterator<T> l) {
      return l._current == _current;
    }

    bool operator!=(LinkedListIterator<T> l) {
      return l._current != _current;
    }

    LinkedListIterator<T> & operator++() {
      if ( _current!=0) _current = _current->next;
      return *this;
    }

    // Postfix operator takes an argument (that we do not use)
    LinkedListIterator<T> operator++(int) {
      LinkedListIterator<T> clone(*this);
      ++(*this);
      return clone;
    }

    T & operator*() {
      return _current->value;
    }

    T * operator->() {
      return &(operator*());
    }
};

template <class T> class LinkedListConstIterator {
  protected:
    friend class LinkedListIterator<T>;
    typename LinkedList<T>::node const *_current;

  public:
    LinkedListConstIterator(typename LinkedList<T>::node const *node) : _current(node) {};

    bool operator==(LinkedListConstIterator<T> l) {
      return l._current == _current;
    }

    bool operator!=(LinkedListConstIterator<T> l) {
      return l._current != _current;
    }

    LinkedListConstIterator<T> & operator++() {
      if ( _current!=0) _current = _current->next;
      return *this;
    }

    // Postfix operator takes an argument (that we do not use)
    LinkedListConstIterator<T> operator++(int) {
      LinkedListConstIterator<T> clone(*this);
      _current = _current->next;
      return clone;
    }

    const T & operator*() {
      return _current->value;
    }
};

template <class T> class LinkedListCircularIterator : public LinkedListIterator<T> {
  protected:
    typename LinkedList<T>::node *_head;

  public:
    LinkedListCircularIterator(typename LinkedList<T>::node *head) : LinkedListIterator<T>(head), _head(head) {};

    LinkedListCircularIterator<T> & operator++() {
      if (this->_current->next) {
        this->_current = this->_current->next;
      }
      else {
        this->_current = this->_head;
      }
      return *this;
    };

    LinkedListIterator<T> operator++(int) {
      LinkedListIterator<T> clone(*this);
      ++(*this);
      return clone;
    };
};

#ifdef DEBUG_TEST_LINKED_LIST
LinkedList<double> DoubleList;

void PrintTestList() {
  for (auto it=DoubleList.begin(); it!=DoubleList.end(); it++) {
    Serial.println((*it));
  }
}
void TestList() {
  DoubleList.add(5);
  DoubleList.add(6);
  DoubleList.add(9);
  PrintTestList();
  DoubleList.remove(6);
  PrintTestList();
}
#endif

#endif

template <class T>
void MoveListItemForward(LinkedList<T> &list, const T &v) {
  #ifdef _USE_STD_LIST_FOR_LINKED_LIST_
  auto it=list.begin();
  if ( it==list.end() ) return; // Empty list

  for ( ;it!=list.end() && (*it)!=v; it++);
  if ( it==list.end() ) return; // not found
  auto itn=it;
  itn++;
  if ( itn==list.end() ) return; // v is last value
  itn++;
  list.erase(it);
  list.insert(itn,v);
  #else
    list.moveForward(v);
  #endif
}

template <class T>
void MoveListItemBack(LinkedList<T> &list, const T &v) {
#ifdef _USE_STD_LIST_FOR_LINKED_LIST_
  auto it=list.begin();
  if ( it==list.end() ) return;  // Empty list
  if ( (*it)==v ) return; // v at begin

  auto itp=it;
  it++;
  for ( ;it!=list.end() && (*it)!=v; itp=it, it++);
  if ( it==list.end() ) return; // not found
  list.erase(it);
  list.insert(itp,v);
#else
  list.moveBack(v);
#endif
}
