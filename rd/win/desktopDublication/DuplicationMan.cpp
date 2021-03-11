#include "DuplicationMan.h"



DuplicationMan::DuplicationMan() : 
	acquired_desktop_image(nullptr),
	desk_dupl(nullptr),
	meta_data_buffer(nullptr),
	m_device(nullptr)
	
{
}


DuplicationMan::~DuplicationMan()
{
	if (desk_dupl)
	{
		desk_dupl->Release();
		desk_dupl = nullptr;
	}

	if (acquired_desktop_image)
	{
		acquired_desktop_image->Release();
		acquired_desktop_image = nullptr;
	}

	if (meta_data_buffer)
	{
		delete[] meta_data_buffer;
		meta_data_buffer = nullptr;
	}

	if (meta_data_buffer)
	{
		delete[] meta_data_buffer;
		meta_data_buffer = NULL;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}
	
}



DUPL_RETURN DuplicationMan::init( ID3D11Device* Device, UINT Output)
{
	m_OutputNumber = Output;

	// Take a reference on the device
	m_device = Device;
	m_device->AddRef();

	// Get DXGI device
	IDXGIDevice* DxgiDevice = nullptr;
	HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr))
	{
		//Log::write("Failed to QI for DXGI Device");
		return ProcessFailure(nullptr, L"Failed to QI for DXGI Device", L"Error", hr, 0);
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// Get DXGI adapter
	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	DxgiDevice->Release();
	DxgiDevice = nullptr;
	if (FAILED(hr))
	{
		return ProcessFailure(nullptr, L"Failed to get parent DXGI Adapter", L"Error", hr, 0);
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// Get output
	IDXGIOutput* DxgiOutput = nullptr;
	hr = DxgiAdapter->EnumOutputs(Output, &DxgiOutput);
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;
	if (FAILED(hr))
	{
		//Log::write("Failed to get specified output in DUPLICATIONMANAGER");
		return ProcessFailure(nullptr, L"Failed to get specified output in DUPLICATIONMANAGER", L"Error", hr, 0);
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	DxgiOutput->GetDesc(&m_OutputDesc);

	// QI for Output 1
	IDXGIOutput1* DxgiOutput1 = nullptr;
	hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
	DxgiOutput->Release();
	DxgiOutput = nullptr;
	if (FAILED(hr))
	{
		//Log::write("Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER");
		return ProcessFailure(nullptr, L"Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER", L"Error", hr, 0);
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// Create desktop duplication
	hr = DxgiOutput1->DuplicateOutput(m_device, &desk_dupl);
	DxgiOutput1->Release();
	DxgiOutput1 = nullptr;
	if (FAILED(hr) || !desk_dupl)
	{
		if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			//Log::write("There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.");
			//MessageBoxW(nullptr, L"There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.", L"Error", MB_OK);
			return DUPL_RETURN_ERROR_UNEXPECTED;
		}
		return ProcessFailure(nullptr, L"Failed to get duplicate output in DUPLICATIONMANAGER", L"Error", hr, 0);
		return DUPL_RETURN_ERROR_EXPECTED;
	}
	
	return DUPL_RETURN_SUCCESS;
}

void DuplicationMan::GetOutputDesc(DXGI_OUTPUT_DESC * DescPtr)
{
	*DescPtr = m_OutputDesc;
}




DUPL_RETURN DuplicationMan::get_frame(FRAME_DATA * data, bool* Timeout)
{
	HRESULT hr = S_OK;

	IDXGIResource* DesktopResource = NULL;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
	//Log::write("Failed to acquire next frame in DUPLICATIONMANAGER  +++++++" + std::to_string(GetLastError()));
	//Get new frame


	hr = desk_dupl->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
	if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		*Timeout = true;
		return DUPL_RETURN_SUCCESS;
	}
	*Timeout = false;
	if (FAILED(hr))
	{
		if ((hr != DXGI_ERROR_ACCESS_LOST) && (hr != DXGI_ERROR_WAIT_TIMEOUT))
		{
			//Log::write("Failed to acquire next frame in DUPLICATIONMANAGER  " + std::to_string(GetLastError()));
			//DisplayErr(L"Failed to acquire next frame in DUPLICATIONMANAGER", L"Error", hr);
		}
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// If still holding old frame, destroy it
	if (acquired_desktop_image)
	{
		acquired_desktop_image->Release();
		acquired_desktop_image = NULL;
	}

	// QI for IDXGIResource
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&acquired_desktop_image));
	DesktopResource->Release();
	DesktopResource = NULL;
	if (FAILED(hr))
	{
	//	Log::write("Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER");
		//DisplayErr(L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", L"Error", hr);
		return DUPL_RETURN_ERROR_EXPECTED;
	}

	// Get metadata
	if (FrameInfo.TotalMetadataBufferSize)
	{
		// Old buffer too small
		if (FrameInfo.TotalMetadataBufferSize > MetaDataSize)
		{
			if (meta_data_buffer)
			{
				delete[] meta_data_buffer;
				meta_data_buffer = NULL;
			}
			meta_data_buffer = new (std::nothrow) BYTE[FrameInfo.TotalMetadataBufferSize];
			if (!meta_data_buffer)
			{
				//DisplayErr(L"Failed to allocate memory for metadata in DUPLICATIONMANAGER", L"Error", E_OUTOFMEMORY);
				MetaDataSize = 0;
				data->MoveCount = 0;
				data->DirtyCount = 0;
				return DUPL_RETURN_ERROR_EXPECTED;
			}
			MetaDataSize = FrameInfo.TotalMetadataBufferSize;
		}

		UINT BufSize = FrameInfo.TotalMetadataBufferSize;
		
		
		// Get move rectangles
		/*hr = desk_dupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(meta_data_buffer), &BufSize);
		if (FAILED(hr))
		{
			if (hr != DXGI_ERROR_ACCESS_LOST)
			{
				

			//	Log::write("Failed to get frame move rects in DUPLICATIONMANAGER  " + std::string(buff));
				//DisplayErr(L"Failed to get frame move rects in DUPLICATIONMANAGER", L"Error", hr);
			}
			data->MoveCount = 0;
			data->DirtyCount = 0;
			return DUPL_RETURN_ERROR_EXPECTED;
		}*/
		data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);
		
		BYTE* DirtyRects = meta_data_buffer + BufSize;
		BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;

		// Get dirty rectangles
		/*hr = desk_dupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize);
		if (FAILED(hr))
		{
			if (hr != DXGI_ERROR_ACCESS_LOST)
			{
			//	Log::write("Failed to get frame dirty rects in DUPLICATIONMANAGER");
				//DisplayErr(L"Failed to get frame dirty rects in DUPLICATIONMANAGER", L"Error", hr);
			}
			data->MoveCount = 0;
			data->DirtyCount = 0;
			return DUPL_RETURN_ERROR_EXPECTED;
		}*/
		data->DirtyCount = BufSize / sizeof(RECT);

		data->MetaData = meta_data_buffer;
	}

	data->Frame = acquired_desktop_image;
	data->FrameInfo = FrameInfo;
	return DUPL_RETURN_SUCCESS;
}

