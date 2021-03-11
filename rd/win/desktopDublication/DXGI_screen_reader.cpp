#include "DXGI_screen_reader.h"



DXGI_screen_reader::DXGI_screen_reader() : 
	m_Device(nullptr),
	m_Context(nullptr),
	m_SamplerLinear(nullptr),
	m_MySharedSurf(nullptr),
	_xhot(0), _yhot(0)
{
	m_shape_data.buffer.reserve(3 * 1024 * 1024);
}


DXGI_screen_reader::~DXGI_screen_reader()
{
	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}

	if (m_Context)
	{
		m_Context->Release();
		m_Context = nullptr;
	}

	if (m_SamplerLinear)
	{
		m_SamplerLinear->Release();
		m_SamplerLinear = nullptr;
	}

	if (m_MySharedSurf)
	{
		m_MySharedSurf->Release();
		m_MySharedSurf = nullptr;
	}
	
}

bool DXGI_screen_reader::init(uint32_t w, uint32_t h)
{
	width = w; height = h;
	if(initDx() != S_OK) return false;
	if (initReader() < 0) return false;
	return true;
}

HRESULT DXGI_screen_reader::initDx()
{
	HRESULT hr = S_OK;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

	// Feature levels supported
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

	D3D_FEATURE_LEVEL FeatureLevel;

	// Create device
	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
	{
		hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &m_Device, &FeatureLevel, &m_Context);
		if (SUCCEEDED(hr))
		{
			//LOG(INFO) << "Device creation success, no need to loop anymore";
			//Log::write("DX driver type index " + std::to_string(DriverTypeIndex) + " feature level " + std::to_string(FeatureLevel));
			// Device creation success, no need to loop anymore
			//printf("DX driver type index %d feature level %d\n", DriverTypeIndex, FeatureLevel);
			break;
		}


	}
	//LOG(INFO) << "D3D11CreateDevice " << (int)hr;
	if (FAILED(hr))
	{

		return hr;
	}

	// VERTEX shader
	/*
	UINT Size = ARRAYSIZE(g_VS);
	hr = m_Device->CreateVertexShader(g_VS, Size, nullptr, &m_VertexShader);
	if (FAILED(hr))
	{
	return ProcessFailure( L"Failed to create vertex shader in InitializeDx",  hr );
	}
	*/

	// Input layout
	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	/*
	UINT NumElements = ARRAYSIZE(Layout);
	hr = m_Device->CreateInputLayout(Layout, NumElements, g_VS, Size, &m_InputLayout);
	if (FAILED(hr))
	{
	return ProcessFailure( L"Failed to create input layout in InitializeDx", hr );
	}

	m_Context->IASetInputLayout(m_InputLayout);
	*/

	/*
	// Pixel shader
	Size = ARRAYSIZE(g_PS);
	hr = m_Device->CreatePixelShader(g_PS, Size, nullptr, &m_PixelShader);
	if (FAILED(hr))
	{
	return ProcessFailure( L"Failed to create pixel shader in InitializeDx", hr );
	}
	*/

	// Set up sampler
	D3D11_SAMPLER_DESC SampDesc;
	RtlZeroMemory(&SampDesc, sizeof(SampDesc));
	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_Device->CreateSamplerState(&SampDesc, &m_SamplerLinear);
	if (FAILED(hr))
	{
		//Log::write("Failed to create sampler state in InitializeDx");
		//return ProcessFailure(L"Failed to create sampler state in InitializeDx", hr);
	}
	return hr;

}

int DXGI_screen_reader::initReader()
{
	HDESK CurrentDesktop = nullptr;
	CurrentDesktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (!CurrentDesktop)
	{
		// cannot open current input desktop?
		return -1;
	}

	// Attach desktop to this thread
	bool DesktopAttached = SetThreadDesktop(CurrentDesktop) != 0;
	CloseDesktop(CurrentDesktop);
	CurrentDesktop = nullptr;
	if (!DesktopAttached)
	{
		return -1;
	}






	// Make duplication manager
	UINT Output = 0;
	DUPL_RETURN Ret = dupl_man.init(m_Device, Output);
	if (Ret != DUPL_RETURN_SUCCESS)
	{
		return -1;
	}

	// Get output description
	RtlZeroMemory(&DesktopDesc, sizeof(DXGI_OUTPUT_DESC));
	dupl_man.GetOutputDesc(&DesktopDesc);










	///////////////////////
	D3D11_TEXTURE2D_DESC DeskTexD;
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = DesktopDesc.DesktopCoordinates.right;
	DeskTexD.Height = DesktopDesc.DesktopCoordinates.bottom;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	DeskTexD.SampleDesc.Count = 1;
	DeskTexD.Usage = D3D11_USAGE_STAGING;
	DeskTexD.BindFlags = 0;
	DeskTexD.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	DeskTexD.MiscFlags = 0;

	




	HRESULT hr = m_Device->CreateTexture2D(&DeskTexD, nullptr, &m_MySharedSurf);
	if (FAILED(hr))
	{
		//	return ProcessFailure(L"Failed to create MY shared texture", hr);
		return -1;
	}
	return 0;
}


