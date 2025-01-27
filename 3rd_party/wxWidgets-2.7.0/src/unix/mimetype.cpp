/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id: mimetype.cpp,v 1.77 2006/07/17 09:39:11 MW Exp $
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

// known bugs; there may be others!! chris elliott, biol75@york.ac.uk 27 Mar 01

// 1) .mailcap and .mimetypes can be either in a netscape or metamail format
//    and entries may get confused during writing (I've tried to fix this; please let me know
//    any files that fail)
// 2) KDE and Gnome do not yet fully support international read/write
// 3) Gnome key lines like open.latex."LaTeX this file"=latex %f will have odd results
// 4) writing to files comments out the existing data; I hope this avoids losing
//    any data which we could not read, and data which we did not store like test=
// 5) results from reading files with multiple entries (especially matches with type/* )
//    may (or may not) work for getXXX commands
// 6) Loading the png icons in Gnome doesn't work for me...
// 7) In Gnome, if keys.mime exists but keys.users does not, there is
//    an error message in debug mode, but the file is still written OK
// 8) Deleting entries is only allowed from the user file; sytem wide entries
//    will be preserved during unassociate
// 9) KDE does not yet handle multiple actions; Netscape mode never will

// TODO: this file is a mess, we need to split it and review everything (VZ)

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE && wxUSE_FILE && wxUSE_TEXTFILE

#include "wx/unix/mimetype.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/file.h"
#include "wx/confbase.h"

#include "wx/ffile.h"
#include "wx/textfile.h"
#include "wx/dir.h"
#include "wx/tokenzr.h"
#include "wx/iconloc.h"
#include "wx/filename.h"

#if wxUSE_LIBGNOMEVFS
    // Not GUI dependent
    #include "wx/gtk/gnome/gvfs.h"
#endif

// other standard headers
#include <ctype.h>

// this class extends wxTextFile
//
// VZ: ???
class wxMimeTextFile : public wxTextFile
{
public:
    // constructors
    wxMimeTextFile () : wxTextFile () {};
    wxMimeTextFile(const wxString& strFile) : wxTextFile(strFile) {};

    int pIndexOf(const wxString & sSearch, bool bIncludeComments = false, int iStart = 0)
    {
        size_t i = iStart;
        int nResult = wxNOT_FOUND;
        if (i >= GetLineCount())
            return wxNOT_FOUND;

        wxString sTest = sSearch;
        sTest.MakeLower();
        wxString sLine;

        if (bIncludeComments)
        {
            while ( i < GetLineCount() )
            {
                sLine = GetLine(i);
                sLine.MakeLower();
                if (sLine.Contains(sTest))
                    nResult = (int) i;

                i++;
            }
        }
        else
        {
            while ( (i < GetLineCount()) )
            {
                sLine = GetLine(i);
                sLine.MakeLower();
                if ( ! sLine.StartsWith(wxT("#")))
                {
                    if (sLine.Contains(sTest))
                        nResult = (int) i;
                }

                i++;
            }
        }

        return  nResult;
    }

    bool CommentLine(int nIndex)
    {
        if (nIndex < 0)
            return false;
        if (nIndex >= (int)GetLineCount() )
            return false;

        GetLine(nIndex) = GetLine(nIndex).Prepend(wxT("#"));
        return true;
    }

    bool CommentLine(const wxString & sTest)
    {
        int nIndex = pIndexOf(sTest);
        if (nIndex < 0)
            return false;
        if (nIndex >= (int)GetLineCount() )
            return false;

        GetLine(nIndex) = GetLine(nIndex).Prepend(wxT("#"));
        return true;
    }

    wxString GetVerb(size_t i)
    {
        if (i > GetLineCount() )
            return wxEmptyString;

        wxString sTmp = GetLine(i).BeforeFirst(wxT('='));
        return sTmp;
    }

    wxString GetCmd(size_t i)
    {
        if (i > GetLineCount() )
            return wxEmptyString;

        wxString sTmp = GetLine(i).AfterFirst(wxT('='));
        return sTmp;
    }
};

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// MIME code tracing mask
#define TRACE_MIME wxT("mime")

// give trace messages about the results of mailcap tests
#define TRACE_MIME_TEST wxT("mimetest")

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// there are some fields which we don't understand but for which we don't give
// warnings as we know that they're not important - this function is used to
// test for them
static bool IsKnownUnimportantField(const wxString& field);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------


// This class uses both mailcap and mime.types to gather information about file
// types.
//
// The information about mailcap file was extracted from metamail(1) sources
// and documentation and subsequently revised when I found the RFC 1524
// describing it.
//
// Format of mailcap file: spaces are ignored, each line is either a comment
// (starts with '#') or a line of the form <field1>;<field2>;...;<fieldN>.
// A backslash can be used to quote semicolons and newlines (and, in fact,
// anything else including itself).
//
// The first field is always the MIME type in the form of type/subtype (see RFC
// 822) where subtype may be '*' meaning "any". Following metamail, we accept
// "type" which means the same as "type/*", although I'm not sure whether this
// is standard.
//
// The second field is always the command to run. It is subject to
// parameter/filename expansion described below.
//
// All the following fields are optional and may not be present at all. If
// they're present they may appear in any order, although each of them should
// appear only once. The optional fields are the following:
//  * notes=xxx is an uninterpreted string which is silently ignored
//  * test=xxx is the command to be used to determine whether this mailcap line
//    applies to our data or not. The RHS of this field goes through the
//    parameter/filename expansion (as the 2nd field) and the resulting string
//    is executed. The line applies only if the command succeeds, i.e. returns 0
//    exit code.
//  * print=xxx is the command to be used to print (and not view) the data of
//    this type (parameter/filename expansion is done here too)
//  * edit=xxx is the command to open/edit the data of this type
//  * needsterminal means that a new interactive console must be created for
//    the viewer
//  * copiousoutput means that the viewer doesn't interact with the user but
//    produces (possibly) a lof of lines of output on stdout (i.e. "cat" is a
//    good example), thus it might be a good idea to use some kind of paging
//    mechanism.
//  * textualnewlines means not to perform CR/LF translation (not honored)
//  * compose and composetyped fields are used to determine the program to be
//    called to create a new message pert in the specified format (unused).
//
// Parameter/filename expansion:
//  * %s is replaced with the (full) file name
//  * %t is replaced with MIME type/subtype of the entry
//  * for multipart type only %n is replaced with the nnumber of parts and %F is
//    replaced by an array of (content-type, temporary file name) pairs for all
//    message parts (TODO)
//  * %{parameter} is replaced with the value of parameter taken from
//    Content-type header line of the message.
//
//
// There are 2 possible formats for mime.types file, one entry per line (used
// for global mime.types and called Mosaic format) and "expanded" format where
// an entry takes multiple lines (used for users mime.types and called
// Netscape format).
//
// For both formats spaces are ignored and lines starting with a '#' are
// comments. Each record has one of two following forms:
//  a) for "brief" format:
//      <mime type>  <space separated list of extensions>
//  b) for "expanded" format:
//      type=<mime type> BACKSLASH
//      desc="<description>" BACKSLASH
//      exts="<comma separated list of extensions>"
//
// (where BACKSLASH is a literal '\\' which we can't put here because cpp
// misinterprets it)
//
// We try to autodetect the format of mime.types: if a non-comment line starts
// with "type=" we assume the second format, otherwise the first one.

// there may be more than one entry for one and the same mime type, to
// choose the right one we have to run the command specified in the test
// field on our data.

// ----------------------------------------------------------------------------
// wxGNOME
// ----------------------------------------------------------------------------

// GNOME stores the info we're interested in in several locations:
//  1. xxx.keys files under /usr/share/mime-info
//  2. xxx.keys files under ~/.gnome/mime-info
//
// Update (Chris Elliott): apparently there may be an optional "[lang]" prefix
// just before the field name.


