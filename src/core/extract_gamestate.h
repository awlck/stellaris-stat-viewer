//
// Created by Adrian Welcker on 2019-07-26.
//

#ifndef STELLARIS_STAT_VIEWER_EXTRACT_GAMESTATE_H
#define STELLARIS_STAT_VIEWER_EXTRACT_GAMESTATE_H

#include <QtCore/QFile>

int extractGamestate(QFile &f, unsigned char **dest, unsigned long *destsize);

#endif //STELLARIS_STAT_VIEWER_EXTRACT_GAMESTATE_H
