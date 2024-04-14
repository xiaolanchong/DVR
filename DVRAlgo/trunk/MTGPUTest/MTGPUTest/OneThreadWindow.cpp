#include "stdafx.h"
#include "OneThreadWindow.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "ExceptionHandler.h"
#include "ExtensionsInitializer.h"

OneThreadWindow::OneThreadWindow( CWnd* parent ):
	m_parent(parent), 
	myThread(0)
{
}

void OneThreadWindow::Start()
{
	if( myThread )
	{
		return;
	}

	m_isInitialized.ResetEvent();
	myThread = AfxBeginThread( &OneThreadWindow::ThreadProcedure, (void*)this );
	WaitForSingleObject( m_isInitialized.m_hObject, INFINITE );
//	Sleep( 3000 );
}

void OneThreadWindow::Stop()
{
	if( !myThread )
	{
		return;
	}
	m_exitEvent.SetEvent();
	WaitForSingleObject( myThread->m_hThread, INFINITE );
	//Sleep( 500 );
	myThread = 0;
}

OneThreadWindow::~OneThreadWindow()
{

}


UINT OneThreadWindow::ThreadProcedure( void * param )
{
__try
{
	OneThreadWindow* pThis = reinterpret_cast<OneThreadWindow*> ( param );
	return pThis->ThreadProc();
}
__except(RecordExceptionInfo(GetExceptionInformation(), _T("main thread")))
{
	// Do nothing here - RecordExceptionInfo() has already done
	// everything that is needed. Actually this code won't even
	// get called unless you return EXCEPTION_EXECUTE_HANDLER from
	// the __except clause.
return UINT(-1);
}
}


UINT OneThreadWindow::ThreadProc()
{

	//required extensions
	const char * requiredExtensions =
		"GL_ARB_fragment_program "
		"GL_ARB_vertex_program "
		"GL_EXT_framebuffer_object "
		"GL_ARB_vertex_buffer_object ";
	InitOpenGL();
	GetGLExtensionsInitializer().Initialize( requiredExtensions );

	ThreadInit();
	m_isInitialized.SetEvent();
	while( WaitForSingleObject( m_exitEvent.m_hObject, 0 ) == WAIT_TIMEOUT )
	{
		DoStuff(); 
		glutSwapBuffers();
	}

	ThreadUnInit();
	GetGLExtensionsInitializer().Uninitialize();
	
	UnInitOpenGL();
	return 0;
}


void OneThreadWindow::InitOpenGL()
{
	GLuint PixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 8;

	m_parentDC = ::GetDC( m_parent->GetSafeHwnd() );
	PixelFormat = ChoosePixelFormat( m_parentDC, &pfd );
	SetPixelFormat( m_parentDC, PixelFormat, &pfd );
	m_glContext = wglCreateContext( m_parentDC );
	wglMakeCurrent( m_parentDC, m_glContext );
}

void OneThreadWindow::UnInitOpenGL()
{
	if( m_glContext != NULL )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_glContext );
		m_glContext = NULL;							
	}

	if( m_parentDC != NULL )
	{
		ReleaseDC( m_parent->GetSafeHwnd(), m_parentDC );
		m_parentDC = NULL;
	}
}

void OneThreadWindow::glutSwapBuffers()
{
	SwapBuffers( m_parentDC );
}

