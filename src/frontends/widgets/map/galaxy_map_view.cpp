#include "galaxy_map_view.h"

#include <QtGui/QPainter>
#include <QtGui/QStaticText>
#include <QtGui/QWheelEvent>

#include "../../../core/empire.h"
#include "../../../core/galaxy_state.h"
#include "../../../core/starbase.h"
#include "../../../core/system.h"

#include <random>


std::unordered_map<std::string, QColor> empireColors {
	{ "dark_brown", QColor::fromHsvF(0.06, 0.5, 0.35) },
	{ "brown", QColor::fromHsvF(0.028, 0.5, 0.55) },
	{ "beige", QColor::fromHsvF(0.075, 0.4, 0.7) },
	{ "yellow", QColor::fromHsvF(0.1, 0.75, 0.99) },
	{ "light_orange", QColor::fromHsvF(0.09, 0.9, 0.8) },
	{ "orange", QColor::fromHsvF(0.075, 0.8, 0.7) },
	{ "red_orange", QColor::fromHsvF(0.028, 0.75, 0.7) },
	{ "red", QColor::fromHsvF(0.99, 0.8, 0.7) },
	{ "burgundy", QColor::fromHsvF(0.93, 0.6, 0.42) },
	{ "pink", QColor::fromHsvF(0.95, 0.35, 0.6) },
	{ "purple", QColor::fromHsvF(0.8, 0.4, 0.5) },
	{ "dark_purple", QColor::fromHsvF(0.85, 0.6, 0.35) },
	{ "indigo", QColor::fromHsvF(0.71, 0.85, 0.5) },
	{ "dark_blue", QColor::fromHsvF(0.58, 0.7, 0.4) },
	{ "blue", QColor::fromHsvF(0.6, 0.5, 0.6) },
	{ "light_blue", QColor::fromHsvF(0.6, 0.4, 0.8) },
	{ "turquoise", QColor::fromHsvF(0.5, 0.7, 0.8) },
	{ "dark_teal", QColor::fromHsvF(0.48, 0.6, 0.42) },
	{ "teal", QColor::fromHsvF(0.43, 0.3, 0.5) },
	{ "light_green", QColor::fromHsvF(0.32, 0.4, 0.7) },
	{ "green", QColor::fromHsvF(0.32, 0.4, 0.55) },
	{ "dark_green", QColor::fromHsvF(0.33, 0.4, 0.3) },
	{ "grey", QColor::fromHsvF(0.58, 0.15, 0.6) },
	{ "dark_grey", QColor::fromHsvF(0.6, 0.2, 0.3) },
	{ "black", QColor::fromHsvF(0.7, 0.1, 0.18) },
	{ "null", QColor(Qt::gray) }
};


inline QPointF mkpoint(jcv_point p) {
	return QPointF(p.x, p.y);
}


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
	} else if (!stateValid) {
		QFont font = painter.font();
		font.setPointSize(32);
		painter.setFont(font);
		QPen pen = painter.pen();
		pen.setColor(Qt::red);
		painter.setPen(pen);
		painter.drawText(100, 100, "Loading...");
		return;
	}
	painter.scale(camera.zoomFactor, camera.zoomFactor);
	painter.translate(-camera.xpos, -camera.ypos);
	painter.rotate(camera.rotation);
	painter.setRenderHint(QPainter::Antialiasing, !moveOngoing);
	painter.setRenderHint(QPainter::TextAntialiasing, !moveOngoing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, !moveOngoing);
	{
		/*QPen pen = painter.pen();
		pen.setColor(Qt::gray);
		pen.setWidth(1);
		painter.setPen(pen);
		const jcv_edge *edge = jcv_diagram_get_edges(diagram);
		while (edge) {
			painter.drawLine(QPointF(edge->pos[0].x, edge->pos[0].y), QPointF(edge->pos[1].x, edge->pos[1].y));
			edge = jcv_diagram_get_next_edge(edge);
		}*/
		QPen pen = painter.pen();
		pen.setStyle(Qt::NoPen);
		painter.setPen(pen);
		const jcv_site *sites = jcv_diagram_get_sites(diagram);
		for (size_t i = 0; i < diagram->numsites; ++i) {
			const jcv_site &site = sites[i];
			QBrush brush = painter.brush();
			auto id = pointsToId[site.p];
			const Galaxy::Starbase *starbase = state->getSystems()[pointsToId[site.p]]->getStarbase();
			if (starbase) {
				brush.setColor(empireColors[starbase->getOwner()->getMapColor().toStdString()]);
			} else {
				brush.setColor(Qt::black);
			}
			//brush.setColor(empireColors[state->getSystems()[pointsToId[site.p]]->getStarbase()->getOwner()->getMapColor().toStdString()]);
			brush.setStyle(Qt::SolidPattern);
			painter.setBrush(brush);
			const jcv_graphedge *e = site.edges;
			while (e) {
				painter.drawPolygon({ mkpoint(site.p), mkpoint(e->pos[0]), mkpoint(e->pos[1]) });
				e = e->next;
			}
		}
	}
	{
		QPen pen = painter.pen();
		pen.setStyle(Qt::SolidLine);
		pen.setColor(Qt::white);
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawPoints(systemPoints.data(), systemPoints.size());
	}
	
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
	stateValid = true;
	systemPoints.clear();
	diagramPoints.clear();
	jcv_point p;
	for (const auto *system : state->getSystems().values()) {
		systemPoints.emplace_back(system->getPosition().x(), system->getPosition().y());
		p.x = system->getPosition().x();
		p.y = system->getPosition().y();
		diagramPoints.push_back(p);
		pointsToId[p] = system->getIndex();
	}
	if (diagram) {
		jcv_diagram_free(diagram);
		delete diagram;
	}
	diagram = new jcv_diagram;
	memset(diagram, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(diagramPoints.size(), diagramPoints.data(), 0, 0, diagram);
}
