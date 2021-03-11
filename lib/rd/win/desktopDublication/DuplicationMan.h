#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <sal.h>
#include <new>
#include <warning.h>
#include <DirectXMath.h>
#include <vector>
#include <iostream>
#pragma comment(lib, "D3D11.lib")

struct SHAPE_DATA
{
	DXGI_OUTDUPL_POINTER_SHAPE_INFO info;
	std::vector<uint8_t> buffer;
};


typedef struct _FRAME_DATA
{
	ID3D11Texture2D* Frame;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
	_Field_size_bytes_((MoveCount * sizeof(DXGI_OUTDUPL_MOVE_RECT)) + (DirtyCount * sizeof(RECT))) BYTE* MetaData;
	UINT DirtyCount;
	UINT MoveCount;
} FRAME_DATA;


typedef _Return_type_success_(return == DUPL_RETURN_SUCCESS) enum
{
	DUPL_RETURN_SUCCESS = 0,
	DUPL_RETURN_ERROR_EXPECTED = 1,
	DUPL_RETURN_ERROR_UNEXPECTED = 2
}DUPL_RETURN;

class DuplicationMan
{
public:
	DuplicationMan();
	~DuplicationMan();	
	DUPL_RETURN get_frame(FRAME_DATA * data, bool* Timeout);
	HRESULT get_shape(SHAPE_DATA &res);
	HRESULT done_with_frame();
	DUPL_RETURN init(ID3D11Device* Device, UINT Output);
	void GetOutputDesc(_Out_ DXGI_OUTPUT_DESC* DescPtr);
private:
	DUPL_RETURN ProcessFailure(_In_opt_ ID3D11Device* Device, _In_ LPCWSTR Str, _In_ LPCWSTR Title, HRESULT hr, _In_opt_z_ HRESULT* ExpectedErrors);
private:
	ID3D11Device* m_device;
	UINT m_OutputNumber;
	DXGI_OUTPUT_DESC m_OutputDesc;

	IDXGIOutputDuplication *desk_dupl;
	ID3D11Texture2D*acquired_desktop_image;
	_Field_size_bytes_(m_MetaDataSize) BYTE* meta_data_buffer;
	UINT MetaDataSize;



	
};

