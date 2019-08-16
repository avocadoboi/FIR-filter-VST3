#include "public.sdk\source\main\pluginfactoryvst3.h"
#include "Processor.hpp"
#include "Controller.hpp"

//---------------------------------------------

BEGIN_FACTORY_DEF("AvoBoi", "bjornsundin.com", "bjornsundin02@gmail.com")

DEF_CLASS2(
	INLINE_UID_FROM_FUID(Processor::classID),
	PClassInfo::kManyInstances,
	kVstAudioEffectClass,
	"Filter VST",
	kDistributable,
	"Fx",
	"0.0.0",
	kVstVersionString,
	Processor::createInstance
)

DEF_CLASS2(
	INLINE_UID_FROM_FUID(Controller::classID),
	PClassInfo::kManyInstances,
	kVstComponentControllerClass,
	"Filter VST Controller",
	0,
	"",
	"0.0.0",
	kVstVersionString,
	Controller::createInstance
)

END_FACTORY

//---------------------------------------------

bool InitModule() {
	return true;
}

bool DeinitModule() {
	return true;
}
