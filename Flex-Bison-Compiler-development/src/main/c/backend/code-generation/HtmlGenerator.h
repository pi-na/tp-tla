#ifndef HTML_GENERATOR_H_INCLUDED
#define HTML_GENERATOR_H_INCLUDED

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mapping de cada Token a su nombre de etiqueta o atributo HTML correspondiente
static const char *TOKEN_STRINGS[] = {
    /* TYPE */             "type",
    /* CONTENT */          "content",
    /* DIV */              "div",
    /* VAR */              "var",
    /* IMG */              "img",
    /* SRC */              "src",
    /* ALT */              "alt",
    /* BODY */             "body",
    /* REF */              "ref",
    /* H1 */               "h1",
    /* H2 */               "h2",
    /* H3 */               "h3",
    /* H4 */               "h4",
    /* A */                "a",
    /* SPAN */             "span",
    /* P */                "p",
    /* TITLE */            "title",
    /* COLOR */            "color",
    /* BACKGROUND_COLOR */ "background-color",
    /* STYLE */            "style",
    /* ITERABLE */         "iterable",
    /* ITERATE */          "iterate",
    /* ITERATOR_REF */     "iterator-ref",
    /* ITERATE_STRING */   "iterate-string",
    /* UL */               "ul",
    /* LI */               "li",
    /* ID */               "id",
    /* CLASS */            "class",
    /* WIDTH */            "width",
    /* HEIGHT */           "height",
    /* MARGIN */           "margin",
    /* MARGIN_TOP */       "margin-top",
    /* MARGIN_RIGHT */     "margin-right",
    /* MARGIN_BOTTOM */    "margin-bottom",
    /* MARGIN_LEFT */      "margin-left",
    /* PADDING */          "padding",
    /* PADDING_TOP */      "padding-top",
    /* PADDING_RIGHT */    "padding-right",
    /* PADDING_BOTTOM */   "padding-bottom",
    /* PADDING_LEFT */     "padding-left",
    /* BORDER_WIDTH */     "border-width",
    /* BORDER_STYLE */     "border-style",
    /* BORDER_COLOR */     "border-color",
    /* BORDER_RADIUS */    "border-radius",
    /* FONT_SIZE */        "font-size",
    /* FONT_FAMILY */      "font-family",
    /* FONT_WEIGHT */      "font-weight",
    /* DISPLAY */          "display",
    /* POSITION */         "position",
    /* TOP */              "top",
    /* RIGHT */            "right",
    /* BOTTOM */           "bottom",
    /* LEFT */             "left",
    /* OVERFLOW */         "overflow",
    /* OPACITY */          "opacity"
};

void initializeHtmlGeneratorModule();
void shutdownHtmlGeneratorModule();
void generateHtml(CompilerState * compilerState);

#endif
