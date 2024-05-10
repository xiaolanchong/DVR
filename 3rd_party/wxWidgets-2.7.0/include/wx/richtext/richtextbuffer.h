/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextbuffer.h
// Purpose:     Buffer for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id: richtextbuffer.h,v 1.14 2006/07/03 21:13:14 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RICHTEXTBUFFER_H_
#define _WX_RICHTEXTBUFFER_H_

/*

  Data structures
  ===============

  Data is represented by a hierarchy of objects, all derived from
  wxRichTextObject.

  The top of the hierarchy is the buffer, a kind of wxRichTextParagraphLayoutBox.
  These boxes will allow flexible placement of text boxes on a page, but
  for now there will be a single box representing the document,
  and this box will a wxRichTextParagraphLayoutBox which contains further
  wxRichTextParagraph objects, each of which can include text and images.

  Each object maintains a range (start and end position) measured
  from the start of the main parent box.
  A paragraph object knows its range, and a text fragment knows its range
  too. So, a character or image in a page has a position relative to the
  start of the document, and a character in an embedded text box has
  a position relative to that text box. For now, we will not be dealing with
  embedded objects but it's something to bear in mind for later.

  Layout
  ======

  When Layout is called on an object, it is given a size which the object
  must limit itself to, or one or more flexible directions (vertical
  or horizontal). So for example a centered paragraph is given the page
  width to play with (minus any margins), but can extend indefinitely
  in the vertical direction. The implementation of Layout can then
  cache the calculated size and position within the parent.

  Note that position and size should be calculated separately, because
  for example inserting a paragraph may result in the following paragraphs
  moving down, but not changing in size.

  Need to determine how objects specify their position. Absolute coordinates,
  or relative to last object? May be hard to determine that. So should probably
  be in absolute coordinates, in which case we'll need a Move virtual function
  that allows quick movement of all elements without layout.

  Let's work through a simple example of layout. Say we're laying out
  a document with the buffer as the top box, with a wxRichTextParagraphLayoutBox
  inside that that consists of wxRichTextParagraph objects.

  We're in a mode whereby changes of window size change the width of the
  page (useful for text editors, as opposed to word processors). The
  window width is 600.

  We pass (600, -1) to the top-level Layout (i.e. restrict size in horizontal
  direction only). The wxRichTextBuffer box doesn't currently have
  well-defined layout behaviour so we simply assume it has one child
  that fills its parent (later we can define sizer-like box layout behaviour).
  So it passes the same dimensions to the child, which is a wxRichTextParagraphLayoutBox.
  This then looks at each child in turn (wxRichTextParagraph) and determines
  the size the paragraph will take up, setting the cached size, and
  splitting the paragraph into lines.

  When the layout for one paragraph returns, the next paragraph is
  fed the position of the previous, so it can position itself.

  Each time Layout is called, the cached list of lines for each paragraph
  is recreated, since it can change for example if the parent object width
  changes.

  Reporting size
  ==============

  Each object can report its size for a given range. It's important that
  it can report a partial size, so that wrapping can be implemented,
  hit test calculations performed, etc. So GetRangeSize must be implemented
  for each object.

 */

/*!
 * Includes
 */

#include "wx/defs.h"

#if wxUSE_RICHTEXT

#include "wx/list.h"
#include "wx/textctrl.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/cmdproc.h"
#include "wx/txtstrm.h"

/*!
 * File types
 */

#define wxRICHTEXT_TYPE_ANY             0
#define wxRICHTEXT_TYPE_TEXT            1
#define wxRICHTEXT_TYPE_XML             2
#define wxRICHTEXT_TYPE_HTML            3
#define wxRICHTEXT_TYPE_RTF             4
#define wxRICHTEXT_TYPE_PDF             5

/*!
 * Forward declarations
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCtrl;
class WXDLLIMPEXP_RICHTEXT wxRichTextObject;
class WXDLLIMPEXP_RICHTEXT wxRichTextCacheObject;
class WXDLLIMPEXP_RICHTEXT wxRichTextObjectList;
class WXDLLIMPEXP_RICHTEXT wxRichTextLine;
class WXDLLIMPEXP_RICHTEXT wxRichTextParagraph;
class WXDLLIMPEXP_RICHTEXT wxRichTextFragment;
class WXDLLIMPEXP_RICHTEXT wxRichTextFileHandler;
class WXDLLIMPEXP_RICHTEXT wxRichTextStyleSheet;
class WXDLLIMPEXP_RICHTEXT wxTextAttrEx;

/*!
 * Flags determining the available space, passed to Layout
 */

#define wxRICHTEXT_FIXED_WIDTH      0x01
#define wxRICHTEXT_FIXED_HEIGHT     0x02
#define wxRICHTEXT_VARIABLE_WIDTH   0x04
#define wxRICHTEXT_VARIABLE_HEIGHT  0x08

// Only lay out the part of the buffer that lies within
// the rect passed to Layout.
#define wxRICHTEXT_LAYOUT_SPECIFIED_RECT 0x10

/*!
 * Flags returned from hit-testing
 */

// The point was not on this object
#define wxRICHTEXT_HITTEST_NONE     0x01
// The point was before the position returned from HitTest
#define wxRICHTEXT_HITTEST_BEFORE   0x02
// The point was after the position returned from HitTest
#define wxRICHTEXT_HITTEST_AFTER    0x04
// The point was on the position returned from HitTest
#define wxRICHTEXT_HITTEST_ON       0x08

/*!
 * Flags for GetRangeSize
 */

#define wxRICHTEXT_FORMATTED        0x01
#define wxRICHTEXT_UNFORMATTED      0x02

/*!
 * Extra formatting flags not in wxTextAttr
 */

#define wxTEXT_ATTR_PARA_SPACING_AFTER      0x00000800
#define wxTEXT_ATTR_PARA_SPACING_BEFORE     0x00001000
#define wxTEXT_ATTR_LINE_SPACING            0x00002000
#define wxTEXT_ATTR_CHARACTER_STYLE_NAME    0x00004000
#define wxTEXT_ATTR_PARAGRAPH_STYLE_NAME    0x00008000
#define wxTEXT_ATTR_BULLET_STYLE            0x00010000
#define wxTEXT_ATTR_BULLET_NUMBER           0x00020000
#define wxTEXT_ATTR_BULLET_SYMBOL           0x00040000

/*!
 * Styles for wxTextAttrEx::SetBulletStyle
 */

#define wxTEXT_ATTR_BULLET_STYLE_NONE           0x0000
#define wxTEXT_ATTR_BULLET_STYLE_ARABIC         0x0001
#define wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER  0x0002
#define wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER  0x0004
#define wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER    0x0008
#define wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER    0x0010
#define wxTEXT_ATTR_BULLET_STYLE_SYMBOL         0x0020
#define wxTEXT_ATTR_BULLET_STYLE_BITMAP         0x0040
#define wxTEXT_ATTR_BULLET_STYLE_PARENTHESES    0x0080
#define wxTEXT_ATTR_BULLET_STYLE_PERIOD         0x0100

/*!
 * Line spacing values
 */

#define wxTEXT_ATTR_LINE_SPACING_NORMAL         10
#define wxTEXT_ATTR_LINE_SPACING_HALF           15
#define wxTEXT_ATTR_LINE_SPACING_TWICE          20

