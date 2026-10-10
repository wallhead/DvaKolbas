#pragma once
#include <cstdint>
namespace TheosRenderPipeline
{
    // Diagnostic bookkeeping only; never influences SDK timing/admission.
    class XessGenerationPacingWindow
    {
    public:
        static constexpr unsigned Size=64,Period=600;
        void Reset() { cycle_=index_=0;source_=epoch_=generation_=0; }
        bool Begin(std::uint64_t source,std::uint64_t epoch,std::uint64_t generation)
        {
            if(source_ && (source!=source_+1 || epoch!=epoch_ || generation!=generation_))Reset();
            source_=source;epoch_=epoch;generation_=generation;
            return Capturing();
        }
        bool Capturing() const { return cycle_<Size; }
        unsigned Index() const { return index_; }
        bool Commit()
        {
            const bool complete=Capturing() && ++index_==Size;
            if(complete)index_=0;
            cycle_=(cycle_+1)%Period;
            return complete;
        }
    private:
        unsigned cycle_{},index_{};
        std::uint64_t source_{},epoch_{},generation_{};
    };
}