void wxMimeTypesManagerImpl::LoadGnomeDataFromKeyFile(const wxString& filename,
                                                      const wxArrayString& dirs)
{
    wxTextFile textfile(filename);
#if defined(__WXGTK20__) && wxUSE_UNICODE
    if ( !textfile.Open(wxMBConvUTF8(wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL)) )
#else
    if ( !textfile.Open() )
#endif
        return;

    wxLogTrace(TRACE_MIME, wxT("--- Opened Gnome file %s  ---"),
            filename.c_str());

    wxArrayString search_dirs( dirs );

    // values for the entry being parsed
    wxString curMimeType, curIconFile;
    wxMimeTypeCommands * entry = new wxMimeTypeCommands;

    wxArrayString strExtensions;
    wxString strDesc;

    const wxChar *pc;
    size_t nLineCount = textfile.GetLineCount();
    size_t nLine = 0;
    while ( nLine < nLineCount )
    {
        pc = textfile[nLine].c_str();
        if ( *pc != wxT('#') )
        {

            wxLogTrace(TRACE_MIME, wxT("--- Reading from Gnome file %s '%s' ---"),
                    filename.c_str(), pc);

            // trim trailing space and tab
            while ((*pc == wxT(' ')) || (*pc == wxT('\t')))
                pc++;

            wxString sTmp(pc);
            int equal_pos = sTmp.Find( wxT('=') );
            if (equal_pos > 0)
            {
                wxString left_of_equal = sTmp.Left( equal_pos );
                const wxChar *right_of_equal = pc;
                right_of_equal += equal_pos+1;

                if (left_of_equal == wxT("icon_filename"))
                {
                    // GNOME 2:
                    curIconFile = right_of_equal;

                    wxFileName newFile( curIconFile );
                    if (newFile.IsRelative() || newFile.FileExists())
                    {
                        size_t nDirs = search_dirs.GetCount();

                        for (size_t nDir = 0; nDir < nDirs; nDir++)
                        {
                            newFile.SetPath( search_dirs[nDir] );
                            newFile.AppendDir( wxT("pixmaps") );
                            newFile.AppendDir( wxT("document-icons") );
                            newFile.SetExt( wxT("png") );
                            if (newFile.FileExists())
                            {
                                curIconFile = newFile.GetFullPath();
                                // reorder search_dirs for speedup (fewer
                                // calls to FileExist() required)
                                if (nDir != 0)
                                {
                                    wxString tmp = search_dirs[nDir];
                                    search_dirs.RemoveAt( nDir );
                                    search_dirs.Insert( tmp, 0 );
                                }
                                break;
                            }
                        }
                    }
                }
                else if (left_of_equal == wxT("open"))
                {
                    sTmp = right_of_equal;
                    sTmp.Replace( wxT("%f"), wxT("%s") );
                    sTmp.Prepend( wxT("open=") );
                    entry->Add(sTmp);
                }
                else if (left_of_equal == wxT("view"))
                {
                    sTmp = right_of_equal;
                    sTmp.Replace( wxT("%f"), wxT("%s") );
                    sTmp.Prepend( wxT("view=") );
                    entry->Add(sTmp);
                }
                else if (left_of_equal == wxT("print"))
                {
                    sTmp = right_of_equal;
                    sTmp.Replace( wxT("%f"), wxT("%s") );
                    sTmp.Prepend( wxT("print=") );
                    entry->Add(sTmp);
                }
                else if (left_of_equal == wxT("description"))
                {
                    strDesc = right_of_equal;
                }
                else if (left_of_equal == wxT("short_list_application_ids_for_novice_user_level"))
                {
                    sTmp = right_of_equal;
                    if (sTmp.Contains( wxT(",") ))
                        sTmp = sTmp.BeforeFirst( wxT(',') );
                    sTmp.Prepend( wxT("open=") );
                    sTmp.Append( wxT(" %s") );
                    entry->Add(sTmp);
                }

            } // emd of has an equals sign
            else
            {
                // not a comment and not an equals sign
                if (sTmp.Contains(wxT('/')))
                {
                    // this is the start of the new mimetype
                    // overwrite any existing data
                    if (! curMimeType.empty())
                    {
                        AddToMimeData( curMimeType, curIconFile, entry, strExtensions, strDesc );

                        // now get ready for next bit
                        entry = new wxMimeTypeCommands;
                    }

                    curMimeType = sTmp.BeforeFirst(wxT(':'));
                }
            }
        } // end of not a comment

        // ignore blank lines
        nLine++;
    } // end of while, save any data

    if ( curMimeType.empty() )
        delete entry;
    else
        AddToMimeData( curMimeType, curIconFile, entry, strExtensions, strDesc);
}

void wxMimeTypesManagerImpl::LoadGnomeMimeTypesFromMimeFile(const wxString& filename)
{
    wxTextFile textfile(filename);
    if ( !textfile.Open() )
        return;

    wxLogTrace(TRACE_MIME,
               wxT("--- Opened Gnome file %s  ---"),
               filename.c_str());

    // values for the entry being parsed
    wxString curMimeType, curExtList;

    const wxChar *pc;
    size_t nLineCount = textfile.GetLineCount();
    for ( size_t nLine = 0; /* nothing */; nLine++ )
    {
        if ( nLine < nLineCount )
        {
            pc = textfile[nLine].c_str();
            if ( *pc == wxT('#') )
            {
                // skip comments
                continue;
            }
        }
        else
        {
            // so that we will fall into the "if" below
            pc = NULL;
        }

        if ( !pc || !*pc )
        {
            // end of the entry
            if ( !curMimeType.empty() && !curExtList.empty() )
            {
                 wxLogTrace(TRACE_MIME,
                            wxT("--- At end of Gnome file  finding mimetype %s  ---"),
                            curMimeType.c_str());

                 AddMimeTypeInfo(curMimeType, curExtList, wxEmptyString);
            }

            if ( !pc )
            {
                // the end: this can only happen if nLine == nLineCount
                break;
            }

            curExtList.Empty();

            continue;
        }

        // what do we have here?
        if ( *pc == wxT('\t') )
        {
            // this is a field=value ling
            pc++; // skip leading TAB

            static const int lenField = 5; // strlen("ext: ")
            if ( wxStrncmp(pc, wxT("ext: "), lenField) == 0 )
            {
                // skip it and take everything left until the end of line
                curExtList = pc + lenField;
            }
            //else: some other field, we don't care
        }
        else
        {
            // this is the start of the new section
            wxLogTrace(TRACE_MIME,
                       wxT("--- In Gnome file  finding mimetype %s  ---"),
                       curMimeType.c_str());

            if (! curMimeType.empty())
                AddMimeTypeInfo(curMimeType, curExtList, wxEmptyString);

            curMimeType.Empty();

            while ( *pc != wxT(':') && *pc != wxT('\0') )
            {
                curMimeType += *pc++;
            }
        }
    }
}


void wxMimeTypesManagerImpl::LoadGnomeMimeFilesFromDir(
                      const wxString& dirbase, const wxArrayString& dirs)
{
    wxASSERT_MSG( !dirbase.empty() && !wxEndsWithPathSeparator(dirbase),
                  wxT("base directory shouldn't end with a slash") );

    wxString dirname = dirbase;
    dirname << wxT("/mime-info");

    if ( !wxDir::Exists(dirname) )
        return;

    wxDir dir(dirname);
    if ( !dir.IsOpened() )
        return;

    // we will concatenate it with filename to get the full path below
    dirname += wxT('/');

    wxString filename;
    bool cont;

    cont = dir.GetFirst(&filename, wxT("*.mime"), wxDIR_FILES);
    while ( cont )
    {
        LoadGnomeMimeTypesFromMimeFile(dirname + filename);

        cont = dir.GetNext(&filename);
    }

    cont = dir.GetFirst(&filename, wxT("*.keys"), wxDIR_FILES);
    while ( cont )
    {
        LoadGnomeDataFromKeyFile(dirname + filename, dirs);

        cont = dir.GetNext(&filename);
    }

    // FIXME: Hack alert: We scan all icons and deduce the
    //             mime-type from the file name.
    dirname = dirbase;
    dirname << wxT("/pixmaps/document-icons");

    // these are always empty in this file
    wxArrayString strExtensions;
    wxString strDesc;

    if ( !wxDir::Exists(dirname) )
    {
        // Just test for default GPE dir also
        dirname = wxT("/usr/share/gpe/pixmaps/default/filemanager/document-icons");

        if ( !wxDir::Exists(dirname) )
           return;
    }

    wxDir dir2( dirname );

    cont = dir2.GetFirst(&filename, wxT("gnome-*.png"), wxDIR_FILES);
    while ( cont )
    {
        wxString mimeType = filename;
        mimeType.Remove( 0, 6 ); // remove "gnome-"
        mimeType.Remove( mimeType.Len() - 4, 4 ); // remove ".png"
        int pos = mimeType.Find( wxT("-") );
        if (pos != wxNOT_FOUND)
        {
            mimeType.SetChar( pos, wxT('/') );
            wxString iconFile = dirname;
            iconFile << wxT("/");
            iconFile << filename;
            AddToMimeData( mimeType, iconFile, NULL, strExtensions, strDesc, true );
        }

        cont = dir2.GetNext(&filename);
    }
}

void wxMimeTypesManagerImpl::GetGnomeMimeInfo(const wxString& sExtraDir)
{
    wxArrayString dirs;

    wxString gnomedir = wxGetenv( wxT("GNOMEDIR") );
    if (!gnomedir.empty())
    {
        gnomedir << wxT("/share");
        dirs.Add( gnomedir );
    }

    dirs.Add(wxT("/usr/share"));
    dirs.Add(wxT("/usr/local/share"));

    gnomedir = wxGetHomeDir();
    gnomedir << wxT("/.gnome");
    dirs.Add( gnomedir );

    if (!sExtraDir.empty())
        dirs.Add( sExtraDir );

    size_t nDirs = dirs.GetCount();
    for ( size_t nDir = 0; nDir < nDirs; nDir++ )
    {
        LoadGnomeMimeFilesFromDir(dirs[nDir], dirs);
    }
}

// ----------------------------------------------------------------------------
// KDE
// ----------------------------------------------------------------------------


// KDE stores the icon info in its .kdelnk files. The file for mimetype/subtype
// may be found in either of the following locations
//
//  1. $KDEDIR/share/mimelnk/mimetype/subtype.kdelnk
//  2. ~/.kde/share/mimelnk/mimetype/subtype.kdelnk
//
// The format of a .kdelnk file is almost the same as the one used by
// wxFileConfig, i.e. there are groups, comments and entries. The icon is the
// value for the entry "Type"

// kde writing; see http://webcvs.kde.org/cgi-bin/cvsweb.cgi/~checkout~/kdelibs/kio/DESKTOP_ENTRY_STANDARD
// for now write to .kdelnk but should eventually do .desktop instead (in preference??)

bool wxMimeTypesManagerImpl::CheckKDEDirsExist( const wxString &sOK, const wxString &sTest )
{
    if (sTest.empty())
    {
        return wxDir::Exists(sOK);
    }
    else
    {
        wxString sStart = sOK + wxT("/") + sTest.BeforeFirst(wxT('/'));
        if (!wxDir::Exists(sStart))
            wxMkdir(sStart);
        wxString sEnd = sTest.AfterFirst(wxT('/'));
        return CheckKDEDirsExist(sStart, sEnd);
    }
}