DUPL_RETURN DXGI_screen_reader::CopyDirty32to24(_In_ ID3D11Texture2D* SrcSurface, unsigned char* dest)
{
	m_Context->CopyResource(m_MySharedSurf, SrcSurface);
	D3D11_MAPPED_SUBRESOURCE mapped_data;
	unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
	HRESULT hr = m_Context->Map(m_MySharedSurf, subresource, D3D11_MAP_READ, 0, &mapped_data);
	m_MySharedSurf->GetDesc(&m_MySurfDescr);
	const int pitch = mapped_data.RowPitch;
	const unsigned char* source = static_cast<const unsigned char*>(mapped_data.pData);

	UINT w = width;
	UINT h = height;

	if (w > m_MySurfDescr.Width) w = m_MySurfDescr.Width;
	if (h > m_MySurfDescr.Height) h = m_MySurfDescr.Height;

	for (unsigned int i = 0; i < h; ++i)
	{
		//memcpy( dest, source, descr.Width * 4 );
		for (unsigned int x = 0; x < w; x++)
		{
			dest[x * 3 + 2] = source[x * 4 + 0];
			dest[x * 3 + 1] = source[x * 4 + 1];
			dest[x * 3 + 0] = source[x * 4 + 2];
		}
		source += pitch;
		dest += width * 3;
	}
	m_Context->Unmap(m_MySharedSurf, subresource);
	return DUPL_RETURN_SUCCESS;
}

void DXGI_screen_reader::get_shape(shape_t& shape)
{
	shape.pos.x = m_fd.FrameInfo.PointerPosition.Position.x;
	shape.pos.y = m_fd.FrameInfo.PointerPosition.Position.y;
	shape.visible = m_fd.FrameInfo.PointerPosition.Visible;
	shape.hot_spot.x = _xhot;
	shape.hot_spot.y = _yhot;

	shape.width = 0;
	shape.height = 0;

	if (m_fd.FrameInfo.PointerShapeBufferSize)
	{
		HRESULT hr = dupl_man.get_shape(m_shape_data);
		//LOG(INFO) << "dupl_man.get_shape  " << shape_data.buffer.size() << "  " << shape_data.info.Width << "  " << shape_data.info.Height << "   " << shape_data.info.Type;
		//return;

				
		shape.width = m_shape_data.info.Width;
		shape.height = m_shape_data.info.Height;
		shape.pitch = m_shape_data.info.Pitch;
		
		
		
		
		if (m_shape_data.info.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
		{
			
			if(shape.width * 2 == shape.height)
				shape.height = shape.width;
			shape.data.resize(shape.width * shape.height * 4, 0xff);

			uint8_t* ptr1 = &m_shape_data.buffer[0];
			uint8_t* ptr2 = &m_shape_data.buffer[shape.width * shape.height / 8];

			int k = 0;
			for (size_t i = 0; i < shape.width * shape.height / 8; i++)
			{
				for (size_t n = 0; n < 8; n++)
				{
					bool v1 = (*ptr1 >> n) & 1;
					bool v2 = (*ptr2 >> n) & 1;
					if ( v1 ^ v2 )
					{
						shape.data[k + 0] = 0x0;
						shape.data[k + 1] = 0x0;
						shape.data[k + 2] = 0x0;
						shape.data[k + 3] = 0x0;
					}
					k+=4;
				}
				ptr1++;
				ptr2++;
			}
		}


		if (m_shape_data.info.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR)
		{
			shape.data = m_shape_data.buffer;
			for (size_t i = 0; i < shape.width * shape.height * 4; i+=4)
			{
				uint8_t tmp = shape.data[i];
				shape.data[i] = shape.data[i + 2];
				shape.data[i + 2] = tmp;
			}
		}

		_xhot =  m_shape_data.info.HotSpot.x;
		_yhot =  m_shape_data.info.HotSpot.y;
		shape.hot_spot.x = _xhot;
		shape.hot_spot.y = _yhot;
	}
}



int DXGI_screen_reader::frame(uint8_t *f, shape_t& shape)
{
	HRESULT Ret;

	bool TimeOut = 0;
	Ret = dupl_man.get_frame(&m_fd, &TimeOut);
	if (TimeOut)
	{
		//std::cout << "timeout \n";
		return 0;
	}
	else
	{
		
	
		CopyDirty32to24(m_fd.Frame, f);
		
		//LOG(INFO) << "get_shape";
		get_shape(shape);
	}
		
	dupl_man.done_with_frame();
	if (Ret != S_OK)
	{
		return -1;
	}
	return 1;

}



