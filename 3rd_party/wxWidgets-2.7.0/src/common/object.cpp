/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/object.cpp
// Purpose:     wxObject implementation
// Author:      Julian Smart
// Modified by: Ron Lee
// Created:     04/01/98
// RCS-ID:      $Id: object.cpp,v 1.103 2006/07/15 22:21:44 MW Exp $
// Copyright:   (c) 1998 Julian Smart
//              (c) 2001 Ron Lee <ron@debian.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/memory.h"
#endif

#include <string.h>

#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
    #if defined(__VISAGECPP__)
        #define DEBUG_PRINTF(NAME) { static int raz=0; \
            printf( #NAME " %i\n",raz); fflush(stdout); raz++; }
    #else
        #define DEBUG_PRINTF(NAME)
    #endif
#endif // __WXDEBUG__ || wxUSE_DEBUG_CONTEXT

// we must disable optimizations for VC.NET because otherwise its too eager
// linker discards wxClassInfo objects in release build thus breaking many,
// many things
#if defined __VISUALC__ && __VISUALC__ >= 1300
    #pragma optimize("", off)
#endif

#if wxUSE_EXTENDED_RTTI
const wxClassInfo* wxObject::ms_classParents[] = { NULL } ;
 wxObject* wxVariantToObjectConverterwxObject ( wxxVariant &data )
{ return data.wxTEMPLATED_MEMBER_CALL(Get , wxObject*) ; }
 wxObject* wxVariantOfPtrToObjectConverterwxObject ( wxxVariant &data )
{ return &data.wxTEMPLATED_MEMBER_CALL(Get , wxObject) ; }
 wxxVariant wxObjectToVariantConverterwxObject ( wxObject *data )
 { return wxxVariant( dynamic_cast<wxObject*> (data)  ) ; }
 wxClassInfo wxObject::ms_classInfo(ms_classParents , wxEmptyString , wxT("wxObject"),
            (int) sizeof(wxObject),                              \
            (wxObjectConstructorFn) 0   ,
            (wxPropertyInfo*) NULL,(wxHandlerInfo*) NULL,0 , 0 ,
            0 , wxVariantOfPtrToObjectConverterwxObject , wxVariantToObjectConverterwxObject , wxObjectToVariantConverterwxObject);
 template<> void wxStringReadValue(const wxString & , wxObject * & ){assert(0) ;}
 template<> void wxStringWriteValue(wxString & , wxObject* const & ){assert(0) ;}
 template<> void wxStringReadValue(const wxString & , wxObject & ){assert(0) ;}
 template<> void wxStringWriteValue(wxString & , wxObject const & ){assert(0) ;}
 wxClassTypeInfo s_typeInfo(wxT_OBJECT_PTR , &wxObject::ms_classInfo , NULL , NULL , typeid(wxObject*).name() ) ;
 wxClassTypeInfo s_typeInfowxObject(wxT_OBJECT , &wxObject::ms_classInfo , NULL , NULL , typeid(wxObject).name() ) ;
#else
wxClassInfo wxObject::ms_classInfo( wxT("wxObject"), 0, 0,
                                        (int) sizeof(wxObject),
                                        (wxObjectConstructorFn) 0 );
#endif

// restore optimizations
#if defined __VISUALC__ && __VISUALC__ >= 1300
    #pragma optimize("", on)
#endif

wxClassInfo* wxClassInfo::sm_first = NULL;
wxHashTable* wxClassInfo::sm_classTable = NULL;

// when using XTI, this method is already implemented inline inside
// DECLARE_DYNAMIC_CLASS but otherwise we intentionally make this function
// non-inline because this allows us to have a non-inline virtual function in
// all wx classes and this solves linking problems for HP-UX native toolchain
// and possibly others (we could make dtor non-inline as well but it's more
// useful to keep it inline than this function)
#if !wxUSE_EXTENDED_RTTI

wxClassInfo *wxObject::GetClassInfo() const
{
    return &wxObject::ms_classInfo;
}

#endif // wxUSE_EXTENDED_RTTI

// this variable exists only so that we can avoid 'always true/false' warnings
const bool wxFalse = false;

// Is this object a kind of (a subclass of) 'info'?
// E.g. is wxWindow a kind of wxObject?
// Go from this class to superclass, taking into account
// two possible base classes.
bool wxObject::IsKindOf(wxClassInfo *info) const
{
    wxClassInfo *thisInfo = GetClassInfo();
    return (thisInfo) ? thisInfo->IsKindOf(info) : false ;
}

#if defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING && defined( new )
    #undef new
#endif


#ifdef _WX_WANT_NEW_SIZET_WXCHAR_INT
void *wxObject::operator new ( size_t size, const wxChar *fileName, int lineNum )
{
    return wxDebugAlloc(size, (wxChar*) fileName, lineNum, true);
}
#endif

#ifdef _WX_WANT_DELETE_VOID
void wxObject::operator delete ( void *buf )
{
    wxDebugFree(buf);
}
#endif

#ifdef _WX_WANT_DELETE_VOID_CONSTCHAR_SIZET
void wxObject::operator delete ( void *buf, const char *_fname, size_t _line )
{
    wxDebugFree(buf);
}
#endif

#ifdef _WX_WANT_DELETE_VOID_WXCHAR_INT
void wxObject::operator delete ( void *buf, const wxChar *WXUNUSED(fileName), int WXUNUSED(lineNum) )
{
     wxDebugFree(buf);
}
#endif

#ifdef _WX_WANT_ARRAY_NEW_SIZET_WXCHAR_INT
void *wxObject::operator new[] ( size_t size, const wxChar* fileName, int lineNum )
{
    return wxDebugAlloc(size, (wxChar*) fileName, lineNum, true, true);
}
#endif

#ifdef _WX_WANT_ARRAY_DELETE_VOID
void wxObject::operator delete[] ( void *buf )
{
    wxDebugFree(buf, true);
}
#endif

#ifdef _WX_WANT_ARRAY_DELETE_VOID_WXCHAR_INT
void wxObject::operator delete[] (void * buf, const wxChar*  WXUNUSED(fileName), int WXUNUSED(lineNum) )
{
    wxDebugFree(buf, true);
}
#endif


// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

wxClassInfo::~wxClassInfo()
{
    // remove this object from the linked list of all class infos: if we don't
    // do it, loading/unloading a DLL containing static wxClassInfo objects is
    // not going to work
    if ( this == sm_first )
    {
        sm_first = m_next;
    }
    else
    {
        wxClassInfo *info = sm_first;
        while (info)
        {
            if ( info->m_next == this )
            {
                info->m_next = m_next;
                break;
            }

            info = info->m_next;
        }
    }
    Unregister();
}

wxClassInfo *wxClassInfo::FindClass(const wxChar *className)
{
    if ( sm_classTable )
    {
        return (wxClassInfo *)wxClassInfo::sm_classTable->Get(className);
    }
    else
    {
        for ( wxClassInfo *info = sm_first; info ; info = info->m_next )
        {
            if ( wxStrcmp(info->GetClassName(), className) == 0 )
                return info;
        }

        return NULL;
    }
}

// This function wasn't written to be reentrant but there is a possiblity of
// reentrance if something it does causes a shared lib to load and register
// classes. On Solaris this happens when the wxHashTable is newed, so the first
// part of the function has been modified to handle it, and a wxASSERT checks
// against reentrance in the remainder of the function.

void wxClassInfo::Register()
{
    if ( !sm_classTable )
    {
        wxHashTable *classTable = new wxHashTable(wxKEY_STRING);

        // check for reentrance
        if ( sm_classTable )
            delete classTable;
        else
            sm_classTable = classTable;
    }

#ifdef __WXDEBUG__
    // reentrance guard - see note above
    static int entry = 0;
    wxASSERT_MSG(++entry == 1, _T("wxClassInfo::Register() reentrance"));
#endif

    // Using IMPLEMENT_DYNAMIC_CLASS() macro twice (which may happen if you
    // link any object module twice mistakenly, or link twice against wx shared
    // library) will break this function because it will enter an infinite loop
    // and eventually die with "out of memory" - as this is quite hard to
    // detect if you're unaware of this, try to do some checks here.
    wxASSERT_MSG( sm_classTable->Get(m_className) == NULL,
        wxString::Format
        (
            _T("Class \"%s\" already in RTTI table - have you used IMPLEMENT_DYNAMIC_CLASS() multiple times or linked some object file twice)?"),
            m_className
        )
    );

    sm_classTable->Put(m_className, (wxObject *)this);

#ifdef __WXDEBUG__
    --entry;
#endif
}

void wxClassInfo::Unregister()
{
    if ( sm_classTable )
    {
        sm_classTable->Delete(m_className);
        if ( sm_classTable->GetCount() == 0 )
        {
            delete sm_classTable;
            sm_classTable = NULL;
        }
    }
}

wxObject *wxCreateDynamicObject(const wxChar *name)
{
#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
    DEBUG_PRINTF(wxObject *wxCreateDynamicObject)
#endif

    if ( wxClassInfo::sm_classTable )
    {
        wxClassInfo *info = (wxClassInfo *)wxClassInfo::sm_classTable->Get(name);
        return info ? info->CreateObject() : NULL;
    }
    else // no sm_classTable yet
    {
        for ( wxClassInfo *info = wxClassInfo::sm_first;
              info;
              info = info->m_next )
        {
            if (info->m_className && wxStrcmp(info->m_className, name) == 0)
                return info->CreateObject();
        }

        return NULL;
    }
}


// ----------------------------------------------------------------------------
// wxObject
// ----------------------------------------------------------------------------

void wxObject::Ref(const wxObject& clone)
{
#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
    DEBUG_PRINTF(wxObject::Ref)
#endif

    // nothing to be done
    if (m_refData == clone.m_refData)
        return;

    // delete reference to old data
    UnRef();

    // reference new data
    if ( clone.m_refData )
    {
        m_refData = clone.m_refData;
        ++(m_refData->m_count);
    }
}

void wxObject::UnRef()
{
    if ( m_refData )
    {
        wxASSERT_MSG( m_refData->m_count > 0, _T("invalid ref data count") );

        if ( --m_refData->m_count == 0 )
            delete m_refData;
        m_refData = NULL;
    }
}

void wxObject::AllocExclusive()
{
    if ( !m_refData )
    {
        m_refData = CreateRefData();
    }
    else if ( m_refData->GetRefCount() > 1 )
    {
        // note that ref is not going to be destroyed in this case
        const wxObjectRefData* ref = m_refData;
        UnRef();

        // ... so we can still access it
        m_refData = CloneRefData(ref);
    }
    //else: ref count is 1, we are exclusive owners of m_refData anyhow

    wxASSERT_MSG( m_refData && m_refData->GetRefCount() == 1,
                  _T("wxObject::AllocExclusive() failed.") );
}

wxObjectRefData *wxObject::CreateRefData() const
{
    // if you use AllocExclusive() you must override this method
    wxFAIL_MSG( _T("CreateRefData() must be overridden if called!") );

    return NULL;
}

wxObjectRefData *
wxObject::CloneRefData(const wxObjectRefData * WXUNUSED(data)) const
{
    // if you use AllocExclusive() you must override this method
    wxFAIL_MSG( _T("CloneRefData() must be overridden if called!") );

    return NULL;
}
