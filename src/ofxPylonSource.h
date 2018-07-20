#pragma once

#undef None
#undef Status

// #include <algorithm>
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

using namespace std;
using namespace Pylon;
using namespace GenApi;

typedef shared_ptr<class PylonSource> PylonSourceRef;

class PylonSource : public CImageEventHandler, CBaslerUsbConfigurationEventHandler {

public:

	static PylonSourceRef create() {
		return make_shared<PylonSource>();
	}

	PylonSource();
	~PylonSource();

	void start(string featureFile);

	bool hasNewFrame() {
		return newFrame;
	}

	bool isRunning() {
		return running;
	}

	void * getFrameData() {
		newFrame = false;
		return frameData;
	}

protected:
	void setupBuffer(int width, int height, int channels);

	void	OnOpened(CBaslerUsbInstantCamera& camera);
	void	OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);
	bool	initializeCamera();
	//bool	registerEvents();
	//bool	startGrabbing();
	bool	running;
	bool	newFrame;
	int		width;
	int		height;
	int		channels;

	int bufferSize;

	void *						frameData;
	PylonAutoInitTerm			autoInitTerm;
	CBaslerUsbInstantCamera *	camera;
};
