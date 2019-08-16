#pragma once
#include "public.sdk\source\vst\vstguieditor.h"
#include "base\source\fdebug.h"

using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace VSTGUI;

//---------------------------------------------

#define PADDING 5.
#define LABEL_HEIGHT 24.

//---------------------------------------------

class WeightsEditor;

class Editor : public VSTGUIEditor {
private:
	WeightsEditor* m_inputWeightsEditor;
	//WeightsEditor* m_outputWeightsEditor;

public:
	Editor(EditController* p_controller);
	~Editor();

	//------------------------------------------------
	// From VSTGUIEditor

	bool PLUGIN_API open(void* p_parent, const PlatformType& p_platformType) override;
	void PLUGIN_API close() override;

	tresult PLUGIN_API onSize(ViewRect* p_newSize) override;
	tresult PLUGIN_API canResize() override;
	tresult PLUGIN_API checkSizeConstraint(ViewRect* p_rect) override;

	//------------------------------------------------
	// From IControlListener

	// Updates a parameter when a knob is turned
	//void valueChanged(CControl* p_control) override;

	// Opens host context menu when right-clicking a knob
	//int controlModifierClicked(CControl* p_control, CButtonState p_buttonState) override;

	//------------------------------------------------

	// Updates a knob when a parameter is changed
	void updateParameterUI(ParamID p_tag, ParamValue p_value);
};
