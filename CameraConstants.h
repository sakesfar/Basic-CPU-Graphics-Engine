#pragma once

namespace CameraConstants
{	
	constexpr uint32_t imageWidth = 640;
	constexpr uint32_t imageHeight = 480;

	constexpr float inchToMm = 25.4;

	constexpr float filmW = 0.980 * inchToMm;
	constexpr float filmH = 0.735 * inchToMm;
	constexpr float focalL = 20;
	constexpr float tanAOV = (filmW / 2) / focalL; // it is tan(AOV/2)!


	constexpr float farClipping = 100;
	constexpr float nearClipping = 1;

	constexpr float filmAspectRatio = filmW / filmH;
	constexpr float deviceAspectRatio = imageWidth / (float)imageHeight;
};