HRESULT DuplicationMan::get_shape(SHAPE_DATA & res)
{
	HRESULT hr = S_OK;
	res.buffer.resize(1000 * 1000 * 3);
	UINT s_res = 0;
	
	hr = desk_dupl->GetFramePointerShape(res.buffer.size(), res.buffer.data(), &s_res, &res.info);
	res.buffer.resize(s_res);
	return hr;

}




HRESULT DuplicationMan::done_with_frame()
{
	HRESULT hr = S_OK;

	hr = desk_dupl->ReleaseFrame();
	if (FAILED(hr))
	{
		//Log::write("Failed to release frame in DUPLICATIONMANAGER");
		//DisplayErr(L"Failed to release frame in DUPLICATIONMANAGER", L"Error", hr);
		return hr;
	}

	if (acquired_desktop_image)
	{
		acquired_desktop_image->Release();
		acquired_desktop_image = NULL;
	}

	return hr;
}


DUPL_RETURN DuplicationMan::ProcessFailure(_In_opt_ ID3D11Device* Device, _In_ LPCWSTR Str, _In_ LPCWSTR Title, HRESULT hr, _In_opt_z_ HRESULT* ExpectedErrors)
{
	HRESULT TranslatedHr;

	// On an error check if the DX device is lost
	if (Device)
	{
		HRESULT DeviceRemovedReason = Device->GetDeviceRemovedReason();

		switch (DeviceRemovedReason)
		{
		case DXGI_ERROR_DEVICE_REMOVED:
		case DXGI_ERROR_DEVICE_RESET:
			case static_cast<HRESULT>(E_OUTOFMEMORY) :
			{
				// Our device has been stopped due to an external event on the GPU so map them all to
				// device removed and continue processing the condition
				TranslatedHr = DXGI_ERROR_DEVICE_REMOVED;
				break;
			}

			case S_OK:
			{
				// Device is not removed so use original error
				TranslatedHr = hr;
				break;
			}

			default:
			{
				// Device is removed but not a error we want to remap
				TranslatedHr = DeviceRemovedReason;
			}
		}
	}
	else
	{
		TranslatedHr = hr;
	}

	// Check if this error was expected or not
	if (ExpectedErrors)
	{
		HRESULT* CurrentResult = ExpectedErrors;

		while (*CurrentResult != S_OK)
		{
			if (*(CurrentResult++) == TranslatedHr)
			{
				return DUPL_RETURN_ERROR_EXPECTED;
			}
		}
	}

	std::wcout << Str << Title << TranslatedHr << std::endl;
	// Error was not expected so display the message box

	return DUPL_RETURN_ERROR_UNEXPECTED;
}
