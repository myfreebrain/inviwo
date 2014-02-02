/**********************************************************************
 * Copyright (C) 2013 Scientific Visualization Group - Link�ping University
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

#include <modules/opencl/kernelmanager.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/vectoroperations.h>
#include <modules/opengl/openglmodule.h>
#include <modules/opengl/processorgl.h>

namespace inviwo {

KernelManager::KernelManager() {
    InviwoApplication::getRef().registerFileObserver(this);
}

KernelManager::~KernelManager() {
    clear();
}

cl::Program* KernelManager::buildProgram( const std::string& fileName, const std::string& defines /*= ""*/ ) {
    std::pair <ProgramMap::iterator, ProgramMap::iterator> range = programs_.equal_range(fileName);
    for(ProgramMap::iterator it = range.first; it != range.second; ++it) {
        if(it->second.defines == defines) {
            return it->second.program;
        }
    }
    cl::Program *program = new cl::Program();
    try {
        *program = cl::Program(OpenCL::buildProgram(fileName, defines));
        try {
            std::vector<cl::Kernel> kernels;
            program->createKernels(&kernels);
            for(std::vector<cl::Kernel>::iterator kernelIt = kernels.begin(); kernelIt != kernels.end(); ++kernelIt) {
                kernels_.insert(std::pair<cl::Program*, cl::Kernel*>(program, new cl::Kernel(*kernelIt)));
            }
            
        } catch(cl::Error& err) {
            LogError(fileName << " Failed to create kernels, Error:" << err.what() << "(" << err.err() << "), " << errorCodeToString(err.err()) << std::endl);
        }
    } catch (cl::Error&) {

    }
    ProgramIdentifier uniqueProgram; uniqueProgram.defines = defines; uniqueProgram.program = program;
    programs_.insert(std::pair<std::string, ProgramIdentifier>(fileName, uniqueProgram));
    startFileObservation(fileName);
    return program;
}

cl::Kernel* KernelManager::getKernel( cl::Program* program, const std::string& kernelName ) {
    std::pair <KernelMap::iterator, KernelMap::iterator> kernelRange = kernels_.equal_range(program);
    for(KernelMap::iterator kernelIt = kernelRange.first; kernelIt != kernelRange.second; ++kernelIt) {
        std::string thisKernelName;
        kernelIt->second->getInfo(CL_KERNEL_FUNCTION_NAME, &thisKernelName);
        if(thisKernelName == kernelName) {
            return kernelIt->second;
        }
    }

    
    return NULL;
}

void KernelManager::fileChanged( std::string fileName ) {
    std::pair <ProgramMap::iterator, ProgramMap::iterator> programRange = programs_.equal_range(fileName);
    for(ProgramMap::iterator programIt = programRange.first; programIt != programRange.second; ++programIt) {
        cl::Program* program = programIt->second.program; 
        // Get all kernels associated with the program

        std::pair <KernelMap::iterator, KernelMap::iterator> kernelRange = kernels_.equal_range(program);
        std::vector<std::string> kernelNames;
        for(KernelMap::iterator kernelIt = kernelRange.first; kernelIt != kernelRange.second; ++kernelIt) {
            std::string thisKernelName;
            try {
                kernelIt->second->getInfo(CL_KERNEL_FUNCTION_NAME, &thisKernelName);
            } catch(cl::Error&) {

            }
            kernelNames.push_back(thisKernelName);
        }

        try {
            LogInfo(fileName + " building program");

            *program = OpenCL::buildProgram(fileName, programIt->second.defines);

            LogInfo(fileName + " finished building program");
            std::vector<cl::Kernel> newKernels;
            try {
                LogInfo(fileName + " creating kernels");
                program->createKernels(&newKernels);
            } catch (cl::Error& err) {
                LogError(fileName << " Failed to create kernels, error:" << err.what() << "(" << err.err() << "), " << errorCodeToString(err.err()) << std::endl);
                throw err;
            }
            // Find corresponding old kernel for each newly compiled kernel.
            // Add kernel if it was not found
            for(std::vector<cl::Kernel>::iterator newKernelIt = newKernels.begin(); newKernelIt != newKernels.end(); ++newKernelIt) {
                std::string newKernelName;
                newKernelIt->getInfo(CL_KERNEL_FUNCTION_NAME, &newKernelName);
                std::vector<std::string>::iterator kernelNameIt = std::find(kernelNames.begin(), kernelNames.end(), newKernelName);

                if(kernelNameIt != kernelNames.end()) {
                    size_t index = kernelNameIt - kernelNames.begin();
                    KernelMap::iterator oldKernelIt(kernelRange.first);
                    for(size_t j = 0; j < index; ++j, ++oldKernelIt) {};

                    *(oldKernelIt->second) = *newKernelIt;
                    //(*programKernels)[index] = *newKernelIt;
                } else {
                    kernels_.insert(std::pair<cl::Program*, cl::Kernel*>(program, new cl::Kernel(*newKernelIt)));
                    //programKernels->push_back(*newKernelIt);
                }
            }

            InviwoApplication::getRef().playSound(InviwoApplication::IVW_OK);
            std::vector<Processor*> processors = InviwoApplication::getRef().getProcessorNetwork()->getProcessors();
            for (size_t i=0;i<processors.size();i++) {
                processors[i]->invalidate(PropertyOwner::INVALID_RESOURCES);
            }


        } catch (cl::Error& err) {
            LogError(fileName << " Failed to create kernels, error:" << err.what() << "(" << err.err() << "), " << errorCodeToString(err.err()) << std::endl);
            InviwoApplication::getRef().playSound(InviwoApplication::IVW_ERROR);
        }
    }
}

void KernelManager::clear() {
    for(ProgramMap::iterator programIt = programs_.begin(); programIt != programs_.end(); ++programIt) {
        InviwoApplication::getRef().stopFileObservation(programIt->first);
    }
    for(KernelMap::iterator kernelIt = kernels_.begin(); kernelIt != kernels_.end(); ++kernelIt) {
        delete kernelIt->second; 
        kernelIt->second = NULL;
    }
    for(ProgramMap::iterator programIt = programs_.begin(); programIt != programs_.end(); ++programIt) {
        delete programIt->second.program;
        programIt->second.program = NULL;
    }
}




} // namespace
