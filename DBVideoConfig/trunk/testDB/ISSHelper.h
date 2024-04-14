// ISSHelper.h: interface for the CISSHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISSHELPER_H__7B88E5F3_263F_11D2_9D1F_00C04F96B8B2__INCLUDED_)
#define AFX_ISSHELPER_H__7B88E5F3_263F_11D2_9D1F_00C04F96B8B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CISSHelper : public ISequentialStream  
{
public:
	// Constructor/destructor.
	CISSHelper();
	virtual ~CISSHelper();

	// Helper function to clean up memory.
	virtual void Clear();

	// ISequentialStream interface implementation.
	STDMETHODIMP_(ULONG)	AddRef(void);
	STDMETHODIMP_(ULONG)	Release(void);
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP Read( 
            /* [out] */ void __RPC_FAR *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG __RPC_FAR *pcbRead);
    STDMETHODIMP Write( 
            /* [in] */ const void __RPC_FAR *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG __RPC_FAR *pcbWritten);

public:
	void* m_pBuffer;  // Buffer
	ULONG m_ulLength; // Total buffer size.
	ULONG m_ulStatus; // Column status.

private:
	ULONG m_cRef;      // Reference count (not used).
	ULONG m_iReadPos;  // Current index position for reading from the buffer.
	ULONG m_iWritePos; // Current index position for writing to the buffer.

};

#endif // !defined(AFX_ISSHELPER_H__7B88E5F3_263F_11D2_9D1F_00C04F96B8B2__INCLUDED_)
