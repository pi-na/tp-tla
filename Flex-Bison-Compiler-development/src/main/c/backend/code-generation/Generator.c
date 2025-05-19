#include "Generator.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static const char _expressionTypeToCharacter(const ExpressionType type);
static void _generateConstant(const unsigned int indentationLevel, Constant * constant);
static void _generateEpilogue(const int value);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);
static void _generateProgram(Program * program);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static void _generateHTMLElement(const unsigned int indentationLevel, HTMLElement * element);
static void _generateHTMLElementHTML(const unsigned int indentationLevel, HTMLElementHTML * element);
static void _generateHTMLElementHead(const unsigned int indentationLevel, HTMLElementHead * element);
static void _generateHTMLElementTitle(const unsigned int indentationLevel, HTMLElementTitle * element);
static void _generateHTMLElementBody(const unsigned int indentationLevel, HTMLElementBody * element);
static void _generateHTMLElementDiv(const unsigned int indentationLevel, HTMLElementDiv * element);
static void _generateHTMLElementP(const unsigned int indentationLevel, HTMLElementP * element);
static void _generateHTMLElementH1(const unsigned int indentationLevel, HTMLElementH1 * element);
static void _generateHTMLElementImg(const unsigned int indentationLevel, HTMLElementImg * element);
static void _generateHTMLElementA(const unsigned int indentationLevel, HTMLElementA * element);
static void _generateHTMLElementCenter(const unsigned int indentationLevel, HTMLElementCenter * element);
static void _generateValue(const unsigned int indentationLevel, Value * value);
static void _generateObject(const unsigned int indentationLevel, Object * object);
static void _generateArray(const unsigned int indentationLevel, Array * array);

/**
 * Converts and expression type to the proper character of the operation
 * involved, or returns '\0' if that's not possible.
 */
static const char _expressionTypeToCharacter(const ExpressionType type) {
	switch (type) {
		case ADDITION: return '+';
		case DIVISION: return '/';
		case MULTIPLICATION: return '*';
		case SUBTRACTION: return '-';
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return '\0';
	}
}

/**
 * Generates the output of a constant.
 */
