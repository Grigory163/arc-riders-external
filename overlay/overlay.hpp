#pragma once

#include "../includes/includes.hpp"

namespace overlay {
	inline ID3D11Device* device{ };
	inline ID3D11DeviceContext* device_context{ };
	inline IDXGISwapChain* swapchain{ };
	inline ID3D11RenderTargetView* render_target{ };

	inline HWND hwnd{ };
	inline MSG msg{ };

	bool initialize( );
	void loop( );
}