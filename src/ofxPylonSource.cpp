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

void PylonSource::start(int camWidth, int camHeight, int camChannels, string featureFile) {
	width = camWidth;
	height = camHeight;
	channels = camChannels;
	frameData = malloc(camWidth * camHeight * camChannels);
	bufferSize = camWidth * camHeight * camChannels;
	bool success = initializeCamera();

	if (success) {
		camera->RegisterImageEventHandler(this, RegistrationMode_ReplaceAll, Cleanup_None);
		camera->Open();
		string featureFilePath = ofFilePath::getAbsolutePath(featureFile);
		ofLogNotice("applying camera settings from features file", featureFilePath);
		CFeaturePersistence::Load(featureFilePath.c_str(), &camera->GetNodeMap(), true);
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
		ofLogVerbose("Using device", camera->GetDeviceInfo().GetModelName());

		return true;
	}
	catch (const GenericException & e) {
		ofLogError("PylonSource::initializeCamera", e.GetDescription());
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
		stringstream ss;
		ss << "PylonSource::OnImageGrabbed" << ofToString(ptrGrabResult->GetErrorCode());
		ss << " " << ptrGrabResult->GetErrorDescription();
		ofLogError(ss.str());
	}

}
