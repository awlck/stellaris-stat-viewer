#include "galaxy_map_view.h"

#include <QtGui/QPainter>
#include <QtGui/QStaticText>
#include <QtGui/QWheelEvent>

#include "../../../core/galaxy_state.h"
#include "../../../core/system.h"

//GalaxyMapView::GalaxyMapView(QWidget* parent) : QWidget(parent) {
GalaxyMapView::GalaxyMapView(QWidget* parent) : QOpenGLWidget(parent) {
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	camera.xpos = -width()/2;
	camera.ypos = -height()/2;
	camera.zoomFactor = 0.75;
}

//void GalaxyMapView::paintEvent(QPaintEvent *event) {
void GalaxyMapView::paintGL() {
	QPainter painter(this);
	painter.fillRect(rect(), Qt::black);
	if (state == nullptr) {
		QFont font = painter.font();
		font.setPointSize(32);
		painter.setFont(font);
		QPen pen = painter.pen();
		pen.setColor(Qt::red);
		painter.setPen(pen);
		painter.drawText(100, 100, "Load a file to begin!");
		return;
	}
	painter.scale(camera.zoomFactor, camera.zoomFactor);
	painter.translate(-camera.xpos, -camera.ypos);
	painter.rotate(camera.rotation);
	painter.setRenderHint(QPainter::Antialiasing, !moveOngoing);
	painter.setRenderHint(QPainter::TextAntialiasing, !moveOngoing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, !moveOngoing);

	QPen pen = painter.pen();
	pen.setColor(Qt::white);
	pen.setWidth(3);
	painter.setPen(pen);

	/*for (const auto *system : state->getSystems().values()) {
		painter.drawPoint(QPointF(system->getPosition().x(), system->getPosition().y()));
	}*/
	painter.drawPoints(systemPoints.data(), systemPoints.size());
}

void GalaxyMapView::wheelEvent(QWheelEvent* event) {
	int y = event->angleDelta().y();
	int ysteps = y / 120;
	if (ysteps > 0) {
		for (int i = 0; i < ysteps; i++) {
			camera.zoomFactor *= 1.25;
		}
	} else {
		for (int i = 0; i > ysteps; i--) {
			camera.zoomFactor *= 0.75;
		}
	}
	update();
}

void GalaxyMapView::mousePressEvent(QMouseEvent* event) {
	if (event->buttons() & Qt::LeftButton) {
		moveOngoing = true;
		previous = event->pos();
		qDebug() << "Mouse pressed:" << event;
	}
}

void GalaxyMapView::mouseMoveEvent(QMouseEvent* event) {
	if (moveOngoing) {
		camera.xpos += previous.x() - event->pos().x();
		camera.ypos += previous.y() - event->pos().y();
		previous = event->pos();
		update();
		qDebug() << "Mouse dragged:" << event;
	}
}

void GalaxyMapView::mouseReleaseEvent(QMouseEvent* event) {
	if (moveOngoing) {
		moveOngoing = false;
		previous = QPoint(-1, -1);
		qDebug() << "Mouse released:" << event;
		update();
	}
}

void GalaxyMapView::modelChanged(const Galaxy::State* newState) {
	state = newState;
	systemPoints.clear();
	for (const auto *system : state->getSystems().values()) {
		systemPoints.emplace_back(system->getPosition().x(), system->getPosition().y());
	}
}