/*!
 * wxRichTextRange class declaration
 * This stores beginning and end positions for a range of data.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextRange
{
public:
// Constructors

    wxRichTextRange() { m_start = 0; m_end = 0; }
    wxRichTextRange(long start, long end) { m_start = start; m_end = end; }
    wxRichTextRange(const wxRichTextRange& range) { m_start = range.m_start; m_end = range.m_end; }
    ~wxRichTextRange() {}

    void operator =(const wxRichTextRange& range) { m_start = range.m_start; m_end = range.m_end; }
    bool operator ==(const wxRichTextRange& range) const { return (m_start == range.m_start && m_end == range.m_end); }
    wxRichTextRange operator -(const wxRichTextRange& range) const { return wxRichTextRange(m_start - range.m_start, m_end - range.m_end); }
    wxRichTextRange operator +(const wxRichTextRange& range) const { return wxRichTextRange(m_start + range.m_start, m_end + range.m_end); }

    void SetRange(long start, long end) { m_start = start; m_end = end; }

    void SetStart(long start) { m_start = start; }
    long GetStart() const { return m_start; }

    void SetEnd(long end) { m_end = end; }
    long GetEnd() const { return m_end; }

    /// Returns true if this range is completely outside 'range'
    bool IsOutside(const wxRichTextRange& range) const { return range.m_start > m_end || range.m_end < m_start; }

    /// Returns true if this range is completely within 'range'
    bool IsWithin(const wxRichTextRange& range) const { return m_start >= range.m_start && m_end <= range.m_end; }

    /// Returns true if the given position is within this range. Allow
    /// for the possibility of an empty range - assume the position
    /// is within this empty range. NO, I think we should not match with an empty range.
    // bool Contains(long pos) const { return pos >= m_start && (pos <= m_end || GetLength() == 0); }
    bool Contains(long pos) const { return pos >= m_start && pos <= m_end ; }

    /// Limit this range to be within 'range'
    bool LimitTo(const wxRichTextRange& range) ;

    /// Gets the length of the range
    long GetLength() const { return m_end - m_start + 1; }

    /// Swaps the start and end
    void Swap() { long tmp = m_start; m_start = m_end; m_end = tmp; }

protected:
    long m_start;
    long m_end;
};

#define wxRICHTEXT_ALL  wxRichTextRange(-2, -2)
#define wxRICHTEXT_NONE  wxRichTextRange(-1, -1)

/*!
 * wxTextAttrEx is an extended version of wxTextAttr with more paragraph attributes.
 */

class WXDLLIMPEXP_RICHTEXT wxTextAttrEx: public wxTextAttr
{
public:
    // ctors
    wxTextAttrEx(const wxTextAttrEx& attr);
    wxTextAttrEx(const wxTextAttr& attr) { Init(); (*this) = attr; }
    wxTextAttrEx() { Init(); }

    // Initialise this object.
    void Init();

    // Assignment from a wxTextAttrEx object
    void operator= (const wxTextAttrEx& attr);

    // Assignment from a wxTextAttr object.
    void operator= (const wxTextAttr& attr);

    // setters
    void SetCharacterStyleName(const wxString& name) { m_characterStyleName = name; }
    void SetParagraphStyleName(const wxString& name) { m_paragraphStyleName = name; }
    void SetParagraphSpacingAfter(int spacing) { m_paragraphSpacingAfter = spacing; }
    void SetParagraphSpacingBefore(int spacing) { m_paragraphSpacingBefore = spacing; }
    void SetLineSpacing(int spacing) { m_lineSpacing = spacing; }
    void SetBulletStyle(int style) { m_bulletStyle = style; }
    void SetBulletNumber(int n) { m_bulletNumber = n; }
    void SetBulletSymbol(wxChar symbol) { m_bulletSymbol = symbol; }

    const wxString& GetCharacterStyleName() const { return m_characterStyleName; }
    const wxString& GetParagraphStyleName() const { return m_paragraphStyleName; }
    int GetParagraphSpacingAfter() const { return m_paragraphSpacingAfter; }
    int GetParagraphSpacingBefore() const { return m_paragraphSpacingBefore; }
    int GetLineSpacing() const { return m_lineSpacing; }
    int GetBulletStyle() const { return m_bulletStyle; }
    int GetBulletNumber() const { return m_bulletNumber; }
    wxChar GetBulletSymbol() const { return m_bulletSymbol; }

    bool HasWeight() const { return (GetFlags() & wxTEXT_ATTR_FONT_WEIGHT) != 0; }
    bool HasSize() const { return (GetFlags() & wxTEXT_ATTR_FONT_SIZE) != 0; }
    bool HasItalic() const { return (GetFlags() & wxTEXT_ATTR_FONT_ITALIC) != 0; }
    bool HasUnderlined() const { return (GetFlags() & wxTEXT_ATTR_FONT_UNDERLINE) != 0; }
    bool HasFaceName() const { return (GetFlags() & wxTEXT_ATTR_FONT_FACE) != 0; }

    bool HasParagraphSpacingAfter() const { return HasFlag(wxTEXT_ATTR_PARA_SPACING_AFTER); }
    bool HasParagraphSpacingBefore() const { return HasFlag(wxTEXT_ATTR_PARA_SPACING_BEFORE); }
    bool HasLineSpacing() const { return HasFlag(wxTEXT_ATTR_LINE_SPACING); }
    bool HasCharacterStyleName() const { return HasFlag(wxTEXT_ATTR_CHARACTER_STYLE_NAME); }
    bool HasParagraphStyleName() const { return HasFlag(wxTEXT_ATTR_PARAGRAPH_STYLE_NAME); }
    bool HasBulletStyle() const { return HasFlag(wxTEXT_ATTR_BULLET_STYLE); }
    bool HasBulletNumber() const { return HasFlag(wxTEXT_ATTR_BULLET_NUMBER); }
    bool HasBulletSymbol() const { return HasFlag(wxTEXT_ATTR_BULLET_SYMBOL); }

    // Is this a character style?
    bool IsCharacterStyle() const { return (0 != (GetFlags() & (wxTEXT_ATTR_FONT | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_TEXT_COLOUR))); }
    bool IsParagraphStyle() const { return (0 != (GetFlags() & (wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|
                            wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|
                            wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER))); }

    // returns false if we have any attributes set, true otherwise
    bool IsDefault() const
    {
        return !HasTextColour() && !HasBackgroundColour() && !HasFont() && !HasAlignment() &&
               !HasTabs() && !HasLeftIndent() && !HasRightIndent() &&
               !HasParagraphSpacingAfter() && !HasParagraphSpacingBefore() && !HasLineSpacing() &&
               !HasCharacterStyleName() && !HasParagraphStyleName() && !HasBulletNumber() && !HasBulletStyle() && !HasBulletSymbol();
    }

    // return the attribute having the valid font and colours: it uses the
    // attributes set in attr and falls back first to attrDefault and then to
    // the text control font/colours for those attributes which are not set
    static wxTextAttrEx CombineEx(const wxTextAttrEx& attr,
                              const wxTextAttrEx& attrDef,
                              const wxTextCtrlBase *text);

private:
    // Paragraph styles
    int                 m_paragraphSpacingAfter;
    int                 m_paragraphSpacingBefore;
    int                 m_lineSpacing;
    int                 m_bulletStyle;
    int                 m_bulletNumber;
    wxChar              m_bulletSymbol;

    // Character style
    wxString            m_characterStyleName;

    // Paragraph style
    wxString            m_paragraphStyleName;
};

