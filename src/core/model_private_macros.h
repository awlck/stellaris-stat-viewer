/* core/model_private_macros.h: Utility macros used for constructing objects from ASTs.
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

#pragma once

#ifndef STELLARIS_STAT_VIEWER_MODEL_PRIVATE_MACROS_H
#define STELLARIS_STAT_VIEWER_MODEL_PRIVATE_MACROS_H

#define CHECK_PTR(ptr) do { if (!(ptr)) { delete state; return nullptr; } } while (0)
#define CHECK_COMPOUND(node) do { if (!(node) || (node)->type != Parsing::NT_COMPOUND) { delete state; return nullptr; } } while (0)
#define ITERATE_CHILDREN(node, cn) for (AstNode *cn = (node)->val.firstChild; cn; cn = cn->nextSibling)

#endif //STELLARIS_STAT_VIEWER_MODEL_PRIVATE_MACROS_H
