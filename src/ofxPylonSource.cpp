#include "ofxPylonSource.h"
#include "ofMain.h"

PylonSource::PylonSource() {
	running = false;
	newFrame = false;
}

PylonSource::~PylonSource() {
	if (camera && running) {
		camera->DeregisterImageEventHandler(this);
		free(frameData);
		delete camera;
	}
}

void PylonSource::start(int camWidth, int camHeight, int camChannels, const char* nodeFile) {
	width = camWidth;
	height = camHeight;
	channels = camChannels;
	frameData = malloc(camWidth * camHeight * camChannels);
	bufferSize = camWidth * camHeight * camChannels;
	bool success = initializeCamera();

	if (success) {
		camera->RegisterImageEventHandler(this, RegistrationMode_ReplaceAll, Cleanup_None);
		camera->Open();
		ofLogNotice("Will load camera settings from node file", ofToString(nodeFile));
		CFeaturePersistence::Load(nodeFile, &camera->GetNodeMap(), true);
		camera->StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByInstantCamera);
		running = true;
	}
	else {
		throw RUNTIME_EXCEPTION("Device did not initialize");
	}
}

bool PylonSource::initializeCamera() {
	try {
		camera = new CBaslerUsbInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
		cout << "Using device " << camera->GetDeviceInfo().GetModelName() << endl;

		return true;
	}
	catch (const GenericException & e) {
		cerr << "PylonSource::initializeCamera: " << e.GetDescription() << endl;
		return false;
	}
}

void PylonSource::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult) {
	newFrame = true;
	if (ptrGrabResult->GrabSucceeded()) {
		CImageFormatConverter converter;
		converter.OutputPixelFormat = PixelType_RGB8packed;
		converter.OutputBitAlignment = OutputBitAlignment_MsbAligned;
		CPylonImage targetImage;
		converter.Convert(targetImage, ptrGrabResult);
		memcpy(frameData, targetImage.GetBuffer(), bufferSize);
		//memcpy(frameData, ptrGrabResult->GetBuffer(), bufferSize);
		newFrame = true;
	} else {
		cerr << "PylonSource::OnImageGrabbed: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
	}

}
