// win32dll.cpp : 定义 DLL 的导出函数。
//

#include "framework.h"
#include "win32dll.h"


// 这是导出变量的一个示例
WIN32DLL_API int nwin32dll=0;

// 这是导出函数的一个示例。
WIN32DLL_API int fnwin32dll(void)
{
    return 0;
}

// 这是已导出类的构造函数。
Cwin32dll::Cwin32dll()
{
    return;
}
