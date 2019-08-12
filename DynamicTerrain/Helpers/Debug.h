#ifndef ENGINE_HELPERS_DEBUG_H
#define ENGINE_HELPERS_DEBUG_H

#include <Windows.h>

namespace Engine
{
namespace Helpers
{

class Debug {
public:
	static inline void ShowLastErrorInMessageBox() {
		void* lpBuffer;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL,
					  GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR) &lpBuffer,
					  0,
					  NULL);

		MessageBox(NULL, (LPCTSTR)lpBuffer, L"LastError", MB_OK);
		LocalFree(lpBuffer);
	};
};

}
}

#endif