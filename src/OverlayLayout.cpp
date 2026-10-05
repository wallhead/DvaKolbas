#include "OverlayLayout.h"

#include <imgui.h>

namespace TheosRenderPipeline::Overlay
{
ColumnSizes DrawColumnSplitter(float width, float, float&)
{
    // Keep old saved layout values readable, but the ordinary menu uses one column.
    return {(std::max)(1.0f, width), 0.0f};
}
} // namespace TheosRenderPipeline::Overlay
