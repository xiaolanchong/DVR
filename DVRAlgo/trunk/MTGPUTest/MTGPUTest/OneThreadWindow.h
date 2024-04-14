#pragma once

class OneThreadWindow
{
public:
	OneThreadWindow( CWnd* parent );
	virtual ~OneThreadWindow();

	void Stop();
	void Start();	
	void BeginStarting()
	{
		static int initCount = 0;
	}
private:
	virtual void	ThreadInit() = 0;
	virtual void	ThreadUnInit() = 0;
	virtual void	DoStuff() = 0;
	
protected:
	void glutSwapBuffers();
private:
	static UINT ThreadProcedure( void* param );
	UINT ThreadProc();

	CWinThread* myThread;
	CEvent m_exitEvent;
	CWnd* m_parent;

	HDC m_parentDC;
	HGLRC m_glContext; 
	void InitOpenGL();
	void UnInitOpenGL();

	CEvent m_isInitialized;
	static CMutex m_isUnInitialized;
};

