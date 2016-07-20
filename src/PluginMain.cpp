//
//  PluginMain.cpp
//  ClonerMultiThread
//
//  Created by Hunyadi Janos on 24/07/15.
//  Copyright (c) 2015 Janos Hunyadi. All rights reserved.
//

#include "BaseLoc.h"
#include "BaseLocCommand.h"
#include "icons.h"
#include "AETemplate.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
{
	MStatus status;

	MFnPlugin fnPlugin( obj, "Creative Case", "1.5", "Any" );

	icons_data_write();

	MGlobal::executeCommand( mel_createShelf() );
	MGlobal::executeCommand( mel_AETemplate() );

	// Locator
	status = fnPlugin.registerNode( "BaseLoc", BaseLoc::id, &BaseLoc::creator, &BaseLoc::initialize, MPxNode::kLocatorNode, &BaseLoc::drawDbClassification);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerCommand( "BaseLocCommand", BaseLocCommand::creator, BaseLocCommand::newSyntax );
    CHECK_MSTATUS_AND_RETURN_IT( status );

	status = MHWRender::MDrawRegistry::registerDrawOverrideCreator( BaseLoc::drawDbClassification, BaseLoc::drawRegistrantId, BaseLocOverride::Creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);


	return MS::kSuccess;
}


MStatus uninitializePlugin( MObject obj )
{
	MStatus status;

	MFnPlugin fnPlugin( obj );

	// Locator
	status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator( BaseLoc::drawDbClassification, BaseLoc::drawRegistrantId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterNode( BaseLoc::id );
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterCommand( "BaseLocCommand" );
    CHECK_MSTATUS_AND_RETURN_IT( status );


	return MS::kSuccess;
}

