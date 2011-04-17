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

ID MHTerrainBindings::parametersKey = NULL;

MHTerrainBindings::MHTerrainBindings()
: RubyBindings<MHTerrain, false>(
    rb_define_class("MHTerrain", rb_cObject),
    "MHTerrainBindings")
{
    parametersKey = rb_intern("parameters");

    rb_define_method(_class, "tile_empty?", RUBY_METHOD_FUNC(MHTerrainBindings::IsTileEmpty), 3);
    rb_define_method(_class, "set_tile_empty", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileEmpty), 3);

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

bool MHTerrainBindings::convertRubyParameter(VALUE rParameter, ParameterData &cParameter) {
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
        } else if(rClass == rb_cInteger) {
            cParameter = ParameterData(NUM2INT(rParameter));
        } else if(rClass == rb_cFloat) {
            cParameter = ParameterData(NUM2DBL(rParameter));
        } else {
            Error("Can't parse parameter type " << rClass);
            return false;
        }
    }
    return true;
}

bool MHTerrainBindings::convertCParameter(const ParameterData &cParameter, VALUE &rParameter) {
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

VALUE MHTerrainBindings::OutOfBounds(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    int cX = NUM2INT(x);
    int cY = NUM2INT(y);
    int cZ = NUM2INT(z);

    return cSelf->isOutOfBounds(cX, cY, cZ) ? Qtrue : Qfalse;
}

VALUE MHTerrainBindings::IsTileEmpty(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return (cSelf->isTileEmpty(NUM2INT(x), NUM2INT(y), NUM2INT(z)))? Qtrue : Qfalse;
}

VALUE MHTerrainBindings::SetTileEmpty(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->setTileEmpty(NUM2INT(x), NUM2INT(y), NUM2INT(z));
    return rSelf;
}

VALUE MHTerrainBindings::SetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rTile) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    VALUE rClass = rb_class_of(rTile);

    Tile cTile;
    cTile.setType(rClass);

    // Get the tile parameters from the instance attributes
    VALUE rAttrs = rb_iv_get(rTile, "@inst_attributes");
    VALUE rParameters = rb_hash_aref(rAttrs, ID2SYM(parametersKey));

    VALUE nextParameter;
    for(int i = 0; (nextParameter = rb_ary_entry(rParameters, i)) != Qnil; i++) {
        VALUE paramKey = rb_ary_entry(nextParameter, 0);
        VALUE paramValue = rb_ary_entry(nextParameter, 1);

        ParameterData pData;
        if(convertRubyParameter(paramValue, pData)) {
            cTile.addParameter(rb_string_value_cstr(&paramKey), pData);
        }
    }

    PaletteIndex index = cSelf->getPalette()->getPaletteIndex(cTile);

    // If the index is not found, we need to create the material and register it. We do
    // this here because there are several variables in the ruby class object that we need
    // access to but do not want to expose elsewhere.
    if (index == TilePalette::IndexNotFound) {
        // Get the shader and texture from the class variables
        VALUE rShader = rb_iv_get(rClass, "@shader");
        VALUE rTexture = rb_iv_get(rClass, "@textureGrid");
        std::string cShader = rb_string_value_cstr(&rShader);
        std::string cTexture = rb_string_value_cstr(&rTexture);

        // Begin by creating a new material for this tile
        Material *newMat = new Material();
        newMat->setShader(Content::GetOrLoad<Shader>(cShader));
        newMat->setShaderParameter("textureGrid", Content::GetOrLoad<Texture>(cTexture));

        // Deal with changing the color of selected tiles.
        if(boost::any_cast<bool>(cTile.getParameter("selected")) == true) {
            newMat->setShaderParameter("colorHint", new Vector4(1.0, 1.0, 0.2, 0.0));
        } else {
            newMat->setShaderParameter("colorHint", new Vector4(1.0, 1.0, 1.0, 1.0));
        }

        // Set the lighting characetistics.
        newMat->setShaderParameter("ambientFactor", new float(NUM2DBL(rb_iv_get(rClass, "@ambientFactor"))));
        newMat->setShaderParameter("diffuseFactor", new float(NUM2DBL(rb_iv_get(rClass, "@diffuseFactor"))));

        // Handle standard terrain tiles v. multi-texture terrain tiles.
        VALUE rOffsets = rb_iv_get(rClass, "@gridOffsets");
        if (rOffsets != Qnil) {
            newMat->setShaderParameter("gridOffsets", new Vector2(
                NUM2INT(rb_ary_entry(rOffsets, 0)),
                NUM2INT(rb_ary_entry(rOffsets, 1))));
        } else {
            rOffsets = rb_iv_get(rClass, "@bottomGridOffsets");
            newMat->setShaderParameter("bottomGridOffsets", new Vector2(
                NUM2INT(rb_ary_entry(rOffsets, 0)),
                NUM2INT(rb_ary_entry(rOffsets, 1))));

            rOffsets = rb_iv_get(rClass, "@sideGridOffsets");
            newMat->setShaderParameter("sideGridOffsets", new Vector2(
                NUM2INT(rb_ary_entry(rOffsets, 0)),
                NUM2INT(rb_ary_entry(rOffsets, 1))));

            rOffsets = rb_iv_get(rClass, "@topGridOffsets");
            newMat->setShaderParameter("topGridOffsets", new Vector2(
                NUM2INT(rb_ary_entry(rOffsets, 0)),
                NUM2INT(rb_ary_entry(rOffsets, 1))));
        }

        // Get the tile type's name and register our new tile with the tile palette.
        VALUE rTileName = rb_funcall(rClass, rb_intern("to_s"), 0);
        std::string cTileName = rb_string_value_cstr(&rTileName);
        index = cSelf->getPalette()->registerTile(cTileName, cTile, newMat);

        // FIXME: REQUIRES DUPLICATE CODE IN TILEPALETTE'S D'TOR.
        std::string matName = std::string("tile palette entry [" + index) + "]";
        Content::GetMaterialManager()->registerResource(matName, newMat);
    }

    cSelf->setTileIndex(NUM2INT(x), NUM2INT(y), NUM2INT(z), index);

    return rSelf;
}

VALUE MHTerrainBindings::GetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);

    const Tile &reference = cSelf->getTile(NUM2INT(x), NUM2INT(y), NUM2INT(z));
    return reference.getType();
}

VALUE MHTerrainBindings::SetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter, VALUE rParamValue) {
    ParameterData pData;
    if(convertRubyParameter(rParamValue, pData)) {
        MHTerrain *cSelf = Get()->getPointer(rSelf);
        int cX = NUM2INT(x),
            cY = NUM2INT(y),
            cZ = NUM2INT(z);

        Tile newTile = Tile(cSelf->getTile(cX, cY, cZ));
        newTile.setParameter(rb_string_value_cstr(&rParameter), pData);
        cSelf->setTile(cX, cY, cZ, newTile);
    }

    return rSelf;
}

VALUE MHTerrainBindings::GetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);

    VALUE rParamValue;
    const Tile &reference = cSelf->getTile(NUM2INT(x), NUM2INT(y), NUM2INT(z));
    convertCParameter(reference.getParameter(rb_string_value_cstr(&rParameter)), rParamValue);

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
