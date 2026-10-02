// Compile the actual presenter with the Windows min/max macros enabled, as in
// the FSR-disabled plugin. It must not rely on FSR's transitive NOMINMAX flag.
#include "FrameGen/OrdinaryPresentation.cpp"
#ifndef max
#error "This regression target must retain the Windows max macro"
#endif
int main() {
    TheosRenderPipeline::OrdinaryPresentation ordinary;
    return ordinary.Ready() || FAILED(ordinary.Retire()) ? 1 : 0;
}