/*!
 * wxRichTextAttr stores attributes without a wxFont object, so is a much more
 * efficient way to query styles.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextAttr
{
public:
    // ctors
    wxRichTextAttr(const wxTextAttrEx& attr);
    wxRichTextAttr() { Init(); }
    wxRichTextAttr(const wxColour& colText,
               const wxColour& colBack = wxNullColour,
               wxTextAttrAlignment alignment = wxTEXT_ALIGNMENT_DEFAULT);

    // Initialise this object.
    void Init();

    // Assignment from a wxRichTextAttr object.
    void operator= (const wxRichTextAttr& attr);

    // Assignment from a wxTextAttrEx object.
    void operator= (const wxTextAttrEx& attr);

    // Making a wxTextAttrEx object.
    operator wxTextAttrEx () const ;

    // Copy to a wxTextAttr
    void CopyTo(wxTextAttrEx& attr) const;

    // Create font from font attributes.
    wxFont CreateFont() const;

    // Get attributes from font.
    bool GetFontAttributes(const wxFont& font);

    // setters
    void SetTextColour(const wxColour& colText) { m_colText = colText; m_flags |= wxTEXT_ATTR_TEXT_COLOUR; }
    void SetBackgroundColour(const wxColour& colBack) { m_colBack = colBack; m_flags |= wxTEXT_ATTR_BACKGROUND_COLOUR; }
    void SetAlignment(wxTextAttrAlignment alignment) { m_textAlignment = alignment; m_flags |= wxTEXT_ATTR_ALIGNMENT; }
    void SetTabs(const wxArrayInt& tabs) { m_tabs = tabs; m_flags |= wxTEXT_ATTR_TABS; }
    void SetLeftIndent(int indent, int subIndent = 0) { m_leftIndent = indent; m_leftSubIndent = subIndent; m_flags |= wxTEXT_ATTR_LEFT_INDENT; }
    void SetRightIndent(int indent) { m_rightIndent = indent; m_flags |= wxTEXT_ATTR_RIGHT_INDENT; }

    void SetFontSize(int pointSize) { m_fontSize = pointSize; m_flags |= wxTEXT_ATTR_FONT_SIZE; }
    void SetFontStyle(int fontStyle) { m_fontStyle = fontStyle; m_flags |= wxTEXT_ATTR_FONT_ITALIC; }
    void SetFontWeight(int fontWeight) { m_fontWeight = fontWeight; m_flags |= wxTEXT_ATTR_FONT_WEIGHT; }
    void SetFontFaceName(const wxString& faceName) { m_fontFaceName = faceName; m_flags |= wxTEXT_ATTR_FONT_FACE; }
    void SetFontUnderlined(bool underlined) { m_fontUnderlined = underlined; m_flags |= wxTEXT_ATTR_FONT_UNDERLINE; }

    void SetFlags(long flags) { m_flags = flags; }

    void SetCharacterStyleName(const wxString& name) { m_characterStyleName = name; }
    void SetParagraphStyleName(const wxString& name) { m_paragraphStyleName = name; }
    void SetParagraphSpacingAfter(int spacing) { m_paragraphSpacingAfter = spacing; }
    void SetParagraphSpacingBefore(int spacing) { m_paragraphSpacingBefore = spacing; }
    void SetLineSpacing(int spacing) { m_lineSpacing = spacing; }
    void SetBulletStyle(int style) { m_bulletStyle = style; }
    void SetBulletNumber(int n) { m_bulletNumber = n; }
    void SetBulletSymbol(wxChar symbol) { m_bulletSymbol = symbol; }

    const wxColour& GetTextColour() const { return m_colText; }
    const wxColour& GetBackgroundColour() const { return m_colBack; }
    wxTextAttrAlignment GetAlignment() const { return m_textAlignment; }
    const wxArrayInt& GetTabs() const { return m_tabs; }
    long GetLeftIndent() const { return m_leftIndent; }
    long GetLeftSubIndent() const { return m_leftSubIndent; }
    long GetRightIndent() const { return m_rightIndent; }
    long GetFlags() const { return m_flags; }

    int GetFontSize() const { return m_fontSize; }
    int GetFontStyle() const { return m_fontStyle; }
    int GetFontWeight() const { return m_fontWeight; }
    bool GetFontUnderlined() const { return m_fontUnderlined; }
    const wxString& GetFontFaceName() const { return m_fontFaceName; }

    const wxString& GetCharacterStyleName() const { return m_characterStyleName; }
    const wxString& GetParagraphStyleName() const { return m_paragraphStyleName; }
    int GetParagraphSpacingAfter() const { return m_paragraphSpacingAfter; }
    int GetParagraphSpacingBefore() const { return m_paragraphSpacingBefore; }
    int GetLineSpacing() const { return m_lineSpacing; }
    int GetBulletStyle() const { return m_bulletStyle; }
    int GetBulletNumber() const { return m_bulletNumber; }
    wxChar GetBulletSymbol() const { return m_bulletSymbol; }

    // accessors
    bool HasTextColour() const { return m_colText.Ok() && HasFlag(wxTEXT_ATTR_TEXT_COLOUR) ; }
    bool HasBackgroundColour() const { return m_colBack.Ok() && HasFlag(wxTEXT_ATTR_BACKGROUND_COLOUR) ; }
    bool HasAlignment() const { return (m_textAlignment != wxTEXT_ALIGNMENT_DEFAULT) || ((m_flags & wxTEXT_ATTR_ALIGNMENT) != 0) ; }
    bool HasTabs() const { return (m_flags & wxTEXT_ATTR_TABS) != 0 ; }
    bool HasLeftIndent() const { return (m_flags & wxTEXT_ATTR_LEFT_INDENT) != 0 ; }
    bool HasRightIndent() const { return (m_flags & wxTEXT_ATTR_RIGHT_INDENT) != 0 ; }
    bool HasWeight() const { return (m_flags & wxTEXT_ATTR_FONT_WEIGHT) != 0; }
    bool HasSize() const { return (m_flags & wxTEXT_ATTR_FONT_SIZE) != 0; }
    bool HasItalic() const { return (m_flags & wxTEXT_ATTR_FONT_ITALIC) != 0; }
    bool HasUnderlined() const { return (m_flags & wxTEXT_ATTR_FONT_UNDERLINE) != 0; }
    bool HasFaceName() const { return (m_flags & wxTEXT_ATTR_FONT_FACE) != 0; }
    bool HasFont() const { return (m_flags & (wxTEXT_ATTR_FONT)) != 0; }

    bool HasParagraphSpacingAfter() const { return (m_flags & wxTEXT_ATTR_PARA_SPACING_AFTER) != 0; }
    bool HasParagraphSpacingBefore() const { return (m_flags & wxTEXT_ATTR_PARA_SPACING_BEFORE) != 0; }
    bool HasLineSpacing() const { return (m_flags & wxTEXT_ATTR_LINE_SPACING) != 0; }
    bool HasCharacterStyleName() const { return (m_flags & wxTEXT_ATTR_CHARACTER_STYLE_NAME) != 0; }
    bool HasParagraphStyleName() const { return (m_flags & wxTEXT_ATTR_PARAGRAPH_STYLE_NAME) != 0; }
    bool HasBulletStyle() const { return (m_flags & wxTEXT_ATTR_BULLET_STYLE) != 0; }
    bool HasBulletNumber() const { return (m_flags & wxTEXT_ATTR_BULLET_NUMBER) != 0; }
    bool HasBulletSymbol() const { return (m_flags & wxTEXT_ATTR_BULLET_SYMBOL) != 0; }

    bool HasFlag(long flag) const { return (m_flags & flag) != 0; }

    // Is this a character style?
    bool IsCharacterStyle() const { return (0 != (GetFlags() & (wxTEXT_ATTR_FONT | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_TEXT_COLOUR))); }
    bool IsParagraphStyle() const { return (0 != (GetFlags() & (wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|
                            wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|
                            wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER))); }

    // returns false if we have any attributes set, true otherwise
    bool IsDefault() const
    {
        return !HasTextColour() && !HasBackgroundColour() && !HasFont() && !HasAlignment() &&
               !HasTabs() && !HasLeftIndent() && !HasRightIndent() &&
               !HasParagraphSpacingAfter() && !HasParagraphSpacingBefore() && !HasLineSpacing() &&
               !HasCharacterStyleName() && !HasParagraphStyleName() && !HasBulletNumber() && !HasBulletStyle() && !HasBulletSymbol();
    }

    // return the attribute having the valid font and colours: it uses the
    // attributes set in attr and falls back first to attrDefault and then to
    // the text control font/colours for those attributes which are not set
    static wxRichTextAttr Combine(const wxRichTextAttr& attr,
                              const wxRichTextAttr& attrDef,
                              const wxTextCtrlBase *text);
private:
    long                m_flags;

    // Paragraph styles
    wxArrayInt          m_tabs; // array of int: tab stops in 1/10 mm
    int                 m_leftIndent; // left indent in 1/10 mm
    int                 m_leftSubIndent; // left indent for all but the first
                                         // line in a paragraph relative to the
                                         // first line, in 1/10 mm
    int                 m_rightIndent; // right indent in 1/10 mm
    wxTextAttrAlignment m_textAlignment;

    int                 m_paragraphSpacingAfter;
    int                 m_paragraphSpacingBefore;
    int                 m_lineSpacing;
    int                 m_bulletStyle;
    int                 m_bulletNumber;
    wxChar              m_bulletSymbol;

    // Character styles
    wxColour            m_colText,
                        m_colBack;
    int                 m_fontSize;
    int                 m_fontStyle;
    int                 m_fontWeight;
    bool                m_fontUnderlined;
    wxString            m_fontFaceName;

    // Character style
    wxString            m_characterStyleName;

    // Paragraph style
    wxString            m_paragraphStyleName;
};

#define wxTEXT_ATTR_CHARACTER (wxTEXT_ATTR_FONT | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_TEXT_COLOUR)

#define wxTEXT_ATTR_PARAGRAPH (wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|\
    wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|\
    wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_BULLET_SYMBOL)

#define wxTEXT_ATTR_ALL (wxTEXT_ATTR_CHARACTER|wxTEXT_ATTR_PARAGRAPH)

/*!
 * wxRichTextObject class declaration
 * This is the base for drawable objects.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextObject: public wxObject
{
    DECLARE_CLASS(wxRichTextObject)
public:
// Constructors

    wxRichTextObject(wxRichTextObject* parent = NULL);
    ~wxRichTextObject();

// Overrideables

    /// Draw the item, within the given range. Some objects may ignore the range (for
    /// example paragraphs) while others must obey it (lines, to implement wrapping)
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style) = 0;

    /// Lay the item out at the specified position with the given size constraint.
    /// Layout must set the cached size.
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style) = 0;

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& WXUNUSED(dc), const wxPoint& WXUNUSED(pt), long& WXUNUSED(textPosition)) { return false; }

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& WXUNUSED(dc), long WXUNUSED(index), wxPoint& WXUNUSED(pt), int* WXUNUSED(height), bool WXUNUSED(forceLineStart)) { return false; }

    /// Get the best size, i.e. the ideal starting size for this object irrespective
    /// of available space. For a short text string, it will be the size that exactly encloses
    /// the text. For a longer string, it might use the parent width for example.
    virtual wxSize GetBestSize() const { return m_size; }

    /// Get the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0)) const  = 0;

    /// Do a split, returning an object containing the second part, and setting
    /// the first part in 'this'.
    virtual wxRichTextObject* DoSplit(long WXUNUSED(pos)) { return NULL; }

    /// Calculate range. By default, guess that the object is 1 unit long.
    virtual void CalculateRange(long start, long& end) { end = start ; m_range.SetRange(start, end); }

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& WXUNUSED(range)) { return false; }

    /// Returns true if the object is empty
    virtual bool IsEmpty() const { return false; }

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& WXUNUSED(range)) const { return wxEmptyString; }

    /// Returns true if this object can merge itself with the given one.
    virtual bool CanMerge(wxRichTextObject* WXUNUSED(object)) const { return false; }

    /// Returns true if this object merged itself with the given one.
    /// The calling code will then delete the given object.
    virtual bool Merge(wxRichTextObject* WXUNUSED(object)) { return false; }

    /// Dump to output stream for debugging
    virtual void Dump(wxTextOutputStream& stream);

// Accessors

    /// Get/set the cached object size as calculated by Layout.
    virtual wxSize GetCachedSize() const { return m_size; }
    virtual void SetCachedSize(const wxSize& sz) { m_size = sz; }

    /// Get/set the object position
    virtual wxPoint GetPosition() const { return m_pos; }
    virtual void SetPosition(const wxPoint& pos) { m_pos = pos; }

    /// Get the rectangle enclosing the object
    virtual wxRect GetRect() const { return wxRect(GetPosition(), GetCachedSize()); }

    /// Set the range
    void SetRange(const wxRichTextRange& range) { m_range = range; }

    /// Get the range
    const wxRichTextRange& GetRange() const { return m_range; }
    wxRichTextRange& GetRange() { return m_range; }

    /// Get/set dirty flag (whether the object needs Layout to be called)
    virtual bool GetDirty() const { return m_dirty; }
    virtual void SetDirty(bool dirty) { m_dirty = dirty; }

    /// Is this composite?
    virtual bool IsComposite() const { return false; }

    /// Get/set the parent.
    virtual wxRichTextObject* GetParent() const { return m_parent; }
    virtual void SetParent(wxRichTextObject* parent) { m_parent = parent; }

    /// Set the margin around the object
    virtual void SetMargins(int margin);
    virtual void SetMargins(int leftMargin, int rightMargin, int topMargin, int bottomMargin);
    virtual int GetLeftMargin() const { return m_leftMargin; }
    virtual int GetRightMargin() const { return m_rightMargin; }
    virtual int GetTopMargin() const { return m_topMargin; }
    virtual int GetBottomMargin() const { return m_bottomMargin; }

    /// Set attributes object
    void SetAttributes(const wxTextAttrEx& attr) { m_attributes = attr; }
    const wxTextAttrEx& GetAttributes() const { return m_attributes; }
    wxTextAttrEx& GetAttributes() { return m_attributes; }

    /// Set/get stored descent
    void SetDescent(int descent) { m_descent = descent; }
    int GetDescent() const { return m_descent; }

// Operations

    /// Clone the object
    virtual wxRichTextObject* Clone() const { return NULL; }

    /// Copy
    void Copy(const wxRichTextObject& obj);

    /// Reference-counting allows us to use the same object in multiple
    /// lists (not yet used)
    void Reference() { m_refCount ++; }
    void Dereference();

    /// Convert units in tends of a millimetre to device units
    int ConvertTenthsMMToPixels(wxDC& dc, int units);

protected:
    wxSize                  m_size;
    wxPoint                 m_pos;
    int                     m_descent; // Descent for this object (if any)
    bool                    m_dirty;
    int                     m_refCount;
    wxRichTextObject*       m_parent;

    /// The range of this object (start position to end position)
    wxRichTextRange         m_range;

    /// Margins
    int                     m_leftMargin;
    int                     m_rightMargin;
    int                     m_topMargin;
    int                     m_bottomMargin;

    /// Attributes
    wxTextAttrEx            m_attributes;
};

WX_DECLARE_LIST_WITH_DECL( wxRichTextObject, wxRichTextObjectList, class WXDLLIMPEXP_RICHTEXT );

/*!
 * wxRichTextCompositeObject class declaration
 * Objects of this class can contain other objects.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCompositeObject: public wxRichTextObject
{
    DECLARE_CLASS(wxRichTextCompositeObject)
public:
// Constructors

    wxRichTextCompositeObject(wxRichTextObject* parent = NULL);
    ~wxRichTextCompositeObject();

// Overrideables

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition);

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

    /// Dump to output stream for debugging
    virtual void Dump(wxTextOutputStream& stream);

// Accessors

    /// Get the children
    wxRichTextObjectList& GetChildren() { return m_children; }
    const wxRichTextObjectList& GetChildren() const { return m_children; }

    /// Get the child count
    size_t GetChildCount() const ;

    /// Get the nth child
    wxRichTextObject* GetChild(size_t n) const ;

    /// Get/set dirty flag
    virtual bool GetDirty() const { return m_dirty; }
    virtual void SetDirty(bool dirty) { m_dirty = dirty; }

    /// Is this composite?
    virtual bool IsComposite() const { return true; }

    /// Returns true if the buffer is empty
    virtual bool IsEmpty() const { return GetChildCount() == 0; }

// Operations

    /// Copy
    void Copy(const wxRichTextCompositeObject& obj);

    /// Append a child, returning the position
    size_t AppendChild(wxRichTextObject* child) ;

    /// Insert the child in front of the given object, or at the beginning
    bool InsertChild(wxRichTextObject* child, wxRichTextObject* inFrontOf) ;

    /// Delete the child
    bool RemoveChild(wxRichTextObject* child, bool deleteChild = false) ;

    /// Delete all children
    bool DeleteChildren() ;

    /// Recursively merge all pieces that can be merged.
    bool Defragment();

protected:
    wxRichTextObjectList    m_children;
};

/*!
 * wxRichTextBox class declaration
 * This defines a 2D space to lay out objects
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextBox: public wxRichTextCompositeObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextBox)
public:
// Constructors

    wxRichTextBox(wxRichTextObject* parent = NULL);
    wxRichTextBox(const wxRichTextBox& obj): wxRichTextCompositeObject() { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Get/set the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0)) const;

// Accessors

// Operations

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextBox(*this); }

    /// Copy
    void Copy(const wxRichTextBox& obj);

protected:
};

/*!
 * wxRichTextParagraphBox class declaration
 * This box knows how to lay out paragraphs.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextParagraphLayoutBox: public wxRichTextBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextParagraphLayoutBox)
public:
// Constructors

    wxRichTextParagraphLayoutBox(wxRichTextObject* parent = NULL);
    wxRichTextParagraphLayoutBox(const wxRichTextParagraphLayoutBox& obj):wxRichTextBox() { Init(); Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Get/set the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0)) const;

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

// Accessors

    /// Associate a control with the buffer, for operations that for example require refreshing the window.
    void SetRichTextCtrl(wxRichTextCtrl* ctrl) { m_ctrl = ctrl; }

    /// Get the associated control.
    wxRichTextCtrl* GetRichTextCtrl() const { return m_ctrl; }

// Operations

    /// Initialize the object.
    void Init();

    /// Clear all children
    virtual void Clear();

    /// Clear and initialize with one blank paragraph
    virtual void Reset();

    /// Convenience function to add a paragraph of text
    virtual wxRichTextRange AddParagraph(const wxString& text);

    /// Convenience function to add an image
    virtual wxRichTextRange AddImage(const wxImage& image);

    /// Adds multiple paragraphs, based on newlines.
    virtual wxRichTextRange AddParagraphs(const wxString& text);

    /// Get the line at the given position. If caretPosition is true, the position is
    /// a caret position, which is normally a smaller number.
    virtual wxRichTextLine* GetLineAtPosition(long pos, bool caretPosition = false) const;

    /// Get the line at the given y pixel position, or the last line.
    virtual wxRichTextLine* GetLineAtYPosition(int y) const;

    /// Get the paragraph at the given character or caret position
    virtual wxRichTextParagraph* GetParagraphAtPosition(long pos, bool caretPosition = false) const;

    /// Get the line size at the given position
    virtual wxSize GetLineSizeAtPosition(long pos, bool caretPosition = false) const;

    /// Given a position, get the number of the visible line (potentially many to a paragraph),
    /// starting from zero at the start of the buffer. We also have to pass a bool (startOfLine)
    /// that indicates whether the caret is being shown at the end of the previous line or at the start
    /// of the next, since the caret can be shown at 2 visible positions for the same underlying
    /// position.
    virtual long GetVisibleLineNumber(long pos, bool caretPosition = false, bool startOfLine = false) const;

    /// Given a line number, get the corresponding wxRichTextLine object.
    virtual wxRichTextLine* GetLineForVisibleLineNumber(long lineNumber) const;

    /// Get the leaf object in a paragraph at this position.
    /// Given a line number, get the corresponding wxRichTextLine object.
    virtual wxRichTextObject* GetLeafObjectAtPosition(long position) const;

    /// Get the paragraph by number
    virtual wxRichTextParagraph* GetParagraphAtLine(long paragraphNumber) const;

    /// Get the paragraph for a given line
    virtual wxRichTextParagraph* GetParagraphForLine(wxRichTextLine* line) const;

    /// Get the length of the paragraph
    virtual int GetParagraphLength(long paragraphNumber) const;

    /// Get the number of paragraphs
    virtual int GetParagraphCount() const { return GetChildCount(); }

    /// Get the number of visible lines
    virtual int GetLineCount() const;

    /// Get the text of the paragraph
    virtual wxString GetParagraphText(long paragraphNumber) const;

    /// Convert zero-based line column and paragraph number to a position.
    virtual long XYToPosition(long x, long y) const;

    /// Convert zero-based position to line column and paragraph number
    virtual bool PositionToXY(long pos, long* x, long* y) const;

    /// Set text attributes: character and/or paragraph styles.
    virtual bool SetStyle(const wxRichTextRange& range, const wxRichTextAttr& style, bool withUndo = true);
    virtual bool SetStyle(const wxRichTextRange& range, const wxTextAttrEx& style, bool withUndo = true);

    /// Get the text attributes for this position.
    virtual bool GetStyle(long position, wxTextAttrEx& style) const;
    virtual bool GetStyle(long position, wxRichTextAttr& style) const;

    /// Test if this whole range has character attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, bold button updating. style must have
    /// flags indicating which attributes are of interest.
    virtual bool HasCharacterAttributes(const wxRichTextRange& range, const wxTextAttrEx& style) const;
    virtual bool HasCharacterAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const;

    /// Test if this whole range has paragraph attributes of the specified kind. If any
    /// of the attributes are different within the range, the test fails. You
    /// can use this to implement, for example, centering button updating. style must have
    /// flags indicating which attributes are of interest.
    virtual bool HasParagraphAttributes(const wxRichTextRange& range, const wxTextAttrEx& style) const;
    virtual bool HasParagraphAttributes(const wxRichTextRange& range, const wxRichTextAttr& style) const;

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextParagraphLayoutBox(*this); }

    /// Insert fragment into this box at the given position. If partialParagraph is true,
    /// it is assumed that the last (or only) paragraph is just a piece of data with no paragraph
    /// marker.
    virtual bool InsertFragment(long position, wxRichTextFragment& fragment);

    /// Make a copy of the fragment corresponding to the given range, putting it in 'fragment'.
    virtual bool CopyFragment(const wxRichTextRange& range, wxRichTextFragment& fragment);

    /// Copy
    void Copy(const wxRichTextParagraphLayoutBox& obj);

    /// Calculate ranges
    virtual void UpdateRanges() { long end; CalculateRange(0, end); }

    /// Get all the text
    virtual wxString GetText() const;

    /// Set default style for new content. Setting it to a default attribute
    /// makes new content take on the 'basic' style.
    virtual bool SetDefaultStyle(const wxTextAttrEx& style);

    /// Get default style
    virtual const wxTextAttrEx& GetDefaultStyle() const { return m_defaultAttributes; }

    /// Set basic (overall) style
    virtual void SetBasicStyle(const wxTextAttrEx& style) { m_attributes = style; }
    virtual void SetBasicStyle(const wxRichTextAttr& style) { style.CopyTo(m_attributes); }

    /// Get basic (overall) style
    virtual const wxTextAttrEx& GetBasicStyle() const { return m_attributes; }

    /// Invalidate the buffer. With no argument, invalidates whole buffer.
    void Invalidate(const wxRichTextRange& invalidRange = wxRICHTEXT_ALL);

    /// Get invalid range, rounding to entire paragraphs if argument is true.
    wxRichTextRange GetInvalidRange(bool wholeParagraphs = false) const;

protected:
    wxRichTextCtrl* m_ctrl;
    wxTextAttrEx    m_defaultAttributes;

    /// The invalidated range that will need full layout
    wxRichTextRange         m_invalidRange;
};

/*!
 * wxRichTextFragment class declaration
 * This is a lind of paragraph layout box used for storing
 * paragraphs for Undo/Redo, for example.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextFragment: public wxRichTextParagraphLayoutBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextFragment)
public:
// Constructors

    wxRichTextFragment() { Init(); }
    wxRichTextFragment(const wxRichTextFragment& obj):wxRichTextParagraphLayoutBox() { Init(); Copy(obj); }

// Accessors

    /// Get/set whether the last paragraph is partial or complete
    void SetPartialParagraph(bool partialPara) { m_partialParagraph = partialPara; }
    bool GetPartialParagraph() const { return m_partialParagraph; }

// Overrideables

// Operations

    /// Initialise
    void Init();

    /// Copy
    void Copy(const wxRichTextFragment& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextFragment(*this); }

protected:

    // Is the last paragraph partial or complete?
    bool        m_partialParagraph;
};

/*!
 * wxRichTextLine class declaration
 * This object represents a line in a paragraph, and stores
 * offsets from the start of the paragraph representing the
 * start and end positions of the line.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextLine
{
public:
// Constructors

    wxRichTextLine(wxRichTextParagraph* parent);
    wxRichTextLine(const wxRichTextLine& obj) { Init( NULL); Copy(obj); }
    virtual ~wxRichTextLine() {}

// Overrideables

// Accessors

    /// Set the range
    void SetRange(const wxRichTextRange& range) { m_range = range; }
    void SetRange(long from, long to) { m_range = wxRichTextRange(from, to); }

    /// Get the parent paragraph
    wxRichTextParagraph* GetParent() { return m_parent; }

    /// Get the range
    const wxRichTextRange& GetRange() const { return m_range; }
    wxRichTextRange& GetRange() { return m_range; }

    /// Get the absolute range
    wxRichTextRange GetAbsoluteRange() const;

    /// Get/set the line size as calculated by Layout.
    virtual wxSize GetSize() const { return m_size; }
    virtual void SetSize(const wxSize& sz) { m_size = sz; }

    /// Get/set the object position relative to the parent
    virtual wxPoint GetPosition() const { return m_pos; }
    virtual void SetPosition(const wxPoint& pos) { m_pos = pos; }

    /// Get the absolute object position
    virtual wxPoint GetAbsolutePosition() const;

    /// Get the rectangle enclosing the line
    virtual wxRect GetRect() const { return wxRect(GetAbsolutePosition(), GetSize()); }

    /// Set/get stored descent
    void SetDescent(int descent) { m_descent = descent; }
    int GetDescent() const { return m_descent; }

// Operations

    /// Initialisation
    void Init(wxRichTextParagraph* parent);

    /// Copy
    void Copy(const wxRichTextLine& obj);

    /// Clone
    virtual wxRichTextLine* Clone() const { return new wxRichTextLine(*this); }

protected:

    /// The range of the line (start position to end position)
    /// This is relative to the parent paragraph.
    wxRichTextRange     m_range;

    /// Size and position measured relative to top of paragraph
    wxPoint             m_pos;
    wxSize              m_size;

    /// Maximum descent for this line (location of text baseline)
    int                 m_descent;

    // The parent object
    wxRichTextParagraph* m_parent;
};

WX_DECLARE_LIST_WITH_DECL( wxRichTextLine, wxRichTextLineList , class WXDLLIMPEXP_RICHTEXT );

/*!
 * wxRichTextParagraph class declaration
 * This object represents a single paragraph (or in a straight text editor, a line).
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextParagraph: public wxRichTextBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextParagraph)
public:
// Constructors

    wxRichTextParagraph(wxRichTextObject* parent = NULL, wxTextAttrEx* style = NULL);
    wxRichTextParagraph(const wxString& text, wxRichTextObject* parent = NULL, wxTextAttrEx* style = NULL);
    ~wxRichTextParagraph();
    wxRichTextParagraph(const wxRichTextParagraph& obj):wxRichTextBox() { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Get/set the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0)) const;

    /// Finds the absolute position and row height for the given character position
    virtual bool FindPosition(wxDC& dc, long index, wxPoint& pt, int* height, bool forceLineStart);

    /// Hit-testing: returns a flag indicating hit test details, plus
    /// information about position
    virtual int HitTest(wxDC& dc, const wxPoint& pt, long& textPosition);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

// Accessors

    /// Get the cached lines
    wxRichTextLineList& GetLines() { return m_cachedLines; }

// Operations

    /// Copy
    void Copy(const wxRichTextParagraph& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextParagraph(*this); }

    /// Clear the cached lines
    void ClearLines();

// Implementation

    /// Apply paragraph styles such as centering to the wrapped lines
    virtual void ApplyParagraphStyle(const wxRect& rect);

    /// Insert text at the given position
    virtual bool InsertText(long pos, const wxString& text);

    /// Split an object at this position if necessary, and return
    /// the previous object, or NULL if inserting at beginning.
    virtual wxRichTextObject* SplitAt(long pos, wxRichTextObject** previousObject = NULL);

    /// Move content to a list from this point
    virtual void MoveToList(wxRichTextObject* obj, wxList& list);

    /// Add content back from list
    virtual void MoveFromList(wxList& list);

    /// Get the plain text searching from the start or end of the range.
    /// The resulting string may be shorter than the range given.
    bool GetContiguousPlainText(wxString& text, const wxRichTextRange& range, bool fromStart = true);

    /// Find a suitable wrap position. wrapPosition is the last position in the line to the left
    /// of the split.
    bool FindWrapPosition(const wxRichTextRange& range, wxDC& dc, int availableSpace, long& wrapPosition);

    /// Find the object at the given position
    wxRichTextObject* FindObjectAtPosition(long position);

    /// Get the bullet text for this paragraph.
    wxString GetBulletText();

    /// Allocate or reuse a line object
    wxRichTextLine* AllocateLine(int pos);

    /// Clear remaining unused line objects, if any
    bool ClearUnusedLines(int lineCount);

protected:
    /// The lines that make up the wrapped paragraph
    wxRichTextLineList m_cachedLines;
};

/*!
 * wxRichTextPlainText class declaration
 * This object represents a single piece of text.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextPlainText: public wxRichTextObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextPlainText)
public:
// Constructors

    wxRichTextPlainText(const wxString& text = wxEmptyString, wxRichTextObject* parent = NULL, wxTextAttrEx* style = NULL);
    wxRichTextPlainText(const wxRichTextPlainText& obj):wxRichTextObject() { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Get/set the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position/* = wxPoint(0,0)*/) const;

    /// Get any text in this object for the given range
    virtual wxString GetTextForRange(const wxRichTextRange& range) const;

    /// Do a split, returning an object containing the second part, and setting
    /// the first part in 'this'.
    virtual wxRichTextObject* DoSplit(long pos);

    /// Calculate range
    virtual void CalculateRange(long start, long& end);

    /// Delete range
    virtual bool DeleteRange(const wxRichTextRange& range);

    /// Returns true if the object is empty
    virtual bool IsEmpty() const { return m_text.empty(); }

    /// Returns true if this object can merge itself with the given one.
    virtual bool CanMerge(wxRichTextObject* object) const;

    /// Returns true if this object merged itself with the given one.
    /// The calling code will then delete the given object.
    virtual bool Merge(wxRichTextObject* object);

    /// Dump to output stream for debugging
    virtual void Dump(wxTextOutputStream& stream);

