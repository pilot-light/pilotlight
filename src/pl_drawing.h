/*
   pl_drawing.h
*/

/*
Index of this file:
// [SECTION] header mess
// [SECTION] includes
// [SECTION] forward declarations
// [SECTION] public api
// [SECTION] structs
*/

#ifndef PL_DRAWING_H
#define PL_DRAWING_H

//-----------------------------------------------------------------------------
// [SECTION] header mess
//-----------------------------------------------------------------------------

#ifndef PL_MAX_NAME_LENGTH
#define PL_MAX_NAME_LENGTH 1024
#endif

#ifndef PL_MAX_FRAMES_IN_FLIGHT
#define PL_MAX_FRAMES_IN_FLIGHT 2
#endif

#ifndef PL_ASSERT
#include <assert.h>
#define PL_ASSERT(x) assert(x)
#endif

#ifndef PL_ALLOC
#include <stdlib.h>
#define PL_ALLOC(x) malloc(x)
#endif

#ifndef PL_FREE
#define PL_FREE(x) free(x)
#endif

#ifdef PL_USE_STB_SPRINTF
#include "stb_sprintf.h"
#endif

#ifndef pl_sprintf
#ifdef PL_USE_STB_SPRINTF
    #define pl_sprintf stbsp_sprintf
    #define pl_vsprintf stbsp_vsprintf
#else
    #define pl_sprintf sprintf
    #define pl_vsprintf vsprintf
#endif
#endif

#ifndef PL_DECLARE_STRUCT
#define PL_DECLARE_STRUCT(name) typedef struct name ##_t name
#endif

//-----------------------------------------------------------------------------
// [SECTION] includes
//-----------------------------------------------------------------------------

#include <stdint.h>  // uint*_t
#include <stdbool.h> // bool

//-----------------------------------------------------------------------------
// [SECTION] forward declarations
//-----------------------------------------------------------------------------

// drawing
PL_DECLARE_STRUCT(plDrawContext);    // pl_drawing context (opaque structure)
PL_DECLARE_STRUCT(plDrawList);       // collection of draw layers for a specific target (opaque structure)
PL_DECLARE_STRUCT(plDrawLayer);      // layer for out of order drawing(opaque structure)
PL_DECLARE_STRUCT(plDrawCommand);    // single draw call (opaque structure)
PL_DECLARE_STRUCT(plDrawVertex);     // single vertex (pos + uv + color)

// fonts
PL_DECLARE_STRUCT(plFontChar);       // internal for now (opaque structure)
PL_DECLARE_STRUCT(plFontGlyph);      // internal for now (opaque structure)
PL_DECLARE_STRUCT(plFontCustomRect); // internal for now (opaque structure)
PL_DECLARE_STRUCT(plFontPrepData);   // internal for now (opaque structure)
PL_DECLARE_STRUCT(plFontRange);      // a range of characters
PL_DECLARE_STRUCT(plFont);           // a single font with a specific size and config
PL_DECLARE_STRUCT(plFontConfig);     // configuration for loading a single font
PL_DECLARE_STRUCT(plFontAtlas);      // atlas for multiple fonts

// math
typedef union plVec2_t plVec2;
typedef union plVec3_t plVec3;
typedef union plVec4_t plVec4;

// plTextureID: used to represent texture for renderer backend
typedef void* plTextureId;

//-----------------------------------------------------------------------------
// [SECTION] public api
//-----------------------------------------------------------------------------

// setup/shutdown
plDrawList*     pl_create_drawlist     (plDrawContext* ctx);
plDrawLayer*    pl_request_draw_layer  (plDrawList* drawlist, const char* name);
void            pl_return_draw_layer   (plDrawLayer* layer);
void            pl_cleanup_draw_context(plDrawContext* ctx);  // implemented by backend

// per frame
void pl_new_draw_frame   (plDrawContext* ctx); // implemented by backend
void pl_submit_draw_layer(plDrawLayer* layer);

// drawing
void pl_add_text           (plDrawLayer* layer, plFont* font, float size, plVec2 p, plVec4 color, const char* text, float wrap);
void pl_add_triangle_filled(plDrawLayer* layer, plVec2 p0, plVec2 p1, plVec2 p2, plVec4 color);
void pl_add_rect_filled    (plDrawLayer* layer, plVec2 minP, plVec2 maxP, plVec4 color);

// fonts
void   pl_build_font_atlas        (plDrawContext* ctx, plFontAtlas* atlas); // implemented by backend
void   pl_cleanup_font_atlas      (plFontAtlas* atlas);                     // implemented by backend
void   pl_add_default_font        (plFontAtlas* atlas);
void   pl_add_font_from_file_ttf  (plFontAtlas* atlas, plFontConfig config, const char* file);
void   pl_add_font_from_memory_ttf(plFontAtlas* atlas, plFontConfig config, void* data);
plVec2 pl_calculate_text_size     (plFont* font, float size, const char* text, float wrap);

//-----------------------------------------------------------------------------
// [SECTION] structs
//-----------------------------------------------------------------------------

typedef union plVec2_t
{
    struct { float x, y; };
    struct { float u, v; };
} plVec2;

typedef union plVec3_t
{
    struct { float x, y, z; };
    struct { float r, g, b; };
} plVec3;

typedef union plVec4_t
{
    struct{ float x, y, z, w;};
    struct{ float r, g, b, a;};
} plVec4;

typedef struct plDrawVertex_t
{
    float pos[2];
    float uv[2];
    float color[4];
} plDrawVertex;

typedef struct plFontRange_t
{
    int         firstCodePoint;
    uint32_t    charCount;
    plFontChar* ptrFontChar; // offset into parent font's char data
} plFontRange;

typedef struct plFontConfig_t
{
    float        fontSize;
    plFontRange* sbRanges;
    int*         sbIndividualChars;

    // BITMAP
    uint32_t vOverSampling;
    uint32_t hOverSampling;

    // SDF
    bool          sdf;
    int           sdfPadding;
    unsigned char onEdgeValue;
    float         sdfPixelDistScale;
} plFontConfig;

typedef struct plFont_t
{
    plFontConfig config;
    plFontAtlas* parentAtlas;
    float        lineSpacing;
    float        ascent;
    float        descent;
    
    uint32_t*    sbCodePoints; // glyph index lookup based on codepoint
    plFontGlyph* sbGlyphs;     // glyphs
    plFontChar*  sbCharData;
} plFont;

typedef struct plFontAtlas_t
{
    plDrawContext*    ctx;
    plFont*           sbFonts;
    plFontCustomRect* sbCustomRects;
    unsigned char*    pixelsAsAlpha8;
    unsigned char*    pixelsAsRGBA32;
    uint32_t          atlasSize[2];
    float             whiteUv[2];
    bool              dirty;
    int               glyphPadding;
    size_t            pixelDataSize;
    plFontCustomRect* whiteRect;
    plTextureId       texture;
    plFontPrepData*   _sbPrepData;
    void*             _platformData;
} plFontAtlas;

#endif // PL_DRAWING_H