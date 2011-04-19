/*
 *  MHTerrainBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHTerrainBindings.h"

#include <Content/Content.h>
#include <Content/MaterialManager.h>
#include <Content/ShaderManager.h>

//////////////////////////////////////
#pragma mark Terrain utility functions
//////////////////////////////////////

bool MHTerrainBindings::ConvertRubyParameter(VALUE rParameter, ParameterData &cParameter) {
    if(rParameter == Qtrue) {
        cParameter = ParameterData(true);
    } else if(rParameter == Qfalse) {
        cParameter = ParameterData(false);
    } else if(rParameter == Qnil) {
        Error("Can't set parameter to nil!");
        return false;
    }
    else {
        VALUE rClass = rb_class_of(rParameter);
        if(rClass == rb_cString) {
            cParameter = ParameterData(rb_string_value_cstr(&rParameter));
        } else if(rClass == rb_cFixnum) {
            cParameter = ParameterData(NUM2INT(rParameter));
        } else if(rClass == rb_cFloat) {
            cParameter = ParameterData(NUM2DBL(rParameter));
        } else if(rClass == rb_cNumeric || rClass == rb_cInteger) {
            cParameter = ParameterData(NUM2INT(rParameter));
            Warn("Might have difficulty parsing this parameter!");
        } else {
            Error("Can't parse parameter type " << rClass);
            return false;
        }
    }
    return true;
}

bool MHTerrainBindings::ConvertCParameter(const ParameterData &cParameter, VALUE &rParameter) {
    if(cParameter.type() == typeid(bool)) {
        rParameter = (boost::any_cast<bool>(cParameter)) ? Qtrue : Qfalse;
    } else if(cParameter.type() == typeid(int)) {
        rParameter = INT2NUM(boost::any_cast<int>(cParameter));
    } else if(cParameter.type() == typeid(double)) {
        rParameter = DBL2NUM(boost::any_cast<double>(cParameter));
    } else if(cParameter.type() == typeid(std::string)) {
        rParameter = rb_str_new2(boost::any_cast<std::string>(cParameter).c_str());
    } else {
        rParameter = Qnil;
        Error("Can't compare properties");
        return false;
    }
    return true;
}

PaletteIndex MHTerrainBindings::RegisterTileType(MHTerrain *cSelf, const Tile &cTile) {
    VALUE rClass = cTile.getType();

    // Get the shader and texture from the class variables
    VALUE rShader = rb_iv_get(rClass, "@shader");
    std::string cShader = rb_string_value_cstr(&rShader);

    // Begin by creating a new material for this tile
    Material *newMat = new Material();
    newMat->setShader(Content::GetOrLoad<Shader>(cShader));

    // Deal with changing the color of selected tiles.
    if(cTile.hasParameter("selected") && boost::any_cast<bool>(cTile.getParameter("selected")) == true) {
        newMat->setShaderParameter("colorHint", new Vector4(0.9, 0.9, 0.6, 1.0));
    } else {
        newMat->setShaderParameter("colorHint", new Vector4(1.0, 1.0, 1.0, 1.0));
    }

    // Set the lighting characetistics.
    newMat->setShaderParameter("ambientFactor", new float(NUM2DBL(rb_iv_get(rClass, "@ambientFactor"))));
    newMat->setShaderParameter("diffuseFactor", new float(NUM2DBL(rb_iv_get(rClass, "@diffuseFactor"))));

    // Load up the texture information.
#define SET_MATERIAL_TEXTURE(name) \
do { \
    rTextureName = rb_iv_get(rClass, "@" name); \
    cTextureName = cTextureSet + "_" + rb_string_value_cstr(&rTextureName); \
    newMat->setShaderParameter(name, Content::GetOrLoad<Texture>(cTextureName)); \
} while (0)

    VALUE rTextureSet = rb_iv_get(rClass, "@textureSet");
    std::string cTextureSet = rb_string_value_cstr(&rTextureSet);

    std::string cTextureName;
    VALUE rTextureName = rb_iv_get(rClass, "@texture");
    if (rTextureName != Qnil) {
        SET_MATERIAL_TEXTURE("texture");
    } else {
        SET_MATERIAL_TEXTURE("topTexture");
        SET_MATERIAL_TEXTURE("bottomTexture");
        SET_MATERIAL_TEXTURE("sideTexture");
    }

#undef SET_MATERIAL_TEXTURE

    // Get the tile type's name and register our new tile with the tile palette.
    VALUE rTileName = rb_funcall(rClass, rb_intern("to_s"), 0);
    std::string cTileName = rb_string_value_cstr(&rTileName);
    PaletteIndex index = cSelf->getPalette()->registerTile(cTileName, cTile, newMat);

    // FIXME: REQUIRES DUPLICATE CODE IN TILEPALETTE'S D'TOR.
    std::string matName = std::string("tile palette entry [" + index) + "]";
    Content::GetMaterialManager()->registerResource(matName, newMat);

    return index;
}

void MHTerrainBindings::GenerateCTileFromRTile(MHTerrain *cSelf, VALUE rTile, Tile &cTile) {
    cTile.setType(rb_class_of(rTile));

    // Get the tile parameters from the instance attributes
    VALUE rAttrs = rb_iv_get(rTile, "@inst_attributes");
    VALUE rParameters = rb_hash_aref(rAttrs, ID2SYM(parametersKey));

    VALUE nextParameter;
    for(int i = 0; (nextParameter = rb_ary_entry(rParameters, i)) != Qnil; i++) {
        VALUE paramKey = rb_ary_entry(nextParameter, 0);
        VALUE paramValue = rb_ary_entry(nextParameter, 1);

        ParameterData pData;
        if(ConvertRubyParameter(paramValue, pData)) {
            cTile.addParameter(rb_string_value_cstr(&paramKey), pData);
        }
    }

}

void MHTerrainBindings::SetAndRegisterTile(MHTerrain *cSelf, int x, int y, int z, const Tile &cTile) {
    PaletteIndex index = cSelf->getPalette()->getPaletteIndex(cTile);

    if (index == TilePalette::EmptyTile) {
        index = RegisterTileType(cSelf, cTile);
    }

    ASSERT(index != TilePalette::EmptyTile);

    cSelf->setPaletteIndex(x, y, z, index);
}

////////////////////////////////
#pragma mark Actual binding code
////////////////////////////////
ID MHTerrainBindings::parametersKey = NULL;

MHTerrainBindings::MHTerrainBindings()
: RubyBindings<MHTerrain, false>(
    rb_define_class("MHTerrain", rb_cObject),
    "MHTerrainBindings")
{
    parametersKey = rb_intern("parameters");

    rb_define_method(_class, "set_tile", RUBY_METHOD_FUNC(MHTerrainBindings::SetTile), 4);
    rb_define_method(_class, "get_tile", RUBY_METHOD_FUNC(MHTerrainBindings::GetTile), 3);
    rb_define_method(_class, "set_tile_parameter", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileParameter), 5);
    rb_define_method(_class, "get_tile_parameter", RUBY_METHOD_FUNC(MHTerrainBindings::GetTileParameter), 4);

    rb_define_method(_class, "out_of_bounds?", RUBY_METHOD_FUNC(MHTerrainBindings::OutOfBounds), 3);
    rb_define_method(_class, "get_surface", RUBY_METHOD_FUNC(MHTerrainBindings::SurfaceTile), 2);
    rb_define_method(_class, "each_empty_range", RUBY_METHOD_FUNC(MHTerrainBindings::EachEmptyRange), 2);
    rb_define_method(_class, "each_filled_range", RUBY_METHOD_FUNC(MHTerrainBindings::EachFilledRange), 2);
    rb_define_method(_class, "clear", RUBY_METHOD_FUNC(MHTerrainBindings::Clear), 0);
    rb_define_method(_class, "width", RUBY_METHOD_FUNC(MHTerrainBindings::GetWidth), 0);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(MHTerrainBindings::GetHeight), 0);
    rb_define_method(_class, "depth", RUBY_METHOD_FUNC(MHTerrainBindings::GetDepth), 0);
    rb_define_method(_class, "poly_reduction=", RUBY_METHOD_FUNC(MHTerrainBindings::SetPolyReduction), 1);
    rb_define_method(_class, "auto_update=", RUBY_METHOD_FUNC(MHTerrainBindings::SetAutoUpdate), 1);
}

VALUE MHTerrainBindings::OutOfBounds(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    int cX = NUM2INT(x);
    int cY = NUM2INT(y);
    int cZ = NUM2INT(z);

    return cSelf->isOutOfBounds(cX, cY, cZ) ? Qtrue : Qfalse;
}

VALUE MHTerrainBindings::SetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rTile) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);

    if (rTile != Qnil) {
        Tile cTile;
        GenerateCTileFromRTile(cSelf, rTile, cTile);
        SetAndRegisterTile(cSelf, NUM2INT(x), NUM2INT(y), NUM2INT(z), cTile);
    } else {
        cSelf->setPaletteIndex(NUM2INT(x), NUM2INT(y), NUM2INT(z), TilePalette::EmptyTile);
    }

    return rSelf;
}

VALUE MHTerrainBindings::GetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return cSelf->getTile(NUM2INT(x), NUM2INT(y), NUM2INT(z))->getType();
}

VALUE MHTerrainBindings::SetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter, VALUE rParamValue) {
    ParameterData pData;
    if(ConvertRubyParameter(rParamValue, pData)) {
        MHTerrain *cSelf = Get()->getPointer(rSelf);
        int cX = NUM2INT(x),
            cY = NUM2INT(y),
            cZ = NUM2INT(z);

        Tile cTile = Tile(*cSelf->getTile(cX, cY, cZ));
        cTile.setParameter(rb_string_value_cstr(&rParameter), pData);
        SetAndRegisterTile(cSelf, cX, cY, cZ, cTile);
    }

    return rSelf;
}

VALUE MHTerrainBindings::GetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);

    VALUE rParamValue;
    const Tile * tile = cSelf->getTile(NUM2INT(x), NUM2INT(y), NUM2INT(z));
    ConvertCParameter(tile->getParameter(rb_string_value_cstr(&rParameter)), rParamValue);

    return rParamValue;
}

VALUE MHTerrainBindings::SurfaceTile(VALUE rSelf, VALUE x, VALUE y) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getSurfaceLevel(NUM2INT(x), NUM2INT(y)));
}

VALUE MHTerrainBindings::EachEmptyRange(VALUE rSelf, VALUE x, VALUE y) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    std::vector<std::pair<int,int> > ranges;

    int numRanges = cSelf->getEmptyRanges(NUM2INT(x), NUM2INT(y), ranges);

    std::vector<std::pair<int,int> >::iterator itr;
    for(itr = ranges.begin(); itr != ranges.end(); itr++) {
        rb_yield(rb_ary_new3(2, INT2NUM((*itr).first), INT2NUM((*itr).second)));
    }

    return INT2NUM(numRanges);
}

VALUE MHTerrainBindings::EachFilledRange(VALUE rSelf, VALUE x, VALUE y) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    std::vector<std::pair<int,int> > ranges;

    int numRanges = cSelf->getFilledRanges(NUM2INT(x), NUM2INT(y), ranges);

    std::vector<std::pair<int,int> >::iterator itr;
    for(itr = ranges.begin(); itr != ranges.end(); itr++) {
        rb_yield(rb_ary_new3(2, INT2NUM((*itr).first), INT2NUM((*itr).second)));
    }

    return INT2NUM(numRanges);
}

VALUE MHTerrainBindings::Clear(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->clear();
    return rSelf;
}

VALUE MHTerrainBindings::GetWidth(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHTerrainBindings::GetHeight(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHTerrainBindings::GetDepth(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getDepth());
}

VALUE MHTerrainBindings::SetPolyReduction(VALUE rSelf, VALUE val) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    ///\todo XXXBMW: I swear there is a macro that checks both of these automatically.
    cSelf->setPolyReduction(val != Qnil && val != Qfalse);
    return Qnil;
}

VALUE MHTerrainBindings::SetAutoUpdate(VALUE rSelf, VALUE val) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    ///\todo XXXBMW: I swear there is a macro that checks both of these automatically.
    cSelf->setAutoUpdate(val != Qnil && val != Qfalse);
    return Qnil;
}