// Accessors

    /// Get the text
    const wxString& GetText() const { return m_text; }

    /// Set the text
    void SetText(const wxString& text) { m_text = text; }

// Operations

    /// Copy
    void Copy(const wxRichTextPlainText& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextPlainText(*this); }
private:
    bool DrawTabbedString(wxDC& dc,const wxRect& rect,wxString& str, wxCoord& x, wxCoord& y, bool selected);

protected:
    wxString    m_text;
};

/*!
 * wxRichTextImageBlock stores information about an image, in binary in-memory form
 */

class WXDLLIMPEXP_BASE wxDataInputStream;
class WXDLLIMPEXP_BASE wxDataOutputStream;

class WXDLLIMPEXP_RICHTEXT wxRichTextImageBlock: public wxObject
{
public:
    wxRichTextImageBlock();
    wxRichTextImageBlock(const wxRichTextImageBlock& block);
    ~wxRichTextImageBlock();

    void Init();
    void Clear();

    // Load the original image into a memory block.
    // If the image is not a JPEG, we must convert it into a JPEG
    // to conserve space.
    // If it's not a JPEG we can make use of 'image', already scaled, so we don't have to
    // load the image a 2nd time.
    virtual bool MakeImageBlock(const wxString& filename, int imageType, wxImage& image, bool convertToJPEG = true);

