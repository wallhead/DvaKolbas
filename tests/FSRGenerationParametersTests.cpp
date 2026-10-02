#include "fsr-fg/GenerationTestRig.h"
#include <cmath>
#include <limits>
using namespace GenerationFixture;
int main()
{
    Rig rig;
    auto mapped = BuildFsrGenerationPrepare(rig.list.Get(), rig.frame, rig.resources, rig.limits, true);
    Require(bool(mapped), "valid typed FG preparation");
    Require(mapped->frameID == 41 && mapped->frameTimeDelta == 16.6667f && mapped->reset, "PrepareUsesMillisecondsAndMatchingId");
    Require(mapped->jitterOffset.x == .125f && mapped->jitterOffset.y == -.25f && mapped->motionVectorScale.x == 64 && mapped->motionVectorScale.y == 64, "MotionConventionMatchesSr");
    Require(mapped->cameraNear == 10 && mapped->cameraFar == 10000 && mapped->cameraPosition[0] == 100 && mapped->viewSpaceToMetersFactor == .0142875f, "CameraBasisAndUnits: native world units and explicit meters factor");
    Require(mapped->cameraRight[0] == 1 && mapped->cameraUp[1] == 1 && mapped->cameraForward[2] == 1, "identity camera basis");
    auto rotated = rig.frame;
    rotated.camera.view = {0,0,-1,0, 0,1,0,0, 1,0,0,0, 0,0,0,1};
    mapped = BuildFsrGenerationPrepare(rig.list.Get(), rotated, rig.resources, rig.limits, false);
    Require(mapped && mapped->cameraRight[2] == 1 && mapped->cameraUp[1] == 1 && mapped->cameraForward[0] == -1, "CameraBasisAndUnits: rotated view columns map to world axes");
    auto wrong = rig.frame; wrong.camera.depthInverted = true;
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), wrong, rig.resources, rig.limits, false), "DepthFlagsMatch: incompatible depth rejected");
    wrong = rig.frame; wrong.motionConvention.includesJitter = true;
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), wrong, rig.resources, rig.limits, false), "MotionConventionMatchesSr: jitter convention mismatch rejected");
    wrong = rig.frame; wrong.camera.view = {};
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), wrong, rig.resources, rig.limits, false), "degenerate camera rejected");
    wrong = rig.frame; wrong.deltaMilliseconds = std::numeric_limits<float>::quiet_NaN();
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), wrong, rig.resources, rig.limits, false), "invalid milliseconds rejected");
    auto encoding = rig.resources; encoding.sceneEncoding = ColorEncoding::Gamma22;
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), rig.frame, encoding, rig.limits, false), "Gamma22 cannot be mislabeled as SDK sRGB");
    auto alias = rig.resources; alias.ui = alias.scene;
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), rig.frame, alias, rig.limits, false), "scene and UI cannot alias");
    // D3D12 devices are adapter singletons within a process. A second call on
    // the same adapter returns the original device, so use WARP as the foreign
    // resource owner and assert that this test really crosses the boundary.
    ComPtr<IDXGIAdapter> warp; Check(rig.factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)), "WARP adapter");
    ComPtr<ID3D12Device> other; Check(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&other)), "foreign D3D12 device");
    Require(other.Get() != rig.device12.Get(), "foreign fixture device identity is distinct");
    auto foreign = Texture(other.Get(), rig.limits.render, DXGI_FORMAT_R32_FLOAT);
    auto mixed = rig.resources; mixed.depth = foreign.Get();
    Require(!BuildFsrGenerationPrepare(rig.list.Get(), rig.frame, mixed, rig.limits, false), "ResourceDeviceMismatchRejected: foreign resource owner");
    auto infinite = rig.frame; infinite.camera.depthInfinite = true; infinite.camera.farDistance = std::numeric_limits<float>::infinity();
    auto limits = rig.limits; limits.input.depthInfinite = true;
    Require(bool(BuildFsrGenerationPrepare(rig.list.Get(), infinite, rig.resources, limits, false)), "valid infinite depth retains explicit policy");
    rig.ValidateDebug(); std::puts("PASS: typed FG PrepareV2 frame, camera, conventions and device ownership");
}
