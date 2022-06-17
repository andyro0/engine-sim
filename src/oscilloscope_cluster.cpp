#include "../include/oscilloscope_cluster.h"

#include "../include/engine_sim_application.h"

#include <sstream>

OscilloscopeCluster::OscilloscopeCluster() {
    m_simulator = nullptr;
    m_torqueScope = nullptr;
    m_hpScope = nullptr;
    m_exhaustFlowScope = nullptr;
    m_exhaustValveLiftScope = nullptr;
    m_intakeValveLiftScope = nullptr;
    m_audioWaveformScope = nullptr;
    m_cylinderPressureScope = nullptr;
    m_sparkAdvanceScope = nullptr;
    m_currentFocusScope0 = nullptr;
    m_currentFocusScope1 = nullptr;

    m_torque = 0;
    m_hp = 0;

    m_updatePeriod = 0.25f;
    m_updateTimer = 0.0f;
}

OscilloscopeCluster::~OscilloscopeCluster() {
    /* void */
}

void OscilloscopeCluster::initialize(EngineSimApplication *app) {
    UiElement::initialize(app);

    m_torqueScope = addElement<Oscilloscope>(this);
    m_hpScope = addElement<Oscilloscope>(this);
    m_exhaustFlowScope = addElement<Oscilloscope>(this);
    m_audioWaveformScope = addElement<Oscilloscope>(this);
    m_intakeValveLiftScope = addElement<Oscilloscope>(this);
    m_exhaustValveLiftScope = addElement<Oscilloscope>(this);
    m_cylinderPressureScope = addElement<Oscilloscope>(this);
    m_sparkAdvanceScope = addElement<Oscilloscope>(this);

    // Torque
    m_torqueScope->setBufferSize(100);
    m_torqueScope->m_xMin = 0.0f;
    m_torqueScope->m_xMax = 9000.0f;
    m_torqueScope->m_yMin = -100.0;
    m_torqueScope->m_yMax = 1000.0;
    m_torqueScope->m_lineWidth = 2.0f;
    m_torqueScope->m_drawReverse = false;
    m_torqueScope->i_color = m_app->getOrange();

    // Horsepower
    m_hpScope->setBufferSize(100);
    m_hpScope->m_xMin = 0.0f;
    m_hpScope->m_xMax = 9000.0f;
    m_hpScope->m_yMin = -100.0f;
    m_hpScope->m_yMax = 1000.0f;
    m_hpScope->m_lineWidth = 2.0f;
    m_hpScope->m_drawReverse = false;
    m_hpScope->i_color = m_app->getPink();

    // Exhaust flow
    m_exhaustFlowScope->setBufferSize(1024);
    m_exhaustFlowScope->m_xMin = 0.0f;
    m_exhaustFlowScope->m_xMax = constants::pi * 4;
    m_exhaustFlowScope->m_yMin = -units::flow(400.0, units::scfm);
    m_exhaustFlowScope->m_yMax = units::flow(1500.0, units::scfm);
    m_exhaustFlowScope->m_lineWidth = 2.0f;
    m_exhaustFlowScope->m_drawReverse = false;
    m_exhaustFlowScope->i_color = m_app->getOrange();

    // Audio waveform scope
    m_audioWaveformScope->setBufferSize(44100 / 50);
    m_audioWaveformScope->m_xMin = 0.0f;
    m_audioWaveformScope->m_xMax = 44100 / 10;
    m_audioWaveformScope->m_yMin = -1.5f;
    m_audioWaveformScope->m_yMax = 1.5f;
    m_audioWaveformScope->m_lineWidth = 2.0f;
    m_audioWaveformScope->m_drawReverse = false;
    m_audioWaveformScope->i_color = m_app->getBlue();

    // Valve lift scopes
    m_exhaustValveLiftScope->setBufferSize(1024);
    m_exhaustValveLiftScope->m_xMin = 0.0f;
    m_exhaustValveLiftScope->m_xMax = constants::pi * 4;
    m_exhaustValveLiftScope->m_yMin = -(float)units::distance(250, units::thou);
    m_exhaustValveLiftScope->m_yMax = (float)units::distance(1000, units::thou);
    m_exhaustValveLiftScope->m_lineWidth = 2.0f;
    m_exhaustValveLiftScope->m_drawReverse = false;
    m_exhaustValveLiftScope->i_color = m_app->getOrange();

    m_intakeValveLiftScope->setBufferSize(1024);
    m_intakeValveLiftScope->m_xMin = 0.0f;
    m_intakeValveLiftScope->m_xMax = constants::pi * 4;
    m_intakeValveLiftScope->m_yMin = -(float)units::distance(250, units::thou);
    m_intakeValveLiftScope->m_yMax = (float)units::distance(1000, units::thou);
    m_intakeValveLiftScope->m_lineWidth = 2.0f;
    m_intakeValveLiftScope->m_drawReverse = false;
    m_intakeValveLiftScope->i_color = m_app->getBlue();

    // Cylinder pressure scope
    m_cylinderPressureScope->setBufferSize(1024);
    m_cylinderPressureScope->m_xMin = 0.0f;
    m_cylinderPressureScope->m_xMax = constants::pi * 4;
    m_cylinderPressureScope->m_yMin = -(float)units::pressure(250, units::psi);
    m_cylinderPressureScope->m_yMax = (float)units::pressure(1000, units::psi);
    m_cylinderPressureScope->m_lineWidth = 2.0f;
    m_cylinderPressureScope->m_drawReverse = false;
    m_cylinderPressureScope->i_color = m_app->getOrange();

    // Spark advance scope
    m_sparkAdvanceScope->setBufferSize(1024);
    m_sparkAdvanceScope->m_xMin = 0.0f;
    m_sparkAdvanceScope->m_xMax = units::rpm(10000);
    m_sparkAdvanceScope->m_yMin = -units::angle(30, units::deg);
    m_sparkAdvanceScope->m_yMax = units::angle(60, units::deg);
    m_sparkAdvanceScope->m_lineWidth = 2.0f;
    m_sparkAdvanceScope->m_drawReverse = true;
    m_sparkAdvanceScope->i_color = m_app->getOrange();

    m_currentFocusScope0 = m_exhaustFlowScope;
    m_currentFocusScope1 = nullptr;
}

