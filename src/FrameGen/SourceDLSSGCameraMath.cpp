#include "SourceDLSSGCamera.h"
#include "CameraMeasurements.h"
#include "SourceDLSSGSession.h"

namespace TheosRenderPipeline::SourceDLSSG
{
	bool CameraHistory::Build(const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4X4& view,
		const sl::float3& position, float nearPlane, float farPlane, float jitterX, float jitterY,
		std::uintptr_t cameraIdentity, std::uint32_t gameFrame, bool reset, sl::Constants& result)
	{
		using namespace DirectX;
        auto measurements=TheosRenderPipeline::MeasureCamera(projection,view,{position.x,position.y,position.z},
            nearPlane,farPlane,cameraIdentity,reset);
        if(!measurements) { Reset(); return false; }
        const bool inverted=measurements->depthInverted;
        const float direction=projection._34>0?1.0f:-1.0f;
        XMFLOAT4X4 absoluteView;
        std::memcpy(&absoluteView,measurements->view.data(),sizeof(absoluteView));
		const auto p = XMLoadFloat4x4(&projection);
		const auto v = XMLoadFloat4x4(&absoluteView);
		const auto vp = v * p;
		XMVECTOR determinant;
		const auto inverseVP = XMMatrixInverse(&determinant, vp);
		if (!std::isfinite(XMVectorGetX(determinant)) || std::abs(XMVectorGetX(determinant)) < 1e-12f) { Reset(); return false; }
		const bool discontinuity = reset || !valid || identity != cameraIdentity || gameFrame != frame + 1 ||
			depthInverted != inverted;
		const auto previousVP = discontinuity ? vp : XMLoadFloat4x4(&previous);
		const auto clipToPrevious = inverseVP * previousVP;
		auto copy = [](sl::float4x4& to, FXMMATRIX from) {
			XMFLOAT4X4 packed; XMStoreFloat4x4(&packed, from);
			static_assert(sizeof(packed) == sizeof(to));
			std::memcpy(&to, &packed, sizeof(to));
		};
		result = sl::Constants{};
		copy(result.cameraViewToClip, p);
		copy(result.clipToCameraView, XMMatrixInverse(nullptr, p));
		copy(result.clipToLensClip, XMMatrixIdentity());
		copy(result.clipToPrevClip, clipToPrevious);
		copy(result.prevClipToClip, XMMatrixInverse(nullptr, clipToPrevious));
		result.cameraPos = position;
		result.cameraRight = { view._11, view._21, view._31 };
		result.cameraUp = { view._12, view._22, view._32 };
		result.cameraFwd = { direction * view._13, direction * view._23, direction * view._33 };
		result.cameraNear = nearPlane; result.cameraFar = farPlane;
		result.cameraFOV = measurements->verticalFovRadians;
		result.cameraAspectRatio = projection._22 / projection._11;
		result.jitterOffset = { jitterX, jitterY };
		result.mvecScale = { 1, 1 }; // Skyrim's guides already contain normalized screen motion.
		result.cameraPinholeOffset = { 0, 0 };
		result.depthInverted = inverted ? sl::eTrue : sl::eFalse;
		result.cameraMotionIncluded = sl::eTrue;
		result.motionVectors3D = sl::eFalse;
		result.reset = discontinuity ? sl::eTrue : sl::eFalse;
		if (!Session::ValidConstants(result)) { Reset(); return false; }
		XMStoreFloat4x4(&previous, vp);
		identity = cameraIdentity; frame = gameFrame; depthInverted = inverted; valid = true;
		return true;
	}
}
