# Game-facing Present boundary

CMake copies production `GameSwapChain.cpp` and `.h` unchanged. Recording
facades replace only the native DXGI boundary, host preparation/completion,
and timing service. The fixture invokes actual outer Present/Present1 code;
unexercised buffer/device/resize paths explicitly return E_NOTIMPL.

The original NVIDIA/ordinary route submitted a frame when preparation had
just latched a terminal host failure. RED has four failed assertions across
Present and Present1. The two post-preparation failure checks prevent native
submission and completion while preserving successful/failing native calls,
argument forwarding, TEST semantics, existing failures and FSR rejection.

This is a CPU dispatch test. It does not emulate Streamline, GPU retirement,
Skyrim hooks, UI rendering or mouse clicks. Those have separate fixtures and
the owner-operated End-menu gate.
