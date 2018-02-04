// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TK103_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TK103_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TK103_EXPORTS
#define TK103_API __declspec(dllexport)
#else
#define TK103_API __declspec(dllimport)
#endif

// This class is exported from the tk103.dll
class TK103_API Ctk103 {
public:
	Ctk103(void);
	// TODO: add your methods here.
};

extern TK103_API int ntk103;

TK103_API int fntk103(void);
