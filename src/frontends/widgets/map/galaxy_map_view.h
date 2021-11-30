#pragma once

#ifdef SSV_WITH_GALAXY_MAP
#ifndef STELLARIS_STAT_VIEWER_GALAXY_MAP_VIEW_H
#define STELLARIS_STAT_VIEWER_GALAXY_MAP_VIEW_H

#include <QtOpenGLWidgets/QOpenGLWidget>
//#include <QtWidgets/QWidget>

namespace Galaxy { class State;  }

struct Camera {
	qreal xpos = 0.0;
	qreal ypos = 0.0;
	qreal zoomFactor = 0.0;
	qreal rotation = 0.0;
};

//class GalaxyMapView : public QWidget {
class GalaxyMapView : public QOpenGLWidget {
	Q_OBJECT

public:
	GalaxyMapView(QWidget* parent = nullptr);
	void paintGL() override;
	//void paintEvent(QPaintEvent *event) override;
	void wheelEvent(QWheelEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

public slots:
	void modelChanged(const Galaxy::State* newState);

private:
	Camera camera;
	bool moveOngoing = false;
	QPoint previous;

	const Galaxy::State* state = nullptr;
	std::vector<QPointF> systemPoints;
};

#endif  // !STELLARIS_STAT_VIEWER_GALAXY_MAP_VIEW_H
#endif  // SSV_WITH_GALAXY_MAP