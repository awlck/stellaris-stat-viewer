/* extract_gamestate.h: rudimentarily interpret ZIP files to get at
 *                      the gamestate file inside (header file)
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STELLARIS_STAT_VIEWER_EXTRACT_GAMESTATE_H
#define STELLARIS_STAT_VIEWER_EXTRACT_GAMESTATE_H

#include <QtCore/QFile>

int extractGamestate(const QByteArray &f, unsigned char **dest, unsigned long *destsize);

QString getInflateErrmsg(int result);

#endif //STELLARIS_STAT_VIEWER_EXTRACT_GAMESTATE_H