bool wxMimeTypesManagerImpl::WriteKDEMimeFile(int index, bool delete_index)
{
    wxMimeTextFile appoutfile, mimeoutfile;
    wxString sHome = wxGetHomeDir();
    wxString sTmp = wxT(".kde/share/mimelnk/");
    wxString sMime = m_aTypes[index];
    CheckKDEDirsExist(sHome, sTmp + sMime.BeforeFirst(wxT('/')) );
    sTmp = sHome + wxT('/') + sTmp + sMime + wxT(".kdelnk");

    bool bTemp;
    bool bMimeExists = mimeoutfile.Open(sTmp);
    if (!bMimeExists)
    {
        bTemp = mimeoutfile.Create(sTmp);
        // some unknown error eg out of disk space
        if (!bTemp)
            return false;
    }

    sTmp = wxT(".kde/share/applnk/");
    CheckKDEDirsExist(sHome, sTmp + sMime.AfterFirst(wxT('/')) );
    sTmp = sHome + wxT('/') + sTmp + sMime.AfterFirst(wxT('/')) + wxT(".kdelnk");

    bool bAppExists;
    bAppExists = appoutfile.Open(sTmp);
    if (!bAppExists)
    {
        bTemp = appoutfile.Create(sTmp);
        // some unknown error eg out of disk space
        if (!bTemp)
            return false;
    }

    // fixed data; write if new file
    if (!bMimeExists)
    {
        mimeoutfile.AddLine(wxT("#KDE Config File"));
        mimeoutfile.AddLine(wxT("[KDE Desktop Entry]"));
        mimeoutfile.AddLine(wxT("Version=1.0"));
        mimeoutfile.AddLine(wxT("Type=MimeType"));
        mimeoutfile.AddLine(wxT("MimeType=") + sMime);
    }

    if (!bAppExists)
    {
        mimeoutfile.AddLine(wxT("#KDE Config File"));
        mimeoutfile.AddLine(wxT("[KDE Desktop Entry]"));
        appoutfile.AddLine(wxT("Version=1.0"));
        appoutfile.AddLine(wxT("Type=Application"));
        appoutfile.AddLine(wxT("MimeType=") + sMime + wxT(';'));
    }

    // variable data
    // ignore locale
    mimeoutfile.CommentLine(wxT("Comment="));
    if (!delete_index)
        mimeoutfile.AddLine(wxT("Comment=") + m_aDescriptions[index]);
    appoutfile.CommentLine(wxT("Name="));
    if (!delete_index)
        appoutfile.AddLine(wxT("Comment=") + m_aDescriptions[index]);

    sTmp = m_aIcons[index];
    // we can either give the full path, or the shortfilename if its in
    // one of the directories we search
    mimeoutfile.CommentLine(wxT("Icon=") );
    if (!delete_index)
        mimeoutfile.AddLine(wxT("Icon=") + sTmp );
    appoutfile.CommentLine(wxT("Icon=") );
    if (!delete_index)
        appoutfile.AddLine(wxT("Icon=") + sTmp );

    sTmp = wxT(" ") + m_aExtensions[index];

    wxStringTokenizer tokenizer(sTmp, wxT(" "));
    sTmp = wxT("Patterns=");
    mimeoutfile.CommentLine(sTmp);
    while ( tokenizer.HasMoreTokens() )
    {
        // holds an extension; need to change it to *.ext;
        wxString e = wxT("*.") + tokenizer.GetNextToken() + wxT(";");
        sTmp = sTmp + e;
    }

    if (!delete_index)
        mimeoutfile.AddLine(sTmp);

    wxMimeTypeCommands * entries = m_aEntries[index];
    // if we don't find open just have an empty string ... FIX this
    sTmp = entries->GetCommandForVerb(wxT("open"));
    sTmp.Replace( wxT("%s"), wxT("%f") );

    mimeoutfile.CommentLine(wxT("DefaultApp=") );
    if (!delete_index)
        mimeoutfile.AddLine(wxT("DefaultApp=") + sTmp);

    sTmp.Replace( wxT("%f"), wxT("") );
    appoutfile.CommentLine(wxT("Exec="));
    if (!delete_index)
        appoutfile.AddLine(wxT("Exec=") + sTmp);

    if (entries->GetCount() > 1)
    {
        //other actions as well as open
    }

    bTemp = false;
    if (mimeoutfile.Write())
        bTemp = true;
    mimeoutfile.Close();
    if (appoutfile.Write())
        bTemp = true;
    appoutfile.Close();

    return bTemp;
}

void wxMimeTypesManagerImpl::LoadKDELinksForMimeSubtype(const wxString& dirbase,
                                               const wxString& subdir,
                                               const wxString& filename,
                                               const wxArrayString& icondirs)
{
    wxMimeTextFile file;
    if ( !file.Open(dirbase + filename) )
        return;

    wxLogTrace(TRACE_MIME, wxT("loading KDE file %s"),
                           (dirbase + filename).c_str());

    wxMimeTypeCommands * entry = new wxMimeTypeCommands;
    wxArrayString sExts;
    wxString mimetype, mime_desc, strIcon;

    int nIndex = file.pIndexOf( wxT("MimeType=") );
    if (nIndex == wxNOT_FOUND)
    {
        // construct mimetype from the directory name and the basename of the
        // file (it always has .kdelnk extension)
        mimetype << subdir << wxT('/') << filename.BeforeLast( wxT('.') );
    }
    else
        mimetype = file.GetCmd(nIndex);

    // first find the description string: it is the value in either "Comment="
    // line or "Comment[<locale_name>]=" one
    nIndex = wxNOT_FOUND;

    wxString comment;

#if wxUSE_INTL
    wxLocale *locale = wxGetLocale();
    if ( locale )
    {
        // try "Comment[locale name]" first
        comment << wxT("Comment[") + locale->GetName() + wxT("]=");
        nIndex = file.pIndexOf(comment);
    }
#endif

    if ( nIndex == wxNOT_FOUND )
    {
        comment = wxT("Comment=");
        nIndex = file.pIndexOf(comment);
    }

    if ( nIndex != wxNOT_FOUND )
        mime_desc = file.GetCmd(nIndex);
    //else: no description

    // next find the extensions
    wxString mime_extension;

    nIndex = file.pIndexOf(wxT("Patterns="));
    if ( nIndex != wxNOT_FOUND )
    {
        wxString exts = file.GetCmd(nIndex);

        wxStringTokenizer tokenizer(exts, wxT(";"));
        while ( tokenizer.HasMoreTokens() )
        {
            wxString e = tokenizer.GetNextToken();

            // don't support too difficult patterns
            if ( e.Left(2) != wxT("*.") )
                continue;

            if ( !mime_extension.empty() )
            {
                // separate from the previous ext
                mime_extension << wxT(' ');
            }

            mime_extension << e.Mid(2);
        }
    }

    sExts.Add(mime_extension);

    // ok, now we can take care of icon:

    nIndex = file.pIndexOf(wxT("Icon="));
    if ( nIndex != wxNOT_FOUND )
    {
        strIcon = file.GetCmd(nIndex);

        wxLogTrace(TRACE_MIME, wxT("  icon %s"), strIcon.c_str());

        // it could be the real path, but more often a short name
        if (!wxFileExists(strIcon))
        {
            // icon is just the short name
            if ( !strIcon.empty() )
            {
                // we must check if the file exists because it may be stored
                // in many locations, at least ~/.kde and $KDEDIR
                size_t nDir, nDirs = icondirs.GetCount();
                for ( nDir = 0; nDir < nDirs; nDir++ )
                {
                    wxFileName fnameIcon( strIcon );
                    wxFileName fname( icondirs[nDir], fnameIcon.GetName() );
                    fname.SetExt( wxT("png") );
                    if (fname.FileExists())
                    {
                        strIcon = fname.GetFullPath();
                        wxLogTrace(TRACE_MIME, wxT("  iconfile %s"), strIcon.c_str());
                        break;
                    }
                }
            }
        }
    }

    // now look for lines which know about the application
    // exec= or DefaultApp=

    nIndex = file.pIndexOf(wxT("DefaultApp"));

    if ( nIndex == wxNOT_FOUND )
    {
        // no entry try exec
        nIndex = file.pIndexOf(wxT("Exec"));
    }

    if ( nIndex != wxNOT_FOUND )
    {
        // we expect %f; others including  %F and %U and %u are possible
        wxString sTmp = file.GetCmd(nIndex);
        if (0 == sTmp.Replace( wxT("%f"), wxT("%s") ))
            sTmp = sTmp + wxT(" %s");
        entry->AddOrReplaceVerb(wxString(wxT("open")), sTmp );
    }

    AddToMimeData(mimetype, strIcon, entry, sExts, mime_desc);
}

void wxMimeTypesManagerImpl::LoadKDELinksForMimeType(const wxString& dirbase,
                                            const wxString& subdir,
                                            const wxArrayString& icondirs)
{
    wxString dirname = dirbase;
    dirname += subdir;
    wxDir dir(dirname);
    if ( !dir.IsOpened() )
        return;

    wxLogTrace(TRACE_MIME, wxT("--- Loading from KDE directory %s  ---"),
                           dirname.c_str());

    dirname += wxT('/');

    wxString filename;
    bool cont = dir.GetFirst(&filename, wxT("*.kdelnk"), wxDIR_FILES);
    while ( cont )
    {
        LoadKDELinksForMimeSubtype(dirname, subdir, filename, icondirs);

        cont = dir.GetNext(&filename);
    }

    // new standard for Gnome and KDE
    cont = dir.GetFirst(&filename, wxT("*.desktop"), wxDIR_FILES);
    while ( cont )
    {
        LoadKDELinksForMimeSubtype(dirname, subdir, filename, icondirs);

        cont = dir.GetNext(&filename);
    }
}

void wxMimeTypesManagerImpl::LoadKDELinkFilesFromDir(const wxString& dirbase,
                                            const wxArrayString& icondirs)
{
    wxASSERT_MSG( !dirbase.empty() && !wxEndsWithPathSeparator(dirbase),
                  wxT("base directory shouldn't end with a slash") );

    wxString dirname = dirbase;
    dirname << wxT("/mimelnk");

    if ( !wxDir::Exists(dirname) )
        return;

    wxDir dir(dirname);
    if ( !dir.IsOpened() )
        return;

    // we will concatenate it with dir name to get the full path below
    dirname += wxT('/');

    wxString subdir;
    bool cont = dir.GetFirst(&subdir, wxEmptyString, wxDIR_DIRS);
    while ( cont )
    {
        LoadKDELinksForMimeType(dirname, subdir, icondirs);

        cont = dir.GetNext(&subdir);
    }
}

