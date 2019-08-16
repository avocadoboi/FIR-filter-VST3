#pragma once
#include "public.sdk\source\vst\vsteditcontroller.h"
#include "Editor.hpp"

using namespace Steinberg;
using namespace Steinberg::Vst;

//---------------------------------------------

class Controller : public EditController {
private:
	Editor* m_editor;

public:
	Controller();

	//------------------------------------------------
	// From EditController

	IPlugView* PLUGIN_API createView(FIDString p_name);
	tresult PLUGIN_API setParamNormalized(ParamID p_tag, ParamValue p_value);

	void editorAttached(EditorView* p_editor);
	void editorRemoved(EditorView* p_editor);

	tresult PLUGIN_API setComponentState(IBStream* p_data);

	//---------------------------------------------

	static FUnknown* createInstance(void*);
	static FUID classID;
};
