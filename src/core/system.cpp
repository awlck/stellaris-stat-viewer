#include "system.h"

#include "galaxy_state.h"
#include "model_private_macros.h"
#include "parser.h"

using Parsing::AstNode;

namespace Galaxy {
	System::System(State* parent) : QObject(parent) {}

	int64_t System::getIndex() const {
		return index;
	}

	const QString& System::getName() const {
		return name;
	}

	const QVector2D& System::getPosition() const {
		return position;
	}

	System* System::createFromAst(const AstNode* tree, State* parent) {
		if (tree->type != Parsing::NT_COMPOUND) return nullptr;
		System* state = new System(parent);
		state->index = static_cast<int64_t>(QString(tree->myName).toLongLong());

		const AstNode* coordNode = tree->findChildWithName("coordinate");
		CHECK_COMPOUND(coordNode);
		const AstNode* typeNode = coordNode->nextSibling;
		if (!typeNode || strcmp(typeNode->myName, "type") != 0) {
			typeNode = tree->findChildWithName("type");
			CHECK_PTR(typeNode);
		}
		if (strcmp(typeNode->val.Str, "star") != 0) {
			delete state;
			return nullptr;
		}

		const AstNode* nameNode = typeNode->nextSibling;
		if (!nameNode || strcmp(typeNode->myName, "name") != 0) {
			nameNode = tree->findChildWithName("name");
			CHECK_PTR(nameNode);
		}

		state->name = nameNode->val.Str;
		const AstNode* xCoordNode = coordNode->findChildWithName("x");
		CHECK_PTR(xCoordNode);
		const AstNode* yCoordNode = coordNode->findChildWithName("y");
		CHECK_PTR(yCoordNode);
		if (xCoordNode->type == Parsing::NT_DOUBLE) {
			state->position.setX(xCoordNode->val.Double);
		} else if (xCoordNode->type == Parsing::NT_INT) {
			state->position.setX(xCoordNode->val.Int);
		} else {
			delete state;
			return nullptr;
		}
		if (yCoordNode->type == Parsing::NT_DOUBLE) {
			state->position.setY(yCoordNode->val.Double);
		} else if (yCoordNode->type == Parsing::NT_INT) {
			state->position.setY(yCoordNode->val.Int);
		} else {
			delete state;
			return nullptr;
		}

		return state;
	}
}