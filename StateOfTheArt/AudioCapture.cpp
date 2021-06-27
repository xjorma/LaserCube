#include "pch.h"
#include "AudioCapture.h"

#include <Windows.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)	if (FAILED(hres)) DebugBreak();

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

void AudioCapture::AudioThread()
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioCaptureClient* pCaptureClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	BOOL bDone = FALSE;

	hr = CoInitialize(nullptr);
	EXIT_ON_ERROR(hr)

	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

	hr = pDevice->Activate(
		__uuidof(IAudioClient), CLSCTX_ALL,
		NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr)

	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_LOOPBACK,
		hnsRequestedDuration,
		0,
		pwfx,
		NULL);
	EXIT_ON_ERROR(hr)

	// Get the size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetService(
		__uuidof(IAudioCaptureClient),
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr)

	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = REFERENCE_TIME((double)REFTIMES_PER_SEC * (double)bufferFrameCount / (double)pwfx->nSamplesPerSec);

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr)

	while(!requestStop)
	{
		UINT32 packetLength = 0;
		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		EXIT_ON_ERROR(hr)
		if (packetLength != 0)
		{
			UINT32 numFramesAvailable;
			BYTE* pData;
			DWORD flags;
			// Get the available data in the shared buffer.
			hr = pCaptureClient->GetBuffer(
				&pData,
				&numFramesAvailable,
				&flags, NULL, NULL);
			EXIT_ON_ERROR(hr)

			const std::lock_guard<std::mutex> lock(mutex);

			if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
			{
				samples.resize(packetLength);
				for (int i = 0; i < (int)packetLength; i++)
					samples[i] = vec2(0, 0);
			}
			else
			{
				float* fData = (float*)pData;
				samples.resize(packetLength);
				for (int i = 0; i < (int)packetLength; i++)
					samples[i] = vec2(fData[i * 2], fData[i * 2 + 1]);
			}

			hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
		}
	}

	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr)

	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pAudioClient)
	SAFE_RELEASE(pCaptureClient)
}

AudioCapture::AudioCapture() : requestStop(false), audioThread(&AudioCapture::AudioThread, this)
{

}


AudioCapture::~AudioCapture()
{
	requestStop = true;
	audioThread.join();
}