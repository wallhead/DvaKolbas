#include "FrameGen/XessGenerationInputHandoff.h"
#include "FrameGen/XessGenerationPacingWindow.h"
#include <cstdio>
#include <cstdlib>
#include <thread>
using TheosRenderPipeline::XessGenerationInputHandoff;
static void Require(bool value,const char* message)
{if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
int main()
{
    TheosRenderPipeline::XessGenerationPacingWindow window;
    for(std::uint64_t source=1;source<=64;++source) {
        Require(window.Begin(source,1,1),"first burst retains consecutive source samples");
        Require(window.Commit()==(source==64),"only a complete bounded burst can be emitted");
    }
    Require(!window.Begin(65,1,1),"completed burst enters sparse cooldown");window.Commit();
    Require(window.Begin(66,1,2) && window.Index()==0,"lifecycle change starts a new burst even with consecutive source IDs");window.Commit();
    Require(window.Begin(68,1,2) && window.Index()==0,"source gap drops partial adjacent burst");window.Commit();
    Require(window.Begin(69,2,2) && window.Index()==0,"epoch change drops partial adjacent burst");window.Commit();
    window.Reset();Require(window.Begin(70,2,2) && window.Index()==0,"diagnostics/menu reset drops partial burst");
    XessGenerationInputHandoff handoff;
    const auto unarmed=handoff.Begin(5,17);
    Require(!unarmed,"unarmed worker cannot borrow a source");
    Require(!handoff.Arm(99,7,6),"unarmed poll in flight prevents publication");
    Require(!handoff.Complete(unarmed,7,17),"unarmed poll returns without source admission");
    Require(handoff.Arm(100,7,10),"owner publishes next source after completed pre-input sleep");
    XessGenerationInputHandoff early;
    Require(early.Arm(50,7,10),"separate early-input source armed");
    auto tooEarly=early.Begin(9,17);
    Require(!tooEarly && !early.Complete(tooEarly,12,17) && !early.Consume(50,7),"input preceding sleep/publication cannot qualify");
    bool completed{};
    std::thread worker([&]{auto ticket=handoff.Begin(11,17);Require(bool(ticket),"worker retains exact source ticket");
        Require(!handoff.Consume(100,7),"in-flight input cannot qualify rendering");completed=handoff.Complete(ticket,12,17);});
    worker.join();Require(completed,"same worker records actual input return");
    Require(!handoff.Consume(101,7) && !handoff.Consume(100,8),"source/epoch must match exactly");
    Require(handoff.Consume(100,7) && !handoff.Consume(100,7),"owner consumes proven input exactly once");
    Require(!handoff.Arm(100,7,13),"extra Present cannot recreate a consumed source");
    Require(handoff.Arm(101,7,14),"next genuine source can be armed");
    auto interrupted=handoff.Begin(15,23);Require(bool(interrupted),"input pending before interruption");
    handoff.Cancel();Require(!handoff.Complete(interrupted,16,23) && !handoff.Consume(101,7),"cancelled input cannot cross interruption");
    Require(handoff.Arm(102,7,17),"next source recovers after worker completion");
    auto first=handoff.Begin(18,17);Require(bool(first),"first job for a source starts");
    auto overlap=handoff.Begin(19,23);
    Require(!overlap,"duplicate/overlapping job poisons admission");
    Require(!handoff.Complete(first,20,17) && !handoff.Consume(102,7),"ambiguous jobs never manufacture one completion");
    Require(!handoff.Arm(103,7,20),"rejected overlapping job still owns its physical input lifetime");
    Require(!handoff.Complete(overlap,20,23),"overlapping worker returns without admission");
    Require(handoff.Arm(103,7,21),"fresh source after overlap");
    auto foreign=handoff.Begin(22,17);Require(bool(foreign),"ticket thread established");
    Require(!handoff.Complete(foreign,23,23) && !handoff.Consume(103,7),"foreign completion cannot replace original worker");
    // Original worker must still complete before any new source is publishable.
    Require(!handoff.Arm(104,7,24),"new source cannot retire an active worker");
    Require(!handoff.Complete(foreign,25,17),"poisoned active worker completes without admission");
    Require(handoff.Arm(104,7,26),"recovery does not block or wait on workers");
    auto old=handoff.Begin(27,17);Require(bool(old),"old ticket captured");
    Require(handoff.Complete(old,28,17) && handoff.Consume(104,7),"old cycle finishes");
    Require(handoff.Arm(105,8,29),"epoch change publishes distinct generation");
    Require(!handoff.Complete(old,30,17) && !handoff.Consume(105,8),"stale completion cannot contaminate new epoch");
    auto current=handoff.Begin(31,23);
    Require(bool(current) && handoff.Complete(current,32,23) && handoff.Consume(105,8),"new epoch uses its own completed input");
    auto late=handoff.Begin(33,17);
    Require(!late && !handoff.Seal(105,8),"late input after render consumption prevents native tags");
    Require(!handoff.Complete(late,34,17),"late job retains lifetime until actual return");
    Require(handoff.Arm(106,8,35),"fresh cycle after rejected late input");
    auto cancelled=handoff.Begin(36,17);
    Require(handoff.Complete(cancelled,37,17) && handoff.Consume(106,8),"render consumed before foreign cancellation");
    handoff.Cancel();Require(!handoff.Seal(106,8),"foreign cancellation before publication rejects tags");
    Require(handoff.Arm(107,8,38),"next source after cancelled publication");
    auto sealed=handoff.Begin(39,17);
    Require(handoff.Complete(sealed,40,17) && handoff.Consume(107,8) && handoff.Seal(107,8),"intact completed render seals once");
    Require(!handoff.Seal(107,8),"extra Present cannot reuse sealed input proof");
    auto future=handoff.Begin(41,23);
    Require(!future && !handoff.Arm(108,8,42) && !handoff.Complete(future,43,23),"post-seal job cannot be borrowed by next-source sleep");
    Require(handoff.Arm(108,8,44),"new source arms after genuine future job returns");
    using Next=XessGenerationInputHandoff::NextSource;
    Require(XessGenerationInputHandoff::AfterPresent(true,false,true,false,false)==Next::Preserve,
        "harmless repeated Present preserves the existing next-source reservation");
    auto duplicate=handoff.Begin(45,17);
    Require(handoff.Complete(duplicate,46,17) && handoff.Consume(108,8) && handoff.Seal(108,8),
        "input following harmless duplicate still qualifies its exact source");
    Require(XessGenerationInputHandoff::AfterPresent(true,true,false,false,true)==Next::Reserve,
        "genuine reset temporal output may reserve its next source");
    Require(XessGenerationInputHandoff::AfterPresent(true,false,true,true,false)==Next::Cancel &&
        XessGenerationInputHandoff::AfterPresent(false,true,false,false,false)==Next::Cancel &&
        XessGenerationInputHandoff::AfterPresent(true,false,true,false,true)==Next::Cancel &&
        XessGenerationInputHandoff::AfterPresent(true,false,false,false,false)==Next::Cancel,
        "menu, failure, reset duplicate and spatial recovery invalidate reservation");
    std::puts("PASS: exact-source worker input handoff without foreign-thread SDK markers");
}
