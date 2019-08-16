#include "global.hpp"
#include "WeightsEditor.hpp"
#include "Controller.hpp"
#include "pluginterfaces/vst/ivstcontextmenu.h"

//
// Private
//

void WeightsEditor::editAtPosition(double p_x, double p_y, double p_px, double p_py, bool p_willZero) {
	CRect viewRect(getViewSize());
	p_x = getClipped(p_x, viewRect.left, viewRect.right - 0.001);
	p_y = getClipped(p_y, viewRect.top, viewRect.bottom);
	
	p_px = getClipped(p_px, viewRect.left, viewRect.right - 0.001);
	p_py = getClipped(p_py, viewRect.top, viewRect.bottom);

	uint32_t index = getParameterIndexFromMousePosition(p_x, p_y);
	m_weights[index] = p_willZero ? 0 : -2.*(p_y - viewRect.top)/viewRect.getHeight() + 1.;
	m_controller->beginEdit(index);
	m_controller->performEdit(index, m_weights[index]*0.5 + 0.5);
	m_controller->endEdit(index);

	uint32_t indexBefore = getParameterIndexFromMousePosition(p_px, p_py);

	if (index != indexBefore) {
		uint32_t minIndex = index < indexBefore ? index : indexBefore;
		uint32_t maxIndex = index > indexBefore ? index : indexBefore;
		for (uint32_t a = minIndex + 1; a < maxIndex; a++) {
			m_weights[a] = p_willZero ? 0 :
				m_weights[minIndex] * (1. - double(a - minIndex) / double(maxIndex - minIndex)) + 
				m_weights[maxIndex] * double(a - minIndex)/double(maxIndex - minIndex);

			m_controller->beginEdit(a);
			m_controller->performEdit(a, m_weights[a] * 0.5 + 0.5);
			m_controller->endEdit(a);
		}
	}


	invalid();
}

//
// Public
//

WeightsEditor::WeightsEditor(Controller* p_controller, uint32_t p_parameterIndexOffset, uint32_t p_numberOfWeights) : 
	CView(CRect()), m_controller(p_controller), m_parameterIndexOffset(p_parameterIndexOffset), m_numberOfWeights(p_numberOfWeights)
{
	m_font = new CFontDesc("Microsoft Sans Serif, Trebuchet MS, Lucidia Sans Unicode, Arial", 8);

	//------------------------------------------------

	double centerOffset = double((m_numberOfWeights - 1) / 2);
	double s = 40;
	double smallValue = 0.00001;

	m_weights = new double[m_numberOfWeights];
	for (uint32_t a = 0; a < m_numberOfWeights; a++) {
		m_weights[a] = m_controller->getParameterObject(a + m_parameterIndexOffset)->getNormalized()*2. - 1.;
	}

	//for (uint32_t a = 0; a < m_numberOfWeights; a++) {
	//	m_weights[a] = (0.08 + 0.46*(1. + cos(PI*((double)a - centerOffset + smallValue)/centerOffset)))*sin(3.*PI*((double)a-centerOffset + smallValue) / centerOffset)/((double)a-centerOffset + smallValue);
	//	//m_weights[a] = 2.5*s*exp(-((double)a - centerOffset)*((double)a - centerOffset) / (2.*s*s))/sqrt(2*PI*s*s);
	//	m_controller->beginEdit(a);
	//	m_controller->performEdit(a, m_weights[a] * 0.5 + 0.5);
	//	m_controller->endEdit(a);
	//}

	//------------------------------------------------

	setMouseEnabled();
}
WeightsEditor::~WeightsEditor() {
	delete[] m_weights;
	delete m_font;
}

//---------------------------------------------

CMouseEventResult WeightsEditor::onMouseDown(CPoint& p_position, const CButtonState& p_buttonState) {
	if (p_buttonState.isLeftButton()) {
		editAtPosition(p_position.x, p_position.y, p_position.x, p_position.y, p_buttonState.getModifierState() & (1 << 6));
		return CMouseEventResult::kMouseEventHandled;
	}
	else if (p_buttonState.isRightButton()) {
		if (p_buttonState.isRightButton()) {
			FUnknownPtr<IComponentHandler3> componentHandler(m_controller->getComponentHandler());
			if (componentHandler.get()) {
				ParamID* tag = new ParamID(getParameterIndexFromMousePosition(p_position.x, p_position.y));
				IContextMenu* contextMenu = componentHandler->createContextMenu((Editor*)getEditor(), tag);
				contextMenu->popup(p_position.x, p_position.y);
				contextMenu->release();
				delete tag;
			}
		}
	}
	return CMouseEventResult::kMouseEventNotHandled;
}
CMouseEventResult WeightsEditor::onMouseMoved(CPoint& p_position, const CButtonState& p_buttonState) {
	if (p_buttonState.isLeftButton()) {
		editAtPosition(p_position.x, p_position.y, m_lastMousePosition.x, m_lastMousePosition.y, p_buttonState.getModifierState() & (1 << 6));
		m_lastMousePosition = p_position;
		return CMouseEventResult::kMouseEventHandled;
	}
	m_lastMousePosition = p_position;
	return CMouseEventResult::kMouseEventNotHandled;
}

void WeightsEditor::setWeight(uint32_t p_index, double p_weight) {
	m_weights[p_index] = p_weight;
	invalid();
}

//---------------------------------------------

void WeightsEditor::draw(CDrawContext* p_drawingContext) {
	CRect viewRect(getViewSize());

	double verticalCenter = (viewRect.top + viewRect.bottom)*0.5;

	p_drawingContext->setDrawMode(CDrawModeFlags::kAntiAliasing);

	p_drawingContext->setFillColor(m_color);
	for (uint32_t a = 0; a < m_numberOfWeights; a++) { 
		p_drawingContext->drawRect(
			CRect(
				viewRect.left + viewRect.getWidth()*(double)a/(double)m_numberOfWeights, 
				verticalCenter - m_weights[a]*viewRect.getHeight()*0.5f,
				viewRect.left + viewRect.getWidth()*(double)(a + 1)/(double)m_numberOfWeights,
				verticalCenter
			), CDrawStyle::kDrawFilled
		);
	}

	p_drawingContext->setFrameColor(CColor(50, 50, 50));
	p_drawingContext->drawLine(CPoint(viewRect.left, verticalCenter), CPoint(viewRect.right, verticalCenter));
	p_drawingContext->drawRect(viewRect);
}
