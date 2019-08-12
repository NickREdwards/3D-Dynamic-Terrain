#ifndef SHARED_H
#define SHARED_H

#include <Windows.h>

class Shared {
public:
	~Shared() {}

	static Shared& GetInstance() {
		static Shared instance;
		return instance;
	}

	void SetHInstance(const HINSTANCE& hInstance) { _hInstance = hInstance; }
	const HINSTANCE& GetHInstance() { return _hInstance; }

private:
	Shared() : _hInstance(nullptr) {}

private:
	HINSTANCE _hInstance;

};

#endif