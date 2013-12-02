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

#ifndef IVW_PROCESSORCONNECTION_H
#define IVW_PROCESSORCONNECTION_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/linkevaluator.h>

namespace inviwo {

class IVW_CORE_API PropertyLink : public IvwSerializable {
public :
    PropertyLink();
    PropertyLink(Property* srcProperty, Property* dstProperty);
    virtual ~PropertyLink();
    
    void setSourceProperty(Property* src) { srcProperty_=src; }
    void setDestinationProperty(Property* dst) { dstProperty_=dst; } 

    Property* getSourceProperty() const{ return srcProperty_; }
    Property* getDestinationProperty() const{ return dstProperty_; }  

    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& d);

    void switchDirection();

private:
    Property* srcProperty_;
    Property* dstProperty_;
};

class IVW_CORE_API ProcessorLink : public IvwSerializable {

public:    

    ProcessorLink();
    ProcessorLink(Processor* outProcessor, Processor* inProcessor);
    virtual ~ProcessorLink();

    Processor* getSourceProcessor() const{ return sourceProcessor_; }
    Processor* getDestinationProcessor() const{ return destinationProcessor_; }

    bool involvesProcessor(Processor* processor) const {
        return (sourceProcessor_==processor || destinationProcessor_==processor);
    }

    void autoLinkPropertiesByType();
    void evaluate(LinkEvaluator *leval);
    bool isLinked(Property* startProperty, Property* endProperty);
    bool isValid();
    std::vector<Property*> getSourceProperties();

    void addPropertyLinks(Property* startProperty, Property* endProperty);
    void removePropertyLinks(Property* startProperty, Property* endProperty);
    void removeBidirectionalPair(Property* startProperty, Property* endProperty);
    std::vector<PropertyLink*> getPropertyLinks() {return propertyLinks_;} 
    PropertyLink* getPropertyLink(Property* startProperty, Property* endProperty);
    PropertyLink* getBidirectionalPair(PropertyLink* propertyLink);
    PropertyLink* getBidirectionalPair(Property* startProperty, Property* endProperty);

    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& d);

private:    
   Processor* sourceProcessor_;
   Processor* destinationProcessor_;
   std::vector<PropertyLink*> propertyLinks_;
};

} // namespace

#endif // IVW_PORTCONNECTION_H
