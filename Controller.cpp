#include "Controller.hpp"
#include "global.hpp"
#include "pluginterfaces\base\ibstream.h"
#include "base\source\fstreamer.h"

//---------------------------------------------

Controller::Controller() : m_editor(0) {
	setKnobMode(KnobModes::kLinearMode);

	std::wstring name;
	for (int32_t a = NUMBER_OF_INPUT_WEIGHTS + NUMBER_OF_OUTPUT_WEIGHTS - 1; a >= 0; a--) {
		name = (a < NUMBER_OF_INPUT_WEIGHTS ? L"Input weight" : L"Output weight") + std::to_wstring(a);
		parameters.addParameter(new RangeParameter(name.c_str(), a, 0, -1., 1., 0.));
	}
}

//---------------------------------------------
// From EditController

IPlugView* PLUGIN_API Controller::createView(FIDString p_name) {
	if (ConstString(p_name) == ViewType::kEditor) {
		return new Editor(this);
	}
	return nullptr;
}
tresult PLUGIN_API Controller::setParamNormalized(ParamID p_tag, ParamValue p_value) {
	parameters.getParameter(p_tag)->setNormalized(p_value);
	if (m_editor != nullptr) {
		m_editor->updateParameterUI(p_tag, p_value);
	}
	return kResultOk;
}

void Controller::editorAttached(EditorView* p_editor) {
	m_editor = (Editor*)p_editor;
	m_editor->addRef();
}
void Controller::editorRemoved(EditorView* p_editor) {
	m_editor->release();
	m_editor = 0;
}

tresult PLUGIN_API Controller::setComponentState(IBStream* p_data) {
	if (!p_data) return kResultFalse;

	IBStreamer streamer(p_data);
	for (int a = 0; a < NUMBER_OF_INPUT_WEIGHTS; a++) {
		double value;
		streamer.readDouble(value);

		if (m_editor) {
			m_editor->updateParameterUI(a, value*0.5 + 0.5);
		}
		parameters.getParameterByIndex(a)->setNormalized(value*0.5 + 0.5);
	}

	return kResultOk;
}

//---------------------------------------------

FUnknown* Controller::createInstance(void*) {
	return (IEditController*)new Controller();
}
FUID Controller::classID(3074007437, 1699037524, 2989979229, 1730931819);
