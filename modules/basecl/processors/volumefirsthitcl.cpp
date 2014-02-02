/**********************************************************************
 * Copyright (C) 2014 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Daniel J�nsson
 *
 **********************************************************************/

#include <modules/basecl/processors/volumefirsthitcl.h>
#include <modules/opencl/inviwoopencl.h>
#include <modules/opencl/syncclgl.h>
#include <modules/opencl/image/imagecl.h>
#include <modules/opencl/image/imageclgl.h>
#include <modules/opencl/volume/volumecl.h>
#include <modules/opencl/volume/volumeclgl.h>
#include <modules/opencl/kernelmanager.h>

namespace inviwo {

ProcessorClassName(VolumeFirstHitCL, "VolumeFirstHitCL"); 
ProcessorCategory(VolumeFirstHitCL, "Volume Rendering");
ProcessorCodeState(VolumeFirstHitCL, CODE_STATE_EXPERIMENTAL);

VolumeFirstHitCL::VolumeFirstHitCL()
    : Processor()
    , volumePort_("volume")
    , entryPort_("entry-points")
    , exitPort_("exit-points")
    , outport_("outport", ImageType::COLOR_ONLY, DataVec4FLOAT32::get())
    , samplingRate_("samplingRate", "Sampling rate", 1.0f, 1.0f, 15.0f)
    , transferFunction_("transferFunction", "Transfer function", TransferFunction())
    , workGroupSize_("wgsize", "Work group size", ivec2(8, 8), ivec2(0), ivec2(256))
    , useGLSharing_("glsharing", "Use OpenGL sharing", true)
    , kernel_(NULL)
{
    addPort(volumePort_, "VolumePortGroup");
    addPort(entryPort_, "ImagePortGroup1");
    addPort(exitPort_, "ImagePortGroup1");
    addPort(outport_, "ImagePortGroup1");

    addProperty(samplingRate_);
    addProperty(transferFunction_);
    addProperty(workGroupSize_);
    addProperty(useGLSharing_);
}

VolumeFirstHitCL::~VolumeFirstHitCL() {}

void VolumeFirstHitCL::initialize() {
    Processor::initialize();
    try {
        cl::Program* program = KernelManager::getRef().buildProgram(InviwoApplication::getPtr()->getPath(InviwoApplication::PATH_MODULES)+"basecl/cl/volumefirsthit.cl");
        kernel_ = KernelManager::getRef().getKernel(program, "volumeFirstHit");

    } catch (cl::Error&) {
        
    }

}

void VolumeFirstHitCL::deinitialize() {
    Processor::deinitialize();
}

void VolumeFirstHitCL::process() {
    if( kernel_ == NULL) {
        return;
    }
    uvec2 outportDim = outport_.getDimension();



    mat4 volumeTextureToWorld = volumePort_.getData()->getCoordinateTransformer().getTextureToWorldMatrix();
    uvec3 volumeDim = volumePort_.getData()->getDimension();
    float stepSize = 1.f/(samplingRate_.get()*static_cast<float>(std::max(volumeDim.x, std::max(volumeDim.y, volumeDim.z))));

    svec2 localWorkGroupSize(workGroupSize_.get());
    svec2 globalWorkGroupSize(getGlobalWorkGroupSize(entryPort_.getData()->getDimension().x, localWorkGroupSize.x), getGlobalWorkGroupSize(entryPort_.getData()->getDimension().y, localWorkGroupSize.y));
#if IVW_PROFILING
    cl::Event* profilingEvent = new cl::Event(); 
#else 
    cl::Event* profilingEvent = NULL;
#endif
    if (useGLSharing_.get()) {
        // Will synchronize with OpenGL upon creation and destruction
        SyncCLGL glSync;
        const ImageCLGL* entry = entryPort_.getData()->getRepresentation<ImageCLGL>();
        const ImageCLGL* exit = exitPort_.getData()->getRepresentation<ImageCLGL>();
        const ImageCLGL* output = outport_.getData()->getEditableRepresentation<ImageCLGL>();
        const VolumeCLGL* volume = volumePort_.getData()->getRepresentation<VolumeCLGL>();
        const LayerCLGL* tfCL = transferFunction_.get().getData()->getRepresentation<LayerCLGL>();
        volume->aquireGLObject(glSync.getGLSyncEvent());
        entry->getLayerCLGL()->aquireGLObject();
        exit->getLayerCLGL()->aquireGLObject();
        output->getLayerCLGL()->aquireGLObject();
        tfCL->aquireGLObject();
        const cl::Image3D& volumeCL = volume->getVolume();
        const cl::Image2D& entryCL = entry->getLayerCLGL()->get();
        const cl::Image2D& exitCL = exit->getLayerCLGL()->get();
        const cl::Image2D& outImageCL = output->getLayerCLGL()->get();
        const cl::Image2D& tf = tfCL->get();
        firstHit(volumeCL, entryCL, exitCL, tf, outImageCL, stepSize, globalWorkGroupSize, localWorkGroupSize, profilingEvent);
        entry->getLayerCLGL()->releaseGLObject();
        exit->getLayerCLGL()->releaseGLObject();
        output->getLayerCLGL()->releaseGLObject();
        tfCL->releaseGLObject();
        volume->releaseGLObject(NULL, glSync.getLastReleaseGLEvent());
    } else {
        const ImageCL* entry = entryPort_.getData()->getRepresentation<ImageCL>();
        const ImageCL* exit = exitPort_.getData()->getRepresentation<ImageCL>();
        const ImageCL* output = outport_.getData()->getEditableRepresentation<ImageCL>();
        const VolumeCL* volume = volumePort_.getData()->getRepresentation<VolumeCL>();
        const LayerCL* tfCL = transferFunction_.get().getData()->getRepresentation<LayerCL>();
        const cl::Image3D& volumeCL = volume->getVolume();
        const cl::Image2D& entryCL = entry->getLayerCL()->get();
        const cl::Image2D& exitCL = exit->getLayerCL()->get();
        const cl::Image2D& outImageCL = output->getLayerCL()->get();
        const cl::Image2D& tf = tfCL->get();
        firstHit(volumeCL, entryCL, exitCL, tf, outImageCL, stepSize, globalWorkGroupSize, localWorkGroupSize, profilingEvent);
    }
    
#if IVW_PROFILING
    try {
        profilingEvent->wait();
        LogInfo("Exec time: " << profilingEvent->getElapsedTime() << " ms");
    } catch (cl::Error& err) {
        LogError(getCLErrorString(err));
    }
    delete profilingEvent;
#endif
}

void VolumeFirstHitCL::firstHit( const cl::Image3D& volumeCL, const cl::Image2D& entryPoints, const cl::Image2D& exitPoints, const cl::Image2D& transferFunctionCL, const cl::Image2D& output, float stepSize, svec2 globalWorkGroupSize, svec2 localWorkGroupSize, cl::Event* profilingEvent ) {
    try
    {
        cl_uint arg = 0;
        kernel_->setArg(arg++, volumeCL);
        kernel_->setArg(arg++, entryPoints);
        kernel_->setArg(arg++, exitPoints);
        kernel_->setArg(arg++, transferFunctionCL);
        kernel_->setArg(arg++, stepSize);
        kernel_->setArg(arg++, output);
        OpenCL::instance()->getQueue().enqueueNDRangeKernel(*kernel_, cl::NullRange, globalWorkGroupSize, localWorkGroupSize, NULL, profilingEvent);
    } catch (cl::Error& err) {
        LogError(getCLErrorString(err));
    }
}

} // namespace


