//
// Created by awelcker on 29.12.19.
//

#ifndef STELLARIS_STAT_VIEWER_TECHTREE_H
#define STELLARIS_STAT_VIEWER_TECHTREE_H

#include <QtCore/QMap>
#include <QtCore/QString>

class GameTranslator;
class QFile;
namespace Galaxy { class Technology; }

void writeTechTreePreamble(QFile *out);
void writeTechTreeClosing(QFile *out);

void writeTechTreeNodes(QFile *out, const QMap<QString, Galaxy::Technology *> &techs, GameTranslator *translator);
void writeTechTreeRelations(QFile *out, const QMap<QString, Galaxy::Technology *> &techs, bool enableWeights);

#endif //STELLARIS_STAT_VIEWER_TECHTREE_H