    // Make an image block from the wxImage in the given
    // format.
    virtual bool MakeImageBlock(wxImage& image, int imageType, int quality = 80);

    // Write to a file
    bool Write(const wxString& filename);

    // Write data in hex to a stream
    bool WriteHex(wxOutputStream& stream);

    // Read data in hex from a stream
    bool ReadHex(wxInputStream& stream, int length, int imageType);

    // Copy from 'block'
    void Copy(const wxRichTextImageBlock& block);

    // Load a wxImage from the block
    bool Load(wxImage& image);

//// Operators
    void operator=(const wxRichTextImageBlock& block);

//// Accessors

    unsigned char* GetData() const { return m_data; }
    size_t GetDataSize() const { return m_dataSize; }
    int GetImageType() const { return m_imageType; }

    void SetData(unsigned char* image) { m_data = image; }
    void SetDataSize(size_t size) { m_dataSize = size; }
    void SetImageType(int imageType) { m_imageType = imageType; }

    bool Ok() const { return GetData() != NULL; }

/// Implementation

    /// Allocate and read from stream as a block of memory
    static unsigned char* ReadBlock(wxInputStream& stream, size_t size);
    static unsigned char* ReadBlock(const wxString& filename, size_t size);

    // Write memory block to stream
    static bool WriteBlock(wxOutputStream& stream, unsigned char* block, size_t size);

