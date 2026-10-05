#include "OverlayLayout.h"
#include "OverlayPipeline.h"
#include "OverlayUIStyle.h"
#include <imgui_internal.h>
#include <SimpleIni.h>
#include <imgui.h>

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

using namespace TheosRenderPipeline::Overlay;
namespace
{
void Require(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}
bool Near(float a, float b)
{
    return std::abs(a - b) < 0.001f;
}

void Settings()
{
    CSimpleIniA ini;
    Require(ini.LoadData("[Settings]\nQualityLevel=4\n[SourceDLSSG]\nNRPasses=2\n[Unrecognized]\nKeep=hello\n") >= 0,
            "legacy settings load");
    const auto defaults = LoadLayout(ini);
    Require(defaults.width == 1100 && defaults.height == 720 && defaults.leftFraction == 0.5f,
            "missing layout keys keep the ordinary menu defaults");
    const Layout edited{152, 86, 1450, 920, 0.62f};
    StoreLayout(ini, edited);
    std::string serialized;
    Require(ini.Save(serialized) >= 0, "serialize INI with layout");
    CSimpleIniA restart;
    Require(restart.LoadData(serialized.c_str(), serialized.size()) >= 0, "reload saved INI");
    const auto loaded = LoadLayout(restart);
    Require(loaded.x == edited.x && loaded.y == edited.y && loaded.width == edited.width &&
                loaded.height == edited.height && Near(loaded.leftFraction, edited.leftFraction),
            "geometry and shared divider survive serialization");
    Require(restart.GetLongValue("Settings", "QualityLevel") == 4 &&
                restart.GetLongValue("SourceDLSSG", "NRPasses") == 2 &&
                std::string(restart.GetValue("Unrecognized", "Keep", "")) == "hello",
            "layout save preserves rendering and unknown keys");

    restart.SetValue("Overlay", "WindowWidth", "nan");
    restart.SetValue("Overlay", "WindowHeight", "-700");
    restart.SetValue("Overlay", "LeftColumnFraction", "garbage");
    const auto corrupt = LoadLayout(restart);
    Require(corrupt.width == 1100 && corrupt.height == 720 && corrupt.leftFraction == 0.5f,
            "malformed geometry and divider use valid defaults");
    const auto fit = FitLayout({4000, 1000, 4000, 1400, 0.65f}, 1280, 720);
    Require(fit.x == 0 && fit.y == 0 && fit.width == 1280 && fit.height == 720,
            "large ultrawide layout fits a smaller output");
    const auto small = FitLayout({-500, -300, 10, 20, 0.5f}, 640, 480);
    Require(small.x == 0 && small.y == 0 && small.width == 640 && small.height == 480,
            "small output wins over normal minimum size");
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const auto invalid = FitLayout({nan, nan, nan, nan, nan}, 1920, 1080);
    Require(invalid.x == 40 && invalid.y == 40 && invalid.width == 1100 && invalid.leftFraction == 0.5f,
            "nonfinite inputs cannot make menu unreachable");
    const auto left = FitColumns(740, 0.01f);
    const auto right = FitColumns(740, 0.99f);
    Require(left.left >= 260 && right.right >= 350 && Near(left.left + left.right + ColumnGap, 740),
            "extreme divider positions leave usable controls without overflow");
    Require(GraphHeight(900) > GraphHeight(500) && GraphHeight(5000) == 240,
            "graph grows with window but leaves room for measurements");
}

struct FrameResult
{
    ColumnSizes columns;
    ImVec2 divider;
    float width;
};
FrameResult Frame(float& fraction, ImVec2 mouse, bool down, const char* tab = "Image", float width = 1200)
{
    auto& io = ImGui::GetIO();
    io.AddMousePosEvent(mouse.x, mouse.y);
    io.AddMouseButtonEvent(ImGuiMouseButton_Left, down);
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, 700), ImGuiCond_Always);
    ImGui::Begin("Layout fixture", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::PushID(tab);
    const auto origin = ImGui::GetCursorScreenPos();
    const float available = ImGui::GetContentRegionAvail().x;
    const auto columns = DrawColumnSplitter(available, 400, fraction);
    FrameResult result{columns, ImVec2(origin.x + columns.left + ColumnGap * 0.5f, origin.y + 100), available};
    ImGui::BeginChild("left", ImVec2(columns.left, 400));
    ImGui::TextUnformatted("measurements");
    ImGui::EndChild();
    ImGui::PopID();
    ImGui::End();
    ImGui::Render();
    return result;
}

void Dragging()
{
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1920, 1080);
    io.DeltaTime = 1.0f / 60;
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    float fraction = 0.5f;
    auto first = Frame(fraction, ImVec2(-100, -100), false);
    Frame(fraction, first.divider, false);
    Frame(fraction, first.divider, true);
    auto moved = Frame(fraction, ImVec2(first.divider.x + 100, first.divider.y), true);
    Frame(fraction, ImVec2(first.divider.x + 100, first.divider.y), false);
    Require(Near(fraction, 0.5f) && Near(moved.columns.left, first.width) && moved.columns.right == 0,
            "settings must occupy the full width; mouse drag must not create a split pane");
    const float selected = fraction;
    const auto switched = Frame(fraction, ImVec2(-100, -100), false, "NR");
    Require(fraction == selected && Near(switched.columns.left, moved.columns.left),
            "shared fraction carries into a different tab");
    const auto grown = Frame(fraction, ImVec2(-100, -100), false, "NR", 1600);
    Require(fraction == selected && grown.columns.left > moved.columns.left,
            "window growth preserves divider proportion");
    // Two clicks on an off-centre divider must not implement an implicit reset.
    Frame(fraction, grown.divider, true, "NR", 1600);
    Frame(fraction, grown.divider, false, "NR", 1600);
    Frame(fraction, grown.divider, true, "NR", 1600);
    Frame(fraction, grown.divider, false, "NR", 1600);
    Require(Near(fraction, selected), "double click does not reset the divider");
    ImGui::DestroyContext();
}

