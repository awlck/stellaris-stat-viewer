/* model/model_private_macros.h: Utility macros used for constructing objects from ASTs.
 *
 * Copyright 2019 by Adrian "ArdiMaster" Welcker, distributed
 * under the MIT License - see file "LICENSE" for details.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_MODEL_PRIVATE_MACROS_H
#define STELLARIS_STAT_VIEWER_MODEL_PRIVATE_MACROS_H

#define CHECK_PTR(ptr) do { if (!(ptr)) { delete state; return nullptr; } } while (0)
#define CHECK_COMPOUND(node) do { if (!(node) || (node)->type != Parsing::NT_COMPOUND) { delete state; return nullptr; } } while (0)
#define ITERATE_CHILDREN(node, cn) for (AstNode *cn = (node)->val.firstChild; cn; cn = cn->nextSibling)

#endif //STELLARIS_STAT_VIEWER_MODEL_PRIVATE_MACROS_H
