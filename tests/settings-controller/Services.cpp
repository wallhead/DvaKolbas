#include "TextureProviderBridge.h"
// Optional external TextureDownscaler is absent in this fixture.
bool TextureProviderBridge::Read(Settings&,Telemetry*){return false;}
bool TextureProviderBridge::Apply(const Settings&,bool){return false;}