void wxMimeTypesManagerImpl::GetKDEMimeInfo(const wxString& sExtraDir)
{
    wxArrayString dirs;
    wxArrayString icondirs;

    // FIXME: This code is heavily broken. There are three bugs in it:
    //        1) it uses only KDEDIR, which is deprecated, instead of using
    //           list of paths from KDEDIRS and using KDEDIR only if KDEDIRS
    //           is not set
    //        2) it doesn't look into ~/.kde/share/config/kdeglobals where
    //           user's settings are stored and thus *ignores* user's settings
    //           instead of respecting them
    //        3) it "tries to guess KDEDIR" and "tries a few likely theme
    //           names", both of which is completely arbitrary; instead, the
    //           code should give up if KDEDIR(S) is not set and/or the icon
    //           theme cannot be determined, because it means that the user is
    //           not using KDE (and thus is not interested in KDE icons anyway)

    // the variable $KDEDIR is set when KDE is running
    wxString kdedir = wxGetenv( wxT("KDEDIR") );

    if (!kdedir.empty())
    {
        // $(KDEDIR)/share/config/kdeglobals holds info
        // the current icons theme
        wxFileName configFile( kdedir, wxEmptyString );
        configFile.AppendDir( wxT("share") );
        configFile.AppendDir( wxT("config") );
        configFile.SetName( wxT("kdeglobals") );

        wxTextFile config;
        if (configFile.FileExists() && config.Open(configFile.GetFullPath()))
        {
            // $(KDEDIR)/share/config -> $(KDEDIR)/share
            configFile.RemoveDir( configFile.GetDirCount() - 1 );
            // $(KDEDIR)/share/ -> $(KDEDIR)/share/icons
            configFile.AppendDir( wxT("icons") );

            // Check for entry
            wxString theme(wxT("default.kde"));
            size_t cnt = config.GetLineCount();
            for (size_t i = 0; i < cnt; i++)
            {
                if (config[i].StartsWith(wxT("Theme="), &theme/*rest*/))
                    break;
            }

            configFile.AppendDir(theme);
        }
        else
        {
            // $(KDEDIR)/share/config -> $(KDEDIR)/share
            configFile.RemoveDir( configFile.GetDirCount() - 1 );

            // $(KDEDIR)/share/ -> $(KDEDIR)/share/icons
            configFile.AppendDir( wxT("icons") );

            // $(KDEDIR)/share/icons -> $(KDEDIR)/share/icons/default.kde
            configFile.AppendDir( wxT("default.kde") );
        }

        configFile.SetName( wxEmptyString );
        configFile.AppendDir( wxT("32x32") );
        configFile.AppendDir( wxT("mimetypes") );

        // Just try a few likely icons theme names

        int pos = configFile.GetDirCount() - 3;

        if (!wxDir::Exists(configFile.GetPath()))
        {
            configFile.RemoveDir( pos );
            configFile.InsertDir( pos, wxT("default.kde") );
        }

        if (!wxDir::Exists(configFile.GetPath()))
        {
            configFile.RemoveDir( pos );
            configFile.InsertDir( pos, wxT("default") );
        }

        if (!wxDir::Exists(configFile.GetPath()))
        {
            configFile.RemoveDir( pos );
            configFile.InsertDir( pos, wxT("crystalsvg") );
        }

        if (!wxDir::Exists(configFile.GetPath()))
        {
            configFile.RemoveDir( pos );
            configFile.InsertDir( pos, wxT("crystal") );
        }

        if (wxDir::Exists(configFile.GetPath()))
            icondirs.Add( configFile.GetFullPath() );
    }

    // settings in ~/.kde have maximal priority
    dirs.Add(wxGetHomeDir() + wxT("/.kde/share"));
    icondirs.Add(wxGetHomeDir() + wxT("/.kde/share/icons/"));

    if (kdedir)
    {
        dirs.Add( wxString(kdedir) + wxT("/share") );
        icondirs.Add( wxString(kdedir) + wxT("/share/icons/") );
    }
    else
    {
        // try to guess KDEDIR
        dirs.Add(wxT("/usr/share"));
        dirs.Add(wxT("/opt/kde/share"));
        icondirs.Add(wxT("/usr/share/icons/"));
        icondirs.Add(wxT("/usr/X11R6/share/icons/")); // Debian/Corel linux
        icondirs.Add(wxT("/opt/kde/share/icons/"));
    }

    if (!sExtraDir.empty())
        dirs.Add(sExtraDir);
    icondirs.Add(sExtraDir + wxT("/icons"));

    size_t nDirs = dirs.GetCount();
    for ( size_t nDir = 0; nDir < nDirs; nDir++ )
    {
        LoadKDELinkFilesFromDir(dirs[nDir], icondirs);
    }
}

// ----------------------------------------------------------------------------
// wxFileTypeImpl (Unix)
// ----------------------------------------------------------------------------

wxString wxFileTypeImpl::GetExpandedCommand(const wxString & verb, const wxFileType::MessageParameters& params) const
{
    wxString sTmp;
    size_t i = 0;
    while ( (i < m_index.GetCount() ) && sTmp.empty() )
    {
        sTmp = m_manager->GetCommand( verb, m_index[i] );
        i++;
    }

    return wxFileType::ExpandCommand(sTmp, params);
}

bool wxFileTypeImpl::GetIcon(wxIconLocation *iconLoc) const
{
    wxString sTmp;
    size_t i = 0;
    while ( (i < m_index.GetCount() ) && sTmp.empty() )
    {
        sTmp = m_manager->m_aIcons[m_index[i]];
        i++;
    }

    if ( sTmp.empty() )
        return false;

    if ( iconLoc )
    {
        iconLoc->SetFileName(sTmp);
    }

    return true;
}

bool wxFileTypeImpl::GetMimeTypes(wxArrayString& mimeTypes) const
{
    mimeTypes.Clear();
    for (size_t i = 0; i < m_index.GetCount(); i++)
        mimeTypes.Add(m_manager->m_aTypes[m_index[i]]);

    return true;
}

size_t wxFileTypeImpl::GetAllCommands(wxArrayString *verbs,
                                  wxArrayString *commands,
                                  const wxFileType::MessageParameters& params) const
{
    wxString vrb, cmd, sTmp;
    size_t count = 0;
    wxMimeTypeCommands * sPairs;

    // verbs and commands have been cleared already in mimecmn.cpp...
    // if we find no entries in the exact match, try the inexact match
    for (size_t n = 0; ((count == 0) && (n < m_index.GetCount())); n++)
    {
        // list of verb = command pairs for this mimetype
        sPairs = m_manager->m_aEntries [m_index[n]];
        size_t i;
        for ( i = 0; i < sPairs->GetCount(); i++ )
        {
            vrb = sPairs->GetVerb(i);
            // some gnome entries have "." inside
            vrb = vrb.AfterLast(wxT('.'));
            cmd = sPairs->GetCmd(i);
            if (! cmd.empty() )
            {
                 cmd = wxFileType::ExpandCommand(cmd, params);
                 count++;
                 if ( vrb.IsSameAs(wxT("open")))
                 {
                     if ( verbs )
                        verbs->Insert(vrb, 0u);
                     if ( commands )
                        commands ->Insert(cmd, 0u);
                 }
                 else
                 {
                     if ( verbs )
                        verbs->Add(vrb);
                     if ( commands )
                        commands->Add(cmd);
                 }
             }
        }
    }

    return count;
}

bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    wxString strExtensions = m_manager->GetExtension(m_index[0]);
    extensions.Empty();

    // one extension in the space or comma-delimited list
    wxString strExt;
    for ( const wxChar *p = strExtensions; /* nothing */; p++ )
    {
        if ( *p == wxT(' ') || *p == wxT(',') || *p == wxT('\0') )
        {
            if ( !strExt.empty() )
            {
                extensions.Add(strExt);
                strExt.Empty();
            }
            //else: repeated spaces
            // (shouldn't happen, but it's not that important if it does happen)

            if ( *p == wxT('\0') )
                break;
        }
        else if ( *p == wxT('.') )
        {
            // remove the dot from extension (but only if it's the first char)
            if ( !strExt.empty() )
            {
                strExt += wxT('.');
            }
            //else: no, don't append it
        }
        else
        {
            strExt += *p;
        }
    }

    return true;
}

// set an arbitrary command:
// could adjust the code to ask confirmation if it already exists and
// overwriteprompt is true, but this is currently ignored as *Associate* has
// no overwrite prompt
bool
wxFileTypeImpl::SetCommand(const wxString& cmd,
                           const wxString& verb,
                           bool WXUNUSED(overwriteprompt))
{
    wxArrayString strExtensions;
    wxString strDesc, strIcon;

    wxArrayString strTypes;
    GetMimeTypes(strTypes);
    if ( strTypes.IsEmpty() )
        return false;

    wxMimeTypeCommands *entry = new wxMimeTypeCommands();
    entry->Add(verb + wxT("=")  + cmd + wxT(" %s "));

    bool ok = true;
    for ( size_t i = 0; i < strTypes.GetCount(); i++ )
    {
        if (!m_manager->DoAssociation(strTypes[i], strIcon, entry, strExtensions, strDesc))
            ok = false;
    }

    return ok;
}

// ignore index on the grouds that we only have one icon in a Unix file
bool wxFileTypeImpl::SetDefaultIcon(const wxString& strIcon, int WXUNUSED(index))
{
    if (strIcon.empty())
        return false;

    wxArrayString strExtensions;
    wxString strDesc;

    wxArrayString strTypes;
    GetMimeTypes(strTypes);
    if ( strTypes.IsEmpty() )
        return false;

    wxMimeTypeCommands *entry = new wxMimeTypeCommands();
    bool ok = true;
    for ( size_t i = 0; i < strTypes.GetCount(); i++ )
    {
        if ( !m_manager->DoAssociation
                         (
                            strTypes[i],
                            strIcon,
                            entry,
                            strExtensions,
                            strDesc
                         ) )
        {
            ok = false;
        }
    }

    return ok;
}

