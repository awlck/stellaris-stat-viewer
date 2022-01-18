#pragma once

#ifdef SSV_WITH_GALAXY_MAP
#ifndef STELLARIS_STAT_VIEWER_GALAXY_MAP_VIEW_H
#define STELLARIS_STAT_VIEWER_GALAXY_MAP_VIEW_H

#include <unordered_map>
#include <QtOpenGLWidgets/QOpenGLWidget>
//#include <QtWidgets/QWidget>

#include "jc_voronoi.h"

namespace Galaxy { class State;  }

struct Camera {
	qreal xpos = 0.0;
	qreal ypos = 0.0;
	qreal zoomFactor = 0.0;
	qreal rotation = 0.0;
};

namespace std {
	template<> struct hash<jcv_point> {
		std::size_t operator()(const jcv_point &k) const {
			return std::hash<JCV_REAL_TYPE>()(k.x) ^ (std::hash<JCV_REAL_TYPE>()(k.y) << 1);
		}
	};

	template<> struct equal_to<jcv_point> {
		bool operator()(const jcv_point &lhs, const jcv_point &rhs) const {
			return lhs.x == rhs.x && lhs.y == rhs.y;
		}
	};
}

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
	inline void stopDrawing() { stateValid = false; };

public slots:
	void modelChanged(const Galaxy::State* newState);

private:
	Camera camera;
	bool moveOngoing = false;
	QPoint previous;

	const Galaxy::State* state = nullptr;
	std::vector<QPointF> systemPoints;
	std::vector<int> systemOwnership;
	std::vector<jcv_point> diagramPoints;
	std::unordered_map<jcv_point, int> pointsToId;
	jcv_diagram *diagram = nullptr;
	bool stateValid = false;
};

#endif  // !STELLARIS_STAT_VIEWER_GALAXY_MAP_VIEW_H
#endif  // SSV_WITH_GALAXY_MAP