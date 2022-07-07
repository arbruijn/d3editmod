/*
 THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF OUTRAGE
 ENTERTAINMENT, INC. ("OUTRAGE").  OUTRAGE, IN DISTRIBUTING THE CODE TO
 END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
 ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
 IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
 SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
 FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
 CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
 AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
 COPYRIGHT 1996-2000 OUTRAGE ENTERTAINMENT, INC.  ALL RIGHTS RESERVED.
 */
 #if !defined(AFX_OBJECTLISTCOMBO_H__C8C81360_C67E_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_OBJECTLISTCOMBO_H__C8C81360_C67E_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectListCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectListCombo window

class CObjectListCombo : public CComboBox
{
// Construction
public:
	CObjectListCombo();
	void Init(int type,int selected_handle);
	void SetSelected(int selected_handle);
	int GetSelected();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectListCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjectListCombo();

private:
	int m_type;

	// Generated message map functions
protected:
	//{{AFX_MSG(CObjectListCombo)
	afx_msg void OnSetfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTLISTCOMBO_H__C8C81360_C67E_11D2_A13A_0060089A83BE__INCLUDED_)
