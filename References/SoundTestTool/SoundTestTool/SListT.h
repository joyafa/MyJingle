//----------------------------------------------------------------------//
//                                                                      //
// MyJingle is a GoogleTalk compatible VoIP Client based on Jingle      //
//                                                                      //
// Copyright (C) 2006  bluehands GmbH & Co.mmunication KG               //
//                                                                      //
// This program is free software; you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation; either version 2 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful, but  //
// WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     //
// General Public License for more details.                             //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA //
//                                                                      //
//----------------------------------------------------------------------//

#if !defined(AFX_SLISTT_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_)
#define AFX_SLISTT_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SElem.h"

typedef bool (*SElemFindFilterProc) (void *refCon, void *e);

template <class E, class L> class SListT: public L
{
public:

  void ClearList()
  {
    m_sublink= 0;
    m_lastlink= 0;
    m_nelem= 0;
  }

  SListT()
  {
    ClearList();
  }

  SListT(SCHAR* name)
  {
    ClearList();
    Name(name);
  }

  SListT(const SCHAR* name)
  {
    ClearList();
    Name((SCHAR*) name);
  }

  SListT(SListT<E,L>  &l)
   : L ()
  {
    *this= l;
  }

  virtual ~SListT()
  {
    Empty();
  }

  // ============================================================================
  // Add/Remove

  bool Add(SCHAR* name)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
  	  return Add(e);
    }
    return false;
  }
  bool Add(const SCHAR* name) { return Add((SCHAR*) name); }

  bool AddLast(SCHAR* name)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
  	  return AddLast(e);
    }
    return false;
  }
  bool AddLast(const SCHAR* name) { return AddLast((SCHAR*) name); }

  bool AddLast(SCHAR* name, SCHAR* str)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
      e->String(str);
  	  return AddLast(e);
    }
    return false;
  }
  bool AddLast(const SCHAR* name, const SCHAR* str) { return AddLast((SCHAR*) name, (SCHAR*) str); }

  bool AddLast(SCHAR* name, long value)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
      e->Long(value);
  	  return AddLast(e);
    }
    return false;
  }
  bool AddLast(const SCHAR* name, long value) { return AddLast((SCHAR*) name, value); }

  bool Add(SCHAR* name, SCHAR* str)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
      e->String(str);
  	  return Add(e);
    }
    return false;
  }
  bool Add(const SCHAR* name, SCHAR* str) { return Add((SCHAR*) name, str); }
  bool Add(SCHAR* name, const SCHAR* str) { return Add(name, (SCHAR*) str); }
  bool Add(const SCHAR* name, const SCHAR* str) { return Add((SCHAR*) name, (SCHAR*) str); }

  bool Add(SCHAR* name, long value)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
      e->Long(value);
  	  return Add(e);
    }
    return false;
  }
  bool Add(const SCHAR* name, long value) { return Add((SCHAR*) name, value); }

  bool Add(E* e)
  {
    if (e != 0) {
      e->m_link= m_sublink;
      m_sublink = e;
      m_nelem++;
      return true;
    }
    return false;
  }

  bool AddFirst(E* e)
  {
    return Add(e);
  }

  bool AddLast(E* e)
  {
    E* *referer;
    bool found = false;

    if (e != 0) {
      referer = &(m_sublink);
      while (! found) {
        if (*referer == 0) {
          found = true;
          *referer = e;
          e->m_link = 0;
          m_nelem++;
        } else {
          referer = (E* *) &((*referer)->m_link);
        }
      }
    }

    return true;
  }

  bool AddAfter(E* prev, E* e)
  {
    if (prev == 0) {
      m_sublink = e;
      e->m_link = 0;
      m_nelem++;
    } else {
      e->m_link = prev->m_link;
      prev->m_link = e;
    }

    return true;
  }

  void Insert(E* previous, E* e)
  {
    e->m_link= previous->m_link;
    previous->m_link= e;
    m_nelem++;
  }

  bool AddSorted(E* e)
  {
    return Add(e);
  }

  bool Rmv(E* e)
  {
    return (FindElemCore(&SElem_FindByObjectRefFilter, (void *) e, true) != 0);
  }
  bool Remove(E* e) { return Rmv(e); }

  // ============================================================================
  // Write to sublist members

  // adds an E, if <value> not in list
  E* Write(long value)
  {
    E* tmpElem= FindByLong(value);
    if (tmpElem == 0) {
      tmpElem= new E();
      if (tmpElem != 0) {
        tmpElem->WriteLong(value);
        Add(tmpElem);
      }
    }
    return tmpElem;
  }

  // search <name>, replaces <value> or adds an E, if <name> not in list
  E* Write(SCHAR* name, long value)
  {
    E* tmpElem= FindByName(name);
    if (tmpElem == 0) {
      tmpElem= new E();
      if (tmpElem != 0) {
        tmpElem->Name(name);
        Add(tmpElem);
      }
    }

    if (tmpElem != 0) {
      tmpElem->WriteLong(value);
    }

    return tmpElem;
  }

  // ============================================================================
  // Browse

  void Empty()
  {
    E* subElem;
    while (m_sublink != 0) {
      subElem = m_sublink;
      Rmv(subElem);
      delete subElem;
    }
  }

  E* Next(E* e)
  {
    E* retval= 0;

    if (e== 0) {
      retval= m_sublink;
    } else {
      retval= (E*) (e->m_link);
    }
    return retval;
  }

  E* First()
  {
    return Next(0);
  }

  long NumElem()
  {
    return m_nelem;
  }
  size_t current_size() { return NumElem(); }
  size_t length() { return NumElem(); }
  size_t count() { return NumElem(); }

  bool IsEmpty()
  {
    return (m_nelem == 0);
  }

  E* Last()
  {
    E* tmpElem, *retElem= 0;

    tmpElem = m_sublink;
    while (tmpElem != 0) {
      retElem= tmpElem;
      tmpElem= (E*) tmpElem->m_link;
    }

    return retElem;
  }

  // ============================================================================
  // Find

  E* FindElemCore(SElemFindFilterProc filter, void *refCon, bool dqFlag)
  {
    E* retelem= 0;
    E* *referer= 0;
    bool found= false;

    if (m_sublink != 0) {
      referer = &(m_sublink);
      while (((*referer) != 0) && ! found) {
        if ((*filter) (refCon, *referer)) {
          found = true;
        } else {
          referer =  (E* *) &((*referer)->m_link);
        }
      }
    }

    if (found) {
      retelem= *referer;
      if (dqFlag) {
        *referer = (E*) (*referer)->m_link;
        m_nelem--;
      }
    }

    return retelem;
  }

  E* FindByString(SCHAR* value)
	{
		return FindElemCore(&SElem_FindByStringFilter, (void *) value, false);
	}
  E* FindByString(const SCHAR* name) { return FindByString((SCHAR*) name); }

  E* FindByName(SCHAR* name)
  {
    return FindElemCore(&SElem_FindByNameFilter, name, false);
  }
  E* FindByName(const SCHAR* name) { return FindByName((SCHAR*) name); }

  E* FindByNameCase(SCHAR* name)
  {
    return FindElemCore(&SElem_FindByNameCaseFilter, name, false);
  }
  E* FindByNameI(SCHAR* name) { return FindByNameCase(name); }
  E* FindByNameCase(const SCHAR* name) { return FindByNameCase((SCHAR*) name); }

  E* FindByLong(long value)
  {
    return FindElemCore(&SElem_FindByLongFilter, (void *) value, false);
  }

  E* FindByPtr(void *ptr)
  {
    return FindElemCore(&SElem_FindByPtrFilter, ptr, false);
  }

  E* Find(SElemFindFilterProc filter, void *p)
  {
    return FindElemCore(filter, p, false);
  }

  E* Find(E* e)
  {
    return FindElemCore(&SElem_FindByObjectRefFilter, (void *) e, false);
  }

  E &operator[](const SCHAR* name)
  {
    E* tmpElem= FindByName((SCHAR*) name);

    if (tmpElem == 0) {
      tmpElem = (E*) &SElem_gEmpty;
    }

    return *tmpElem;
  }

  void operator=(SListT<E,L> &l)
  {
    ClearList();

    SElem::operator =(l);

    E* e= 0;
    while ((e= l.Next(e)) != 0) {
      E* e_dupl= new E();
      *e_dupl= *e;

      AddLast(e_dupl);
    }
  };

  // ============================================================================
  // Private

