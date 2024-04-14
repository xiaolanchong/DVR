//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2006                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Утилиты для работы в режиме реестра

    Author: Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date:   30.05.2006
*/                                                                                      //
//======================================================================================//
#ifndef _REG_COMMON_3698980566050891_
#define _REG_COMMON_3698980566050891_

template<typename T> struct RegAssign
{
	RegAssign& operator = ( DWORD dwRes )
	{
		if( dwRes != ERROR_SUCCESS )throw T("Error result");
		return *this;
	}
};

template<typename T> struct HrAssign
{
	HrAssign& operator = ( HRESULT hr )
	{
		if( hr != S_OK )throw T();
		return *this;
	}
};

#endif // _REG_COMMON_3698980566050891_