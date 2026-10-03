#pragma once
#include <memory>
namespace TheosRenderPipeline::NeuralRendering::Detail {
class TicketOwnership {
public:
    TicketOwnership()=default;
    TicketOwnership(const TicketOwnership&)=delete;
    TicketOwnership& operator=(const TicketOwnership&)=delete;
    class Token { friend class TicketOwnership;std::shared_ptr<const unsigned char> owner_; };
    Token Seal()const {Token t;t.owner_=identity_;return t;}
    bool Owns(const Token& t)const {return t.owner_==identity_;}
private:
    // Copies of sealed tickets retain the old allocation, so its identity
    // cannot be recycled into a replacement stage while any ticket survives.
    std::shared_ptr<const unsigned char> identity_=std::make_shared<const unsigned char>(0);
};
}