// ----------------------------------------------------------------------------
// wxMimeTypesManagerImpl (Unix)
// ----------------------------------------------------------------------------

wxMimeTypesManagerImpl::wxMimeTypesManagerImpl()
{
    m_initialized = false;
    m_mailcapStylesInited = 0;
}

void wxMimeTypesManagerImpl::InitIfNeeded()
{
    if ( !m_initialized )
    {
        // set the flag first to prevent recursion
        m_initialized = true;

    wxString wm = wxGetenv( wxT("WINDOWMANAGER") );

    if (wm.Find( wxT("kde") ) != wxNOT_FOUND)
        Initialize( wxMAILCAP_KDE  );
    else if (wm.Find( wxT("gnome") ) != wxNOT_FOUND)
        Initialize( wxMAILCAP_GNOME );
    else
        Initialize();
    }
}

// read system and user mailcaps and other files
void wxMimeTypesManagerImpl::Initialize(int mailcapStyles,
                                        const wxString& sExtraDir)
{
    // read mimecap amd mime.types
    if ( (mailcapStyles & wxMAILCAP_NETSCAPE) ||
         (mailcapStyles & wxMAILCAP_STANDARD) )
        GetMimeInfo(sExtraDir);

    // read GNOME tables
    if (mailcapStyles & wxMAILCAP_GNOME)
        GetGnomeMimeInfo(sExtraDir);

    // read KDE tables
    if (mailcapStyles & wxMAILCAP_KDE)
        GetKDEMimeInfo(sExtraDir);

    m_mailcapStylesInited |= mailcapStyles;
}

// clear data so you can read another group of WM files
void wxMimeTypesManagerImpl::ClearData()
{
    m_aTypes.Clear();
    m_aIcons.Clear();
    m_aExtensions.Clear();
    m_aDescriptions.Clear();

    WX_CLEAR_ARRAY(m_aEntries);
    m_aEntries.Empty();

    m_mailcapStylesInited = 0;
}

wxMimeTypesManagerImpl::~wxMimeTypesManagerImpl()
{
    ClearData();
}

void wxMimeTypesManagerImpl::GetMimeInfo(const wxString& sExtraDir)
{
    // read this for netscape or Metamail formats

    // directories where we look for mailcap and mime.types by default
    // used by netscape and pine and other mailers, using 2 different formats!

    // (taken from metamail(1) sources)
    //
    // although RFC 1524 specifies the search path of
    // /etc/:/usr/etc:/usr/local/etc only, it doesn't hurt to search in more
    // places - OTOH, the RFC also says that this path can be changed with
    // MAILCAPS environment variable (containing the colon separated full
    // filenames to try) which is not done yet (TODO?)

    wxString strHome = wxGetenv(wxT("HOME"));

    wxArrayString dirs;
    dirs.Add( strHome + wxT("/.") );
    dirs.Add( wxT("/etc/") );
    dirs.Add( wxT("/usr/etc/") );
    dirs.Add( wxT("/usr/local/etc/") );
    dirs.Add( wxT("/etc/mail/") );
    dirs.Add( wxT("/usr/public/lib/") );
    if (!sExtraDir.empty())
        dirs.Add( sExtraDir + wxT("/") );

    size_t nDirs = dirs.GetCount();
    for ( size_t nDir = 0; nDir < nDirs; nDir++ )
    {
        wxString file = dirs[nDir] + wxT("mailcap");
        if ( wxFile::Exists(file) )
        {
            ReadMailcap(file);
        }

        file = dirs[nDir] + wxT("mime.types");
        if ( wxFile::Exists(file) )
        {
            ReadMimeTypes(file);
        }
    }
}

bool wxMimeTypesManagerImpl::WriteToMimeTypes(int index, bool delete_index)
{
    // check we have the right manager
    if (! ( m_mailcapStylesInited & wxMAILCAP_STANDARD) )
        return false;

    bool bTemp;
    wxString strHome = wxGetenv(wxT("HOME"));

    // and now the users mailcap
    wxString strUserMailcap = strHome + wxT("/.mime.types");

    wxMimeTextFile file;
    if ( wxFile::Exists(strUserMailcap) )
    {
        bTemp = file.Open(strUserMailcap);
    }
    else
    {
        if (delete_index)
            return false;

        bTemp = file.Create(strUserMailcap);
    }

    if (bTemp)
    {
        int nIndex;
        // test for netscape's header and return false if its found
        nIndex = file.pIndexOf(wxT("#--Netscape"));
        if (nIndex != wxNOT_FOUND)
        {
            wxFAIL_MSG(wxT("Error in .mime.types\nTrying to mix Netscape and Metamail formats\nFile not modified"));
            return false;
        }

        // write it in alternative format
        // get rid of unwanted entries
        wxString strType = m_aTypes[index];
        nIndex = file.pIndexOf(strType);

        // get rid of all the unwanted entries...
        if (nIndex != wxNOT_FOUND)
            file.CommentLine(nIndex);

        if (!delete_index)
        {
            // add the new entries in
            wxString sTmp = strType.Append( wxT(' '), 40 - strType.Len() );
            sTmp = sTmp + m_aExtensions[index];
            file.AddLine(sTmp);
        }

        bTemp = file.Write();
        file.Close();
    }

    return bTemp;
}

bool wxMimeTypesManagerImpl::WriteToNSMimeTypes(int index, bool delete_index)
{
    //check we have the right managers
    if (! ( m_mailcapStylesInited & wxMAILCAP_NETSCAPE) )
        return false;

    bool bTemp;
    wxString strHome = wxGetenv(wxT("HOME"));

    // and now the users mailcap
    wxString strUserMailcap = strHome + wxT("/.mime.types");

    wxMimeTextFile file;
    if ( wxFile::Exists(strUserMailcap) )
    {
        bTemp = file.Open(strUserMailcap);
    }
    else
    {
        if (delete_index)
            return false;

        bTemp = file.Create(strUserMailcap);
    }

    if (bTemp)
    {
        // write it in the format that Netscape uses
        int nIndex;
        // test for netscape's header and insert if required...
        // this is a comment so use true
        nIndex = file.pIndexOf(wxT("#--Netscape"), true);
        if (nIndex == wxNOT_FOUND)
        {
            // either empty file or metamail format
            // at present we can't cope with mixed formats, so exit to preseve
            // metamail entreies
            if (file.GetLineCount() > 0)
            {
                wxFAIL_MSG(wxT(".mime.types File not in Netscape format\nNo entries written to\n.mime.types or to .mailcap"));
                return false;
            }

            file.InsertLine(wxT( "#--Netscape Communications Corporation MIME Information" ), 0);
            nIndex = 0;
        }

        wxString strType = wxT("type=") + m_aTypes[index];
        nIndex = file.pIndexOf(strType);

        // get rid of all the unwanted entries...
        if (nIndex != wxNOT_FOUND)
        {
            wxString sOld = file[nIndex];
            while ( (sOld.Contains(wxT("\\"))) && (nIndex < (int) file.GetLineCount()) )
            {
                file.CommentLine(nIndex);
                sOld = file[nIndex];

                wxLogTrace(TRACE_MIME, wxT("--- Deleting from mime.types line '%d %s' ---"), nIndex, sOld.c_str());

                nIndex++;
            }

            if (nIndex < (int) file.GetLineCount())
                file.CommentLine(nIndex);
        }
        else
            nIndex = (int) file.GetLineCount();

        wxString sTmp = strType + wxT(" \\");
        if (!delete_index)
            file.InsertLine(sTmp, nIndex);

        if ( ! m_aDescriptions.Item(index).empty() )
        {
            sTmp = wxT("desc=\"") + m_aDescriptions[index]+ wxT("\" \\"); //.trim ??
            if (!delete_index)
            {
                nIndex++;
                file.InsertLine(sTmp, nIndex);
            }
        }

        wxString sExts = m_aExtensions.Item(index);
        sTmp = wxT("exts=\"") + sExts.Trim(false).Trim() + wxT("\"");
        if (!delete_index)
        {
            nIndex++;
            file.InsertLine(sTmp, nIndex);
        }

        bTemp = file.Write();
        file.Close();
    }

    return bTemp;
}

