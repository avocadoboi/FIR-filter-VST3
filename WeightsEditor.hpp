#pragma once
#include "base\source\fdebug.h"
#include "vstgui4/vstgui/vstgui.h"

using namespace VSTGUI;

//---------------------------------------------

class Controller;

class WeightsEditor : public CView {
private:
	Controller* m_controller;
	CFontDesc* m_font;
	uint32_t m_parameterIndexOffset;
	uint32_t m_numberOfWeights;
	CColor m_color;

	CPoint m_lastMousePosition;

	double* m_weights;

	uint32_t getParameterIndexFromMousePosition(double p_x, double p_y) {
		return m_parameterIndexOffset + uint32_t(m_numberOfWeights * (p_x - getViewSize().left) / getViewSize().getWidth());
	}
	void editAtPosition(double p_x, double p_y, double p_px, double p_py, bool p_willZero = false);

public:
	WeightsEditor(Controller* p_controller, uint32_t p_parameterIndexOffset, uint32_t p_numberOfWeights);
	~WeightsEditor();
	
	//---------------------------------------------

	CMouseEventResult onMouseDown(CPoint& p_position, const CButtonState& p_buttonState) override;
	CMouseEventResult onMouseMoved(CPoint& p_position, const CButtonState& p_buttonState) override;

	void setWeight(uint32_t p_index, double p_weight);

	void setColor(const CColor& p_color) {
		m_color = p_color;
	}

	//---------------------------------------------

	void draw(CDrawContext* p_drawingContext) override;
};
