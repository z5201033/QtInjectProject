// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 WIN32DLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// WIN32DLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef WIN32DLL_EXPORTS
#define WIN32DLL_API __declspec(dllexport)
#else
#define WIN32DLL_API __declspec(dllimport)
#endif

// 此类是从 dll 导出的
class WIN32DLL_API Cwin32dll {
public:
	Cwin32dll(void);
	// TODO: 在此处添加方法。
};

extern WIN32DLL_API int nwin32dll;

WIN32DLL_API int fnwin32dll(void);