private:
  E* m_sublink;
  E* m_lastlink;
  long m_nelem;
};

// ============================================================================

template <class E, class L> class SArrayT: public SListT<E, L>
{
public:
  E &operator[](const SCHAR* name)
  {
    E* tmpElem= FindByName((SCHAR*) name);

    if (tmpElem == 0) {
      tmpElem = new E(name);
      if (tmpElem != 0) {
        Add(tmpElem);
      }
    }

    return *tmpElem;
  }

  void RemoveEmptyEntries()
  {
    SElem* removeElem = (E*) &SElem_gEmpty;
    while (removeElem) {
      removeElem = 0;
      for (SElem* e = 0; (e = Next(e)) != 0 && removeElem == 0; ) {
        if (e->String() == _S("")) {
          removeElem = e;
        }
      }
      if (removeElem != 0) {
        Rmv(removeElem);
        delete removeElem;
      }
    }
  }
};

// ============================================================================

template <class E, class L> class SStackT: public SListT<E, L>
{
public:
  bool Push(E* e)
  {
    return AddFirst(e);
  }

  E* Pop()
  {
    E* e= Next(0);
    if (e != 0) {
      Rmv(e);
    }
    return e;
  }

  E* Top()
  {
    E* e= Next(0);
    return e;
  }

  bool Push(SCHAR* name)
  {
    E* e= new E();
    if (e != 0) {
      e->Name(name);
      return Push(e);
    }
    return false;
  }

};

// ============================================================================

typedef SListT<SElem, SElem> SList;
typedef SElem SStackElem;
typedef SElem SArrayElem;
typedef SStackT<SStackElem, SElem> SStack;
typedef SArrayT<SArrayElem, SElem> SArray;
typedef SListT<SList, SElem> SListList;

// ============================================================================

void KeyValueBlob2SList(const SCHAR* _text, SList &_list, const SCHAR* _linesep = _S("\r\n"), const SCHAR* _fieldsep = _S("=: \t"));
void KeyValueLfBlob2SList(const SCHAR* _text, SList &_list);
void KeyValueLfBlob2SList(SCHAR* _text, SList &_list);

#endif // !defined(AFX_SLISTT_H__77688240_5EAC_11D3_8616_004005477C68__INCLUDED_)
