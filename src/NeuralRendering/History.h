#pragma once
#include "ImagePacket.h"
namespace TheosRenderPipeline::NeuralRendering {
class History;
class HistoryDecision {
public:
    bool Reset()const noexcept{return reset_;}
private:
    friend class History;
    HistoryDecision()=default;
    const History* owner_{};
    uint64_t generation_{},epoch_{},source_{},image_{},settingsRevision_{};
    double time_{};
    bool reset_{};
};
// Render/worker owners serialize access. This only tracks temporal identities;
// CommitRecorded is not submission, completion, or permission to free resources.
class History {
public:
    History()=default;
    History(const History&)=delete;
    History& operator=(const History&)=delete;
    Result<HistoryDecision> Check(const ImagePacket&,const SettingsSnapshot&)const;
    Result<void> CommitRecorded(const HistoryDecision&);
    void ResetNext()noexcept;
private:
    uint64_t generation_{},epoch_{},source_{},image_{},settingsRevision_{};
    double time_{};
    bool resetNext_{true},exhausted_{};
};
}
