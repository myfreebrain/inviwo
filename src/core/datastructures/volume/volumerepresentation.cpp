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
 * Primary author : Erik Sund�n
 *
 **********************************************************************/

#include <inviwo/core/datastructures/volume/volumerepresentation.h>

namespace inviwo {

    VolumeRepresentation::VolumeRepresentation(uvec3 dimension)
        : DataRepresentation(),
        dimensions_(dimension),
        borders_(VolumeBorders())
    {}

    VolumeRepresentation::VolumeRepresentation(uvec3 dimension, const DataFormatBase* format)
        : DataRepresentation(format),
        dimensions_(dimension),
        borders_(VolumeBorders())
    {}

    VolumeRepresentation::VolumeRepresentation(uvec3 dimension, const DataFormatBase* format, VolumeBorders border)
        : DataRepresentation(format),
        dimensions_(dimension),
        borders_(border)

    {}

    VolumeRepresentation::~VolumeRepresentation() {}

} // namespace
