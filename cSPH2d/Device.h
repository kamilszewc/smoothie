#if !defined(__DEVICE_H__)
#define __DEVICE_H__

class Device
{
private:
	int _numberOfThreads;
public:
	Device();
	void SetNumberOfThreads(int);
	void SetNumberOfThreadsFromFile(const char*);
	void SetMaxNumberOfThreads();
	int GetNumberOfThreads();
};

#endif