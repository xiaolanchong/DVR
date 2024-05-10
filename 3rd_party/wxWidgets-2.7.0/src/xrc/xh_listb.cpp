/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_listb.cpp
// Purpose:     XRC resource for wxListBox
// Author:      Bob Mitchell & Vaclav Slavik
// Created:     2000/07/29
// RCS-ID:      $Id: xh_listb.cpp,v 1.15 2006/06/01 08:23:42 ABX Exp $
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_LISTBOX

#include "wx/xrc/xh_listb.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/listbox.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxListBoxXmlHandler, wxXmlResourceHandler)

wxListBoxXmlHandler::wxListBoxXmlHandler()
: wxXmlResourceHandler() , m_insideBox(false)
{
    XRC_ADD_STYLE(wxLB_SINGLE);
    XRC_ADD_STYLE(wxLB_MULTIPLE);
    XRC_ADD_STYLE(wxLB_EXTENDED);
    XRC_ADD_STYLE(wxLB_HSCROLL);
    XRC_ADD_STYLE(wxLB_ALWAYS_SB);
    XRC_ADD_STYLE(wxLB_NEEDED_SB);
    XRC_ADD_STYLE(wxLB_SORT);
    AddWindowStyles();
}

wxObject *wxListBoxXmlHandler::DoCreateResource()
{
    if( m_class == wxT("wxListBox"))
    {
        // find the selection
        long selection = GetLong(wxT("selection"), -1);

        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately(NULL, GetParamNode(wxT("content")));
        wxString *strings = (wxString *) NULL;
        if (strList.GetCount() > 0)
        {
            strings = new wxString[strList.GetCount()];
            int count = strList.GetCount();
            for (int i = 0; i < count; i++)
                strings[i]=strList[i];
        }

        XRC_MAKE_INSTANCE(control, wxListBox)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetPosition(), GetSize(),
                        strList.GetCount(),
                        strings,
                        GetStyle(),
                        wxDefaultValidator,
                        GetName());

        if (selection != -1)
            control->SetSelection(selection);

        SetupWindow(control);

        if (strings != NULL)
            delete[] strings;
        strList.Clear();    // dump the strings

        return control;
    }
    else
    {
        // on the inside now.
        // handle <item>Label</item>

        // add to the list
        wxString str = GetNodeContent(m_node);
        if (m_resource->GetFlags() & wxXRC_USE_LOCALE)
            str = wxGetTranslation(str);
        strList.Add(str);

        return NULL;
    }
}

bool wxListBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, wxT("wxListBox")) ||
           (m_insideBox && node->GetName() == wxT("item")));
}

#endif // wxUSE_XRC && wxUSE_LISTBOX
