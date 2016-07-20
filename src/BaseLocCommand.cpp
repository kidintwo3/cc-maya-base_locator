//	Example python command:
//
//	import maya.cmds as mc
//	locObj = mc.BaseLocCommand( n="ReName_Me", oy= 2, rx= -45.0, p=5, it=2, r=1.0, c=4)
//	cmds.move( 0, 3, 0, locObj, absolute=True )
//	cmds.makeIdentity( locObj, apply=True, t=1, r=1, s=1, n=2 )
//

#include "BaseLocCommand.h"

BaseLocCommand::BaseLocCommand()
{
}


void* BaseLocCommand::creator()
{
	return new BaseLocCommand;
}


bool BaseLocCommand::isUndoable() const
{
	return true;
}

MSyntax BaseLocCommand::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag( "-n", "-name", MSyntax::kString );
	syntax.addFlag( "-p", "-preset", MSyntax::kUnsigned );
	syntax.addFlag( "-it", "-icontype", MSyntax::kUnsigned );

	syntax.addFlag( "-c", "-color", MSyntax::kUnsigned );

	syntax.addFlag( "-r", "-radius", MSyntax::kDouble );

	syntax.addFlag( "-ox", "-offsetx", MSyntax::kDouble );
	syntax.addFlag( "-oy", "-offsety", MSyntax::kDouble );
	syntax.addFlag( "-oz", "-offsetz", MSyntax::kDouble );

	syntax.addFlag( "-rx", "-rotatex", MSyntax::kDouble );
	syntax.addFlag( "-ry", "-rotatey", MSyntax::kDouble );
	syntax.addFlag( "-rz", "-rotatez", MSyntax::kDouble );

	//syntax.setObjectType( MSyntax::kSelectionList, 1, 1 );
	//syntax.useSelectionAsDefault( true );

	syntax.enableEdit( false );
	syntax.enableQuery( false );

	return syntax;
}


