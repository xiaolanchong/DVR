/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fs_mem.cpp
// Purpose:     in-memory file system
// Author:      Vaclav Slavik
// RCS-ID:      $Id: fs_mem.cpp,v 1.34 2006/07/03 19:02:13 ABX Exp $
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILESYSTEM && wxUSE_STREAMS

#include "wx/fs_mem.h"

#ifndef WXPRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/hash.h"
    #if wxUSE_GUI
        #include "wx/bitmap.h"
        #include "wx/image.h"
    #endif // wxUSE_GUI
#endif

#include "wx/mstream.h"

class MemFSHashObj : public wxObject
{
    public:

        MemFSHashObj(const void *data, size_t len)
        {
            m_Data = new char[len];
            memcpy(m_Data, data, len);
            m_Len = len;
            InitTime();
        }

        MemFSHashObj(const wxMemoryOutputStream& stream)
        {
            m_Len = stream.GetSize();
            m_Data = new char[m_Len];
            stream.CopyTo(m_Data, m_Len);
            InitTime();
        }

        ~MemFSHashObj()
        {
            delete[] m_Data;
        }

        char *m_Data;
        size_t m_Len;
#if wxUSE_DATETIME
        wxDateTime m_Time;
#endif // wxUSE_DATETIME

    DECLARE_NO_COPY_CLASS(MemFSHashObj)

    private:
        void InitTime()
        {
#if wxUSE_DATETIME
            m_Time = wxDateTime::Now();
#endif // wxUSE_DATETIME
        }
};

#if wxUSE_BASE


//--------------------------------------------------------------------------------
// wxMemoryFSHandler
//--------------------------------------------------------------------------------


wxHashTable *wxMemoryFSHandlerBase::m_Hash = NULL;


wxMemoryFSHandlerBase::wxMemoryFSHandlerBase() : wxFileSystemHandler()
{
}



wxMemoryFSHandlerBase::~wxMemoryFSHandlerBase()
{
    // as only one copy of FS handler is supposed to exist, we may silently
    // delete static data here. (There is no way how to remove FS handler from
    // wxFileSystem other than releasing _all_ handlers.)

    if (m_Hash)
    {
        WX_CLEAR_HASH_TABLE(*m_Hash);
        delete m_Hash;
        m_Hash = NULL;
    }
}



bool wxMemoryFSHandlerBase::CanOpen(const wxString& location)
{
    wxString p = GetProtocol(location);
    return (p == wxT("memory"));
}




wxFSFile* wxMemoryFSHandlerBase::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    if (m_Hash)
    {
        MemFSHashObj *obj = (MemFSHashObj*) m_Hash -> Get(GetRightLocation(location));
        if (obj == NULL)  return NULL;
        else return new wxFSFile(new wxMemoryInputStream(obj -> m_Data, obj -> m_Len),
                            location,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location)
#if wxUSE_DATETIME
                            , obj -> m_Time
#endif // wxUSE_DATETIME
                            );
    }
    else return NULL;
}



wxString wxMemoryFSHandlerBase::FindFirst(const wxString& WXUNUSED(spec),
                                      int WXUNUSED(flags))
{
    wxFAIL_MSG(wxT("wxMemoryFSHandlerBase::FindFirst not implemented"));

    return wxEmptyString;
}



wxString wxMemoryFSHandlerBase::FindNext()
{
    wxFAIL_MSG(wxT("wxMemoryFSHandlerBase::FindNext not implemented"));

    return wxEmptyString;
}


bool wxMemoryFSHandlerBase::CheckHash(const wxString& filename)
{
    if (m_Hash == NULL)
    {
        m_Hash = new wxHashTable(wxKEY_STRING);
    }

    if (m_Hash -> Get(filename) != NULL)
    {
        wxString s;
        s.Printf(_("Memory VFS already contains file '%s'!"), filename.c_str());
        wxLogError(s);
        return false;
    }
    else
        return true;
}


/*static*/ void wxMemoryFSHandlerBase::AddFile(const wxString& filename, const wxString& textdata)
{
    AddFile(filename, (const void*) textdata.mb_str(), textdata.length());
}


/*static*/ void wxMemoryFSHandlerBase::AddFile(const wxString& filename, const void *binarydata, size_t size)
{
    if (!CheckHash(filename)) return;
    m_Hash -> Put(filename, new MemFSHashObj(binarydata, size));
}



/*static*/ void wxMemoryFSHandlerBase::RemoveFile(const wxString& filename)
{
    if (m_Hash == NULL ||
        m_Hash -> Get(filename) == NULL)
    {
        wxString s;
        s.Printf(_("Trying to remove file '%s' from memory VFS, but it is not loaded!"), filename.c_str());
        wxLogError(s);
    }

    else
        delete m_Hash -> Delete(filename);
}

#endif // wxUSE_BASE

#if wxUSE_GUI

#if wxUSE_IMAGE
/*static*/ void
wxMemoryFSHandler::AddFile(const wxString& filename,
                           const wxImage& image,
                           long type)
{
    if (!CheckHash(filename)) return;

    wxMemoryOutputStream mems;
    if (image.Ok() && image.SaveFile(mems, (int)type))
        m_Hash -> Put(filename, new MemFSHashObj(mems));
    else
    {
        wxString s;
        s.Printf(_("Failed to store image '%s' to memory VFS!"), filename.c_str());
        wxPrintf(wxT("'%s'\n"), s.c_str());
        wxLogError(s);
    }
}

/*static*/ void
wxMemoryFSHandler::AddFile(const wxString& filename,
                           const wxBitmap& bitmap,
                           long type)
{
#if !defined(__WXMSW__) || wxUSE_WXDIB
    wxImage img = bitmap.ConvertToImage();
    AddFile(filename, img, type);
#endif
}

#endif // wxUSE_IMAGE

#endif // wxUSE_GUI


#endif // wxUSE_FILESYSTEM && wxUSE_FS_ZIP