void OscilloscopeCluster::destroy() {
    UiElement::destroy();
}

void OscilloscopeCluster::signal(UiElement *element, Event event) {
    if (event == Event::Clicked) {
        if (element == m_audioWaveformScope) {
            m_currentFocusScope0 = m_audioWaveformScope;
            m_currentFocusScope1 = nullptr;
        }
        else if (element == m_hpScope || element == m_torqueScope) {
            m_currentFocusScope0 = m_torqueScope;
            m_currentFocusScope1 = m_hpScope;
        }
        else if (element == m_exhaustFlowScope) {
            m_currentFocusScope0 = m_exhaustFlowScope;
            m_currentFocusScope1 = nullptr;
        }
        else if (element == m_intakeValveLiftScope || element == m_exhaustValveLiftScope) {
            m_currentFocusScope0 = m_intakeValveLiftScope;
            m_currentFocusScope1 = m_exhaustValveLiftScope;
        }
        else if (element == m_cylinderPressureScope) {
            m_currentFocusScope0 = m_cylinderPressureScope;
            m_currentFocusScope1 = nullptr;
        }
        else if (element == m_sparkAdvanceScope) {
            m_currentFocusScope0 = m_sparkAdvanceScope;
            m_currentFocusScope1 = nullptr;
        }
    }
}

void OscilloscopeCluster::update(float dt) {
    Engine *engine = m_simulator->getEngine();

    const double torque =
        units::convert(m_simulator->m_dyno.getTorque(), units::ft_lb);
    const double hp = torque * engine->getRpm() / 5252.0;

    m_torque = m_torque * 0.95 + 0.05 * torque;
    m_hp = m_hp * 0.95 + 0.05 * hp;

    if (m_updateTimer <= 0 && m_simulator->m_dyno.m_enabled) {
        m_updateTimer = m_updatePeriod;
        
        m_torqueScope->addDataPoint(engine->getRpm(), m_torque);
        m_hpScope->addDataPoint(engine->getRpm(), m_hp);
    }

    m_sparkAdvanceScope->addDataPoint(
        -m_simulator->getEngine()->getCrankshaft(0)->m_body.v_theta,
        m_simulator->getEngine()->getIgnitionModule()->getTimingAdvance());

    m_updateTimer -= dt;

    UiElement::update(dt);
}

void OscilloscopeCluster::render() {
    Grid grid;
    grid.h_cells = 3;
    grid.v_cells = 4;

    const Bounds &hpTorqueBounds = grid.get(m_bounds, 0, 3);
    renderScope(m_torqueScope, hpTorqueBounds, "Torque/HP");
    renderScope(m_hpScope, hpTorqueBounds, "", true);
    
    const Bounds &exhaustFlowBounds = grid.get(m_bounds, 1, 2);
    renderScope(m_exhaustFlowScope, exhaustFlowBounds, "Exhaust Flow");

    const Bounds &audioWaveformBounds = grid.get(m_bounds, 0, 2);
    renderScope(m_audioWaveformScope, audioWaveformBounds, "Waveform");

    const Bounds &valveLiftBounds = grid.get(m_bounds, 2, 2);
    renderScope(m_intakeValveLiftScope, valveLiftBounds, "Valve Lift");
    renderScope(m_exhaustValveLiftScope, valveLiftBounds, "", true);

    const Bounds &cylinderPressureBounds = grid.get(m_bounds, 1, 3);
    renderScope(m_cylinderPressureScope, cylinderPressureBounds, "Cylinder Pressure");

    const Bounds &sparkAdvanceBounds = grid.get(m_bounds, 2, 3);
    renderScope(m_sparkAdvanceScope, sparkAdvanceBounds, "Spark Advance");

    const Bounds &focusBounds = grid.get(m_bounds, 0, 0, 3, 2);
    const Bounds &focusTitle = focusBounds.verticalSplit(0.85f, 1.0f);
    const Bounds &focusBody = focusBounds.verticalSplit(0.0f, 0.85f);

    drawFrame(focusTitle, 1.0, ysMath::Constants::One, m_app->getBackgroundColor());
    drawFrame(focusBody, 1.0, ysMath::Constants::One, m_app->getBackgroundColor());

    if (m_currentFocusScope0 != nullptr) {
        m_currentFocusScope0->render(focusBody);
    }
    
    if (m_currentFocusScope1 != nullptr) {
        m_currentFocusScope1->render(focusBody);
    }

    UiElement::render();
}

void OscilloscopeCluster::renderScope(
    Oscilloscope *osc,
    const Bounds &bounds,
    const std::string &title,
    bool overlay)
{
    if (!overlay) {
        drawFrame(bounds, 1.0, ysMath::Constants::One, m_app->getBackgroundColor());
    }

    if (osc == m_currentFocusScope0) {
        Grid grid;
        grid.h_cells = 3;
        grid.v_cells = 4;

        const Bounds &focusBounds = grid.get(m_bounds, 0, 0, 3, 2);
        const Bounds &focusTitle = focusBounds.verticalSplit(0.85f, 1.0f);

        drawText(title, focusTitle.inset(20.0f), 24.0f, Bounds::tl);
    }

    osc->m_bounds = bounds;
}