    // Write memory block to file
    static bool WriteBlock(const wxString& filename, unsigned char* block, size_t size);

protected:
    // Size in bytes of the image stored.
    // This is in the raw, original form such as a JPEG file.
    unsigned char*      m_data;
    size_t              m_dataSize;
    int                 m_imageType; // wxWin type id
};


/*!
 * wxRichTextImage class declaration
 * This object represents an image.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextImage: public wxRichTextObject
{
    DECLARE_DYNAMIC_CLASS(wxRichTextImage)
public:
// Constructors

    wxRichTextImage(wxRichTextObject* parent = NULL):wxRichTextObject(parent) { }
    wxRichTextImage(const wxImage& image, wxRichTextObject* parent = NULL);
    wxRichTextImage(const wxRichTextImageBlock& imageBlock, wxRichTextObject* parent = NULL);
    wxRichTextImage(const wxRichTextImage& obj):wxRichTextObject() { Copy(obj); }

// Overrideables

    /// Draw the item
    virtual bool Draw(wxDC& dc, const wxRichTextRange& range, const wxRichTextRange& selectionRange, const wxRect& rect, int descent, int style);

    /// Lay the item out
    virtual bool Layout(wxDC& dc, const wxRect& rect, int style);

    /// Get the object size for the given range. Returns false if the range
    /// is invalid for this object.
    virtual bool GetRangeSize(const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc, int flags, wxPoint position = wxPoint(0,0)) const;

    /// Returns true if the object is empty
    virtual bool IsEmpty() const { return !m_image.Ok(); }

// Accessors

    /// Get the image
    const wxImage& GetImage() const { return m_image; }

    /// Set the image
    void SetImage(const wxImage& image) { m_image = image; }

    /// Get the image block containing the raw data
    wxRichTextImageBlock& GetImageBlock() { return m_imageBlock; }

// Operations

    /// Copy
    void Copy(const wxRichTextImage& obj);

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextImage(*this); }

    /// Load wxImage from the block
    virtual bool LoadFromBlock();

    /// Make block from the wxImage
    virtual bool MakeBlock();

protected:
    // TODO: reduce the multiple representations of data
    wxImage                 m_image;
    wxBitmap                m_bitmap;
    wxRichTextImageBlock    m_imageBlock;
};


/*!
 * wxRichTextBuffer class declaration
 * This is a kind of box, used to represent the whole buffer
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCommand;
class WXDLLIMPEXP_RICHTEXT wxRichTextAction;

class WXDLLIMPEXP_RICHTEXT wxRichTextBuffer: public wxRichTextParagraphLayoutBox
{
    DECLARE_DYNAMIC_CLASS(wxRichTextBuffer)
public:
// Constructors

    wxRichTextBuffer() { Init(); }
    wxRichTextBuffer(const wxRichTextBuffer& obj):wxRichTextParagraphLayoutBox() { Init(); Copy(obj); }
    ~wxRichTextBuffer() ;

// Accessors

    /// Gets the command processor
    wxCommandProcessor* GetCommandProcessor() const { return m_commandProcessor; }

    /// Set style sheet, if any.
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet) { m_styleSheet = styleSheet; }
    wxRichTextStyleSheet* GetStyleSheet() const { return m_styleSheet; }

// Operations

    /// Initialisation
    void Init();

    /// Clears the buffer and resets the command processor
    virtual void Clear();

    /// The same as Clear, and adds an empty paragraph.
    virtual void Reset();

    /// Load a file
    virtual bool LoadFile(const wxString& filename, int type = wxRICHTEXT_TYPE_ANY);

    /// Save a file
    virtual bool SaveFile(const wxString& filename, int type = wxRICHTEXT_TYPE_ANY);

    /// Load from a stream
    virtual bool LoadFile(wxInputStream& stream, int type = wxRICHTEXT_TYPE_ANY);

    /// Save to a stream
    virtual bool SaveFile(wxOutputStream& stream, int type = wxRICHTEXT_TYPE_ANY);

    /// Convenience function to add a paragraph of text
    virtual wxRichTextRange AddParagraph(const wxString& text) { Modify(); return wxRichTextParagraphLayoutBox::AddParagraph(text); }

    /// Begin collapsing undo/redo commands. Note that this may not work properly
    /// if combining commands that delete or insert content, changing ranges for
    /// subsequent actions.
    virtual bool BeginBatchUndo(const wxString& cmdName);

    /// End collapsing undo/redo commands
    virtual bool EndBatchUndo();

    /// Collapsing commands?
    virtual bool BatchingUndo() const { return m_batchedCommandDepth > 0; }

    /// Submit immediately, or delay according to whether collapsing is on
    virtual bool SubmitAction(wxRichTextAction* action);

    /// Get collapsed command
    virtual wxRichTextCommand* GetBatchedCommand() const { return m_batchedCommand; }

    /// Begin suppressing undo/redo commands. The way undo is suppressed may be implemented
    /// differently by each command. If not dealt with by a command implementation, then
    /// it will be implemented automatically by not storing the command in the undo history
    /// when the action is submitted to the command processor.
    virtual bool BeginSuppressUndo();

    /// End suppressing undo/redo commands.
    virtual bool EndSuppressUndo();

    /// Collapsing commands?
    virtual bool SuppressingUndo() const { return m_suppressUndo > 0; }

    /// Copy the range to the clipboard
    virtual bool CopyToClipboard(const wxRichTextRange& range);

    /// Paste the clipboard content to the buffer
    virtual bool PasteFromClipboard(long position);

    /// Can we paste from the clipboard?
    virtual bool CanPasteFromClipboard() const;

    /// Begin using a style
    virtual bool BeginStyle(const wxTextAttrEx& style);

    /// End the style
    virtual bool EndStyle();

    /// End all styles
    virtual bool EndAllStyles();

    /// Clear the style stack
    virtual void ClearStyleStack();

    /// Get the size of the style stack, for example to check correct nesting
    virtual size_t GetStyleStackSize() const { return m_attributeStack.GetCount(); }

    /// Begin using bold
    bool BeginBold();

    /// End using bold
    bool EndBold() { return EndStyle(); }

    /// Begin using italic
    bool BeginItalic();

    /// End using italic
    bool EndItalic() { return EndStyle(); }

    /// Begin using underline
    bool BeginUnderline();

    /// End using underline
    bool EndUnderline() { return EndStyle(); }

    /// Begin using point size
    bool BeginFontSize(int pointSize);

    /// End using point size
    bool EndFontSize() { return EndStyle(); }

    /// Begin using this font
    bool BeginFont(const wxFont& font);

    /// End using a font
    bool EndFont() { return EndStyle(); }

    /// Begin using this colour
    bool BeginTextColour(const wxColour& colour);

    /// End using a colour
    bool EndTextColour() { return EndStyle(); }

    /// Begin using alignment
    bool BeginAlignment(wxTextAttrAlignment alignment);

    /// End alignment
    bool EndAlignment() { return EndStyle(); }

    /// Begin left indent
    bool BeginLeftIndent(int leftIndent, int leftSubIndent = 0);

    /// End left indent
    bool EndLeftIndent() { return EndStyle(); }

    /// Begin right indent
    bool BeginRightIndent(int rightIndent);

    /// End right indent
    bool EndRightIndent() { return EndStyle(); }

    /// Begin paragraph spacing
    bool BeginParagraphSpacing(int before, int after);

    /// End paragraph spacing
    bool EndParagraphSpacing() { return EndStyle(); }

    /// Begin line spacing
    bool BeginLineSpacing(int lineSpacing);

    /// End line spacing
    bool EndLineSpacing() { return EndStyle(); }

    /// Begin numbered bullet
    bool BeginNumberedBullet(int bulletNumber, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD);

    /// End numbered bullet
    bool EndNumberedBullet() { return EndStyle(); }

    /// Begin symbol bullet
    bool BeginSymbolBullet(wxChar symbol, int leftIndent, int leftSubIndent, int bulletStyle = wxTEXT_ATTR_BULLET_STYLE_SYMBOL);

    /// End symbol bullet
    bool EndSymbolBullet() { return EndStyle(); }

    /// Begin named character style
    bool BeginCharacterStyle(const wxString& characterStyle);

    /// End named character style
    bool EndCharacterStyle() { return EndStyle(); }

    /// Begin named paragraph style
    bool BeginParagraphStyle(const wxString& paragraphStyle);

    /// End named character style
    bool EndParagraphStyle() { return EndStyle(); }

// Implementation

    /// Copy
    void Copy(const wxRichTextBuffer& obj) { wxRichTextBox::Copy(obj); }

    /// Clone
    virtual wxRichTextObject* Clone() const { return new wxRichTextBuffer(*this); }

    /// Submit command to insert the given text
    bool InsertTextWithUndo(long pos, const wxString& text, wxRichTextCtrl* ctrl);

    /// Submit command to insert a newline
    bool InsertNewlineWithUndo(long pos, wxRichTextCtrl* ctrl);

    /// Submit command to insert the given image
    bool InsertImageWithUndo(long pos, const wxRichTextImageBlock& imageBlock, wxRichTextCtrl* ctrl);

    /// Submit command to delete this range
    bool DeleteRangeWithUndo(const wxRichTextRange& range, long initialCaretPosition, long newCaretPositon, wxRichTextCtrl* ctrl);

    /// Mark modified
    void Modify(bool modify = true) { m_modified = modify; }
    bool IsModified() const { return m_modified; }

    /// Dumps contents of buffer for debugging purposes
    virtual void Dump();
    virtual void Dump(wxTextOutputStream& stream) { wxRichTextParagraphLayoutBox::Dump(stream); }

    /// Returns the file handlers
    static wxList& GetHandlers() { return sm_handlers; }

    /// Adds a handler to the end
    static void AddHandler(wxRichTextFileHandler *handler);

    /// Inserts a handler at the front
    static void InsertHandler(wxRichTextFileHandler *handler);

    /// Removes a handler
    static bool RemoveHandler(const wxString& name);

    /// Finds a handler by name
    static wxRichTextFileHandler *FindHandler(const wxString& name);

    /// Finds a handler by extension and type
    static wxRichTextFileHandler *FindHandler(const wxString& extension, int imageType);

    /// Finds a handler by filename or, if supplied, type
    static wxRichTextFileHandler *FindHandlerFilenameOrType(const wxString& filename, int imageType);

    /// Finds a handler by type
    static wxRichTextFileHandler *FindHandler(int imageType);

    /// Gets a wildcard incorporating all visible handlers. If 'types' is present,
    /// will be filled with the file type corresponding to each filter. This can be
    /// used to determine the type to pass to LoadFile given a selected filter.
    static wxString GetExtWildcard(bool combine = false, bool save = false, wxArrayInt* types = NULL);

    /// Clean up handlers
    static void CleanUpHandlers();

    /// Initialise the standard handlers
    static void InitStandardHandlers();

protected:

    /// Command processor
    wxCommandProcessor*     m_commandProcessor;

    /// Has been modified?
    bool                    m_modified;

    /// Collapsed command stack
    int                     m_batchedCommandDepth;

    /// Name for collapsed command
    wxString                m_batchedCommandsName;

    /// Current collapsed command accumulating actions
    wxRichTextCommand*      m_batchedCommand;

    /// Whether to suppress undo
    int                     m_suppressUndo;

    /// Style sheet, if any
    wxRichTextStyleSheet*   m_styleSheet;

    /// Stack of attributes for convenience functions
    wxList                  m_attributeStack;

    /// File handlers
    static wxList           sm_handlers;
};

/*!
 * The command identifiers
 *
 */