static void _generateConstant(const unsigned int indentationLevel, Constant * constant) {
	_output(indentationLevel, "%s", "[ $C$, circle, draw, black!20\n");
	_output(1 + indentationLevel, "%s%d%s", "[ $", constant->value, "$, circle, draw ]\n");
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Creates the epilogue of the generated output, that is, the final lines that
 * completes a valid Latex document.
 */
static void _generateEpilogue(const int value) {
	_output(0, "%s%d%s",
		"            [ $", value, "$, circle, draw, blue ]\n"
		"        ]\n"
		"    \\end{forest}\n"
		"\\end{document}\n\n"
	);
}

/**
 * Generates the output of an expression.
 */
static void _generateExpression(const unsigned int indentationLevel, Expression * expression) {
	_output(indentationLevel, "%s", "[ $E$, circle, draw, black!20\n");
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			_generateExpression(1 + indentationLevel, expression->leftExpression);
			_output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			break;
		case FACTOR:
			_generateFactor(1 + indentationLevel, expression->factor);
			break;
		default:
			logError(_logger, "The specified expression type is unknown: %d", expression->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a factor.
 */
static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	_output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
	switch (factor->type) {
		case CONSTANT:
			_generateConstant(1 + indentationLevel, factor->constant);
			break;
		case EXPRESSION:
			_output(1 + indentationLevel, "%s", "[ $($, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, factor->expression);
			_output(1 + indentationLevel, "%s", "[ $)$, circle, draw, purple ]\n");
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {
	if (program->root != NULL) {
		_generateHTMLElement(3, program->root);
	}
}

/**
 * Creates the prologue of the generated output, a Latex document that renders
 * a tree thanks to the Forest package.
 *
 * @see https://ctan.dcc.uchile.cl/graphics/pgf/contrib/forest/forest-doc.pdf
 */
static void _generatePrologue(void) {
	_output(0, "%s",
		"\\documentclass{standalone}\n\n"
		"\\usepackage[utf8]{inputenc}\n"
		"\\usepackage[T1]{fontenc}\n"
		"\\usepackage{amsmath}\n"
		"\\usepackage{forest}\n"
		"\\usepackage{microtype}\n\n"
		"\\begin{document}\n"
		"    \\centering\n"
		"    \\begin{forest}\n"
		"        [ \\text{$=$}, circle, draw, purple\n"
	);
}

/**
 * Generates an indentation string for the specified level.
 */
static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
static void _output(const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	fflush(stdout);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

/**
 * Generates the output of an HTML element.
 */
static void _generateHTMLElement(const unsigned int indentationLevel, HTMLElement * element) {
	if (element == NULL) return;

	switch (element->type) {
		case HTML_ELEMENT:
			_generateHTMLElementHTML(indentationLevel, (HTMLElementHTML*)element);
			break;
		case HEAD_ELEMENT:
			_generateHTMLElementHead(indentationLevel, (HTMLElementHead*)element);
			break;
		case TITLE_ELEMENT:
			_generateHTMLElementTitle(indentationLevel, (HTMLElementTitle*)element);
			break;
		case BODY_ELEMENT:
			_generateHTMLElementBody(indentationLevel, (HTMLElementBody*)element);
			break;
		case DIV_ELEMENT:
			_generateHTMLElementDiv(indentationLevel, (HTMLElementDiv*)element);
			break;
		case P_ELEMENT:
			_generateHTMLElementP(indentationLevel, (HTMLElementP*)element);
			break;
		case H1_ELEMENT:
			_generateHTMLElementH1(indentationLevel, (HTMLElementH1*)element);
			break;
		case IMG_ELEMENT:
			_generateHTMLElementImg(indentationLevel, (HTMLElementImg*)element);
			break;
		case A_ELEMENT:
			_generateHTMLElementA(indentationLevel, (HTMLElementA*)element);
			break;
		case CENTER_ELEMENT:
			_generateHTMLElementCenter(indentationLevel, (HTMLElementCenter*)element);
			break;
		default:
			logError(_logger, "Unknown HTML element type: %d", element->type);
			break;
	}
}

static void _generateHTMLElementHTML(const unsigned int indentationLevel, HTMLElementHTML * element) {
	_output(indentationLevel, "<html>\n");
	if (element->head != NULL) {
		_generateHTMLElementHead(indentationLevel + 1, (HTMLElementHead*)element->head);
	}
	if (element->body != NULL) {
		_generateHTMLElementBody(indentationLevel + 1, (HTMLElementBody*)element->body);
	}
	_output(indentationLevel, "</html>\n");
}

static void _generateHTMLElementHead(const unsigned int indentationLevel, HTMLElementHead * element) {
	_output(indentationLevel, "<head>\n");
	if (element->title != NULL) {
		_generateHTMLElementTitle(indentationLevel + 1, (HTMLElementTitle*)element->title);
	}
	_output(indentationLevel, "</head>\n");
}

static void _generateHTMLElementTitle(const unsigned int indentationLevel, HTMLElementTitle * element) {
	_output(indentationLevel, "<title>%s</title>\n", element->content);
}

static void _generateHTMLElementBody(const unsigned int indentationLevel, HTMLElementBody * element) {
	_output(indentationLevel, "<body>\n");
	if (element->content != NULL) {
		for (int i = 0; i < element->content->count; i++) {
			Value * value = element->content->elements[i];
			if (value != NULL) {
				_generateValue(indentationLevel + 1, value);
			}
		}
	}
	_output(indentationLevel, "</body>\n");
}

static void _generateHTMLElementDiv(const unsigned int indentationLevel, HTMLElementDiv * element) {
	_output(indentationLevel, "<div>\n");
	if (element->content != NULL) {
		for (int i = 0; i < element->content->count; i++) {
			Value * value = element->content->elements[i];
			if (value != NULL) {
				_generateValue(indentationLevel + 1, value);
			}
		}
	}
	_output(indentationLevel, "</div>\n");
}

static void _generateHTMLElementP(const unsigned int indentationLevel, HTMLElementP * element) {
	_output(indentationLevel, "<p>");
	if (element->content != NULL) {
		_generateValue(indentationLevel, element->content);
	}
	_output(indentationLevel, "</p>\n");
}

static void _generateHTMLElementH1(const unsigned int indentationLevel, HTMLElementH1 * element) {
	_output(indentationLevel, "<h1>");
	if (element->content != NULL) {
		_generateValue(indentationLevel, element->content);
	}
	_output(indentationLevel, "</h1>\n");
}

static void _generateHTMLElementImg(const unsigned int indentationLevel, HTMLElementImg * element) {
	_output(indentationLevel, "<img src=\"%s\" alt=\"%s\" />\n", element->src, element->alt);
}

static void _generateHTMLElementA(const unsigned int indentationLevel, HTMLElementA * element) {
	_output(indentationLevel, "<a href=\"%s\">", element->href);
	if (element->content != NULL) {
		_generateValue(indentationLevel, element->content);
	}
	_output(indentationLevel, "</a>\n");
}

static void _generateHTMLElementCenter(const unsigned int indentationLevel, HTMLElementCenter * element) {
	_output(indentationLevel, "<center>\n");
	if (element->content != NULL) {
		for (int i = 0; i < element->content->count; i++) {
			Value * value = element->content->elements[i];
			if (value != NULL) {
				_generateValue(indentationLevel + 1, value);
			}
		}
	}
	_output(indentationLevel, "</center>\n");
}

static void _generateValue(const unsigned int indentationLevel, Value * value) {
	if (value == NULL) return;

	switch (value->type) {
		case STRING_VALUE:
			_output(indentationLevel, "%s", value->string);
			break;
		case INTEGER_VALUE:
			_output(indentationLevel, "%d", value->integer);
			break;
		case REAL_VALUE:
			_output(indentationLevel, "%f", value->real);
			break;
		case BOOLEAN_VALUE:
			_output(indentationLevel, "%s", value->boolean ? "true" : "false");
			break;
		case NULL_VALUE:
			_output(indentationLevel, "null");
			break;
		case OBJECT_VALUE:
			if (value->object != NULL) {
				_generateObject(indentationLevel, value->object);
			}
			break;
		case ARRAY_VALUE:
			if (value->array != NULL) {
				_generateArray(indentationLevel, value->array);
			}
			break;
		default:
			logError(_logger, "Unknown value type: %d", value->type);
			break;
	}
}

static void _generateObject(const unsigned int indentationLevel, Object * object) {
	if (object == NULL) return;

	_output(indentationLevel, "{\n");
	if (object->entries != NULL) {
		for (int i = 0; i < object->entries->count; i++) {
			Entry * entry = object->entries->entries[i];
			if (entry != NULL) {
				_output(indentationLevel + 1, "\"%s\": ", entry->key);
				_generateValue(indentationLevel + 1, entry->value);
				if (i < object->entries->count - 1) {
					_output(indentationLevel, ",");
				}
				_output(indentationLevel, "\n");
			}
		}
	}
	_output(indentationLevel, "}");
}

static void _generateArray(const unsigned int indentationLevel, Array * array) {
	if (array == NULL) return;

	_output(indentationLevel, "[\n");
	for (int i = 0; i < array->count; i++) {
		Value * value = array->elements[i];
		if (value != NULL) {
			_generateValue(indentationLevel + 1, value);
			if (i < array->count - 1) {
				_output(indentationLevel, ",");
			}
			_output(indentationLevel, "\n");
		}
	}
	_output(indentationLevel, "]");
}

/** PUBLIC FUNCTIONS */

void generate(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateEpilogue(compilerState->value);
	logDebugging(_logger, "Generation is done.");
}
