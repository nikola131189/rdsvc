#pragma once
#include "DuplicationMan.h"
#include <iostream>
#include <thread>
#include <mutex>

struct point_t
{
	int x, y;
};

enum shape_type : uint8_t
{
	format_rgba32,
	format_alpha8
};
struct shape_t
{
	point_t pos;
	shape_type type;
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
	point_t hot_spot;
	bool visible;
	std::vector<uint8_t> data;
};


class DXGI_screen_reader
{
public:
	DXGI_screen_reader();
	~DXGI_screen_reader();
	bool init(uint32_t w, uint32_t h);
	int frame(uint8_t *f, shape_t &shape);
private:
	HRESULT initDx();
	int initReader();

	DUPL_RETURN CopyDirty32to24(ID3D11Texture2D* SrcSurface, unsigned char* dest);
	void get_shape(shape_t& shape);




private:
	std::thread m_thr;
	std::mutex m_mut;
	FRAME_DATA m_fd;
	int width, height;
	DuplicationMan dupl_man;
	ID3D11Device *m_Device;
	ID3D11DeviceContext* m_Context;
	ID3D11SamplerState*  m_SamplerLinear;
	ID3D11Texture2D*	 m_MySharedSurf;

	DXGI_OUTPUT_DESC DesktopDesc;
	D3D11_TEXTURE2D_DESC m_MySurfDescr;

	SHAPE_DATA m_shape_data;
	int _xhot, _yhot;
};