enum wxRichTextCommandId
{
    wxRICHTEXT_INSERT,
    wxRICHTEXT_DELETE,
    wxRICHTEXT_CHANGE_STYLE
};

/*!
 * Command classes for undo/redo
 *
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextAction;
class WXDLLIMPEXP_RICHTEXT wxRichTextCommand: public wxCommand
{
public:
    // Ctor for one action
    wxRichTextCommand(const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
        wxRichTextCtrl* ctrl, bool ignoreFirstTime = false);

    // Ctor for multiple actions
    wxRichTextCommand(const wxString& name);

    ~wxRichTextCommand();

    bool Do();
    bool Undo();

    void AddAction(wxRichTextAction* action);
    void ClearActions();

    wxList& GetActions() { return m_actions; }

protected:

    wxList  m_actions;
};

/*!
 * wxRichTextAction class declaration
 * There can be more than one action in a command.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextAction: public wxObject
{
public:
    wxRichTextAction(wxRichTextCommand* cmd, const wxString& name, wxRichTextCommandId id, wxRichTextBuffer* buffer,
        wxRichTextCtrl* ctrl, bool ignoreFirstTime = false);

    ~wxRichTextAction();

    bool Do();
    bool Undo();

    /// Update the control appearance
    void UpdateAppearance(long caretPosition, bool sendUpdateEvent = false);

    /// Replace the buffer paragraphs with the given fragment.
    void ApplyParagraphs(const wxRichTextFragment& fragment);

    /// Get the fragments
    wxRichTextFragment& GetNewParagraphs() { return m_newParagraphs; }
    wxRichTextFragment& GetOldParagraphs() { return m_oldParagraphs; }

    /// Set/get the position used for e.g. insertion
    void SetPosition(long pos) { m_position = pos; }
    long GetPosition() const { return m_position; }

    /// Set/get the range for e.g. deletion
    void SetRange(const wxRichTextRange& range) { m_range = range; }
    const wxRichTextRange& GetRange() const { return m_range; }

    /// Get name
    const wxString& GetName() const { return m_name; }

protected:
    // Action name
    wxString                        m_name;

    // Buffer
    wxRichTextBuffer*               m_buffer;

    // Control
    wxRichTextCtrl*                 m_ctrl;

    // Stores the new paragraphs
    wxRichTextFragment              m_newParagraphs;

    // Stores the old paragraphs
    wxRichTextFragment              m_oldParagraphs;

    // The affected range
    wxRichTextRange                 m_range;

    // The insertion point for this command
    long                            m_position;

    // Ignore 1st 'Do' operation because we already did it
    bool                            m_ignoreThis;

    // The command identifier
    wxRichTextCommandId             m_cmdId;
};

/*!
 * wxRichTextFileHandler
 * Base class for file handlers
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextFileHandler: public wxObject
{
    DECLARE_CLASS(wxRichTextFileHandler)
public:
    wxRichTextFileHandler(const wxString& name = wxEmptyString, const wxString& ext = wxEmptyString, int type = 0)
        : m_name(name), m_extension(ext), m_type(type), m_visible(true)
        { }

#if wxUSE_STREAMS
    bool LoadFile(wxRichTextBuffer *buffer, wxInputStream& stream)
    { return DoLoadFile(buffer, stream); }
    bool SaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
    { return DoSaveFile(buffer, stream); }
#endif

    bool LoadFile(wxRichTextBuffer *buffer, const wxString& filename);
    bool SaveFile(wxRichTextBuffer *buffer, const wxString& filename);

    /// Can we handle this filename (if using files)? By default, checks the extension.
    virtual bool CanHandle(const wxString& filename) const;

    /// Can we save using this handler?
    virtual bool CanSave() const { return false; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return false; }

    /// Should this handler be visible to the user?
    virtual bool IsVisible() const { return m_visible; }
    virtual void SetVisible(bool visible) { m_visible = visible; }

    /// The name of the nandler
    void SetName(const wxString& name) { m_name = name; }
    wxString GetName() const { return m_name; }

    /// The default extension to recognise
    void SetExtension(const wxString& ext) { m_extension = ext; }
    wxString GetExtension() const { return m_extension; }

    /// The handler type
    void SetType(int type) { m_type = type; }
    int GetType() const { return m_type; }

    /// Encoding to use when saving a file. If empty, a suitable encoding is chosen
    void SetEncoding(const wxString& encoding) { m_encoding = encoding; }
    const wxString& GetEncoding() const { return m_encoding; }

protected:

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream) = 0;
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream) = 0;
#endif

    wxString  m_name;
    wxString  m_encoding;
    wxString  m_extension;
    int       m_type;
    bool      m_visible;
};

/*!
 * wxRichTextPlainTextHandler
 * Plain text handler
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextPlainTextHandler: public wxRichTextFileHandler
{
    DECLARE_CLASS(wxRichTextPlainTextHandler)
public:
    wxRichTextPlainTextHandler(const wxString& name = wxT("Text"), const wxString& ext = wxT("txt"), int type = wxRICHTEXT_TYPE_TEXT)
        : wxRichTextFileHandler(name, ext, type)
        { }

    /// Can we save using this handler?
    virtual bool CanSave() const { return true; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return true; }

protected:

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream);
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream);
#endif

};

/*!
 * Utilities
 *
 */

inline bool wxRichTextHasStyle(int flags, int style)
{
    return ((flags & style) == style);
}

/// Compare two attribute objects
bool wxTextAttrEq(const wxTextAttrEx& attr1, const wxTextAttrEx& attr2);
bool wxTextAttrEq(const wxTextAttr& attr1, const wxRichTextAttr& attr2);

/// Compare two attribute objects, but take into account the flags
/// specifying attributes of interest.
bool wxTextAttrEqPartial(const wxTextAttrEx& attr1, const wxTextAttrEx& attr2, int flags);
bool wxTextAttrEqPartial(const wxTextAttrEx& attr1, const wxRichTextAttr& attr2, int flags);

/// Apply one style to another
bool wxRichTextApplyStyle(wxTextAttrEx& destStyle, const wxTextAttrEx& style);
bool wxRichTextApplyStyle(wxRichTextAttr& destStyle, const wxTextAttrEx& style);
bool wxRichTextApplyStyle(wxTextAttrEx& destStyle, const wxRichTextAttr& style);

#endif
    // wxUSE_RICHTEXT

#endif
    // _WX_RICHTEXTBUFFER_H_
