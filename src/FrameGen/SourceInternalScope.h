#pragma once
#include <utility>
namespace TheosRenderPipeline {
struct SourceInternalScope {
    bool& flag;bool previous;
    explicit SourceInternalScope(bool& value):flag(value),previous(std::exchange(value,true)){}
    ~SourceInternalScope(){flag=previous;}
};
}
