/**********************************************************************
 * Copyright (C) 2012-2013 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Sathish Kottravel
 *
 **********************************************************************/

#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/util/filedirectory.h>

namespace inviwo {

FileProperty::FileProperty(std::string identifier, std::string displayName, std::string value, PropertyOwner::InvalidationLevel invalidationLevel, 
PropertySemantics::Type semantics)
    : TemplateProperty<std::string>(identifier, displayName,value, invalidationLevel, semantics){
        addNameFilter("All Files (*.*)");
}

int FileProperty::getVariantType() {
    return Variant::VariantTypeString;
}

Variant FileProperty::getVariant() {
    return Variant(get());
}

void  FileProperty::setVariant(const Variant& val) {
    if (val.canConvert(getVariantType())) {
        set(val.getString());
    }
}


void FileProperty::serialize(IvwSerializer& s) const {
    Property::serialize(s) ;
    std::string basePath = s.getFileName();
    std::string absoluteFilePath = get();

    //FIXME: Make sure valid base path is set
    //ivwAssert(!basePath.empty(), "File name cannot be empty.");
    if (basePath.empty())
        basePath = IVW_DIR+"data/workspace";
    
    std::string relativePath = URLParser::getRelativePath(basePath, absoluteFilePath);
    s.serialize("url", relativePath);

    s.serialize("nameFilter", nameFilters_, "filter");
}

void FileProperty::deserialize(IvwDeserializer& d) {
    Property::deserialize(d) ;
    std::string relativePath;
    d.deserialize("url", relativePath);

    //FIXME: Make sure valid base path is set
    //ivwAssert(!basePath.empty(), "File name cannot be empty.")
    std::string basePath = d.getFileName();
    if (basePath.empty())
        basePath = IVW_DIR+"data/workspace";
    
    basePath = URLParser::getFileDirectory(basePath);
    set(basePath+relativePath);

    nameFilters_.clear();
    d.deserialize("nameFilter", nameFilters_, "filter");
}

void FileProperty::addNameFilter( std::string filter){
    nameFilters_.push_back(filter);
}

void FileProperty::clearNameFilters(){
    nameFilters_.clear();
}

std::vector<std::string> FileProperty::getNameFilters(){
    return nameFilters_;
}

} // namespace