bool wxMimeTypesManagerImpl::WriteToMailCap(int index, bool delete_index)
{
    //check we have the right managers
    if ( !( ( m_mailcapStylesInited & wxMAILCAP_NETSCAPE) ||
            ( m_mailcapStylesInited & wxMAILCAP_STANDARD) ) )
        return false;

    bool bTemp = false;
    wxString strHome = wxGetenv(wxT("HOME"));

    // and now the users mailcap
    wxString strUserMailcap = strHome + wxT("/.mailcap");

    wxMimeTextFile file;
    if ( wxFile::Exists(strUserMailcap) )
    {
        bTemp = file.Open(strUserMailcap);
    }
    else
    {
        if (delete_index)
            return false;

        bTemp = file.Create(strUserMailcap);
    }

    if (bTemp)
    {
        // now got a file we can write to ....
        wxMimeTypeCommands * entries = m_aEntries[index];
        size_t iOpen;
        wxString sCmd = entries->GetCommandForVerb(wxT("open"), &iOpen);
        wxString sTmp;

        sTmp = m_aTypes[index];
        wxString sOld;
        int nIndex = file.pIndexOf(sTmp);

        // get rid of all the unwanted entries...
        if (nIndex == wxNOT_FOUND)
        {
            nIndex = (int) file.GetLineCount();
        }
        else
        {
            sOld = file[nIndex];
            wxLogTrace(TRACE_MIME, wxT("--- Deleting from mailcap line '%d' ---"), nIndex);

            while ( (sOld.Contains(wxT("\\"))) && (nIndex < (int) file.GetLineCount()) )
            {
                file.CommentLine(nIndex);
                if (nIndex < (int) file.GetLineCount())
                    sOld = sOld + file[nIndex];
            }

            if (nIndex < (int)
                file.GetLineCount()) file.CommentLine(nIndex);
        }

        sTmp = sTmp + wxT(";") + sCmd; //includes wxT(" %s ");

        // write it in the format that Netscape uses (default)
        if (! ( m_mailcapStylesInited & wxMAILCAP_STANDARD ) )
        {
            if (! delete_index)
                file.InsertLine(sTmp, nIndex);
            nIndex++;
        }
        else
        {
            // write extended format

            // TODO - FIX this code:
            // ii) lost entries
            // sOld holds all the entries, but our data store only has some
            // eg test= is not stored

            // so far we have written the mimetype and command out
            wxStringTokenizer sT(sOld, wxT(";\\"));
            if (sT.CountTokens() > 2)
            {
                // first one mimetype; second one command, rest unknown...
                wxString s;
                s = sT.GetNextToken();
                s = sT.GetNextToken();

                // first unknown
                s = sT.GetNextToken();
                while ( ! s.empty() )
                {
                    bool bKnownToken = false;
                    if (s.Contains(wxT("description=")))
                        bKnownToken = true;
                    if (s.Contains(wxT("x11-bitmap=")))
                        bKnownToken = true;

                    size_t i;
                    for (i=0; i < entries->GetCount(); i++)
                    {
                        if (s.Contains(entries->GetVerb(i)))
                            bKnownToken = true;
                    }

                    if (!bKnownToken)
                    {
                        sTmp = sTmp + wxT("; \\");
                        file.InsertLine(sTmp, nIndex);
                        sTmp = s;
                    }

                    s = sT.GetNextToken();
                }
            }

            if (! m_aDescriptions[index].empty() )
            {
                sTmp = sTmp + wxT("; \\");
                file.InsertLine(sTmp, nIndex);
                nIndex++;
                sTmp = wxT("       description=\"") + m_aDescriptions[index] + wxT("\"");
            }

            if (! m_aIcons[index].empty() )
            {
                sTmp = sTmp + wxT("; \\");
                file.InsertLine(sTmp, nIndex);
                nIndex++;
                sTmp = wxT("       x11-bitmap=\"") + m_aIcons[index] + wxT("\"");
            }

            if ( entries->GetCount() > 1 )
            {
                size_t i;
                for (i=0; i < entries->GetCount(); i++)
                    if ( i != iOpen )
                    {
                        sTmp = sTmp + wxT("; \\");
                        file.InsertLine(sTmp, nIndex);
                        nIndex++;
                        sTmp = wxT("       ") + entries->GetVerbCmd(i);
                    }
            }

            file.InsertLine(sTmp, nIndex);
            nIndex++;
        }

        bTemp = file.Write();
        file.Close();
    }

    return bTemp;
}

wxFileType * wxMimeTypesManagerImpl::Associate(const wxFileTypeInfo& ftInfo)
{
    InitIfNeeded();

    wxString strType = ftInfo.GetMimeType();
    wxString strDesc = ftInfo.GetDescription();
    wxString strIcon = ftInfo.GetIconFile();

    wxMimeTypeCommands *entry = new wxMimeTypeCommands();

    if ( ! ftInfo.GetOpenCommand().empty())
        entry->Add(wxT("open=")  + ftInfo.GetOpenCommand() + wxT(" %s "));
    if ( ! ftInfo.GetPrintCommand().empty())
        entry->Add(wxT("print=") + ftInfo.GetPrintCommand() + wxT(" %s "));

    // now find where these extensions are in the data store and remove them
    wxArrayString sA_Exts = ftInfo.GetExtensions();
    wxString sExt, sExtStore;
    size_t i, nIndex;
    for (i=0; i < sA_Exts.GetCount(); i++)
    {
        sExt = sA_Exts.Item(i);

        // clean up to just a space before and after
        sExt.Trim().Trim(false);
        sExt = wxT(' ') + sExt + wxT(' ');
        for (nIndex = 0; nIndex < m_aExtensions.GetCount(); nIndex++)
        {
            sExtStore = m_aExtensions.Item(nIndex);
            if (sExtStore.Replace(sExt, wxT(" ") ) > 0)
                m_aExtensions.Item(nIndex) = sExtStore;
        }
    }

    if ( !DoAssociation(strType, strIcon, entry, sA_Exts, strDesc) )
        return NULL;

    return GetFileTypeFromMimeType(strType);
}

bool wxMimeTypesManagerImpl::DoAssociation(const wxString& strType,
                                           const wxString& strIcon,
                                           wxMimeTypeCommands *entry,
                                           const wxArrayString& strExtensions,
                                           const wxString& strDesc)
{
    int nIndex = AddToMimeData(strType, strIcon, entry, strExtensions, strDesc, true);

    if ( nIndex == wxNOT_FOUND )
        return false;

    return WriteMimeInfo(nIndex, false);
}

bool wxMimeTypesManagerImpl::WriteMimeInfo(int nIndex, bool delete_mime )
{
    bool ok = true;

    if ( m_mailcapStylesInited & wxMAILCAP_STANDARD )
    {
        // write in metamail  format;
        if (WriteToMimeTypes(nIndex, delete_mime) )
            if ( WriteToMailCap(nIndex, delete_mime) )
                ok = false;
    }

    if ( m_mailcapStylesInited & wxMAILCAP_NETSCAPE )
    {
        // write in netsacpe format;
        if (WriteToNSMimeTypes(nIndex, delete_mime) )
            if ( WriteToMailCap(nIndex, delete_mime) )
                ok = false;
    }

    // Don't write GNOME files here as this is not
    // allowed and simply doesn't work

    if (m_mailcapStylesInited & wxMAILCAP_KDE)
    {
        // write in KDE format;
        if (WriteKDEMimeFile(nIndex, delete_mime) )
            ok = false;
    }

    return ok;
}

int wxMimeTypesManagerImpl::AddToMimeData(const wxString& strType,
                                          const wxString& strIcon,
                                          wxMimeTypeCommands *entry,
                                          const wxArrayString& strExtensions,
                                          const wxString& strDesc,
                                          bool replaceExisting)
{
    InitIfNeeded();

    // ensure mimetype is always lower case
    wxString mimeType = strType.Lower();

    // is this a known MIME type?
    int nIndex = m_aTypes.Index(mimeType);
    if ( nIndex == wxNOT_FOUND )
    {
        // new file type
        m_aTypes.Add(mimeType);
        m_aIcons.Add(strIcon);
        m_aEntries.Add(entry ? entry : new wxMimeTypeCommands);

        // change nIndex so we can use it below to add the extensions
        m_aExtensions.Add(wxEmptyString);
        nIndex = m_aExtensions.size() - 1;

        m_aDescriptions.Add(strDesc);
    }
    else // yes, we already have it
    {
        if ( replaceExisting )
        {
            // if new description change it
            if ( !strDesc.empty())
                m_aDescriptions[nIndex] = strDesc;

            // if new icon change it
            if ( !strIcon.empty())
                m_aIcons[nIndex] = strIcon;

            if ( entry )
            {
                delete m_aEntries[nIndex];
                m_aEntries[nIndex] = entry;
            }
        }
        else // add data we don't already have ...
        {
            // if new description add only if none
            if ( m_aDescriptions[nIndex].empty() )
                m_aDescriptions[nIndex] = strDesc;

            // if new icon and no existing icon
            if ( m_aIcons[nIndex].empty() )
                m_aIcons[nIndex] = strIcon;

            // add any new entries...
            if ( entry )
            {
                wxMimeTypeCommands *entryOld = m_aEntries[nIndex];

                size_t count = entry->GetCount();
                for ( size_t i = 0; i < count; i++ )
                {
                    const wxString& verb = entry->GetVerb(i);
                    if ( !entryOld->HasVerb(verb) )
                    {
                        entryOld->AddOrReplaceVerb(verb, entry->GetCmd(i));
                    }
                }

                // as we don't store it anywhere, it won't be deleted later as
                // usual -- do it immediately instead
                delete entry;
            }
        }
    }

    // always add the extensions to this mimetype
    wxString& exts = m_aExtensions[nIndex];

    // add all extensions we don't have yet
    size_t count = strExtensions.GetCount();
    for ( size_t i = 0; i < count; i++ )
    {
        wxString ext = strExtensions[i] + wxT(' ');

        if ( exts.Find(ext) == wxNOT_FOUND )
        {
            exts += ext;
        }
    }

    // check data integrity
    wxASSERT( m_aTypes.Count() == m_aEntries.Count() &&
              m_aTypes.Count() == m_aExtensions.Count() &&
              m_aTypes.Count() == m_aIcons.Count() &&
              m_aTypes.Count() == m_aDescriptions.Count() );

    return nIndex;
}

wxFileType * wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& ext)
{
    if (ext.empty() )
        return NULL;

    InitIfNeeded();

    size_t count = m_aExtensions.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxStringTokenizer tk(m_aExtensions[n], wxT(' '));

        while ( tk.HasMoreTokens() )
        {
            // consider extensions as not being case-sensitive
            if ( tk.GetNextToken().IsSameAs(ext, false /* no case */) )
            {
                // found
                wxFileType *fileType = new wxFileType;
                fileType->m_impl->Init(this, n);

                return fileType;
            }
        }
    }

    return NULL;
}

