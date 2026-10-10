#pragma once
#include <cstdint>
#include <array>
#include <limits>
#include <mutex>
namespace TheosRenderPipeline
{
    class XessGenerationInputHandoff
    {
    public:
        enum class NextSource { Preserve, Cancel, Reserve };
        static NextSource AfterPresent(bool presented,bool temporal,bool repeated,bool menu,bool reset)
        {
            if(!presented || menu)return NextSource::Cancel;
            if(temporal)return NextSource::Reserve;
            return repeated && !reset?NextSource::Preserve:NextSource::Cancel;
        }
        struct Ticket { std::uint64_t id{}; bool eligible{}; explicit operator bool()const{return eligible;} };
        bool Arm(std::uint64_t source,std::uint64_t epoch,std::uint64_t ticks)
        {
            std::lock_guard lock(mutex_);
            if(pending_){armed_=false;poisoned_=true;return false;}
            if(fault_ || !source || !epoch || !ticks || (epoch==epoch_ && source<=source_))return false;
            source_=source;epoch_=epoch;armedTicks_=ticks;
            armed_=true;poisoned_=ready_=consumed_=false;begins_=0;return true;
        }
        Ticket Begin(std::uint64_t ticks,std::uint32_t thread)
        {
            std::lock_guard lock(mutex_);
            if(fault_ || !thread || !ticks || next_==std::numeric_limits<std::uint64_t>::max()){
                fault_=true;return {};
            }
            Slot* slot{};for(auto& entry:slots_)if(!entry.id){slot=&entry;break;}
            if(!slot){fault_=true;return {};}
            const bool eligible=armed_ && !poisoned_ && !consumed_ && begins_==0 && ticks>=armedTicks_ && pending_==0;
            if(armed_){++begins_;if(!eligible)poisoned_=true;}
            // Track unarmed jobs too. Sleep/publication cannot race an input
            // poll already in flight, even when no source ticket was issued.
            *slot={next_++,ticks,thread,eligible};++pending_;
            return {slot->id,eligible};
        }
        bool Complete(Ticket ticket,std::uint64_t ticks,std::uint32_t thread)
        {
            std::lock_guard lock(mutex_);
            Slot* slot{};for(auto& entry:slots_)if(entry.id && entry.id==ticket.id){slot=&entry;break;}
            if(!slot)return false;
            if(thread!=slot->thread){poisoned_=true;return false;}
            const bool valid=ticket.eligible && slot->eligible && armed_ && !poisoned_ && ticks>=slot->ticks;
            if(armed_ && ticks<slot->ticks)poisoned_=true;
            *slot={};--pending_;if(valid)ready_=true;return valid;
        }
        bool Consume(std::uint64_t source,std::uint64_t epoch)
        {
            std::lock_guard lock(mutex_);
            if(fault_ || !armed_ || poisoned_ || !ready_ || consumed_ || pending_ || source!=source_ || epoch!=epoch_)return false;
            consumed_=true;return true;
        }
        // Close the completed render's input proof immediately before native
        // publication. Late/foreign invalidation before this cut rejects tags.
        // Jobs starting after it are unarmed next-source jobs; retain their
        // lifetimes so the next pre-input sleep cannot borrow them.
        bool Seal(std::uint64_t source,std::uint64_t epoch)
        {
            std::lock_guard lock(mutex_);
            if(fault_ || !armed_ || poisoned_ || !ready_ || !consumed_ || pending_ || source!=source_ || epoch!=epoch_)return false;
            armed_=false;return true;
        }
        void Cancel()
        {std::lock_guard lock(mutex_);armed_=false;poisoned_=true;ready_=false;}
    private:
        struct Slot {std::uint64_t id{},ticks{};std::uint32_t thread{};bool eligible{};};
        std::mutex mutex_;
        std::array<Slot,8> slots_{};
        std::uint64_t next_{1},source_{},epoch_{},armedTicks_{};
        unsigned pending_{},begins_{};
        bool armed_{},poisoned_{},ready_{},consumed_{},fault_{};
    };
}