void PipelineNavigation()
{
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1920, 1080);
    io.DeltaTime = 1.0f / 60;
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    PipelineDiagram diagram{};
    const SettingsPage expected[]{SettingsPage::Image,
#if !defined(TRP_NO_NEURAL_RENDERING)
                                 SettingsPage::NeuralRendering,
#endif
                                 SettingsPage::Image, SettingsPage::FrameGeneration, SettingsPage::Image};
    for (std::size_t i = 0; i < diagram.stages.size(); ++i)
        diagram.stages[i] = {"Stage", "Applied", "Open controls", expected[i]};
    diagram.nativeDetail = "1920 x 1080";
    diagram.nativeUI = true;
    diagram.status = "Healthy";
    std::array<ImVec2, PipelineDiagram::StageCount> centers;
    auto frame = [&](ImVec2 mouse, bool down, float windowWidth) {
        io.AddMousePosEvent(mouse.x, mouse.y);
        io.AddMouseButtonEvent(ImGuiMouseButton_Left, down);
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(windowWidth, 600), ImGuiCond_Always);
        ImGui::Begin("Pipeline navigation");
        auto* parent = ImGui::GetCurrentWindow();
        const auto ids = parent->IDStack.Size;
        const auto origin = ImGui::GetCursorScreenPos();
        const float gap = ImGui::GetTextLineHeight() * 2;
        const float count = static_cast<float>(diagram.stages.size());
        const float nodeWidth = (ImGui::GetContentRegionAvail().x - gap * (count - 1)) / count;
        for (std::size_t i = 0; i < centers.size(); ++i)
            centers[i] = ImVec2(origin.x + i * (nodeWidth + gap) + nodeWidth * 0.5f,
                origin.y + ImGui::GetTextLineHeight() * 2 + ImGui::GetStyle().ItemSpacing.y + 15);
        const auto requested = DrawPipelineDiagram(diagram);
        Require(ImGui::GetCurrentWindow() == parent && parent->IDStack.Size == ids,
                "clickable pipeline must leave the ImGui window and ID stacks balanced");
        ImGui::End();
        ImGui::Render();
        return requested;
    };
    for (const float windowWidth : {780.0f, 1400.0f})
    {
        frame(ImVec2(-100, -100), false, windowWidth);
        for (std::size_t i = 0; i < diagram.stages.size(); ++i)
        {
            const auto center = centers[i];
            frame(center, false, windowWidth);
            frame(center, true, windowWidth);
            Require(frame(center, false, windowWidth) == expected[i],
                    "pipeline click must open the matching settings page at narrow and wide sizes");
        }
        // A runtime update during a click must not cancel navigation.
        const auto center = centers[PipelineDiagram::GenerationStage];
        frame(center, false, windowWidth);
        frame(center, true, windowWidth);
        diagram.stages[PipelineDiagram::GenerationStage].detail = "Updated";
        Require(frame(center, false, windowWidth) == SettingsPage::FrameGeneration,
                "changing applied status during a mouse press must preserve the stage's click target");
        diagram.stages[PipelineDiagram::GenerationStage].detail = "Applied";
    }
    ImGui::DestroyContext();
}

void StockStyle()
{
    ImGui::CreateContext();
    ImGuiStyle baseline;
    ImGui::StyleColorsDark(&baseline);
    auto& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(32, 32);
    style.FramePadding = ImVec2(20, 20);
    style.WindowRounding = style.FrameRounding = 12;
    style.Colors[ImGuiCol_Button] = ImVec4(1, 0.5f, 0, 1);
    ApplyRendererStyle();
    Require(style.WindowPadding.x == baseline.WindowPadding.x && style.WindowPadding.y == baseline.WindowPadding.y &&
                style.FramePadding.x == baseline.FramePadding.x && style.FramePadding.y == baseline.FramePadding.y &&
                style.CellPadding.x == baseline.CellPadding.x && style.CellPadding.y == baseline.CellPadding.y &&
                style.ItemSpacing.x == baseline.ItemSpacing.x && style.ItemSpacing.y == baseline.ItemSpacing.y &&
                style.ItemInnerSpacing.x == baseline.ItemInnerSpacing.x && style.ItemInnerSpacing.y == baseline.ItemInnerSpacing.y &&
                style.WindowRounding == baseline.WindowRounding && style.FrameRounding == baseline.FrameRounding &&
                style.ChildRounding == baseline.ChildRounding && style.FrameBorderSize == baseline.FrameBorderSize,
            "overlay style must restore stock ImGui padding, spacing and geometry");
    for (int i = 0; i < ImGuiCol_COUNT; ++i)
        Require(style.Colors[i].x == baseline.Colors[i].x && style.Colors[i].y == baseline.Colors[i].y &&
                    style.Colors[i].z == baseline.Colors[i].z && style.Colors[i].w == baseline.Colors[i].w,
                "overlay palette must match stock StyleColorsDark for every widget");
    ImGui::DestroyContext();
}
} // namespace

int main()
{
    try
    {
        Settings();
        Dragging();
        PipelineNavigation();
        StockStyle();
        std::cout << "Layout persistence, display fitting and full-width ImGui settings passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        if (ImGui::GetCurrentContext())
            ImGui::DestroyContext();
        std::cerr << error.what() << '\n';
        return 1;
    }
}
