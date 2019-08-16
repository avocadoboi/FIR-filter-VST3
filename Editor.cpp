#include "Editor.hpp"
#include "global.hpp"
#include "pluginterfaces\vst\ivstcontextmenu.h"
#include "WeightsEditor.hpp"

//---------------------------------------------

Editor::Editor(EditController* p_controller) : 
	VSTGUIEditor(p_controller, new ViewRect(0, 0, 600, 500)) 
{
}
Editor::~Editor() {
}

//------------------------------------------------
// From VSTGUIEditor

bool PLUGIN_API Editor::open(void* p_parent, const PlatformType& p_platformType) {
	frame = new CFrame(CRect(0, 0, rect.right, rect.bottom), this);
	frame->open(p_parent, p_platformType);

	frame->idleRate = 60;
	frame->setMouseEnabled(true);
	frame->setBackgroundColor(CColor(22, 15, 20));

	m_inputWeightsEditor = new WeightsEditor((Controller*)controller, 0, NUMBER_OF_INPUT_WEIGHTS);
	m_inputWeightsEditor->setColor(CColor(255, 90, 180));
	frame->addView(m_inputWeightsEditor);

	//m_outputWeightsEditor = new WeightsEditor((Controller*)controller, NUMBER_OF_INPUT_WEIGHTS, NUMBER_OF_OUTPUT_WEIGHTS);
	//m_outputWeightsEditor->setColor(CColor(50, 180, 255));
	//frame->addView(m_outputWeightsEditor);

	m_inputWeightsEditor->setWeight(NUMBER_OF_INPUT_WEIGHTS - 1, 1.);
	controller->beginEdit(NUMBER_OF_INPUT_WEIGHTS - 1);
	controller->performEdit(NUMBER_OF_INPUT_WEIGHTS - 1, 1.);
	controller->endEdit(NUMBER_OF_INPUT_WEIGHTS - 1);

	//------------------------------------------------

	return true;
}
void PLUGIN_API Editor::close() {
	frame->close();
}

tresult PLUGIN_API Editor::onSize(ViewRect* p_newSize) {
	frame->setSize(p_newSize->getWidth(), p_newSize->getHeight());

	m_inputWeightsEditor->setViewSize(CRect(PADDING, PADDING, p_newSize->right - PADDING, p_newSize->bottom - PADDING), false);
	//m_inputWeightsEditor->setViewSize(CRect(PADDING, PADDING, p_newSize->right - PADDING, 0.5*(p_newSize->bottom - PADDING)), false);
	m_inputWeightsEditor->setMouseableArea(m_inputWeightsEditor->getViewSize());

	//m_outputWeightsEditor->setViewSize(CRect(PADDING, 0.5*(p_newSize->bottom + PADDING), p_newSize->right - PADDING, p_newSize->bottom - PADDING), false);
	//m_outputWeightsEditor->setMouseableArea(m_outputWeightsEditor->getViewSize());

	return kResultOk;
}
tresult PLUGIN_API Editor::canResize() {
	return kResultTrue;
}
tresult PLUGIN_API Editor::checkSizeConstraint(ViewRect* p_rect) {
	return kResultOk;
}

//------------------------------------------------

void Editor::updateParameterUI(ParamID p_tag, ParamValue p_value) {
	if (p_tag < NUMBER_OF_INPUT_WEIGHTS) {
		m_inputWeightsEditor->setWeight(p_tag, p_value*2. - 1.);
	}
	else {
		//m_outputWeightsEditor->setWeight(p_tag - NUMBER_OF_INPUT_WEIGHTS, p_value*2. - 1.);
	}
}