MStatus BaseLocCommand::doIt( const MArgList& argList )
{
	MStatus status;

	MArgDatabase argData( syntax(), argList, &status );

	// Presets
	int i_preset = 0;
	int i_icontype = 0;
	int i_color = 1;
	double d_radius = 1.0;

	double d_offX = 0.0;
	double d_offY = 0.0;
	double d_offZ = 0.0;

	double d_rotX = 0.0;
	double d_rotY = 0.0;
	double d_rotZ = 0.0;

	MString s_locName("untitled");

	if ( argData.isFlagSet( "name" ) ) { s_locName = argData.flagArgumentString("name",0); }
	if ( argData.isFlagSet( "preset" ) ) { i_preset = argData.flagArgumentInt("preset",0); }
	if ( argData.isFlagSet( "icontype" ) ) { i_icontype = argData.flagArgumentInt("icontype",0); }
	if ( argData.isFlagSet( "color" ) ) { i_color = argData.flagArgumentInt("color",0); }
	if ( argData.isFlagSet( "radius" ) ) { d_radius = argData.flagArgumentDouble("radius",0); }

	if ( argData.isFlagSet( "offsetx" ) ) { d_offX = argData.flagArgumentDouble("offsetx",0); }
	if ( argData.isFlagSet( "offsety" ) ) { d_offY = argData.flagArgumentDouble("offsety",0); }
	if ( argData.isFlagSet( "offsetz" ) ) { d_offZ = argData.flagArgumentDouble("offsetz",0); }

	if ( argData.isFlagSet( "rotatex" ) ) { d_rotX = argData.flagArgumentDouble("rotatex",0); }
	if ( argData.isFlagSet( "rotatey" ) ) { d_rotY = argData.flagArgumentDouble("rotatey",0); }
	if ( argData.isFlagSet( "rotatez" ) ) { d_rotZ = argData.flagArgumentDouble("rotatez",0); }

	// check results

	if ( i_preset > 10 ) { i_preset = 10;	}
	if ( i_icontype > 26)  { i_icontype = 26;}
	if ( d_radius <= 0.0 ) { d_radius = 1.0;}

	double r,g,b = 0.5;

	switch ( i_color ) 
	{

	case 1: r = 1.0; g = 0.0; b = 0.0; break;
	case 2: r = 0.0; g = 1.0; b = 0.0; break;
	case 3: r = 0.0; g = 0.0; b = 1.0; break;
	case 4: r = 0.0; g = 1.0; b = 1.0; break;
	case 5: r = 1.0; g = 0.0; b = 1.0; break;
	case 6: r = 1.0; g = 1.0; b = 0.0; break;
	case 7: r = 0.5; g = 0.5; b = 0.5; break;
	case 8: r = 1.0; g = 0.5; b = 0.5; break;
	case 9: r = 0.5; g = 1.0; b = 0.5; break;
	case 10: r = 1.0; g = 1.0; b = 1.0; break;

	default:
		r = 0.0; g = 1.0; b = 1.0;
	}

	// Create locator
	o_baseLocNode = m_DEPNode.create("BaseLoc");
	MFnDependencyNode fnDepTrg( o_baseLocNode );

	// Rename it
	if (argData.isFlagSet( "-name" ))
	{
		fnDepTrg.setName( s_locName );
		MPxCommand::setResult(fnDepTrg.name());

	}


	// Set plugs
	MDagPath dag_LocATr;
	MDagPath dag_LocAShape;
	MDagPath dag_LocA;
	MSelectionList sel_list;
	sel_list.add(fnDepTrg.name());
	sel_list.getDagPath(0,dag_LocA);
	dag_LocATr = dag_LocA;
	dag_LocA.extendToShape();
	dag_LocAShape = dag_LocA;

	MFnDependencyNode fnDepLocShape( dag_LocAShape.node() );

	MPlug p_preset = fnDepLocShape.findPlug("presets");
	MPlug p_icontype = fnDepLocShape.findPlug("iconType");
	MPlug p_radius = fnDepLocShape.findPlug("radius");

	MPlug p_trX = fnDepLocShape.findPlug("offsetX");
	MPlug p_trY = fnDepLocShape.findPlug("offsetY");
	MPlug p_trZ = fnDepLocShape.findPlug("offsetZ");

	MPlug p_rotX = fnDepLocShape.findPlug("rotateX");
	MPlug p_rotY = fnDepLocShape.findPlug("rotateY");
	MPlug p_rotZ = fnDepLocShape.findPlug("rotateZ");


	MPlug p_lineColR = fnDepLocShape.findPlug("lineColorR");
	MPlug p_lineColG = fnDepLocShape.findPlug("lineColorG");
	MPlug p_lineColB = fnDepLocShape.findPlug("lineColorB");

	MPlug p_polyColR = fnDepLocShape.findPlug("polygonColorR");
	MPlug p_polyColG = fnDepLocShape.findPlug("polygonColorG");
	MPlug p_polyColB = fnDepLocShape.findPlug("polygonColorB");

	p_preset.setInt( i_preset );
	p_icontype.setInt( i_icontype );
	p_radius.setDouble( d_radius );

	p_trX.setDouble( d_offX );
	p_trY.setDouble( d_offY );
	p_trZ.setDouble( d_offZ );

	p_rotX.setDouble( d_rotX );
	p_rotY.setDouble( d_rotY );
	p_rotZ.setDouble( d_rotZ );

	p_lineColR.setDouble(r);
	p_lineColG.setDouble(g);
	p_lineColB.setDouble(b);

	p_polyColR.setDouble(r - 0.5);
	p_polyColG.setDouble(g - 0.5);
	p_polyColB.setDouble(b - 0.5);

	return redoIt();
}

MStatus BaseLocCommand::redoIt()
{
	MStatus status;







	return MS::kSuccess;
}

MStatus BaseLocCommand::undoIt()
{
	MStatus status;

	// Restore the initial state
	status = m_DGMod.undoIt();
	CHECK_MSTATUS_AND_RETURN_IT( status );

	status = m_DAGMod.undoIt();
	CHECK_MSTATUS_AND_RETURN_IT(status);



	// Delete locator

	if ( !o_baseLocNode.isNull() )
	{
		MGlobal::displayInfo(MString() + "[BaseLocNode] Deleting nodes");

		status = m_DAGMod.deleteNode(o_baseLocNode);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		m_DAGMod.doIt();


	}


	return MS::kSuccess;
}