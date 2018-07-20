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

void PylonSource::start(string featureFile) {
	bool success = initializeCamera();

	if (success) {
		camera->RegisterImageEventHandler(this, RegistrationMode_ReplaceAll, Cleanup_None);
		camera->RegisterConfiguration(new CAcquireContinuousConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
		camera->RegisterConfiguration(this, RegistrationMode_Append, Cleanup_None);

		camera->Open();
		string featureFilePath = ofFilePath::getAbsolutePath(featureFile);
		ofLogNotice("applying camera settings from features file", featureFilePath);
		CFeaturePersistence::Load(featureFilePath.c_str(), &camera->GetNodeMap(), true);
	}
	else {
		throw RUNTIME_EXCEPTION("Device did not initialize");
	}
}

void PylonSource::setupBuffer(int w, int h, int c) {
	ofLogVerbose("setting up buffer with", ofToString(w) + "x" + ofToString(h));
	ofLogVerbose("channels", ofToString(c));
	width = w;
	height = h;
	channels = c;
	bufferSize = width * height * channels;
	frameData = malloc(bufferSize);
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

void PylonSource::OnOpened(CBaslerUsbInstantCamera& camera) {
	ofLogVerbose("OnOpened");
	try {
		INodeMap &control = camera.GetNodeMap();

		const CIntegerPtr widthPtr = control.GetNode("Width");
		const CIntegerPtr heightPtr = control.GetNode("Height");

		CEnumerationPtr pixelFormat( control.GetNode( "PixelFormat"));

		String_t pixelFormatString = pixelFormat->ToString();
		ofLogVerbose("got pixel format", pixelFormatString);

		int numChannels = 3;

		setupBuffer(widthPtr->GetValue(), heightPtr->GetValue(), numChannels);

		// const CIntegerPtr offsetXPtr = control.GetNode("OffsetX");
		// const CIntegerPtr offsetYPtr = control.GetNode("OffsetY");
   //
		// if (IsWritable(offsetXPtr)) offsetXPtr->SetValue(0);
		// if (IsWritable(offsetYPtr)) offsetYPtr->SetValue(0);
   //
		// int w = min((int)widthPtr->GetMax(), width);
		// int h = min((int)heightPtr->GetMax(), height);
		// int x = max((int)((widthPtr->GetMax() - width) * .5), 0);
		// int y = max((int)((heightPtr->GetMax() - height) * .5), 0);
   //
		// if (IsWritable(widthPtr)) widthPtr->SetValue(w);
		// if (IsWritable(heightPtr)) heightPtr->SetValue(h);
		// if (IsWritable(offsetXPtr)) offsetXPtr->SetValue(x);
		// if (IsWritable(offsetYPtr)) offsetYPtr->SetValue(y);

		camera.StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByInstantCamera);
		running = true;

	} catch (const GenericException& e) {
		ofLogError("error reading configuration");
		throw RUNTIME_EXCEPTION("Could not apply configuration. const GenericException caught in OnOpened method msg=%hs", e.what());
	}
}