wxFileType * wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    InitIfNeeded();

    wxFileType * fileType = NULL;
    // mime types are not case-sensitive
    wxString mimetype(mimeType);
    mimetype.MakeLower();

    // first look for an exact match
    int index = m_aTypes.Index(mimetype);
    if ( index != wxNOT_FOUND )
    {
        fileType = new wxFileType;
        fileType->m_impl->Init(this, index);
    }

    // then try to find "text/*" as match for "text/plain" (for example)
    // NB: if mimeType doesn't contain '/' at all, BeforeFirst() will return
    //     the whole string - ok.

    index = wxNOT_FOUND;
    wxString strCategory = mimetype.BeforeFirst(wxT('/'));

    size_t nCount = m_aTypes.Count();
    for ( size_t n = 0; n < nCount; n++ )
    {
        if ( (m_aTypes[n].BeforeFirst(wxT('/')) == strCategory ) &&
                m_aTypes[n].AfterFirst(wxT('/')) == wxT("*") )
        {
            index = n;
            break;
        }
    }

    if ( index != wxNOT_FOUND )
    {
       // don't throw away fileType that was already found
        if (!fileType)
            fileType = new wxFileType;
        fileType->m_impl->Init(this, index);
    }

    return fileType;
}

wxString wxMimeTypesManagerImpl::GetCommand(const wxString & verb, size_t nIndex) const
{
    wxString command, testcmd, sV, sTmp;
    sV = verb + wxT("=");

    // list of verb = command pairs for this mimetype
    wxMimeTypeCommands * sPairs = m_aEntries [nIndex];

    size_t i;
    for ( i = 0; i < sPairs->GetCount (); i++ )
    {
        sTmp = sPairs->GetVerbCmd (i);
        if ( sTmp.Contains(sV) )
            command = sTmp.AfterFirst(wxT('='));
    }

    return command;
}

void wxMimeTypesManagerImpl::AddFallback(const wxFileTypeInfo& filetype)
{
    InitIfNeeded();

    wxString extensions;
    const wxArrayString& exts = filetype.GetExtensions();
    size_t nExts = exts.GetCount();
    for ( size_t nExt = 0; nExt < nExts; nExt++ )
    {
        if ( nExt > 0 )
            extensions += wxT(' ');

        extensions += exts[nExt];
    }

    AddMimeTypeInfo(filetype.GetMimeType(),
                    extensions,
                    filetype.GetDescription());

    AddMailcapInfo(filetype.GetMimeType(),
                   filetype.GetOpenCommand(),
                   filetype.GetPrintCommand(),
                   wxT(""),
                   filetype.GetDescription());
}

void wxMimeTypesManagerImpl::AddMimeTypeInfo(const wxString& strMimeType,
                                             const wxString& strExtensions,
                                             const wxString& strDesc)
{
    // reading mailcap may find image/* , while
    // reading mime.types finds image/gif and no match is made
    // this means all the get functions don't work  fix this
    wxString strIcon;
    wxString sTmp = strExtensions;

    wxArrayString sExts;
    sTmp.Trim().Trim(false);

    while (!sTmp.empty())
    {
        sExts.Add(sTmp.AfterLast(wxT(' ')));
        sTmp = sTmp.BeforeLast(wxT(' '));
    }

    AddToMimeData(strMimeType, strIcon, NULL, sExts, strDesc, true);
}

void wxMimeTypesManagerImpl::AddMailcapInfo(const wxString& strType,
                                            const wxString& strOpenCmd,
                                            const wxString& strPrintCmd,
                                            const wxString& strTest,
                                            const wxString& strDesc)
{
    InitIfNeeded();

    wxMimeTypeCommands *entry = new wxMimeTypeCommands;
    entry->Add(wxT("open=")  + strOpenCmd);
    entry->Add(wxT("print=") + strPrintCmd);
    entry->Add(wxT("test=")  + strTest);

    wxString strIcon;
    wxArrayString strExtensions;

    AddToMimeData(strType, strIcon, entry, strExtensions, strDesc, true);
}

bool wxMimeTypesManagerImpl::ReadMimeTypes(const wxString& strFileName)
{
    wxLogTrace(TRACE_MIME, wxT("--- Parsing mime.types file '%s' ---"),
               strFileName.c_str());

    wxTextFile file(strFileName);
#if defined(__WXGTK20__) && wxUSE_UNICODE
    if ( !file.Open(wxConvUTF8) )
#else
    if ( !file.Open() )
#endif
        return false;

    // the information we extract
    wxString strMimeType, strDesc, strExtensions;

    size_t nLineCount = file.GetLineCount();
    const wxChar *pc = NULL;
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
    {
        if ( pc == NULL )
        {
            // now we're at the start of the line
            pc = file[nLine].c_str();
        }
        else
        {
            // we didn't finish with the previous line yet
            nLine--;
        }

        // skip whitespace
        while ( wxIsspace(*pc) )
            pc++;

        // comment or blank line?
        if ( *pc == wxT('#') || !*pc )
        {
            // skip the whole line
            pc = NULL;
            continue;
        }

        // detect file format
        const wxChar *pEqualSign = wxStrchr(pc, wxT('='));
        if ( pEqualSign == NULL )
        {
            // brief format
            // ------------

            // first field is mime type
            for ( strMimeType.Empty(); !wxIsspace(*pc) && *pc != wxT('\0'); pc++ )
            {
                strMimeType += *pc;
            }

            // skip whitespace
            while ( wxIsspace(*pc) )
                pc++;

            // take all the rest of the string
            strExtensions = pc;

            // no description...
            strDesc.Empty();
        }
        else
        {
            // expanded format
            // ---------------

            // the string on the left of '=' is the field name
            wxString strLHS(pc, pEqualSign - pc);

            // eat whitespace
            for ( pc = pEqualSign + 1; wxIsspace(*pc); pc++ )
              ;

            const wxChar *pEnd;
            if ( *pc == wxT('"') )
            {
                // the string is quoted and ends at the matching quote
                pEnd = wxStrchr(++pc, wxT('"'));
                if ( pEnd == NULL )
                {
                    wxLogWarning(wxT("Mime.types file %s, line %lu: unterminated quoted string."),
                                 strFileName.c_str(), nLine + 1L);
                }
            }
            else
            {
                // unquoted string ends at the first space or at the end of
                // line
                for ( pEnd = pc; *pEnd && !wxIsspace(*pEnd); pEnd++ )
                  ;
            }

            // now we have the RHS (field value)
            wxString strRHS(pc, pEnd - pc);

            // check what follows this entry
            if ( *pEnd == wxT('"') )
            {
                // skip this quote
                pEnd++;
            }

            for ( pc = pEnd; wxIsspace(*pc); pc++ )
              ;

            // if there is something left, it may be either a '\\' to continue
            // the line or the next field of the same entry
            bool entryEnded = *pc == wxT('\0');
            bool nextFieldOnSameLine = false;
            if ( !entryEnded )
            {
                nextFieldOnSameLine = ((*pc != wxT('\\')) || (pc[1] != wxT('\0')));
            }

            // now see what we got
            if ( strLHS == wxT("type") )
            {
                strMimeType = strRHS;
            }
            else if ( strLHS.StartsWith(wxT("desc")) )
            {
                strDesc = strRHS;
            }
            else if ( strLHS == wxT("exts") )
            {
                strExtensions = strRHS;
            }
            else if ( strLHS == wxT("icon") )
            {
                // this one is simply ignored: it usually refers to Netscape
                // built in icons which are useless for us anyhow
            }
            else if ( !strLHS.StartsWith(wxT("x-")) )
            {
                // we suppose that all fields starting with "X-" are
                // unregistered extensions according to the standard practice,
                // but it may be worth telling the user about other junk in
                // his mime.types file
                wxLogWarning(wxT("Unknown field in file %s, line %lu: '%s'."),
                             strFileName.c_str(), nLine + 1L, strLHS.c_str());
            }

            if ( !entryEnded )
            {
                if ( !nextFieldOnSameLine )
                    pc = NULL;
                //else: don't reset it

                // as we don't reset strMimeType, the next field in this entry
                // will be interpreted correctly.

                continue;
            }
        }

        // depending on the format (Mosaic or Netscape) either space or comma
        // is used to separate the extensions
        strExtensions.Replace(wxT(","), wxT(" "));

        // also deal with the leading dot
        if ( !strExtensions.empty() && strExtensions[0u] == wxT('.') )
        {
            strExtensions.erase(0, 1);
        }

        wxLogTrace(TRACE_MIME, wxT("mime.types: '%s' => '%s' (%s)"),
                   strExtensions.c_str(),
                   strMimeType.c_str(),
                   strDesc.c_str());

        AddMimeTypeInfo(strMimeType, strExtensions, strDesc);

        // finished with this line
        pc = NULL;
    }

    return true;
}

// ----------------------------------------------------------------------------
// UNIX mailcap files parsing
// ----------------------------------------------------------------------------

// the data for a single MIME type
struct MailcapLineData
{
    // field values
    wxString type,
             cmdOpen,
             test,
             icon,
             desc;

    wxArrayString verbs,
                  commands;

    // flags
    bool testfailed,
         needsterminal,
         copiousoutput;

    MailcapLineData() { testfailed = needsterminal = copiousoutput = false; }
};

// process a non-standard (i.e. not the first or second one) mailcap field
bool
wxMimeTypesManagerImpl::ProcessOtherMailcapField(MailcapLineData& data,
                                                 const wxString& curField)
{
    if ( curField.empty() )
    {
        // we don't care
        return true;
    }

    // is this something of the form foo=bar?
    const wxChar *pEq = wxStrchr(curField, wxT('='));
    if ( pEq != NULL )
    {
        // split "LHS = RHS" in 2
        wxString lhs = curField.BeforeFirst(wxT('=')),
                 rhs = curField.AfterFirst(wxT('='));

        lhs.Trim(true);     // from right
        rhs.Trim(false);    // from left

        // it might be quoted
        if ( !rhs.empty() && rhs[0u] == wxT('"') && rhs.Last() == wxT('"') )
        {
            rhs = rhs.Mid(1, rhs.length() - 2);
        }

        // is it a command verb or something else?
        if ( lhs == wxT("test") )
        {
            if ( wxSystem(rhs) == 0 )
            {
                // ok, test passed
                wxLogTrace(TRACE_MIME_TEST,
                           wxT("Test '%s' for mime type '%s' succeeded."),
                           rhs.c_str(), data.type.c_str());
            }
            else
            {
                wxLogTrace(TRACE_MIME_TEST,
                           wxT("Test '%s' for mime type '%s' failed, skipping."),
                           rhs.c_str(), data.type.c_str());

                data.testfailed = true;
            }
        }
        else if ( lhs == wxT("desc") )
        {
            data.desc = rhs;
        }
        else if ( lhs == wxT("x11-bitmap") )
        {
            data.icon = rhs;
        }
        else if ( lhs == wxT("notes") )
        {
            // ignore
        }
        else // not a (recognized) special case, must be a verb (e.g. "print")
        {
            data.verbs.Add(lhs);
            data.commands.Add(rhs);
        }
    }
    else // '=' not found
    {
        // so it must be a simple flag
        if ( curField == wxT("needsterminal") )
        {
            data.needsterminal = true;
        }
        else if ( curField == wxT("copiousoutput"))
        {
            // copiousoutput impies that the viewer is a console program
            data.needsterminal =
            data.copiousoutput = true;
        }
        else if ( !IsKnownUnimportantField(curField) )
        {
            return false;
        }
    }

    return true;
}

