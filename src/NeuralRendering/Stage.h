#pragma once
#include "ImagePacket.h"
#include "RuntimeOwner.h"
#include "TicketOwnership.h"
#include <memory>
namespace TheosRenderPipeline::NeuralRendering {
class EvaluationTicket {
public:
    ID3D12Resource* Output()const noexcept{return output_.Get();}
    uint64_t ImageId()const noexcept{return image_;}
private:
    friend class Stage;
    Detail::TicketOwnership::Token owner_;
    uint64_t id_{},image_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> output_;
};
struct StageDiagnostics {
    uint32_t create{},evaluate{},release{},destroyParameters{},allocations{},releases{};
    uint64_t recorded{};
    bool terminal{};
};
class Stage {
public:
    // All calls, including diagnostics, are serialized by the stage's owner.
    // Current qualification is one real-image history and one pending ticket.
    Stage();~Stage();
    Stage(const Stage&)=delete;Stage& operator=(const Stage&)=delete;
    Result<void> Initialize(std::shared_ptr<RuntimeOwner>,const StageContract&,unsigned preset=0);
    Result<EvaluationTicket> Record(ID3D12GraphicsCommandList*,const ImagePacket&,const SettingsSnapshot&);
    // Caller has executed its closed list on the retained contract queue.
    // This enqueues our completion signal AFTER that work; it is not a wait.
    Result<void> MarkSubmitted(const EvaluationTicket&,ID3D12Fence*,uint64_t value);
    Result<void> TrackReader(const EvaluationTicket&,ID3D12Fence*,uint64_t value);
    Result<void> RetireTicket(const EvaluationTicket&);
    Result<void> Retire();
    StageDiagnostics Diagnostics()const;
private:
    struct State;std::unique_ptr<State> state_;
};
}