bool wxMimeTypesManagerImpl::ReadMailcap(const wxString& strFileName,
                                         bool fallback)
{
    wxLogTrace(TRACE_MIME, wxT("--- Parsing mailcap file '%s' ---"),
               strFileName.c_str());

    wxTextFile file(strFileName);
#if defined(__WXGTK20__) && wxUSE_UNICODE
    if ( !file.Open(wxConvUTF8) )
#else
    if ( !file.Open() )
#endif
        return false;

    // indices of MIME types (in m_aTypes) we already found in this file
    //
    // (see the comments near the end of function for the reason we need this)
    wxArrayInt aIndicesSeenHere;

    // accumulator for the current field
    wxString curField;
    curField.reserve(1024);

    size_t nLineCount = file.GetLineCount();
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
    {
        // now we're at the start of the line
        const wxChar *pc = file[nLine].c_str();

        // skip whitespace
        while ( wxIsspace(*pc) )
            pc++;

        // comment or empty string?
        if ( *pc == wxT('#') || *pc == wxT('\0') )
            continue;

        // no, do parse
        // ------------

        // what field are we currently in? The first 2 are fixed and there may
        // be an arbitrary number of other fields parsed by
        // ProcessOtherMailcapField()
        //
        // the first field is the MIME type
        enum
        {
            Field_Type,
            Field_OpenCmd,
            Field_Other
        }
        currentToken = Field_Type;

        // the flags and field values on the current line
        MailcapLineData data;

        bool cont = true;
        while ( cont )
        {
            switch ( *pc )
            {
                case wxT('\\'):
                    // interpret the next character literally (notice that
                    // backslash can be used for line continuation)
                    if ( *++pc == wxT('\0') )
                    {
                        // fetch the next line if there is one
                        if ( nLine == nLineCount - 1 )
                        {
                            // something is wrong, bail out
                            cont = false;

                            wxLogDebug(wxT("Mailcap file %s, line %lu: '\\' on the end of the last line ignored."),
                                       strFileName.c_str(),
                                       nLine + 1L);
                        }
                        else
                        {
                            // pass to the beginning of the next line
                            pc = file[++nLine].c_str();

                            // skip pc++ at the end of the loop
                            continue;
                        }
                    }
                    else
                    {
                        // just a normal character
                        curField += *pc;
                    }
                    break;

                case wxT('\0'):
                    cont = false;   // end of line reached, exit the loop

                    // fall through to still process this field

                case wxT(';'):
                    // trim whitespaces from both sides
                    curField.Trim(true).Trim(false);

                    switch ( currentToken )
                    {
                        case Field_Type:
                            data.type = curField.Lower();
                            if ( data.type.empty() )
                            {
                                // I don't think that this is a valid mailcap
                                // entry, but try to interpret it somehow
                                data.type = wxT('*');
                            }

                            if ( data.type.Find(wxT('/')) == wxNOT_FOUND )
                            {
                                // we interpret "type" as "type/*"
                                data.type += wxT("/*");
                            }

                            currentToken = Field_OpenCmd;
                            break;

                        case Field_OpenCmd:
                            data.cmdOpen = curField;

                            currentToken = Field_Other;
                            break;

                        case Field_Other:
                            if ( !ProcessOtherMailcapField(data, curField) )
                            {
                                // don't flood the user with error messages if
                                // we don't understand something in his
                                // mailcap, but give them in debug mode because
                                // this might be useful for the programmer
                                wxLogDebug
                                (
                                    wxT("Mailcap file %s, line %lu: unknown field '%s' for the MIME type '%s' ignored."),
                                    strFileName.c_str(),
                                    nLine + 1L,
                                    curField.c_str(),
                                    data.type.c_str()
                                );
                            }
                            else if ( data.testfailed )
                            {
                                // skip this entry entirely
                                cont = false;
                            }

                            // it already has this value
                            //currentToken = Field_Other;
                            break;

                        default:
                            wxFAIL_MSG(wxT("unknown field type in mailcap"));
                    }

                    // next token starts immediately after ';'
                    curField.Empty();
                    break;

                default:
                    curField += *pc;
            }

            // continue in the same line
            pc++;
        }

        // we read the entire entry, check what have we got
        // ------------------------------------------------

        // check that we really read something reasonable
        if ( currentToken < Field_Other )
        {
            wxLogWarning(wxT("Mailcap file %s, line %lu: incomplete entry ignored."),
                         strFileName.c_str(), nLine + 1L);

            continue;
        }

        // if the test command failed, it's as if the entry were not there at all
        if ( data.testfailed )
        {
            continue;
        }

        // support for flags:
        //  1. create an xterm for 'needsterminal'
        //  2. append "| $PAGER" for 'copiousoutput'
        //
        // Note that the RFC says that having both needsterminal and
        // copiousoutput is probably a mistake, so it seems that running
        // programs with copiousoutput inside an xterm as it is done now
        // is a bad idea (FIXME)
        if ( data.copiousoutput )
        {
            const wxChar *p = wxGetenv(wxT("PAGER"));
            data.cmdOpen << wxT(" | ") << (p ? p : wxT("more"));
        }

        if ( data.needsterminal )
        {
            data.cmdOpen = wxString::Format(wxT("xterm -e sh -c '%s'"),
                                            data.cmdOpen.c_str());
        }

        if ( !data.cmdOpen.empty() )
        {
            data.verbs.Insert(wxT("open"), 0);
            data.commands.Insert(data.cmdOpen, 0);
        }

        // we have to decide whether the new entry should replace any entries
        // for the same MIME type we had previously found or not
        bool overwrite;

        // the fall back entries have the lowest priority, by definition
        if ( fallback )
        {
            overwrite = false;
        }
        else
        {
            // have we seen this one before?
            int nIndex = m_aTypes.Index(data.type);

            // and if we have, was it in this file? if not, we should
            // overwrite the previously seen one
            overwrite = nIndex == wxNOT_FOUND ||
                            aIndicesSeenHere.Index(nIndex) == wxNOT_FOUND;
        }

        wxLogTrace(TRACE_MIME, wxT("mailcap %s: %s [%s]"),
                   data.type.c_str(), data.cmdOpen.c_str(),
                   overwrite ? wxT("replace") : wxT("add"));

        int n = AddToMimeData
                (
                    data.type,
                    data.icon,
                    new wxMimeTypeCommands(data.verbs, data.commands),
                    wxArrayString() /* extensions */,
                    data.desc,
                    overwrite
                );

        if ( overwrite )
        {
            aIndicesSeenHere.Add(n);
        }
    }

    return true;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxArrayString& mimetypes)
{
    InitIfNeeded();

    mimetypes.Empty();

    wxString type;
    size_t count = m_aTypes.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        // don't return template types from here (i.e. anything containg '*')
        type = m_aTypes[n];
        if ( type.Find(wxT('*')) == wxNOT_FOUND )
        {
            mimetypes.Add(type);
        }
    }

    return mimetypes.GetCount();
}

// ----------------------------------------------------------------------------
// writing to MIME type files
// ----------------------------------------------------------------------------

bool wxMimeTypesManagerImpl::Unassociate(wxFileType *ft)
{
    InitIfNeeded();

    wxArrayString sMimeTypes;
    ft->GetMimeTypes(sMimeTypes);

    wxString sMime;
    size_t i;
    for (i = 0; i < sMimeTypes.GetCount(); i ++)
    {
        sMime = sMimeTypes.Item(i);
        int nIndex = m_aTypes.Index(sMime);
        if ( nIndex == wxNOT_FOUND)
        {
            // error if we get here ??
            return false;
        }
        else
        {
            WriteMimeInfo(nIndex, true);
            m_aTypes.RemoveAt(nIndex);
            m_aEntries.RemoveAt(nIndex);
            m_aExtensions.RemoveAt(nIndex);
            m_aDescriptions.RemoveAt(nIndex);
            m_aIcons.RemoveAt(nIndex);
        }
    }
    // check data integrity
    wxASSERT( m_aTypes.Count() == m_aEntries.Count() &&
            m_aTypes.Count() == m_aExtensions.Count() &&
            m_aTypes.Count() == m_aIcons.Count() &&
            m_aTypes.Count() == m_aDescriptions.Count() );

    return true;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static bool IsKnownUnimportantField(const wxString& fieldAll)
{
    static const wxChar *knownFields[] =
    {
        wxT("x-mozilla-flags"),
        wxT("nametemplate"),
        wxT("textualnewlines"),
    };

    wxString field = fieldAll.BeforeFirst(wxT('='));
    for ( size_t n = 0; n < WXSIZEOF(knownFields); n++ )
    {
        if ( field.CmpNoCase(knownFields[n]) == 0 )
            return true;
    }

    return false;
}

#endif
  // wxUSE_MIMETYPE && wxUSE_FILE && wxUSE_TEXTFILE
