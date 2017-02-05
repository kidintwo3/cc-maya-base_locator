//
//  BaseLoc.cpp
//  ClonerMultiThread
//
//  Created by Hunyadi Janos on 22/07/15.
//  Copyright (c) 2015 Janos Hunyadi. All rights reserved.
//

#include "BaseLoc.h"
#include "BaseLocIcons.h"

// Locator draw Data



int m_locPointsNum;
int m_locTrianglesNum;
int m_locShapes;

// Plugin

MTypeId		BaseLoc::id( 0x00123942 );
MObject		BaseLoc::aLocID;

MObject		BaseLoc::aRadius;

MObject		BaseLoc::aOrient;
MObject		BaseLoc::aLineWidth;
MObject     BaseLoc::aPointSize;

MObject     BaseLoc::aOffsetX;
MObject     BaseLoc::aOffsetY;
MObject     BaseLoc::aOffsetZ;

MObject     BaseLoc::aRotateX;
MObject     BaseLoc::aRotateY;
MObject     BaseLoc::aRotateZ;

MObject     BaseLoc::aScaleX;
MObject     BaseLoc::aScaleY;
MObject     BaseLoc::aScaleZ;

MObject     BaseLoc::aDivision;
MObject		BaseLoc::aDispObj;
MObject		BaseLoc::aDispNum;
MObject		BaseLoc::aDispCard;
MObject		BaseLoc::aDispLoc;
MObject		BaseLoc::aDispLocPivot;
MObject		BaseLoc::aDispText;
MObject     BaseLoc::aDrawOnTop;

MObject     BaseLoc::aLineAlpha;
MObject     BaseLoc::aLineColor;
MObject		BaseLoc::aFadeByDistance;
MObject		BaseLoc::aFadeDistance;
MObject     BaseLoc::aPolygonColor;
MObject     BaseLoc::aPolygonAlpha;
MObject     BaseLoc::aLineStyle;
MObject     BaseLoc::aPaintStyle;
MObject     BaseLoc::aDrawPresets;
MObject		BaseLoc::aDrawIconsTypes;
MObject		BaseLoc::aTwoDIconsTypes;


MObject		BaseLoc::aTextPosition;
MObject		BaseLoc::aTextAlignment;
MObject		BaseLoc::aTextIncline;
MObject		BaseLoc::aTextWeight;
MObject		BaseLoc::aTextStretch;
MObject		BaseLoc::aTextLine;
MObject		BaseLoc::aTextBoxSize;
MObject		BaseLoc::aText;
MObject		BaseLoc::aTextBoxColor;
MObject		BaseLoc::aTextBoxTransparency;
MObject		BaseLoc::aTextFontSize;
MObject		BaseLoc::aFontFaceName;

MObject		BaseLoc::aInLocPosA;
MObject		BaseLoc::aInLocPosB;

MObject		BaseLoc::aPresetFolderPath;
MString     BaseLoc::aPluginLoadPath;

MObject		BaseLoc::aInPointArray;
MObject		BaseLoc::aInTriangleArray;

MObject		BaseLoc::aBoundingBoxA;
MObject		BaseLoc::aBoundingBoxB;

MString		BaseLoc::drawDbClassification("drawdb/geometry/BaseLoc");
MString		BaseLoc::drawRegistrantId("BaseLocPlugin");

MStringArray BaseLocData::m_fFontList;

// ----------------------------------------------------------------------------------------------------------------------------------------------------

BaseLoc::BaseLoc() {}
BaseLoc::~BaseLoc() {}

void BaseLoc::postConstructor()
{
	MFnDependencyNode nodeFn(thisMObject());
	nodeFn.setName("baseLocShape#");

	m_fileInitialized = false;

	// Check preset folder
	BaseLoc::checkPresetFolder();
}

void* BaseLoc::creator() 
{
	return new BaseLoc(); 
}

bool BaseLoc::isTransparent() const
{ 
	return true;
}



#if MAYA_API_VERSION > 201600

BaseLocOverride::BaseLocOverride(const MObject& obj) : MHWRender::MPxDrawOverride(obj, BaseLocOverride::draw, false)
{
	fModelEditorChangedCbId = MEventMessage::addEventCallback("modelEditorChanged", OnModelEditorChanged, this);
	MStatus status;
	MFnDependencyNode node(obj, &status);

	fBaseLoc = status ? dynamic_cast<BaseLoc*>(node.userNode()):NULL;
}

#else

BaseLocOverride::BaseLocOverride(const MObject& obj) : MHWRender::MPxDrawOverride(obj, BaseLocOverride::draw)
{
	fModelEditorChangedCbId = MEventMessage::addEventCallback("modelEditorChanged", OnModelEditorChanged, this);
	MStatus status;
	MFnDependencyNode node(obj, &status);

	fBaseLoc = status ? dynamic_cast<BaseLoc*>(node.userNode()):NULL;
}
#endif



BaseLocOverride::~BaseLocOverride()
{
	fBaseLoc = NULL;
	if (fModelEditorChangedCbId != 0)
	{
		MMessage::removeCallback(fModelEditorChangedCbId);
		fModelEditorChangedCbId = 0;
	}
}

void BaseLocOverride::OnModelEditorChanged(void *clientData)
{
	BaseLocOverride *ovr = static_cast<BaseLocOverride*>(clientData);

	if (ovr && ovr->fBaseLoc)
	{
		MHWRender::MRenderer::setGeometryDrawDirty(ovr->fBaseLoc->thisMObject());
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------

// VP 1.0 functions


MStatus BaseLoc::checkPresetFolder()
{
	MString s_path = BaseLoc::aPluginLoadPath + "/pBaseLoc.cfg";


	s_readPluginPath.clear();

	if (ifstream(s_path.asChar())) // If "pBaseLoc.cfg" exists
	{
		string line;

		ifstream myfile (s_path.asChar());
		if (myfile.is_open())
		{
			while ( getline (myfile,line) )
			{
				istringstream iss(line);
				s_readPluginPath = line.c_str();
			}

			myfile.close();

			// MGlobal::displayInfo(MString() + "[BaseLoc] pBaseLoc.cfg path: " + s_readPluginPath );
			o_presetPath.setRawFullName(s_readPluginPath);



		}

		if (s_readPluginPath.length() == 0) {

			MGlobal::displayWarning(MString() + "[BaseLoc] pBaseLoc.cfg is empty!" );
		}

		return MStatus::kSuccess;
	}



	else // If "pBaseLoc.cfg" does not exist
	{

		ofstream fout(s_path.asChar());
		MGlobal::displayInfo(MString() + "[BaseLoc] Writing default pBaseLoc.cfg to folder: " + s_path);

		MString s_path = BaseLoc::aPluginLoadPath + "/";

		fout << s_path.asChar();

		s_readPluginPath = s_path;


		return MStatus::kSuccess;


	}



	return MStatus::kSuccess;
}




MStatus BaseLoc::compute( const MPlug& plug, MDataBlock& data )
{

	MStatus status;

	MDataHandle h_outPresetPath = data.outputValue(aPresetFolderPath, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (s_readPluginPath.length() != 0)
	{
		h_outPresetPath.setString(s_readPluginPath);
	}

	return MS::kSuccess;
}


void BaseLoc::draw( M3dView & view, const MDagPath & /*path*/, M3dView::DisplayStyle style,  M3dView::DisplayStatus status )
{



#if MAYA_API_VERSION < 201600

	// Get ID

	MObject thisNode = thisMObject();
	MPlug p( thisNode, aLocID );
	int locID;
	p.getValue( locID );

	// Get display Locator
	p = MPlug(thisNode, aDispLoc);
	bool displLoc;
	p.getValue(displLoc);

	m_modelViewMat = MMatrix();
	view.modelViewMatrix(m_modelViewMat);

	//// Get time
	//p = MPlug(thisNode, aTime);
	//MTime currentTime;
	//p.getValue(currentTime);


	// Get input Locator Matricies
	p = MPlug(thisNode, aInLocPosA);
	MObject o_inlocMatA;
	p.getValue(o_inlocMatA);

	p = MPlug(thisNode, aInLocPosB);
	MObject o_inlocMatB;
	p.getValue(o_inlocMatB);

	MMatrix m_inlocMatA, m_inlocMatB;
	MFnMatrixData mfMA(o_inlocMatA);
	MFnMatrixData mfMB(o_inlocMatB);

	MTransformationMatrix inLocA_posMat(mfMA.matrix());
	MTransformationMatrix inLocB_posMat(mfMB.matrix());

	MPoint inLocA_pos = inLocA_posMat.getTranslation(MSpace::kWorld);
	MPoint inLocB_pos = inLocB_posMat.getTranslation(MSpace::kWorld);


	// Get Local position of the Shape node
	// Get local Pos X
	p = MPlug(thisNode, localPositionX);
	double localPosX;
	p.getValue(localPosX);

	// Get local Pos Y
	p = MPlug(thisNode, localPositionY);
	double localPosY;
	p.getValue(localPosY);

	// Get local Pos Z
	p = MPlug(thisNode, localPositionZ);
	double localPosZ;
	p.getValue(localPosZ);


	// Get offsetX
	p = MPlug(thisNode, aOffsetX);
	double offsetX;
	p.getValue(offsetX);

	// Get offsetY
	p = MPlug(thisNode, aOffsetY);
	double offsetY;
	p.getValue(offsetY);

	// Get offsetZ
	p = MPlug(thisNode, aOffsetZ);
	double offsetZ;
	p.getValue(offsetZ);


	// Get rotateX
	p = MPlug(thisNode, aRotateX);
	double rotateX;
	p.getValue(rotateX);

	// Get rotateY
	p = MPlug(thisNode, aRotateY);
	double rotateY;
	p.getValue(rotateY);

	// Get rotateZ
	p = MPlug(thisNode, aRotateZ);
	double rotateZ;
	p.getValue(rotateZ);



	// Get scaleX
	p = MPlug(thisNode, aScaleX);
	double scaleX;
	p.getValue(scaleX);

	// Get scaleY
	p = MPlug(thisNode, aScaleY);
	double scaleY;
	p.getValue(scaleY);

	// Get scaleZ
	p = MPlug(thisNode, aScaleZ);
	double scaleZ;
	p.getValue(scaleZ);

	double scale[3] = {scaleX,scaleY,scaleZ};








	// Calculate rotation
	MEulerRotation rotOffEuler ( rotateX  * ( M_PI / 180.0 ), rotateY  * ( M_PI / 180.0 ), rotateZ * ( M_PI / 180.0 ), MEulerRotation::kXYZ );
	MTransformationMatrix rotOffTMatrix;


	rotOffTMatrix.setScale(scale, MSpace::kObject); 
	rotOffTMatrix.rotateBy(rotOffEuler, MSpace::kObject);

	MMatrix rM = rotOffTMatrix.asMatrix();

	// Get draw preset
	p = MPlug(thisNode, aDrawPresets);
	int drawPresets;
	p.getValue(drawPresets);

	// Get Icon draw type
	p = MPlug(thisNode, aDrawIconsTypes);
	int drawIconType;
	p.getValue(drawIconType); 

	// Get 2D icon draw type
	p = MPlug(thisNode, aTwoDIconsTypes);
	int draw_twod_IconType;
	p.getValue(draw_twod_IconType); 

	// Get Icon draw type
	p = MPlug(thisNode, aRadius);
	float r;
	p.getValue(r);

	// Get allways face camera
	p = MPlug(thisNode, aDispCard);
	bool dispCard;
	p.getValue(dispCard);

	// Get Line color
	MColor lineColor;
	p = MPlug( thisNode, aLineColor );
	MPlug c = p.child(0);
	c.getValue ( lineColor.r );
	c = p.child(1);
	c.getValue ( lineColor.g );
	c = p.child(2);
	c.getValue ( lineColor.b );

	// Get line alpha
	p = MPlug( thisNode, aLineAlpha );
	p.getValue ( lineColor.a );

	// Get polygon color
	MColor polygonColor;
	p = MPlug( thisNode, aPolygonColor );
	c = p.child(0);
	c.getValue ( polygonColor.r );
	c = p.child(1);
	c.getValue ( polygonColor.g );
	c = p.child(2);
	c.getValue ( polygonColor.b );

	// Get polygon alpha
	p = MPlug( thisNode, aPolygonAlpha );
	p.getValue ( polygonColor.a );


	// OFFEST Vector

	MVector offV(offsetX,offsetY,offsetZ);
	offV += MVector(localPosX,localPosY,localPosZ);

	MColor activeLineCol = MColor(lineColor.r,lineColor.g,lineColor.b,lineColor.a);
	MColor activePolyCol = MColor(polygonColor.r,polygonColor.g,polygonColor.b,polygonColor.a);

	if (displLoc)
	{

		// Draw locator
		view.beginGL();

		glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_PIXEL_MODE_BIT ); 


		float colour[4];
		glGetFloatv(GL_CURRENT_COLOR,colour);


		if ( status == M3dView::kActive ) 
		{
			activePolyCol = MColor(1.0,1.0,1.0, polygonColor.a + 0.25);
			activeLineCol = MColor(1.0,1.0,1.0, polygonColor.a + 0.25);
		}

		if ( status == M3dView::kLead ) 
		{
			activePolyCol = MColor(colour[0],colour[1],colour[2], polygonColor.a + 0.25);
			activeLineCol = MColor(colour[0],colour[1],colour[2], 1.0);
		} 

		if ( status == M3dView::kDormant ) 
		{
			activePolyCol = MColor(polygonColor.r,polygonColor.g,polygonColor.b,polygonColor.a);
		}

		if (status == M3dView::kWireFrame)
		{
			activePolyCol = MColor(0.0,0.0,0.0,1.0);
		}

		if (status == M3dView::kTemplate)
		{
			activePolyCol = MColor(0.0,0.0,1.0,1.0);
		}


		if ( polygonColor.a < 1.0f ) 
		{ 
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


		}

		glColor4d(activePolyCol.r,activePolyCol.g,activePolyCol.b, activePolyCol.a);

		// Draw
		glPointSize(5);


		// Draw Circle
		if (drawPresets == 0)
		{

			int division = 50;

			MFloatPointArray points( division );

			glBegin(GL_LINE_LOOP);

			//glColor4d(lineColor.r,lineColor.g,lineColor.b, lineColor.a);

			for (double i = 0; i < 2 * M_PI; i += M_PI / division)
			{
				MPoint circlePoint = MPoint(cos(i) * (r*0.5), 0.0, sin(i) * (r*0.5));
				circlePoint *= rM;
				MVector circlePointVector(circlePoint);

				if(dispCard)
				{

					MEulerRotation rotOffEuler ( 90  * ( M_PI / 180.0 ), 0.0,  0.0, MEulerRotation::kXYZ );
					MTransformationMatrix rotOffTMatrix;

					rotOffTMatrix.rotateBy(rotOffEuler, MSpace::kObject);

					circlePointVector *= rotOffTMatrix.asMatrix();
					circlePointVector *= m_modelViewMat.inverse();

				}

				circlePointVector += offV;



				glVertex3d(circlePointVector.x, circlePointVector.y, circlePointVector.z);
			}

			glEnd();


			//


			glBegin(GL_TRIANGLE_FAN);

			//glColor4d(polygonColor.r,polygonColor.g,polygonColor.b,polygonColor.a);

			for (double i = 0; i < 2 * M_PI; i += M_PI / division)
			{
				MPoint circlePoint = MPoint(cos(i) * (r*0.5), 0.0, sin(i) * (r*0.5));


				circlePoint *= rM;
				MVector circlePointVector(circlePoint);


				if(dispCard)
				{

					MEulerRotation rotOffEuler ( 90  * ( M_PI / 180.0 ), 0.0,  0.0, MEulerRotation::kXYZ );
					MTransformationMatrix rotOffTMatrix;

					rotOffTMatrix.rotateBy(rotOffEuler, MSpace::kObject);

					circlePointVector *= rotOffTMatrix.asMatrix();
					circlePointVector *= m_modelViewMat.inverse();

				}


				circlePointVector += offV;


				glVertex3d(circlePointVector.x, circlePointVector.y, circlePointVector.z);
			}

			glEnd();


			//


			glPopAttrib();

		}

		// Draw Box
		if (drawPresets == 1)
		{

			glBegin(GL_TRIANGLE_FAN);

			glVertex3f( (MPoint( 0.5f*r, -0.5f*r, 0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r, -0.5f*r, 0.5f*r)*rM).y + offV.y,(MPoint( 0.5f*r, -0.5f*r, 0.5f*r)*rM).z + offV.z );
			glVertex3f( (MPoint(  0.5f*r,  0.5f*r, 0.5f*r)*rM).x + offV.x, (MPoint(  0.5f*r,  0.5f*r, 0.5f*r)*rM).y + offV.y, (MPoint(  0.5f*r,  0.5f*r, 0.5f*r)*rM).z + offV.z );
			glVertex3f( (MPoint( -0.5f*r,  0.5f*r, 0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r,  0.5f*r, 0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r,  0.5f*r, 0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r, -0.5f*r, 0.5f*r)*rM).x + offV.x,  (MPoint( -0.5f*r, -0.5f*r, 0.5f*r)*rM).y + offV.y,  (MPoint( -0.5f*r, -0.5f*r, 0.5f*r)*rM).z + offV.z );
			glEnd();

			// Purple side - RIGHT
			glBegin(GL_TRIANGLE_FAN);

			glVertex3f( (MPoint( 0.5f*r, -0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r, -0.5f*r, -0.5f*r)*rM).y + offV.y,(MPoint( 0.5f*r, -0.5f*r, -0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( 0.5f*r,  0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r,  0.5f*r, -0.5f*r)*rM).y + offV.y,(MPoint( 0.5f*r,  0.5f*r, -0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( 0.5f*r,  0.5f*r,  0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r,  0.5f*r,  0.5f*r)*rM).y + offV.y,(MPoint( 0.5f*r,  0.5f*r,  0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( 0.5f*r, -0.5f*r,  0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r, -0.5f*r,  0.5f*r)*rM).y + offV.y,(MPoint( 0.5f*r, -0.5f*r,  0.5f*r)*rM).z + offV.z);
			glEnd();

			// Green side - LEFT
			glBegin(GL_TRIANGLE_FAN);

			glVertex3f( (MPoint( -0.5f*r, -0.5f*r,  0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r, -0.5f*r,  0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r, -0.5f*r,  0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r,  0.5f*r,  0.5f*r)*rM).x + offV.x,  (MPoint( -0.5f*r,  0.5f*r,  0.5f*r)*rM).y + offV.y, (MPoint( -0.5f*r,  0.5f*r,  0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r,  0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r,  0.5f*r, -0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r,  0.5f*r, -0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r, -0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r, -0.5f*r, -0.5f*r)*rM).y + offV.y, (MPoint( -0.5f*r, -0.5f*r, -0.5f*r)*rM).z + offV.z);
			glEnd();

			// Blue side - TOP
			glBegin(GL_TRIANGLE_FAN);

			glVertex3f( (MPoint(  0.5f*r,  0.5f*r,  0.5f*r)*rM).x + offV.x,  (MPoint(  0.5f*r,  0.5f*r,  0.5f*r)*rM).y + offV.y,  (MPoint(  0.5f*r,  0.5f*r,  0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( 0.5f*r,  0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r,  0.5f*r, -0.5f*r)*rM).y + offV.y,(MPoint( 0.5f*r,  0.5f*r, -0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r,  0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r,  0.5f*r, -0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r,  0.5f*r, -0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r,  0.5f*r,  0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r,  0.5f*r,  0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r,  0.5f*r,  0.5f*r)*rM).z + offV.z);
			glEnd();

			// Red side - BOTTOM
			glBegin(GL_TRIANGLE_FAN);

			glVertex3f( (MPoint( 0.5f*r, -0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r, -0.5f*r, -0.5f*r)*rM).y + offV.y, (MPoint( 0.5f*r, -0.5f*r, -0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( 0.5f*r, -0.5f*r,  0.5f*r)*rM).x + offV.x, (MPoint( 0.5f*r, -0.5f*r,  0.5f*r)*rM).y + offV.y, (MPoint( 0.5f*r, -0.5f*r,  0.5f*r)*rM).z + offV.z );
			glVertex3f( (MPoint( -0.5f*r, -0.5f*r,  0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r, -0.5f*r,  0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r, -0.5f*r,  0.5f*r)*rM).z + offV.z);
			glVertex3f( (MPoint( -0.5f*r, -0.5f*r, -0.5f*r)*rM).x + offV.x, (MPoint( -0.5f*r, -0.5f*r, -0.5f*r)*rM).y + offV.y,(MPoint( -0.5f*r, -0.5f*r, -0.5f*r)*rM).z + offV.z);
			glEnd();

			glEnd();
			glPopAttrib();

		}

		// Draw sphere
		if (drawPresets == 2)
		{
			int lats = 20;
			int longs = 20;

			for(int i = 0; i <= lats; i++) {
				double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
				double z0  = sin(lat0);
				double zr0 =  cos(lat0);
				z0 *= r*0.5;
				zr0 *= r*0.5;

				double lat1 = M_PI * (-0.5 + (double) i / lats);
				double z1 = sin(lat1);
				double zr1 = cos(lat1);
				z1 *= r*0.5;
				zr1 *= r*0.5;

				glBegin(GL_QUAD_STRIP);
				for(int j = 0; j <= longs; j++) 
				{
					double lng = 2 * M_PI * (double) (j - 1) / longs;
					double x = cos(lng);
					double y = sin(lng);

					MPoint a(float(x) * float(zr0), float(y) * float(zr0), float(z0));
					MPoint b(float(x) * float(zr1), float(y) * float(zr1), float(z1));

					a *= rM;
					b *= rM;

					glVertex3f(float(a.x) + offV.x,float(a.y) + offV.y,float(a.z) + offV.z);
					glVertex3f(float(b.x) + offV.x,float(b.y) + offV.y,float(b.z) + offV.z);
				}
				glEnd();

			}


		}

		// Draw Cone
		if (drawPresets == 3)
		{
			MPoint conePoint(0.0,  (r*0.5) , 0.0) ;

			glBegin(GL_TRIANGLE_FAN);

			conePoint *= rM;
			conePoint += offV;

			double division = 9;

			glVertex3f(conePoint.x,conePoint.y,conePoint.z);

			for (double angle = 0; angle < 2 * M_PI; angle+= M_PI / division) {
				conePoint = MPoint(sin(angle) *  (r*0.5), 0.0, cos(angle) *  (r*0.5));
				conePoint *= rM;
				conePoint += offV;
				glVertex3f(conePoint.x,conePoint.y,conePoint.z);
			}
			glEnd();

		}


		// Draw Rectangle
		if (drawPresets == 4)
		{

			glBegin(GL_TRIANGLE_FAN);

			MPoint a(-1*(r*0.5f), 0.0, -1*(r*0.5f));
			MPoint b(-1*(r*0.5f), 0.0, (r*0.5f));
			MPoint c((r*0.5f), 0.0, (r*0.5f));
			MPoint d((r*0.5f), 0.0, -1*(r*0.5f));

			a*=rM;
			b*=rM;
			c*=rM;
			d*=rM;

			glVertex3f(a.x + offV.x,a.y + offV.y,a.z + offV.z);
			glVertex3f(b.x + offV.x,b.y + offV.y,b.z + offV.z);
			glVertex3f(c.x + offV.x,c.y + offV.y,c.z + offV.z);
			glVertex3f(d.x + offV.x,d.y + offV.y,d.z + offV.z);

			glEnd();


			glColor4d(activeLineCol.r,activeLineCol.g,activeLineCol.b, activeLineCol.a);
			glBegin(GL_LINE_LOOP);

			glVertex3f(a.x + offV.x,a.y + offV.y,a.z + offV.z);
			glVertex3f(b.x + offV.x,b.y + offV.y,b.z + offV.z);
			glVertex3f(c.x + offV.x,c.y + offV.y,c.z + offV.z);
			glVertex3f(d.x + offV.x,d.y + offV.y,d.z + offV.z);

			glEnd();

			glPopAttrib();

		}


		// Draw Drag handle
		if (drawPresets == 5)
		{

			int division = 20;

			MFloatPointArray points( division );

			MTransformationMatrix rotDragMat;
			rotDragMat.rotateBy(rotOffEuler, MSpace::kObject);
			MMatrix rdM = rotDragMat.asMatrix();

			// Draw circle
			glBegin(GL_TRIANGLE_FAN);

			for (double i = 0; i < 2 * M_PI; i += M_PI / division)
			{

				MPoint circlePoint = MPoint(cos(i) * (r*0.5), sin(i) * (r*0.5), 0.0);
				MVector circlePointVector(circlePoint);


				circlePointVector += MVector(offsetX,offsetY,offsetZ);
				circlePointVector *= rdM;
				circlePointVector -= MVector(offsetX,offsetY,offsetZ);
				circlePointVector += offV;

				glVertex3d(circlePointVector.x, circlePointVector.y, circlePointVector.z);

			}

			glEnd();


			// Draw draghandle
			glBegin(GL_LINE_LOOP);
			glColor4d(activeLineCol.r,activeLineCol.g,activeLineCol.b, activeLineCol.a);
			glVertex3d( localPosX,localPosY,localPosZ );
			MVector endPV;

			endPV += MVector(offsetX,offsetY -(r*0.5) ,offsetZ);
			endPV *= rdM;
			endPV -= MVector(offsetX,offsetY,offsetZ);
			endPV += offV;


			glVertex3d( endPV.x,endPV.y,endPV.z );

			glEnd();

			glPopAttrib();

		}


		// Draw Icons
		if (drawPresets == 6 || drawPresets == 8)
		{



			int locPointsNum;
			int locTrianglesNum;



			MPointArray tmpA;
			if (drawIconType == 0) { locPointsNum = 37; locTrianglesNum = 144; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locCrossPoints[i][0] , m_locCrossPoints[i][1] , m_locCrossPoints[i][2]  )*r*rM + offV); }}
			if (drawIconType == 1) { locPointsNum = 26; locTrianglesNum = 261; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locBoxOpenedPoints[i][0] , m_locBoxOpenedPoints[i][1] , m_locBoxOpenedPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 2) { locPointsNum = 54; locTrianglesNum = 417; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locTagPoints[i][0] , m_locTagPoints[i][1] , m_locTagPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 3) { locPointsNum = 36; locTrianglesNum = 159; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locArrowUpPoints[i][0] , m_locArrowUpPoints[i][1] , m_locArrowUpPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 4) { locPointsNum = 149; locTrianglesNum = 336; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locHorizontalBorderPoints[i][0] , m_locHorizontalBorderPoints[i][1] , m_locHorizontalBorderPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 5) { locPointsNum = 99; locTrianglesNum = 678; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locCameraPoints[i][0] , m_locCameraPoints[i][1] , m_locCameraPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 6) { locPointsNum = 37; locTrianglesNum = 216; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locClosePoints[i][0] , m_locClosePoints[i][1] , m_locClosePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 7) { locPointsNum = 59; locTrianglesNum = 300; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locEyePoints[i][0] , m_locEyePoints[i][1] , m_locEyePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 8) { locPointsNum = 22; locTrianglesNum = 147; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locFlashPoints[i][0] , m_locFlashPoints[i][1] , m_locFlashPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 9) { locPointsNum = 44; locTrianglesNum = 279; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locTshirtPoints[i][0] , m_locTshirtPoints[i][1] , m_locTshirtPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 10){ locPointsNum = 51; locTrianglesNum = 294; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locLockPoints[i][0] , m_locLockPoints[i][1] , m_locLockPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 11){ locPointsNum = 147; locTrianglesNum = 870; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locGroupPoints[i][0] , m_locGroupPoints[i][1] , m_locGroupPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 12){ locPointsNum = 116; locTrianglesNum = 543; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locClockPoints[i][0] , m_locClockPoints[i][1] , m_locClockPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 13){ locPointsNum = 83; locTrianglesNum = 276; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locHomePoints[i][0] , m_locHomePoints[i][1] , m_locHomePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 14){ locPointsNum = 60; locTrianglesNum = 285; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locMalePoints[i][0] , m_locMalePoints[i][1] , m_locMalePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 15){ locPointsNum = 60; locTrianglesNum = 249; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locFemalePoints[i][0] , m_locFemalePoints[i][1] , m_locFemalePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 16){ locPointsNum = 57; locTrianglesNum = 246; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locLightbulbPoints[i][0] , m_locLightbulbPoints[i][1] , m_locLightbulbPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 17){ locPointsNum = 51; locTrianglesNum = 210; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locRotatecounterclockwisePoints[i][0] , m_locRotatecounterclockwisePoints[i][1] , m_locRotatecounterclockwisePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 18){ locPointsNum = 46; locTrianglesNum = 183; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locPinetreePoints[i][0] , m_locPinetreePoints[i][1] , m_locPinetreePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 19){ locPointsNum = 68; locTrianglesNum = 537; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locHazardPoints[i][0] , m_locHazardPoints[i][1] , m_locHazardPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 20){ locPointsNum = 53; locTrianglesNum = 204; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locMovePoints[i][0] , m_locMovePoints[i][1] , m_locMovePoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 21){ locPointsNum = 35; locTrianglesNum = 60; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locCornersPoints[i][0] , m_locCornersPoints[i][1] , m_locCornersPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 22){ locPointsNum = 35; locTrianglesNum = 138; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locDirectionPoints[i][0] , m_locDirectionPoints[i][1] , m_locDirectionPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 23){ locPointsNum = 108; locTrianglesNum = 669; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locManwalkPoints[i][0] , m_locManwalkPoints[i][1] , m_locManwalkPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 24){ locPointsNum = 48; locTrianglesNum = 303; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locFeetPoints[i][0] , m_locFeetPoints[i][1] , m_locFeetPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 25){ locPointsNum = 56; locTrianglesNum = 181; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locmuzzleflashPoints[i][0] , m_locmuzzleflashPoints[i][1] , m_locmuzzleflashPoints[i][2] )*r*rM + offV); }}
			if (drawIconType == 26){ locPointsNum = 49; locTrianglesNum = 138; for (int i = 0; i < locPointsNum; i++) { tmpA.append(MPoint(m_locOrientPoints[i][0] , m_locOrientPoints[i][1] , m_locOrientPoints[i][2] )*r*rM + offV); }}

			if (drawPresets == 8){ tmpA.clear(); locPointsNum = 69; locTrianglesNum = 378; tmpA.setLength(locPointsNum);  for (int i = 0; i < locPointsNum; i++) { tmpA.set(MPoint(m_CameraPoints[i][0]  , m_CameraPoints[i][1]  , m_CameraPoints[i][2])*r*rM + offV, i); }}

			// Calculate Outline points with breaks
			int vC = 0;
			for (int i = 0; i < locPointsNum; i++)
			{ 
				if (tmpA[i].distanceTo( MPoint(0.0f, 0.0f ,0.0f )*r*rM + offV ) == 0.0 )
				{
					vC += 1;
				}
			}
			vector<MPointArray>	locDrawPointsB;
			locDrawPointsB.clear();
			locDrawPointsB.resize(vC+1);


			int vB = 0;
			for (int i = 0; i < locPointsNum; i++)
			{

				if (tmpA[i].distanceTo( MPoint(0.0f, 0.0f ,0.0f )*r*rM + offV ) != 0.0)
				{
					locDrawPointsB[vB].append(tmpA[i]);
				}

				if (tmpA[i].distanceTo( MPoint(0.0f, 0.0f ,0.0f )*r*rM + offV ) == 0.0)
				{
					if (vB != vC)
					{
						vB += 1;
					}

				}

			}

			// calculate Triangles


			// Calculate Polygons

			MPointArray locDrawTriangles;
			locDrawTriangles.clear();

			for (int i = 0; i < locTrianglesNum; i++)
			{




				if (drawIconType == 0) { locDrawTriangles.append(MPoint(m_locCrossTriangles[i][0]*r , m_locCrossTriangles[i][1]*r , m_locCrossTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 1) { locDrawTriangles.append(MPoint(m_locBoxOpenedTriangles[i][0]*r , m_locBoxOpenedTriangles[i][1]*r , m_locBoxOpenedTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 2) { locDrawTriangles.append(MPoint(m_locTagTriangles[i][0]*r , m_locTagTriangles[i][1]*r , m_locTagTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 3) { locDrawTriangles.append(MPoint(m_locArrowUpTriangles[i][0]*r , m_locArrowUpTriangles[i][1]*r , m_locArrowUpTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 4) { locDrawTriangles.append(MPoint(m_locHorizontalBorderTriangles[i][0]*r , m_locHorizontalBorderTriangles[i][1]*r , m_locHorizontalBorderTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 5) { locDrawTriangles.append(MPoint(m_locCameraTriangles[i][0]*r , m_locCameraTriangles[i][1]*r , m_locCameraTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 6) { locDrawTriangles.append(MPoint(m_locCloseTriangles[i][0]*r , m_locCloseTriangles[i][1]*r , m_locCloseTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 7) { locDrawTriangles.append(MPoint(m_locEyeTriangles[i][0]*r , m_locEyeTriangles[i][1]*r , m_locEyeTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 8) { locDrawTriangles.append(MPoint(m_locFlashTriangles[i][0]*r , m_locFlashTriangles[i][1]*r , m_locFlashTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 9) { locDrawTriangles.append(MPoint(m_locTshirtTriangles[i][0]*r , m_locTshirtTriangles[i][1]*r , m_locTshirtTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 10){ locDrawTriangles.append(MPoint(m_locLockTriangles[i][0]*r , m_locLockTriangles[i][1]*r , m_locLockTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 11){ locDrawTriangles.append(MPoint(m_locGroupTriangles[i][0]*r , m_locGroupTriangles[i][1]*r , m_locGroupTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 12){ locDrawTriangles.append(MPoint(m_locClockTriangles[i][0]*r , m_locClockTriangles[i][1]*r , m_locClockTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 13){ locDrawTriangles.append(MPoint(m_locHomeTriangles[i][0]*r , m_locHomeTriangles[i][1]*r , m_locHomeTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 14){ locDrawTriangles.append(MPoint(m_locMaleTriangles[i][0]*r , m_locMaleTriangles[i][1]*r , m_locMaleTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 15){ locDrawTriangles.append(MPoint(m_locFemaleTriangles[i][0]*r , m_locFemaleTriangles[i][1]*r , m_locFemaleTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 16){ locDrawTriangles.append(MPoint(m_locLightbulbTriangles[i][0]*r , m_locLightbulbTriangles[i][1]*r , m_locLightbulbTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 17){ locDrawTriangles.append(MPoint(m_locRotatecounterclockwiseTriangles[i][0]*r , m_locRotatecounterclockwiseTriangles[i][1]*r , m_locRotatecounterclockwiseTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 18){ locDrawTriangles.append(MPoint(m_locPinetreeTriangles[i][0]*r , m_locPinetreeTriangles[i][1]*r , m_locPinetreeTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 19){ locDrawTriangles.append(MPoint(m_locHazardTriangles[i][0]*r , m_locHazardTriangles[i][1]*r , m_locHazardTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 20){ locDrawTriangles.append(MPoint(m_locMoveTriangles[i][0]*r , m_locMoveTriangles[i][1]*r , m_locMoveTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 21){ locDrawTriangles.append(MPoint(m_locCornersTriangles[i][0]*r , m_locCornersTriangles[i][1]*r , m_locCornersTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 22){ locDrawTriangles.append(MPoint(m_locDirectionTriangles[i][0]*r , m_locDirectionTriangles[i][1]*r , m_locDirectionTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 23){ locDrawTriangles.append(MPoint(m_locManwalkTriangles[i][0]*r , m_locManwalkTriangles[i][1]*r , m_locManwalkTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 24){ locDrawTriangles.append(MPoint(m_locFeetTriangles[i][0]*r , m_locFeetTriangles[i][1]*r , m_locFeetTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 25){ locDrawTriangles.append(MPoint(m_locmuzzleflashTriangles[i][0]*r , m_locmuzzleflashTriangles[i][1]*r , m_locmuzzleflashTriangles[i][2]*r )*rM + offV); }
				if (drawIconType == 26){ locDrawTriangles.append(MPoint(m_locOrientTiangles[i][0]*r , m_locOrientTiangles[i][1]*r , m_locOrientTiangles[i][2]*r )*rM + offV); }
			}

			if (drawPresets == 8)
			{

				locDrawTriangles.clear();

				for (int i = 0; i < locTrianglesNum; i++)
				{
					locDrawTriangles.append(MPoint(m_CameraTiangles[i][0]*r , m_CameraTiangles[i][1]*r , m_CameraTiangles[i][2]*r )*rM + offV);
				}

			}

			// Draw the icons

			//glPopAttrib();

			int co = 0;

			glBegin(GL_TRIANGLES);

			for (int i = 0; i < (locDrawTriangles.length() / 3) ; i++)
			{


				glVertex3d(locDrawTriangles[co].x, locDrawTriangles[co].y, locDrawTriangles[co].z);
				glVertex3d(locDrawTriangles[co+1].x, locDrawTriangles[co+1].y, locDrawTriangles[co+1].z);
				glVertex3d(locDrawTriangles[co+2].x, locDrawTriangles[co+2].y, locDrawTriangles[co+2].z);



				co+=3;

			}

			glEnd();

			glColor4d(activeLineCol.r,activeLineCol.g,activeLineCol.b, activeLineCol.a);
			for (int i = 0; i < locDrawPointsB.size(); i++)
			{
				glBegin(GL_LINE_LOOP);
				for (unsigned int z = 0; z < locDrawPointsB[i].length(); z++)
				{
					glVertex3d(locDrawPointsB[i][z].x, locDrawPointsB[i][z].y, locDrawPointsB[i][z].z);
				}
				glEnd();

			}




		}



		// Draw Gyroscope
		if (drawPresets == 7)
		{

			int division = 50;

			MFloatPointArray points( division );

			glBegin(GL_LINE_LOOP);
			glColor4d(activeLineCol.r,activeLineCol.g + 1.0,activeLineCol.b, activeLineCol.a);
			for (double i = 0; i < 2 * M_PI; i += M_PI / division)
			{
				MPoint circlePoint = MPoint(cos(i) * (r*0.5), 0.0, sin(i) * (r*0.5));
				circlePoint *= rM;
				MVector circlePointVector(circlePoint);
				circlePointVector += offV;


				glVertex3d(circlePointVector.x, circlePointVector.y, circlePointVector.z);
			}

			glEnd();



			glBegin(GL_LINE_LOOP);
			glColor4d(activeLineCol.r + 1.0,activeLineCol.g,activeLineCol.b, activeLineCol.a);
			for (double i = 0; i < 2 * M_PI; i += M_PI / division)
			{
				MPoint circlePoint = MPoint(0.0, cos(i) * (r*0.5), sin(i) * (r*0.5));
				circlePoint *= rM;
				MVector circlePointVector(circlePoint);
				circlePointVector += offV;


				glVertex3d(circlePointVector.x, circlePointVector.y, circlePointVector.z);
			}
			glEnd();



			glBegin(GL_LINE_LOOP);
			glColor4d(activeLineCol.r,activeLineCol.g,activeLineCol.b + 1.0, activeLineCol.a);
			for (double i = 0; i < 2 * M_PI; i += M_PI / division)
			{
				MPoint circlePoint = MPoint(cos(i) * (r*0.5), sin(i) * (r*0.5), 0.0);
				circlePoint *= rM;
				MVector circlePointVector(circlePoint);
				circlePointVector += offV;


				glVertex3d(circlePointVector.x, circlePointVector.y, circlePointVector.z);
			}
			glEnd();




			glPopAttrib();

		}


		if (drawPresets == 9)
		{


			glPointSize(5);
			glColor3d(1.0,1.0,0.0);
			glBegin(GL_POINTS);
			glVertex3d(0.0,0.0,0.0);
			glEnd();
			glPopAttrib();
		}

		// Draw A-B
		if (drawPresets == 10)
		{

			glBegin(GL_LINE_LOOP);

			MPoint a = inLocA_pos;
			MPoint b = inLocB_pos;

			a*= rM;
			b*= rM;

			glVertex3d(a.x, a.y, a.z);
			glVertex3d(b.x, b.y, b.z);

			glEnd();
			glPopAttrib();

		}


		glPopAttrib();


	}
	view.endGL();

#endif
}

bool BaseLoc::isBounded() const
{
	return true;
}

MBoundingBox BaseLoc::boundingBox() const
{

	// Get the size
	MObject thisNode = thisMObject();

	MPlug plug( thisNode, aRadius );
	double multiplier;
	plug.getValue( multiplier );

	MPoint corner1(  -0.5, 0.0, -0.5 );
	MPoint corner2( 0.5, 0.0, 0.5);


	MPlug p;

	// Get input Locator Matricies
	p = MPlug(thisNode, aInLocPosA);
	MObject o_inlocMatA;
	p.getValue(o_inlocMatA);

	p = MPlug(thisNode, aInLocPosB);
	MObject o_inlocMatB;
	p.getValue(o_inlocMatB);

	MMatrix m_inlocMatA, m_inlocMatB;
	MFnMatrixData mfMA(o_inlocMatA);
	MFnMatrixData mfMB(o_inlocMatB);

	MTransformationMatrix inLocA_posMat(mfMA.matrix());
	MTransformationMatrix inLocB_posMat(mfMB.matrix());

	MPoint inLocA_pos = inLocA_posMat.getTranslation(MSpace::kWorld);
	MPoint inLocB_pos = inLocB_posMat.getTranslation(MSpace::kWorld);

	// Get allways face camera
	p = MPlug(thisNode, aDispCard);
	bool b_faceCam;
	p.getValue(b_faceCam);


	// Get Local position of the Shape node
	// Get local Pos X
	p = MPlug(thisNode, localPositionX);
	double localPosX;
	p.getValue(localPosX);

	// Get local Pos Y
	p = MPlug(thisNode, localPositionY);
	double localPosY;
	p.getValue(localPosY);

	// Get local Pos Z
	p = MPlug(thisNode, localPositionZ);
	double localPosZ;
	p.getValue(localPosZ);


	// Calculate offset Matrix


	// Get offsetX
	p = MPlug(thisNode, aOffsetX);
	double offsetX;
	p.getValue(offsetX);

	// Get offsetY
	p = MPlug(thisNode, aOffsetY);
	double offsetY;
	p.getValue(offsetY);

	// Get offsetZ
	p = MPlug(thisNode, aOffsetZ);
	double offsetZ;
	p.getValue(offsetZ);

	// Get rotateX
	p = MPlug(thisNode, aRotateX);
	double rotateX;
	p.getValue(rotateX);

	// Get rotateY
	p = MPlug(thisNode, aRotateY);
	double rotateY;
	p.getValue(rotateY);

	// Get rotateZ
	p = MPlug(thisNode, aRotateZ);
	double rotateZ;
	p.getValue(rotateZ);


	// Get scaleX
	p = MPlug(thisNode, aScaleX);
	double scaleX;
	p.getValue(scaleX);

	// Get scaleY
	p = MPlug(thisNode, aScaleY);
	double scaleY;
	p.getValue(scaleY);

	// Get scaleZ
	p = MPlug(thisNode, aScaleZ);
	double scaleZ;
	p.getValue(scaleZ);

	double scale[3] = {scaleX,scaleY,scaleZ};

	// Get draw preset
	p = MPlug(thisNode, aDrawPresets);
	int drawPresets;
	p.getValue(drawPresets);

	MEulerRotation rotOffEuler ( rotateX  * ( M_PI / 180.0 ), rotateY  * ( M_PI / 180.0 ), rotateZ * ( M_PI / 180.0 ), MEulerRotation::kXYZ );
	MTransformationMatrix rotOffTMatrix;


	rotOffTMatrix.setScale(scale, MSpace::kObject);
	rotOffTMatrix.rotateBy(rotOffEuler, MSpace::kObject);

	MMatrix rM = rotOffTMatrix.asMatrix();


	// OFFEST Vector
	MVector offV(offsetX,offsetY,offsetZ);
	offV += MVector(localPosX,localPosY,localPosZ);

	corner1 = corner1 *multiplier*rM + offV;
	corner2 = corner2 *multiplier*rM + offV;


	// Box, Sphere, Gyroscope, Cone
	if (drawPresets == 1 || drawPresets == 2 || drawPresets == 7 || drawPresets == 3)
	{

		
		corner1 = MPoint( -0.5, -0.5, 0.5 );
		corner2 = MPoint( 0.5, 0.5, -0.5 );

		corner1 = (corner1*multiplier *rM) + offV;
		corner2 = (corner2*multiplier *rM) + offV;

	}

		// Camera
	if (drawPresets == 8 )
	{

		
		corner1 = MPoint( -0.2, -0.145, -0.2 );
		corner2 = MPoint( 0.2, 0.45, 0.8 );

		corner1 = (corner1*multiplier *rM) + offV;
		corner2 = (corner2*multiplier *rM) + offV;

	}


	// Drag handle
	if ( drawPresets == 5  )
	{

		corner1 = MPoint( -multiplier*0.5, 0.0, 0.0 );
		//corner2 = MPoint( (multiplier*0.5), 1.0 +  (multiplier*0.5) , 0.0 );
		corner2 = MPoint( (multiplier*0.5) + offsetX,  (multiplier*0.5) + offsetY , offsetZ );

		corner1 *= rM;
		corner2 *= rM;

		corner1 += MVector(localPosX,localPosY,localPosZ);
		corner2 += MVector(localPosX,localPosY,localPosZ);

	}


	// A-B
	if ( drawPresets == 10  )
	{
		corner1 = inLocA_pos;
		corner2 = inLocB_pos;

		corner1 *= rM;
		corner2 *= rM;

		corner1 += MVector(localPosX,localPosY,localPosZ);
		corner2 += MVector(localPosX,localPosY,localPosZ);
	}


	if(b_faceCam)
	{
		corner1 = MPoint( -1.0, 0.0, -1.0 );
		corner2 = MPoint( 1.0, 0.0, 1.0 );



		corner1 = corner1 *multiplier;
		corner2 = corner2 *multiplier;

		corner1 *= rM;
		corner2 *= rM;

		corner1 *= m_modelViewMat.inverse();
		corner2 *= m_modelViewMat.inverse();


		corner1 += offV;
		corner2 += offV;
	}


	// IF file is selected as preset return the read BB corners immediately
	if (drawPresets == 11)
	{
		//MGlobal::displayInfo(MString() + objPath.partialPathName());

		// Get BB A
		p = MPlug(thisNode, BaseLoc::aBoundingBoxA);
		MFnNumericData numdFn_BBA(p.asMObject());
		float v3fVal_BBA[3];
		numdFn_BBA.getData3Float(v3fVal_BBA[0], v3fVal_BBA[1], v3fVal_BBA[2]);

		p = MPlug(thisNode, BaseLoc::aBoundingBoxB);
		MFnNumericData numdFn_BBB(p.asMObject());
		float v3fVal_BBB[3];
		numdFn_BBB.getData3Float(v3fVal_BBB[0], v3fVal_BBB[1], v3fVal_BBB[2]);

		corner1 = MPoint(v3fVal_BBA[0], v3fVal_BBA[1], v3fVal_BBA[2]);
		corner2 = MPoint(v3fVal_BBB[0], v3fVal_BBB[1], v3fVal_BBB[2]);

		corner1 = corner1 *multiplier;
		corner2 = corner2 *multiplier;

		corner1 *= rM;
		corner2 *= rM;

		corner1 *= m_modelViewMat.inverse();
		corner2 *= m_modelViewMat.inverse();


		corner1 += offV;
		corner2 += offV;


	}






	return MBoundingBox( corner1, corner2 );





}




// ----------------------------------------------------------------------------------------------------------------------------------------------------

// VP 2.0 Override functions

MHWRender::DrawAPI BaseLocOverride::supportedDrawAPIs() const
{
	// this plugin supports both GL and DX

#if MAYA_API_VERSION > 201600

	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile );

#else
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 );
#endif


}

bool BaseLocOverride::isBounded(const MDagPath& /*objPath*/, const MDagPath& /*cameraPath*/) const
{
	return true;
}

MBoundingBox BaseLocOverride::boundingBox( const MDagPath& objPath, const MDagPath& cameraPath) const
{

	MStatus status;

	double multiplier;

	MObject o_BaseLocNode = objPath.node(&status);
	if (status)
	{
		MPlug plug(o_BaseLocNode, BaseLoc::aRadius);
		if (!plug.isNull())
		{
			plug.getValue( multiplier );
		}
	}


	MPoint corner1(  -0.5, 0.0, -0.5 );
	MPoint corner2( 0.5, 0.0, 0.5 );

	MPlug p;



	// Get input Locator Matricies
	p = MPlug(o_BaseLocNode, BaseLoc::aInLocPosA);
	MObject o_inlocMatA;
	p.getValue(o_inlocMatA);

	p = MPlug(o_BaseLocNode, BaseLoc::aInLocPosB);
	MObject o_inlocMatB;
	p.getValue(o_inlocMatB);



	MMatrix m_inlocMatA, m_inlocMatB;
	MFnMatrixData mfMA(o_inlocMatA);
	MFnMatrixData mfMB(o_inlocMatB);

	MTransformationMatrix inLocA_posMat(mfMA.matrix());
	MTransformationMatrix inLocB_posMat(mfMB.matrix());

	MPoint inLocA_pos = inLocA_posMat.getTranslation(MSpace::kWorld);
	MPoint inLocB_pos = inLocB_posMat.getTranslation(MSpace::kWorld);

	// Get allways face camera
	p = MPlug(o_BaseLocNode, BaseLoc::aDispCard);
	bool dispCard;
	p.getValue(dispCard);

	// Get Local position of the Shape node
	// Get local Pos X
	p = MPlug(o_BaseLocNode, BaseLoc::localPositionX);
	double localPosX;
	p.getValue(localPosX);

	// Get local Pos Y
	p = MPlug(o_BaseLocNode, BaseLoc::localPositionY);
	double localPosY;
	p.getValue(localPosY);

	// Get local Pos Z
	p = MPlug(o_BaseLocNode, BaseLoc::localPositionZ);
	double localPosZ;
	p.getValue(localPosZ);

	// Calculate offset Matrix

	// Get offsetX
	p = MPlug(o_BaseLocNode, BaseLoc::aOffsetX);
	double offsetX;
	p.getValue(offsetX);

	// Get offsetY
	p = MPlug(o_BaseLocNode, BaseLoc::aOffsetY);
	double offsetY;
	p.getValue(offsetY);

	// Get offsetZ
	p = MPlug(o_BaseLocNode, BaseLoc::aOffsetZ);
	double offsetZ;
	p.getValue(offsetZ);

	// Get rotateX
	p = MPlug(o_BaseLocNode, BaseLoc::aRotateX);
	double rotateX;
	p.getValue(rotateX);

	// Get rotateY
	p = MPlug(o_BaseLocNode, BaseLoc::aRotateY);
	double rotateY;
	p.getValue(rotateY);

	// Get rotateZ
	p = MPlug(o_BaseLocNode, BaseLoc::aRotateZ);
	double rotateZ;
	p.getValue(rotateZ);

	// Get scaleX
	p = MPlug(o_BaseLocNode, BaseLoc::aScaleX);
	double scaleX;
	p.getValue(scaleX);

	// Get scaleY
	p = MPlug(o_BaseLocNode, BaseLoc::aScaleY);
	double scaleY;
	p.getValue(scaleY);

	// Get scaleZ
	p = MPlug(o_BaseLocNode, BaseLoc::aScaleZ);
	double scaleZ;
	p.getValue(scaleZ);

	double scale[3] = {scaleX,scaleY,scaleZ};

	// Get draw preset
	p = MPlug(o_BaseLocNode, BaseLoc::aDrawPresets);
	int drawPresets;
	p.getValue(drawPresets);





	MEulerRotation rotOffEuler ( rotateX  * ( M_PI / 180.0 ), rotateY  * ( M_PI / 180.0 ), rotateZ * ( M_PI / 180.0 ), MEulerRotation::kXYZ );
	MTransformationMatrix rotOffTMatrix;


	rotOffTMatrix.setScale(scale, MSpace::kObject);
	rotOffTMatrix.rotateBy(rotOffEuler, MSpace::kObject);

	MMatrix rM = rotOffTMatrix.asMatrix();


	// OFFEST Vector
	MVector offV(offsetX,offsetY,offsetZ);
	offV += MVector(localPosX,localPosY,localPosZ);



	corner1 = corner1 *multiplier*rM + offV;
	corner2 = corner2 *multiplier*rM + offV;

	

	// Box, Sphere, Gyroscope, Cone
	if (drawPresets == 1 || drawPresets == 2 || drawPresets == 7 || drawPresets == 3)
	{

		
		corner1 = MPoint( -0.5, -0.5, 0.5 );
		corner2 = MPoint( 0.5, 0.5, -0.5 );

		corner1 = (corner1*multiplier *rM) + offV;
		corner2 = (corner2*multiplier *rM) + offV;

	}

	// Camera
	if (drawPresets == 8 )
	{

		
		corner1 = MPoint( -0.2, -0.145, -0.2 );
		corner2 = MPoint( 0.2, 0.45, 0.8 );

		corner1 = (corner1*multiplier *rM) + offV;
		corner2 = (corner2*multiplier *rM) + offV;

	}



	// Drag handle
	if ( drawPresets == 5  )
	{

		corner1 = MPoint( -multiplier*0.5, 0.0, 0.0 );
		corner2 = MPoint( (multiplier*0.5) + offsetX,  (multiplier*0.5) + offsetY , offsetZ );

		corner1 *= rM;
		corner2 *= rM;

		corner1 += MVector(localPosX,localPosY,localPosZ);
		corner2 += MVector(localPosX,localPosY,localPosZ);

	}


	// A-B
	if ( drawPresets == 10  )
	{
		corner1 = inLocA_pos;
		corner2 = inLocB_pos;

		corner1 *= rM;
		corner2 *= rM;

		corner1 += MVector(localPosX,localPosY,localPosZ);
		corner2 += MVector(localPosX,localPosY,localPosZ);
	}

	if(dispCard)
	{
		corner1 = MPoint( -1.0, 0.0, -1.0 );
		corner2 = MPoint( 1.0, 0.0, 1.0 );



		corner1 = corner1 *multiplier;
		corner2 = corner2 *multiplier;

		corner1 *= rM;
		corner2 *= rM;

		corner1 *= m_modelViewMat.inverse();
		corner2 *= m_modelViewMat.inverse();


		corner1 += offV;
		corner2 += offV;
	}


	// IF file is selected as preset return the read BB corners immediately
	if (drawPresets == 11)
	{
		//MGlobal::displayInfo(MString() + objPath.partialPathName());

		// Get BB A
		p = MPlug(o_BaseLocNode, BaseLoc::aBoundingBoxA);
		MFnNumericData numdFn_BBA(p.asMObject());
		float v3fVal_BBA[3];
		numdFn_BBA.getData3Float(v3fVal_BBA[0], v3fVal_BBA[1], v3fVal_BBA[2]);

		p = MPlug(o_BaseLocNode, BaseLoc::aBoundingBoxB);
		MFnNumericData numdFn_BBB(p.asMObject());
		float v3fVal_BBB[3];
		numdFn_BBB.getData3Float(v3fVal_BBB[0], v3fVal_BBB[1], v3fVal_BBB[2]);

		corner1 = MPoint(v3fVal_BBA[0], v3fVal_BBA[1], v3fVal_BBA[2]);
		corner2 = MPoint(v3fVal_BBB[0], v3fVal_BBB[1], v3fVal_BBB[2]);

		corner1 = corner1 *multiplier;
		corner2 = corner2 *multiplier;

		corner1 *= rM;
		corner2 *= rM;

		corner1 *= m_modelViewMat.inverse();
		corner2 *= m_modelViewMat.inverse();


		corner1 += offV;
		corner2 += offV;


	}



	return MBoundingBox( corner1, corner2 );
}


// Called by Maya each time the object needs to be drawn.
MUserData* BaseLocOverride::prepareForDraw( const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* oldData)
{




	// Get outside data from plugs
	MStatus status;
	MObject o_BaseLocNode = objPath.node(&status);



	BaseLocData* data = dynamic_cast<BaseLocData*>(oldData);
	if (!data)
	{
		data = new BaseLocData();
	}

	// Temp plug
	MPlug p,c;
	MObject o;

	MPointArray inPointArray;
	MPointArray inTriangleArray;

	if (status)
	{

		// Get input point array
		p = MPlug(o_BaseLocNode, BaseLoc::aInPointArray);
		MObject o_inPointArray;
		p.getValue(o_inPointArray);
		MFnPointArrayData fn_aPoints(o_inPointArray);
		fn_aPoints.copyTo(inPointArray);

		// Get input point array
		p = MPlug(o_BaseLocNode, BaseLoc::aInTriangleArray);
		MObject o_inTriangleArray;
		p.getValue(o_inTriangleArray);
		MFnPointArrayData fn_aTriangles(o_inTriangleArray);
		fn_aTriangles.copyTo(inTriangleArray);

		// Get input Locator Matricies
		p = MPlug(o_BaseLocNode, BaseLoc::aInLocPosA);
		MObject o_inlocMatA;
		p.getValue(o_inlocMatA);

		p = MPlug(o_BaseLocNode, BaseLoc::aInLocPosB);
		MObject o_inlocMatB;
		p.getValue(o_inlocMatB);

		MMatrix m_inlocMatA, m_inlocMatB;
		MFnMatrixData mfMA(o_inlocMatA);
		MFnMatrixData mfMB(o_inlocMatB);

		MTransformationMatrix inLocA_posMat(mfMA.matrix());
		MTransformationMatrix inLocB_posMat(mfMB.matrix());

		data->m_inLocA_pos = inLocA_posMat.getTranslation(MSpace::kWorld);
		data->m_inLocB_pos = inLocB_posMat.getTranslation(MSpace::kWorld);


		// Get Local position of the Shape node
		// Get local Pos X
		p = MPlug(o_BaseLocNode, BaseLoc::localPositionX);
		p.getValue(data->m_localPosX);

		// Get local Pos Y
		p = MPlug(o_BaseLocNode, BaseLoc::localPositionY);
		p.getValue(data->m_localPosY);

		// Get local Pos Z
		p = MPlug(o_BaseLocNode, BaseLoc::localPositionZ);
		p.getValue(data->m_localPosZ);

		//// Get time
		//p = MPlug(o_BaseLocNode, BaseLoc::aTime);
		//MTime currentTime;
		//p.getValue(currentTime);
		//data->m_inTime = currentTime.value();


		// Get offsetX
		p = MPlug(o_BaseLocNode, BaseLoc::aOffsetX);
		p.getValue(data->m_offsetX);

		// Get offsetY
		p = MPlug(o_BaseLocNode, BaseLoc::aOffsetY);
		p.getValue(data->m_offsetY);

		// Get offsetZ
		p = MPlug(o_BaseLocNode, BaseLoc::aOffsetZ);
		p.getValue(data->m_offsetZ);

		// Get rotateX
		p = MPlug(o_BaseLocNode, BaseLoc::aRotateX);
		p.getValue(data->m_rotateX);

		// Get rotateY
		p = MPlug(o_BaseLocNode, BaseLoc::aRotateY);
		p.getValue(data->m_rotateY);

		// Get rotateZ
		p = MPlug(o_BaseLocNode, BaseLoc::aRotateZ);
		p.getValue(data->m_rotateZ);


		// Get scaleX
		p = MPlug(o_BaseLocNode, BaseLoc::aScaleX);
		p.getValue(data->m_scaleX);

		// Get scaleY
		p = MPlug(o_BaseLocNode, BaseLoc::aScaleY);
		p.getValue(data->m_scaleY);

		// Get scaleZ
		p = MPlug(o_BaseLocNode, BaseLoc::aScaleZ);
		p.getValue(data->m_scaleZ);



		// Get locID
		p = MPlug(o_BaseLocNode, BaseLoc::aLocID);
		p.getValue(data->m_locID);

		// Get drawing Mode
		p = MPlug(o_BaseLocNode, BaseLoc::aDrawPresets);
		p.getValue(data->m_drawPresets);

		// Get Icon draw type
		p = MPlug(o_BaseLocNode, BaseLoc::aDrawIconsTypes);
		p.getValue(data->m_drawIconType); 

		// Get 2D icon draw type
		p = MPlug(o_BaseLocNode, BaseLoc::aTwoDIconsTypes);
		p.getValue(data->m_draw_twod_IconType); 

		// Get radius
		p = MPlug(o_BaseLocNode, BaseLoc::aRadius);
		p.getValue(data->m_radius);

		// Get divisions plug
		p = MPlug( o_BaseLocNode, BaseLoc::aDivision );
		p.getValue(data->m_division);

		// Get Display numbers plug
		p = MPlug( o_BaseLocNode, BaseLoc::aDispNum );
		p.getValue(data->m_dispNum);

		// Get Display as Cards plug
		p = MPlug( o_BaseLocNode, BaseLoc::aDispCard );
		p.getValue(data->m_dispCard);

		// Get Display locators plug
		p = MPlug( o_BaseLocNode, BaseLoc::aDrawOnTop );
		p.getValue(data->m_drawOnTop);

		// Get Display locators plug
		p = MPlug( o_BaseLocNode, BaseLoc::aDispLoc );
		p.getValue(data->m_dispLoc);

		// Get Display locators Pivot plug
		p = MPlug( o_BaseLocNode, BaseLoc::aDispLocPivot );
		p.getValue(data->m_dispLocPivot);

		// Get the width of the lines plug
		p = MPlug( o_BaseLocNode, BaseLoc::aLineWidth );
		p.getValue(data->m_lineWidth);

		// Get the opacity of the lines plug
		p = MPlug( o_BaseLocNode, BaseLoc::aLineAlpha );
		p.getValue(data->m_lineAlpha);

		// Get the opacity of the plugin plug
		p = MPlug( o_BaseLocNode, BaseLoc::aPolygonAlpha );
		p.getValue(data->m_polygonAlpha);

		// Get the size for points plug
		p = MPlug( o_BaseLocNode, BaseLoc::aPointSize );
		p.getValue(data->m_pointSize);

		// Get line Style
		p = MPlug( o_BaseLocNode, BaseLoc::aLineStyle );
		p.getValue(data->m_lineStyle);

		// Get line Style
		p = MPlug( o_BaseLocNode, BaseLoc::aPaintStyle );
		p.getValue(data->m_paintStyle);

		// Get Line color
		p = MPlug( o_BaseLocNode, BaseLoc::aLineColor );
		c = p.child(0);
		c.getValue ( data->m_lineColor.r );
		c = p.child(1);
		c.getValue ( data->m_lineColor.g );
		c = p.child(2);
		c.getValue ( data->m_lineColor.b );

		// Get Polygon color
		p = MPlug( o_BaseLocNode, BaseLoc::aPolygonColor );
		c = p.child(0);
		c.getValue ( data->m_polygonColor.r );
		c = p.child(1);
		c.getValue ( data->m_polygonColor.g );
		c = p.child(2);
		c.getValue ( data->m_polygonColor.b );

		// Text

		//static MObject		aDispText;
		//static MObject		aTextPosition;
		//static MObject      aTextAlignment;
		//static MObject      eTextIncline;
		//static MObject      aTextWeight;
		//static MObject      aTextStretch;
		//static MObject      aTextLine;
		//static MObject      aTextBoxSize;
		//static MObject      aText;
		//static MObject      aTextBoxColor;
		//static MObject      aTextBoxTransparency;
		//static MObject      aTextFontSize;
		//static MObject      aFontFaceName;

		// --------------------------------------------

		//bool					m_dispText;

		//MString					m_text;
		//unsigned int			m_textFontSize;
		//unsigned int			m_fontFaceIndex;

		//MHWRender::MUIDrawManager::TextAlignment m_textAlignment;
		//int						m_textIncline;
		//int						m_textWeight;
		//int						m_textStretch;
		//int						m_extLine;
		//int						m_textBoxWidth;
		//int						m_textBoxHeight;
		//MColor					m_textBoxColor;

		//static MStringArray		m_fFontList;


		// Get disp text
		p = MPlug( o_BaseLocNode, BaseLoc::aDispText );
		p.getValue(data->m_dispText);

		// Get text position
		p = MPlug( o_BaseLocNode, BaseLoc::aTextPosition );
		o = p.asMObject();
		MFnNumericData nData(o);
		nData.getData(data->m_textPosition.x, data->m_textPosition.y, data->m_textPosition.z);
		data->m_textPosition.w = 1.0f;

		data->m_textPosition.x += data->m_offsetX;
		data->m_textPosition.y += data->m_offsetY;
		data->m_textPosition.z += data->m_offsetZ;

		// Get line Style
		p = MPlug( o_BaseLocNode, BaseLoc::aText );
		data->m_text = p.asString();

		// Get textfont size
		p = MPlug( o_BaseLocNode, BaseLoc::aTextFontSize );
		data->m_textFontSize = max(p.asInt(), 0);

		// Get font face index
		p = MPlug( o_BaseLocNode, BaseLoc::aFontFaceName );
		data->m_fontFaceIndex = (unsigned int)p.asInt();

		// Get text alignment
		p = MPlug( o_BaseLocNode, BaseLoc::aTextAlignment );
		data->m_textAlignment = (MHWRender::MUIDrawManager::TextAlignment)p.asShort();

		// Get text incline 
		p = MPlug( o_BaseLocNode, BaseLoc::aTextIncline );
		data->m_textIncline = p.asInt();

		// Get text weight 
		p = MPlug( o_BaseLocNode, BaseLoc::aTextWeight );
		data->m_textWeight = p.asInt();

		// Get text stretch 
		p = MPlug( o_BaseLocNode, BaseLoc::aTextStretch );
		data->m_textStretch = p.asInt();

		// Get text line 
		p = MPlug( o_BaseLocNode, BaseLoc::aTextLine );
		data->m_textLine = p.asInt();

		// Get text box size
		p = MPlug( o_BaseLocNode, BaseLoc::aTextBoxSize );
		o = p.asMObject();
		MFnNumericData nDataS(o);
		nDataS.getData(data->m_textBoxWidth, data->m_textBoxHeight);

		// Get text box color
		p = MPlug( o_BaseLocNode, BaseLoc::aTextBoxColor );
		o = p.asMObject();
		MFnNumericData nDataC(o);
		nDataC.getData(data->m_textBoxColor.r, data->m_textBoxColor.g, data->m_textBoxColor.b);

		// text box transparency
		p = MPlug( o_BaseLocNode, BaseLoc::aTextBoxTransparency );
		data->m_textBoxColor.a = 1.0f - p.asFloat();

		// ------------------------------------------------------------------

		//

		// Get distance to fade the locator by
		p = MPlug( o_BaseLocNode, BaseLoc::aFadeDistance );
		p.getValue(data->m_fadeDistance);
		if(data->m_fadeDistance <= 0.0){data->m_fadeDistance = 0.001;}

		//Toogle Fade by distance attribute
		p = MPlug( o_BaseLocNode, BaseLoc::aFadeByDistance );
		p.getValue(data->m_fadeByDistance);

	}

	// Calculate rotation
	MEulerRotation rotOffEuler ( data->m_rotateX * ( M_PI / 180.0 ), data->m_rotateY * ( M_PI / 180.0 ), data->m_rotateZ * ( M_PI / 180.0 ), MEulerRotation::kXYZ );
	MTransformationMatrix rotOffTMatrix;

	double scale[3] = {data->m_scaleX, data->m_scaleY, data->m_scaleZ};

	rotOffTMatrix.setScale(scale, MSpace::kObject);
	rotOffTMatrix.rotateBy(rotOffEuler, MSpace::kObject);

	//rotOffTMatrix.setTranslation(MVector(data->m_offsetX,data->m_offsetY,data->m_offsetZ),MSpace::kObject );

	//MGlobal::displayInfo(MString() + BaseLoc::localPositionX. );

	MMatrix rM = rotOffTMatrix.asMatrix();
	data->m_rotMatrix = rM;

	// Clear Polygon and outland data arrays
	data->m_locDrawPoints.clear();
	data->m_locDrawTriangles.clear();
	data->m_locDrawPointsA.clear();

	double r = data->m_radius;


	// Calculate Outline points length
	MPointArray tmpA;

	MVector offV(data->m_offsetX,data->m_offsetY,data->m_offsetZ);
	offV += MVector(data->m_localPosX,data->m_localPosY,data->m_localPosZ);



	// Icon + Camera + Box
	if (data->m_drawPresets == 6 || data->m_drawPresets == 8 || data->m_drawPresets == 1 || data->m_drawPresets == 11)
	{


		if (data->m_drawIconType == 0) { m_locPointsNum = 37; m_locTrianglesNum = 144;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locCrossPoints[i][0]  , m_locCrossPoints[i][1]  , m_locCrossPoints[i][2]  )*r*rM + offV, i); }}
		if (data->m_drawIconType == 1) { m_locPointsNum = 26; m_locTrianglesNum = 261;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locBoxOpenedPoints[i][0]  , m_locBoxOpenedPoints[i][1]  , m_locBoxOpenedPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 2) { m_locPointsNum = 54; m_locTrianglesNum = 417;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locTagPoints[i][0]  , m_locTagPoints[i][1]  , m_locTagPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 3) { m_locPointsNum = 36; m_locTrianglesNum = 159;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locArrowUpPoints[i][0]  , m_locArrowUpPoints[i][1]  , m_locArrowUpPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 4) { m_locPointsNum = 149; m_locTrianglesNum = 336;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locHorizontalBorderPoints[i][0]  , m_locHorizontalBorderPoints[i][1]  , m_locHorizontalBorderPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 5) { m_locPointsNum = 99; m_locTrianglesNum = 678;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locCameraPoints[i][0]  , m_locCameraPoints[i][1]  , m_locCameraPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 6) { m_locPointsNum = 37; m_locTrianglesNum = 216;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locClosePoints[i][0]  , m_locClosePoints[i][1]  , m_locClosePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 7) { m_locPointsNum = 59; m_locTrianglesNum = 300;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locEyePoints[i][0]  , m_locEyePoints[i][1]  , m_locEyePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 8) { m_locPointsNum = 22; m_locTrianglesNum = 147;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locFlashPoints[i][0]  , m_locFlashPoints[i][1]  , m_locFlashPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 9) { m_locPointsNum = 44; m_locTrianglesNum = 279;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locTshirtPoints[i][0]  , m_locTshirtPoints[i][1]  , m_locTshirtPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 10){ m_locPointsNum = 51; m_locTrianglesNum = 294;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locLockPoints[i][0]  , m_locLockPoints[i][1]  , m_locLockPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 11){ m_locPointsNum = 147; m_locTrianglesNum = 870;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locGroupPoints[i][0]  , m_locGroupPoints[i][1]  , m_locGroupPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 12){ m_locPointsNum = 116; m_locTrianglesNum = 543;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locClockPoints[i][0]  , m_locClockPoints[i][1]  , m_locClockPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 13){ m_locPointsNum = 83; m_locTrianglesNum = 276;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locHomePoints[i][0]  , m_locHomePoints[i][1]  , m_locHomePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 14){ m_locPointsNum = 60; m_locTrianglesNum = 285;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locMalePoints[i][0]  , m_locMalePoints[i][1]  , m_locMalePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 15){ m_locPointsNum = 60; m_locTrianglesNum = 249;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locFemalePoints[i][0]  , m_locFemalePoints[i][1]  , m_locFemalePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 16){ m_locPointsNum = 57; m_locTrianglesNum = 246;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locLightbulbPoints[i][0]  , m_locLightbulbPoints[i][1]  , m_locLightbulbPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 17){ m_locPointsNum = 51; m_locTrianglesNum = 210;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locRotatecounterclockwisePoints[i][0]  , m_locRotatecounterclockwisePoints[i][1]  , m_locRotatecounterclockwisePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 18){ m_locPointsNum = 46; m_locTrianglesNum = 183;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locPinetreePoints[i][0]  , m_locPinetreePoints[i][1]  , m_locPinetreePoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 19){ m_locPointsNum = 68; m_locTrianglesNum = 537;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locHazardPoints[i][0]  , m_locHazardPoints[i][1]  , m_locHazardPoints[i][2]  )*r*rM+ offV, i); }}
		if (data->m_drawIconType == 20){ m_locPointsNum = 53; m_locTrianglesNum = 204; tmpA.setLength(m_locPointsNum); for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locMovePoints[i][0], m_locMovePoints[i][1], m_locMovePoints[i][2])*r*rM+ offV,i);}}
		if (data->m_drawIconType == 21){ m_locPointsNum = 35; m_locTrianglesNum = 60;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locCornersPoints[i][0]  , m_locCornersPoints[i][1]  , m_locCornersPoints[i][2])*r*rM + offV, i); }}
		if (data->m_drawIconType == 22){ m_locPointsNum = 35; m_locTrianglesNum = 138;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locDirectionPoints[i][0]  , m_locDirectionPoints[i][1]  , m_locDirectionPoints[i][2])*r*rM + offV, i); }}
		if (data->m_drawIconType == 23){ m_locPointsNum = 108; m_locTrianglesNum = 669;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locManwalkPoints[i][0]  , m_locManwalkPoints[i][1]  , m_locManwalkPoints[i][2])*r*rM + offV, i); }}
		if (data->m_drawIconType == 24){ m_locPointsNum = 48; m_locTrianglesNum = 303;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locFeetPoints[i][0]  , m_locFeetPoints[i][1]  , m_locFeetPoints[i][2])*r*rM + offV, i); }}
		if (data->m_drawIconType == 25){ m_locPointsNum = 56; m_locTrianglesNum = 180;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locmuzzleflashPoints[i][0]  , m_locmuzzleflashPoints[i][1]  , m_locmuzzleflashPoints[i][2])*r*rM + offV, i); }}
		if (data->m_drawIconType == 26){ m_locPointsNum = 49; m_locTrianglesNum = 138;tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locOrientPoints[i][0]  , m_locOrientPoints[i][1]  , m_locOrientPoints[i][2])*r*rM + offV, i); }}

		// camera
		if (data->m_drawPresets == 8){ tmpA.clear(); m_locPointsNum = 69; m_locTrianglesNum = 378; tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_CameraPoints[i][0]  , m_CameraPoints[i][1]  , m_CameraPoints[i][2])*r*rM + offV, i); }}

		// box
		if (data->m_drawPresets == 1){ tmpA.clear(); m_locPointsNum = 20; m_locTrianglesNum = 36; tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(m_locBoxPoints[i][0]  , m_locBoxPoints[i][1]  , m_locBoxPoints[i][2])*r*rM + offV, i); }}

		// file
		if (data->m_drawPresets == 11){ tmpA.clear(); m_locPointsNum = inPointArray.length(); m_locTrianglesNum = inTriangleArray.length(); tmpA.setLength(m_locPointsNum);  for (int i = 0; i < m_locPointsNum; i++) { tmpA.set(MPoint(inPointArray[i][0]  , inPointArray[i][1]  , inPointArray[i][2])*r*rM + offV, i); }}



		// Calculate Outline points with breaks
		int vC = 0;
		for (int i = 0; i < m_locPointsNum; i++)
		{ 
			if (tmpA[i].distanceTo( MPoint(0.0f, 0.0f ,0.0f )*r*rM+ offV ) == 0.0 )
			{
				vC += 1;
			}
		}

		data->m_locDrawPointsA.resize(vC+1);


		int vB = 0;
		for (int i = 0; i < m_locPointsNum; i++)
		{

			if (tmpA[i].distanceTo( MPoint(0.0f, 0.0f ,0.0f )*r*rM+ offV ) != 0.0)
			{
				data->m_locDrawPointsA[vB].append(tmpA[i]);
			}

			if (tmpA[i].distanceTo( MPoint(0.0f, 0.0f ,0.0f )*r*rM+ offV ) == 0.0)
			{
				if (vB != vC)
				{
					vB += 1;
				}

			}

		}


		// Calculate Polygons
		for (int i = 0; i < m_locTrianglesNum; i++)
		{




			if (data->m_drawIconType == 0) { data->m_locDrawTriangles.append(MPoint(m_locCrossTriangles[i][0]*r , m_locCrossTriangles[i][1]*r , m_locCrossTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 1) { data->m_locDrawTriangles.append(MPoint(m_locBoxOpenedTriangles[i][0]*r , m_locBoxOpenedTriangles[i][1]*r , m_locBoxOpenedTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 2) { data->m_locDrawTriangles.append(MPoint(m_locTagTriangles[i][0]*r , m_locTagTriangles[i][1]*r , m_locTagTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 3) { data->m_locDrawTriangles.append(MPoint(m_locArrowUpTriangles[i][0]*r , m_locArrowUpTriangles[i][1]*r , m_locArrowUpTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 4) { data->m_locDrawTriangles.append(MPoint(m_locHorizontalBorderTriangles[i][0]*r , m_locHorizontalBorderTriangles[i][1]*r , m_locHorizontalBorderTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 5) { data->m_locDrawTriangles.append(MPoint(m_locCameraTriangles[i][0]*r , m_locCameraTriangles[i][1]*r , m_locCameraTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 6) { data->m_locDrawTriangles.append(MPoint(m_locCloseTriangles[i][0]*r , m_locCloseTriangles[i][1]*r , m_locCloseTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 7) { data->m_locDrawTriangles.append(MPoint(m_locEyeTriangles[i][0]*r , m_locEyeTriangles[i][1]*r , m_locEyeTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 8) { data->m_locDrawTriangles.append(MPoint(m_locFlashTriangles[i][0]*r , m_locFlashTriangles[i][1]*r , m_locFlashTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 9) { data->m_locDrawTriangles.append(MPoint(m_locTshirtTriangles[i][0]*r , m_locTshirtTriangles[i][1]*r , m_locTshirtTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 10){ data->m_locDrawTriangles.append(MPoint(m_locLockTriangles[i][0]*r , m_locLockTriangles[i][1]*r , m_locLockTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 11){ data->m_locDrawTriangles.append(MPoint(m_locGroupTriangles[i][0]*r , m_locGroupTriangles[i][1]*r , m_locGroupTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 12){ data->m_locDrawTriangles.append(MPoint(m_locClockTriangles[i][0]*r , m_locClockTriangles[i][1]*r , m_locClockTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 13){ data->m_locDrawTriangles.append(MPoint(m_locHomeTriangles[i][0]*r , m_locHomeTriangles[i][1]*r , m_locHomeTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 14){ data->m_locDrawTriangles.append(MPoint(m_locMaleTriangles[i][0]*r , m_locMaleTriangles[i][1]*r , m_locMaleTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 15){ data->m_locDrawTriangles.append(MPoint(m_locFemaleTriangles[i][0]*r , m_locFemaleTriangles[i][1]*r , m_locFemaleTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 16){ data->m_locDrawTriangles.append(MPoint(m_locLightbulbTriangles[i][0]*r , m_locLightbulbTriangles[i][1]*r , m_locLightbulbTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 17){ data->m_locDrawTriangles.append(MPoint(m_locRotatecounterclockwiseTriangles[i][0]*r , m_locRotatecounterclockwiseTriangles[i][1]*r , m_locRotatecounterclockwiseTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 18){ data->m_locDrawTriangles.append(MPoint(m_locPinetreeTriangles[i][0]*r , m_locPinetreeTriangles[i][1]*r , m_locPinetreeTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 19){ data->m_locDrawTriangles.append(MPoint(m_locHazardTriangles[i][0]*r , m_locHazardTriangles[i][1]*r , m_locHazardTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 20){ data->m_locDrawTriangles.append(MPoint(m_locMoveTriangles[i][0]*r , m_locMoveTriangles[i][1]*r , m_locMoveTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 21){ data->m_locDrawTriangles.append(MPoint(m_locCornersTriangles[i][0]*r , m_locCornersTriangles[i][1]*r , m_locCornersTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 22){ data->m_locDrawTriangles.append(MPoint(m_locDirectionTriangles[i][0]*r , m_locDirectionTriangles[i][1]*r , m_locDirectionTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 23){ data->m_locDrawTriangles.append(MPoint(m_locManwalkTriangles[i][0]*r , m_locManwalkTriangles[i][1]*r , m_locManwalkTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 24){ data->m_locDrawTriangles.append(MPoint(m_locFeetTriangles[i][0]*r , m_locFeetTriangles[i][1]*r , m_locFeetTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 25){ data->m_locDrawTriangles.append(MPoint(m_locmuzzleflashTriangles[i][0]*r , m_locmuzzleflashTriangles[i][1]*r , m_locmuzzleflashTriangles[i][2]*r )*rM + offV); }
			if (data->m_drawIconType == 26){ data->m_locDrawTriangles.append(MPoint(m_locOrientTiangles[i][0]*r , m_locOrientTiangles[i][1]*r , m_locOrientTiangles[i][2]*r )*rM + offV); }

		}

	}


	// Camera
	if (data->m_drawPresets == 8)
	{

		data->m_locDrawTriangles.clear();

		for (int i = 0; i < m_locTrianglesNum; i++)
		{
			data->m_locDrawTriangles.append(MPoint(m_CameraTiangles[i][0]*r , m_CameraTiangles[i][1]*r , m_CameraTiangles[i][2]*r )*rM + offV);
		}

	}

	// File
	if (data->m_drawPresets == 11)
	{

		data->m_locDrawTriangles.clear();

		for (int i = 0; i < m_locTrianglesNum; i++)
		{
			data->m_locDrawTriangles.append(MPoint(inTriangleArray[i][0]*r , inTriangleArray[i][1]*r , inTriangleArray[i][2]*r )*rM + offV);
		}

	}






	// Circle
	if (data->m_drawPresets == 0)
	{

		data->m_locDrawPoints.clear();
		int division = 50;

		for (double i = 0; i < 2 * M_PI; i += M_PI / division)
		{
			MPoint circlePoint = MPoint(cos(i) * (r*0.5), 0.0, sin(i) * (r*0.5));
			circlePoint *= rM;
			MVector circlePointVector(circlePoint);
			circlePointVector += offV;

			data->m_locDrawPoints.append(circlePointVector);

		}





		//
		data->m_locDrawTriangles.clear();

		for (double i = 0; i < 2 * M_PI; i += M_PI / division)
		{
			MPoint circlePoint = MPoint(cos(i) * (r*0.5), 0.0, sin(i) * (r*0.5));

			circlePoint *= rM;
			MVector circlePointVector(circlePoint);
			circlePointVector += offV;

			MPoint centerP(MPoint::origin);
			centerP *= rM;
			centerP += offV;

			data->m_locDrawTriangles.append(circlePointVector);
			data->m_locDrawTriangles.append(centerP);

		}

	}


	// Box
	if (data->m_drawPresets == 1)
	{



		data->m_locDrawTriangles.clear();

		for (int i = 0; i < m_locTrianglesNum; i++)
		{
			data->m_locDrawTriangles.append(MPoint(m_locBoxTriangles[i][0]*r , m_locBoxTriangles[i][1]*r , m_locBoxTriangles[i][2]*r )*rM + offV);
		}



	}

	// Sphere
	if (data->m_drawPresets == 2)
	{
		data->m_locDrawPoints.clear();
		data->m_locDrawTriangles.clear();

		int lats = 20;
		int longs = 20;

		for(int i = 0; i <= lats; i++) {
			double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
			double z0  = sin(lat0);
			double zr0 =  cos(lat0);
			z0 *= r*0.5;
			zr0 *= r*0.5;

			double lat1 = M_PI * (-0.5 + (double) i / lats);
			double z1 = sin(lat1);
			double zr1 = cos(lat1);
			z1 *= r*0.5;
			zr1 *= r*0.5;

			for(int j = 0; j <= longs; j++) 
			{
				double lng = 2 * M_PI * (double) (j - 1) / longs;
				double x = cos(lng);
				double y = sin(lng);

				MPoint a(float(x) * float(zr0), float(y) * float(zr0), float(z0));
				MPoint b(float(x) * float(zr1), float(y) * float(zr1), float(z1));

				a *= rM;
				b *= rM;

				data->m_locDrawTriangles.append(float(a.x) + offV.x,float(a.y) + offV.y,float(a.z) + offV.z);
				data->m_locDrawTriangles.append(float(b.x) + offV.x,float(b.y) + offV.y,float(b.z) + offV.z);

			}


		}

	}

	// Draw Rectangle
	if (data->m_drawPresets == 4)
	{

		data->m_locDrawPoints.clear();
		data->m_locDrawTriangles.clear();


		MPoint a(-1*(r*0.5f), 0.0, -1*(r*0.5f));
		MPoint b(-1*(r*0.5f), 0.0, (r*0.5f));
		MPoint c((r*0.5f), 0.0, (r*0.5f));
		MPoint d((r*0.5f), 0.0, -1*(r*0.5f));

		a*=rM;
		b*=rM;
		c*=rM;
		d*=rM;

		data->m_locDrawPoints.append(a.x + offV.x,a.y + offV.y,a.z + offV.z);
		data->m_locDrawPoints.append(b.x + offV.x,b.y + offV.y,b.z + offV.z);
		data->m_locDrawPoints.append(c.x + offV.x,c.y + offV.y,c.z + offV.z);
		data->m_locDrawPoints.append(d.x + offV.x,d.y + offV.y,d.z + offV.z);
		data->m_locDrawPoints.append(a.x + offV.x,a.y + offV.y,a.z + offV.z);

		data->m_locDrawTriangles.append(a.x + offV.x,a.y + offV.y,a.z + offV.z);
		data->m_locDrawTriangles.append(b.x + offV.x,b.y + offV.y,b.z + offV.z);
		data->m_locDrawTriangles.append(c.x + offV.x,c.y + offV.y,c.z + offV.z);
		data->m_locDrawTriangles.append(c.x + offV.x,c.y + offV.y,c.z + offV.z);
		data->m_locDrawTriangles.append(d.x + offV.x,d.y + offV.y,d.z + offV.z);
		data->m_locDrawTriangles.append(a.x + offV.x,a.y + offV.y,a.z + offV.z);

	}



	// Cone
	if (data->m_drawPresets == 3)
	{
		data->m_locDrawPoints.clear();
		data->m_locDrawTriangles.clear();

		MPoint conePoint(0.0,  (r*0.5) , 0.0) ;

		conePoint *= rM;
		conePoint += offV;

		double division = 9;

		MPoint conePoint_top = conePoint;

		for (double angle = 0; angle < 2 * M_PI; angle+= M_PI / division) {
			conePoint = MPoint(sin(angle) *  (r*0.5), 0.0, cos(angle) *  (r*0.5));
			conePoint *= rM;
			conePoint += offV;

			data->m_locDrawTriangles.append(conePoint);
			data->m_locDrawTriangles.append(conePoint_top);

		}

	}




	// Draw Gyroscope
	if (data->m_drawPresets == 7)
	{

		data->m_locDrawPoints.clear();
		data->m_locDrawTriangles.clear();
		data->m_locDrawPointsA.clear();
		data->m_locDrawPointsA.resize(3);

		int division = 50;

		MFloatPointArray points( division );


		for (double i = 0; i < 2 * M_PI; i += M_PI / division)
		{
			MPoint circlePoint = MPoint(cos(i) * (r*0.5), 0.0, sin(i) * (r*0.5));
			circlePoint *= rM;
			MVector circlePointVector(circlePoint);
			circlePointVector += offV;

			data->m_locDrawPointsA[0].append(circlePointVector);
		}






		for (double i = 0; i < 2 * M_PI; i += M_PI / division)
		{
			MPoint circlePoint = MPoint(0.0, cos(i) * (r*0.5), sin(i) * (r*0.5));
			circlePoint *= rM;
			MVector circlePointVector(circlePoint);
			circlePointVector += offV;


			data->m_locDrawPointsA[1].append(circlePointVector);
		}




		for (double i = 0; i < 2 * M_PI; i += M_PI / division)
		{
			MPoint circlePoint = MPoint(cos(i) * (r*0.5), sin(i) * (r*0.5), 0.0);
			circlePoint *= rM;
			MVector circlePointVector(circlePoint);
			circlePointVector += offV;


			data->m_locDrawPointsA[2].append(circlePointVector);
		}



	}



	// Add locator Center point
	data->m_locDrawCenter = MPoint(m_locCenter[0],m_locCenter[1],m_locCenter[2]) + offV;

	// get correct color based on the state of object, e.g. active or dormant
	data->m_locColor = MHWRender::MGeometryUtilities::wireframeColor(objPath);

	return data;
}


void BaseLocOverride::addUIDrawables( const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data)
{
	BaseLocData* pLocatorData = (BaseLocData*)data;
	if (!pLocatorData)
	{
		return;
	}

	int apiVer = MGlobal::apiVersion();


#if MAYA_API_VERSION > 201600
	if (pLocatorData->m_drawOnTop )
	{
		drawManager.beginDrawInXray();
	}
	if (!pLocatorData->m_drawOnTop)
	{
		drawManager.beginDrawable();
	}

#else
	drawManager.beginDrawable();

#endif


	if (pLocatorData->m_dispLoc)
	{



		//drawManager.mesh(MHWRender::MUIDrawManager::kClosedLine, pLocatorData->m_locDrawPoints);

		// Icon Names

		// CIRCLE_24 // 
		// DIAMOND_24 // 
		// SQUARE_24 // 
		// UP_TRIANGLE_24 // 
		// DOWN_TRIANGLE_24 // 
		// HEXAGON_24 // 
		// CIRCLE_16 // 
		// DIAMOND_16 // 
		// SQUARE_16 // 
		// UP_TRIANGLE_16 // 
		// DOWN_TRIANGLE_16 // 
		// RIGHT_TRIANGLE_16 // 
		// HEXAGON_16 // 
		// RESIZE // 
		// OPTIONS // 
		// POINT_LIGHT // 
		// AMBIENT_LIGHT // 
		// OMNI_EMITTER // 
		// VOLUME_LIGHT // 
		// AIR_FIELD // 
		// DRAG_FIELD // 
		// GRAVITY_FIELD // 
		// NEWTON_FIELD // 
		// RADIAL_FIELD // 
		// TURBULENCE_FIELD // 
		// UNIFORM_FIELD // 
		// VORTEX_FIELD // 
		// UNLOCK_MONO // 
		// LOCK_MONO // 
		// NUCLEUS // 
		// DOT // 
		// CROSS // 
		// DRAG_POINT // 
		// OFF_RADIO_BTN // 
		// FFD_POINT // 
		// CURVE_ENDS // 
		// DRAG_PT // 
		// ON_RADIO_BTN // 
		// PIVOT // 
		// HOLLOW_BOX // 
		// ROTATE_PIVOT // 
		// SELECT_HANDLE_ROOT // 
		// SOLID_BOX // 
		// HOLLOW_TRIANGLE // 
		// SCALE_PIVOT // 
		// SELECT_HANDLE // 
		// U_CV // 
		// V_CV // 
		// X_AXIS // 
		// Y_AXIS // 
		// Z_AXIS // 
		// IK // 
		// FK //

		drawManager.setDepthPriority(5);


		// Offset

		MVector offV(pLocatorData->m_offsetX + pLocatorData->m_localPosX, pLocatorData->m_offsetY + pLocatorData->m_localPosY, pLocatorData->m_offsetZ + pLocatorData->m_localPosZ);


		MPoint center;
		MVector normal;
		MVector vU(0.0,1.0,0.0);
		MVector vR(1.0,0.0,0.0);
		MVector vN(0.0,0.0,1.0);

		double r = pLocatorData->m_radius;
		r *= 0.5;

		MColor baseCol = pLocatorData->m_locColor;

		bool drawFill = true;

		if (pLocatorData->m_polygonAlpha == 0.0)
		{
			drawFill = false;
		}

		float bCD_r = baseCol.r - pLocatorData->m_polygonColor.r;
		float bCD_g = baseCol.g - pLocatorData->m_polygonColor.g;
		float bCD_b = baseCol.b - pLocatorData->m_polygonColor.b;

		MColor diffCol(bCD_r,bCD_g,bCD_b, 0.0);

		MColor fillCol, lineCol;

		fillCol = MColor( pLocatorData->m_polygonColor.r, pLocatorData->m_polygonColor.g, pLocatorData->m_polygonColor.b, pLocatorData->m_polygonAlpha );
		lineCol = MColor( pLocatorData->m_lineColor.r, pLocatorData->m_lineColor.g, pLocatorData->m_lineColor.b, pLocatorData->m_lineAlpha );

		if ( MHWRender::MGeometryUtilities::displayStatus(objPath) == M3dView::kDormant ) {
			fillCol = MColor( pLocatorData->m_polygonColor.r, pLocatorData->m_polygonColor.g, pLocatorData->m_polygonColor.b, pLocatorData->m_polygonAlpha );
			lineCol = MColor( pLocatorData->m_lineColor.r, pLocatorData->m_lineColor.g, pLocatorData->m_lineColor.b, pLocatorData->m_lineAlpha );
		}

		if ( frameContext.getDisplayStyle() & MHWRender::MDrawContext::kWireFrame ) {


			fillCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b, 0.0 );
			lineCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b,  pLocatorData->m_lineAlpha );
		}

		if ( MHWRender::MGeometryUtilities::displayStatus(objPath) == M3dView::kLead ) {


			fillCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b, pLocatorData->m_polygonAlpha );
			lineCol = MColor( pLocatorData->m_locColor.r + 0.25f, pLocatorData->m_locColor.g + 0.25f, pLocatorData->m_locColor.b + 0.25f,  pLocatorData->m_lineAlpha );
		}

		if ( MHWRender::MGeometryUtilities::displayStatus(objPath) == M3dView::kActive ) {

			fillCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b, pLocatorData->m_polygonAlpha );
			lineCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b,  pLocatorData->m_lineAlpha );
		}

		if ( MHWRender::MGeometryUtilities::displayStatus(objPath) == M3dView::kTemplate ) {

			fillCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b, pLocatorData->m_polygonAlpha );
			lineCol = MColor( pLocatorData->m_locColor.r, pLocatorData->m_locColor.g, pLocatorData->m_locColor.b,  pLocatorData->m_lineAlpha );
		}


		if (pLocatorData->m_lineStyle == 0)
		{
			drawManager.setLineStyle(drawManager.kSolid);
		}
		if (pLocatorData->m_lineStyle == 1)
		{
			drawManager.setLineStyle(drawManager.kShortDotted);
		}
		if (pLocatorData->m_lineStyle == 2)
		{
			drawManager.setLineStyle(drawManager.kShortDashed);
		}
		if (pLocatorData->m_lineStyle == 3)
		{
			drawManager.setLineStyle(drawManager.kDashed);
		}
		if (pLocatorData->m_lineStyle == 4)
		{
			drawManager.setLineStyle(drawManager.kDotted);
		}

		if (pLocatorData->m_paintStyle == 0)
		{
			drawManager.setPaintStyle(drawManager.kFlat);
		}
		if (pLocatorData->m_paintStyle == 1)
		{
			drawManager.setPaintStyle(drawManager.kStippled);
		}


		// Draw Circle
		if (pLocatorData->m_drawPresets == 0)
		{


			if (pLocatorData->m_dispCard)
			{
				vU = MVector(0.0,0.0,1.0);
				vU *= frameContext.getMatrix(MHWRender::MFrameContext::kViewInverseMtx);
			}


			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.mesh(MHWRender::MUIDrawManager::kTriStrip, pLocatorData->m_locDrawTriangles);
			}


			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);
			drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip, pLocatorData->m_locDrawPoints);


		}

		// Draw Box
		if (pLocatorData->m_drawPresets == 1)
		{
			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, pLocatorData->m_locDrawTriangles);
			}

			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);


			for (int i = 0; i < pLocatorData->m_locDrawPointsA.size(); i++)
			{
				drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip,  pLocatorData->m_locDrawPointsA[i]);
			}



		}


		// Draw Sphere
		if (pLocatorData->m_drawPresets == 2)
		{

			// Draw fill
			drawManager.setColor( fillCol );
			drawManager.mesh(MHWRender::MUIDrawManager::kTriStrip, pLocatorData->m_locDrawTriangles);

		}

		// Draw Cone
		if (pLocatorData->m_drawPresets == 3)
		{
			// Draw fill
			drawManager.setColor( fillCol );
			drawManager.mesh(MHWRender::MUIDrawManager::kTriStrip, pLocatorData->m_locDrawTriangles);

		}

		// Draw Rectangle
		if (pLocatorData->m_drawPresets == 4)
		{
			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.mesh(MHWRender::MUIDrawManager::kTriStrip, pLocatorData->m_locDrawTriangles);
			}
			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);
			drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip, pLocatorData->m_locDrawPoints);

		}

		// Draw Drag Handle
		if (pLocatorData->m_drawPresets == 5)
		{

			vU = MVector(0.0,0.0,1.0);
			vU *= pLocatorData->m_rotMatrix;

			center += MVector(pLocatorData->m_offsetX,pLocatorData->m_offsetY,pLocatorData->m_offsetZ);
			center *= pLocatorData->m_rotMatrix;
			center -= MVector(pLocatorData->m_offsetX,pLocatorData->m_offsetY,pLocatorData->m_offsetZ);
			center += offV;

			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.circle(center, vU, r, true);
			}

			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);
			drawManager.circle(center, vU, r, false);

			center = MVector(0.0,0.0,0.0);

			center += MVector(pLocatorData->m_offsetX,pLocatorData->m_offsetY - r,pLocatorData->m_offsetZ);
			center *= pLocatorData->m_rotMatrix;
			center -= MVector(pLocatorData->m_offsetX,pLocatorData->m_offsetY,pLocatorData->m_offsetZ);
			center += offV;


			MVector lineVec(center.x, center.y, center.z);

			drawManager.line(MPoint(pLocatorData->m_localPosX,pLocatorData->m_localPosY,pLocatorData->m_localPosZ) , MPoint(lineVec) );

		}


		// Icon

		if (pLocatorData->m_drawPresets == 6)
		{
			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, pLocatorData->m_locDrawTriangles);
			}

			if (pLocatorData->m_lineWidth != 0.0)
			{
				// Draw outline
				drawManager.setColor( lineCol );
				drawManager.setLineWidth(pLocatorData->m_lineWidth);

				for (int i = 0; i < pLocatorData->m_locDrawPointsA.size(); i++)
				{

					drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip, pLocatorData->m_locDrawPointsA[i]);
				}

			}





		}

		// Draw Gyroscope
		if (pLocatorData->m_drawPresets == 7)
		{
			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);

			drawManager.setPointSize(5);

			for (int i = 0; i < pLocatorData->m_locDrawPointsA.size(); i++)
			{

				if (i==0)
				{
					drawManager.setColor( lineCol + MColor(0.0,1.0,0.0)  );
					drawManager.point(pLocatorData->m_locDrawPointsA[i][0]);
				}

				if (i==1)
				{
					drawManager.setColor( lineCol + MColor(0.0,0.0,1.0)  );
					drawManager.point(pLocatorData->m_locDrawPointsA[i][0]);
				}

				if (i==2)
				{
					drawManager.setColor( lineCol + MColor(1.0,0.0,0.0)  );
				}

				drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip, pLocatorData->m_locDrawPointsA[i]);
			}


		}

		// Draw Camera
		if (pLocatorData->m_drawPresets == 8)
		{
			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, pLocatorData->m_locDrawTriangles);
			}

			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);


			for (int i = 0; i < pLocatorData->m_locDrawPointsA.size(); i++)
			{

				//drawManager.mesh(MHWRender::MUIDrawManager::kClosedLine, pLocatorData->m_locDrawPoints);
				drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip,  pLocatorData->m_locDrawPointsA[i]);
			}


		}


		// Draw File
		if (pLocatorData->m_drawPresets == 11)
		{
			if (drawFill)
			{
				// Draw fill
				drawManager.setColor( fillCol );
				drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, pLocatorData->m_locDrawTriangles);
			}



			// Draw outline
			drawManager.setColor( lineCol );
			drawManager.setLineWidth(pLocatorData->m_lineWidth);


			for (int i = 0; i < pLocatorData->m_locDrawPointsA.size(); i++)
			{

				//drawManager.mesh(MHWRender::MUIDrawManager::kClosedLine, pLocatorData->m_locDrawPoints);
				drawManager.mesh(MHWRender::MUIDrawManager::kLineStrip,  pLocatorData->m_locDrawPointsA[i]);
			}

			// MGlobal::displayInfo(MString() + pLocatorData->m_locDrawPointsA.size());

			//if (pLocatorData->m_locDrawPointsA.size() < 2 || pLocatorData->m_locDrawTriangles.length() == 0)
			//{

			//	drawManager.text(MPoint::origin, "No input point arrays", MHWRender::MUIDrawManager::TextAlignment::kCenter);
			//}

		}

		// Draw 2D Icons
		if (pLocatorData->m_drawPresets == 9)
		{

			center *= pLocatorData->m_rotMatrix;
			vU *= pLocatorData->m_rotMatrix;
			center += offV;

			// Draw fill
			drawManager.setColor( lineCol );


#if MAYA_API_VERSION > 201600


			if ( pLocatorData->m_draw_twod_IconType == 0) { drawManager.icon(center,"CIRCLE_24",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 1) { drawManager.icon(center,"DIAMOND_24",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 2) { drawManager.icon(center,"SQUARE_24",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 3) { drawManager.icon(center,"UP_TRIANGLE_24",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 4) { drawManager.icon(center,"DOWN_TRIANGLE_24",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 5) { drawManager.icon(center,"HEXAGON_24",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 6) { drawManager.icon(center,"CIRCLE_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 7) { drawManager.icon(center,"DIAMOND_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 8) { drawManager.icon(center,"SQUARE_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 9) { drawManager.icon(center,"UP_TRIANGLE_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 10) { drawManager.icon(center,"DOWN_TRIANGLE_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 11) { drawManager.icon(center,"RIGHT_TRIANGLE_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 12) { drawManager.icon(center,"HEXAGON_16",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 13) { drawManager.icon(center,"RESIZE",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 14) { drawManager.icon(center,"OPTIONS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 15) { drawManager.icon(center,"POINT_LIGHT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 16) { drawManager.icon(center,"AMBIENT_LIGHT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 17) { drawManager.icon(center,"OMNI_EMITTER",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 18) { drawManager.icon(center,"VOLUME_LIGHT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 19) { drawManager.icon(center,"AIR_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 20) { drawManager.icon(center,"DRAG_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 21) { drawManager.icon(center,"GRAVITY_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 22) { drawManager.icon(center,"NEWTON_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 23) { drawManager.icon(center,"RADIAL_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 24) { drawManager.icon(center,"TURBULENCE_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 25) { drawManager.icon(center,"UNIFORM_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 26) { drawManager.icon(center,"VORTEX_FIELD",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 27) { drawManager.icon(center,"UNLOCK_MONO",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 28) { drawManager.icon(center,"LOCK_MONO",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 29) { drawManager.icon(center,"NUCLEUS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 30) { drawManager.icon(center,"DOT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 31) { drawManager.icon(center,"CROSS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 32) { drawManager.icon(center,"DRAG_POINT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 33) { drawManager.icon(center,"OFF_RADIO_BTN",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 34) { drawManager.icon(center,"FFD_POINT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 35) { drawManager.icon(center,"CURVE_ENDS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 36) { drawManager.icon(center,"DRAG_PT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 37) { drawManager.icon(center,"PIVOT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 38) { drawManager.icon(center,"HOLLOW_BOX",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 39) { drawManager.icon(center,"ROTATE_PIVOT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 40) { drawManager.icon(center,"SELECT_HANDLE_ROOT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 41) { drawManager.icon(center,"SOLID_BOX",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 42) { drawManager.icon(center,"HOLLOW_TRIANGLE",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 43) { drawManager.icon(center,"SCALE_PIVOT",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 44) { drawManager.icon(center,"SELECT_HANDLE",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 45) { drawManager.icon(center,"U_CV",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 46) { drawManager.icon(center,"V_CV",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 47) { drawManager.icon(center,"X_AXIS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 48) { drawManager.icon(center,"Y_AXIS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 49) { drawManager.icon(center,"Z_AXIS",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 50) { drawManager.icon(center,"IK",1.0); }
			if ( pLocatorData->m_draw_twod_IconType == 51) { drawManager.icon(center,"FK",1.0); }

			// drawManager.icon(center,"SCALE_PIVOT",1.0);

#else

			drawManager.setPointSize(5.0);
			drawManager.point(center);

#endif






		}

		// Draw A-B line
		if (pLocatorData->m_drawPresets == 10)
		{

			// Draw fill
			drawManager.setColor( lineCol );

			drawManager.setLineWidth(pLocatorData->m_lineWidth);
			drawManager.line(pLocatorData->m_inLocA_pos, pLocatorData->m_inLocB_pos);

		}

		// Draw text
		if (pLocatorData->m_dispText)
		{
			drawManager.setColor( lineCol );
			drawManager.setFontSize(pLocatorData->m_textFontSize);
			drawManager.setFontIncline(pLocatorData->m_textIncline);
			drawManager.setFontWeight(pLocatorData->m_textWeight);
			drawManager.setFontStretch(pLocatorData->m_textStretch);
			drawManager.setFontLine(pLocatorData->m_textLine);

			MString faceName = BaseLocData::m_fFontList[pLocatorData->m_fontFaceIndex];
			drawManager.setFontName(faceName);

			int boxSize[] = { pLocatorData->m_textBoxWidth, pLocatorData->m_textBoxHeight };
			drawManager.text(pLocatorData->m_textPosition, pLocatorData->m_text, pLocatorData->m_textAlignment, boxSize[0]+boxSize[1] == 0 ? NULL : boxSize, &pLocatorData->m_textBoxColor, false);

		}


		if (pLocatorData->m_dispNum)
		{
			drawManager.setColor( lineCol );
			drawManager.text(MPoint(0.0,0.0,0.0), MString()+pLocatorData->m_locID ,MHWRender::MUIDrawManager::kCenter);
		}


		// Draw icon for selection center
		if (pLocatorData->m_dispLocPivot)
		{
			drawManager.setColor( lineCol );
			//drawManager.icon(MPoint(0.0,0.0,0.0),"SCALE_PIVOT",1.0);
#if MAYA_API_VERSION > 201600

			drawManager.icon(center,"SCALE_PIVOT",1.0);

#else

			drawManager.setPointSize(5.0);
			drawManager.point(center);

#endif
		}



	}

#if MAYA_API_VERSION > 201600
	if (pLocatorData->m_drawOnTop)
	{
		drawManager.endDrawInXray();
	}
	if (!pLocatorData->m_drawOnTop)
	{
		drawManager.endDrawable();
	}
#else

	drawManager.endDrawable();

#endif



}



MStatus BaseLoc::initialize()
{
	MStatus					status;


	MFnMatrixAttribute		mAttr;
	MFnNumericAttribute     nAttr;
	MFnTypedAttribute       tAttr;
	MFnGenericAttribute     gAttr;
	MFnEnumAttribute        eAttr;
	MFnStringData           fnStringData;
	MFnUnitAttribute		uAttr;

	// ------------------------------

	aDrawPresets = eAttr.create( "presets", "presets", 0);
	eAttr.setStorable(true);
	eAttr.addField("Circle", 0);
	eAttr.addField("Box", 1);
	eAttr.addField("Sphere", 2);
	eAttr.addField("Cone", 3);
	eAttr.addField("Rectangle", 4);
	eAttr.addField("Drag handle", 5);
	eAttr.addField("Icon", 6);
	eAttr.addField("Gyroscope", 7);
	eAttr.addField("Camera", 8);
	eAttr.addField("2D Icons", 9);
	eAttr.addField("A-B", 10);
	eAttr.addField("File", 11);

	eAttr.setDefault(6);

	//eAttr.setInternal(true);
	addAttribute( aDrawPresets );

	aDrawIconsTypes = eAttr.create( "iconType", "iconType", 0);
	eAttr.setStorable(true);
	eAttr.addField("Cross", 0);
	eAttr.addField("Box Closed", 1);
	eAttr.addField("Tag", 2);
	eAttr.addField("Arrow Up", 3);
	eAttr.addField("Horizontal Border", 4);
	eAttr.addField("Camera", 5);
	eAttr.addField("Close", 6);
	eAttr.addField("Eye", 7);
	eAttr.addField("Flash", 8);
	eAttr.addField("T-Shirt", 9);
	eAttr.addField("Lock", 10);
	eAttr.addField("Group", 11);
	eAttr.addField("Clock", 12);
	eAttr.addField("Home", 13);
	eAttr.addField("Male", 14);
	eAttr.addField("Female", 15);
	eAttr.addField("Lightbulb", 16);
	eAttr.addField("Rotate CC", 17);
	eAttr.addField("Pine Tree", 18);
	eAttr.addField("Hazard", 19);
	eAttr.addField("Move", 20);
	eAttr.addField("Corners", 21);
	eAttr.addField("Direction", 22);
	eAttr.addField("Man Walk", 23);
	eAttr.addField("Feet", 24);
	eAttr.addField("Muzzle Flash", 25);
	eAttr.addField("Orient", 26);
	//eAttr.addField("Arrow quad", 2);
	//eAttr.addField("Arrow curve", 3);
	//eAttr.setInternal(true);
	eAttr.setDefault(13);
	addAttribute( aDrawIconsTypes );

	// 2D icon types

	aTwoDIconsTypes = eAttr.create( "2DIconType", "2DIconType", 0);
	eAttr.setStorable(true);
	eAttr.addField("Circle 24", 0);
	eAttr.addField("Diamond 24", 1);
	eAttr.addField("Square 24", 2);
	eAttr.addField("Up Triangle 24", 3);
	eAttr.addField("Down Triangle 24", 4);
	eAttr.addField("Hexagon 24", 5);
	eAttr.addField("Circle 16", 6);
	eAttr.addField("Diamond 16", 7);
	eAttr.addField("Square 16", 8);
	eAttr.addField("Up Triangle 16", 9);
	eAttr.addField("Down Triangle 16", 10);
	eAttr.addField("Right Triangle 16", 11);
	eAttr.addField("Hexagon 16", 12);
	eAttr.addField("Resize", 13);
	eAttr.addField("Options", 14);
	eAttr.addField("Point Light", 15);
	eAttr.addField("Ambient Light", 16);
	eAttr.addField("Omni Emitter", 17);
	eAttr.addField("Volume Light", 18);
	eAttr.addField("Air Field", 19);
	eAttr.addField("Drag Field", 20);
	eAttr.addField("Gravity Field", 21);
	eAttr.addField("Newton Field", 22);
	eAttr.addField("Radial Field", 23);
	eAttr.addField("Turbulence Field", 24);
	eAttr.addField("Uniform Field", 25);
	eAttr.addField("Vortex Field", 26);
	eAttr.addField("Unlock Mono", 27);
	eAttr.addField("Lock Mono", 28);
	eAttr.addField("Nucleus", 29);
	eAttr.addField("Dot", 30);
	eAttr.addField("Cross", 31);
	eAttr.addField("Drag Point", 32);
	eAttr.addField("Off Radio Btn", 33);
	eAttr.addField("FFFD Point", 34);
	eAttr.addField("Curve Ends", 35);
	eAttr.addField("Drag Pt", 36);
	eAttr.addField("Pivot", 37);
	eAttr.addField("Hollow Box", 38);
	eAttr.addField("Rotate Pivot", 39);
	eAttr.addField("Select Handle Root", 40);
	eAttr.addField("Solid Box", 41);
	eAttr.addField("Hollow Triangle", 42);
	eAttr.addField("Scale Pivot", 43);
	eAttr.addField("Select Handle", 44);
	eAttr.addField("U CV", 45);
	eAttr.addField("V CV", 46);
	eAttr.addField("X Axis", 47);
	eAttr.addField("Y Axis", 48);
	eAttr.addField("Z Axis", 49);
	eAttr.addField("IK", 50);
	eAttr.addField("FK", 51);
	eAttr.setDefault(43);
	addAttribute( aTwoDIconsTypes );


	// ---------------------------------------------------------------------------------------------------
	// Size


	aDivision = nAttr.create( "division", "division", MFnNumericData::kInt );
	nAttr.setStorable(true);
	nAttr.setDefault(30);
	nAttr.setMin(2);
	nAttr.setMax(30);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDivision );

	aPointSize = nAttr.create( "pointSize", "pointSize", MFnNumericData::kInt );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault(5);
	nAttr.setMin(1);
	nAttr.setMax(10);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aPointSize );

	aLocID = nAttr.create( "locID", "locID", MFnNumericData::kInt );
	nAttr.setDefault( 1 );
	nAttr.setMin(1);
	nAttr.setSoftMax(10);
	addAttribute( aLocID );

	aRadius = nAttr.create( "radius", "radius", MFnNumericData::kDouble );
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.1);
	nAttr.setSoftMax(5.0);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aRadius );

	// Offset

	aOffsetX = nAttr.create("offsetX", "offsetX", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aOffsetX);

	aOffsetY = nAttr.create("offsetY", "offsetY", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aOffsetY);

	aOffsetZ = nAttr.create("offsetZ", "offsetZ", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aOffsetZ);

	// Rotate
	aRotateX = nAttr.create("rotateX", "rotateX", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	nAttr.setSoftMax(180.0);
	nAttr.setSoftMin(-180.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aRotateX);

	aRotateY = nAttr.create("rotateY", "rotateY", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	nAttr.setSoftMax(180.0);
	nAttr.setSoftMin(-180.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aRotateY);

	aRotateZ = nAttr.create("rotateZ", "rotateZ", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	nAttr.setSoftMax(180.0);
	nAttr.setSoftMin(-180.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aRotateZ);

	aScaleX = nAttr.create("scaleX", "scaleX", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aScaleX);

	aScaleY = nAttr.create("scaleY", "scaleY", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aScaleY);

	aScaleZ = nAttr.create("scaleZ", "scaleZ", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setChannelBox(true);
	addAttribute(aScaleZ);

	aFadeDistance = nAttr.create( "fadeDistance", "fadeDistance", MFnNumericData::kDouble );
	nAttr.setStorable(true);
	nAttr.setDefault(10.0);
	nAttr.setSoftMax(10.0);
	nAttr.setMin(0.0);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aFadeDistance );

	aLineWidth = nAttr.create( "lineWidth", "lineWidth", MFnNumericData::kFloat );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.0);
	nAttr.setMax(3.0);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aLineWidth );



	// ---------------------------------------------------------------------------------------------------
	// Color

	aLineColor = nAttr.createColor( "lineColor", "lineColor" );
	nAttr.setStorable(true);
	nAttr.setDefault( 0.0, 0.5, 1.0 );
	nAttr.setUsedAsColor( true );
	nAttr.setInternal(true);
	addAttribute( aLineColor );

	aLineAlpha = nAttr.create( "lineAlpha", "lineAlpha", MFnNumericData::kFloat );
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aLineAlpha );

	aPolygonColor = nAttr.createColor( "polygonColor", "polygonColor" );
	nAttr.setStorable(true);
	nAttr.setDefault( 0.0, 0.1, 0.3 );
	nAttr.setUsedAsColor( true );
	nAttr.setInternal(true);
	addAttribute( aPolygonColor );

	aPolygonAlpha = nAttr.create( "polygonAlpha", "polygonAlpha", MFnNumericData::kFloat );
	nAttr.setStorable(true);
	nAttr.setDefault(0.1);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aPolygonAlpha );


	aTextBoxTransparency = nAttr.create("textBoxTransparency", "textBoxTransparency", MFnNumericData::kFloat, 1.0);
	nAttr.setMin(0.0);
	nAttr.setDefault(1.0);
	nAttr.setMax(1.0);
	addAttribute(aTextBoxTransparency);


	// ---------------------------------------------------------------------------------------------------
	// Switches

	aDispNum = nAttr.create( "displayLocatorId", "displayLocatorId", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault(false);
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDispNum );

	aDispCard = nAttr.create( "displayCards", "displayCards", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault( false );
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDispCard );

	aDrawOnTop = nAttr.create( "drawOnTop", "drawOnTop", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault( false );
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDrawOnTop );

	aDispLoc = nAttr.create( "displayLocator", "displayLocator", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault( true );
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDispLoc );

	aDispLocPivot = nAttr.create( "displayLocatorPivot", "displayLocatorPivot", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault( false );
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDispLocPivot );

	aDispText = nAttr.create( "displayText", "displayText", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault( false );
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aDispText );

	aFadeByDistance = nAttr.create( "fadeByDistance", "fadeByDistance", MFnNumericData::kBoolean );
	nAttr.setStorable(true);
	nAttr.setReadable(false);
	nAttr.setDefault( true );
	nAttr.setKeyable( true );
	nAttr.setChannelBox( true );
	addAttribute( aFadeByDistance );


	// ---------------------------------------------------------------------------------------------------
	// Draw style

	aLineStyle = eAttr.create( "lineStyle", "lineStyle", 4);
	eAttr.setStorable(true);
	eAttr.addField("Solid", 0);
	eAttr.addField("ShortDotted", 1);
	eAttr.addField("ShortDashed", 2);
	eAttr.addField("Dashed", 3);
	eAttr.addField("Dotted", 4);
	eAttr.setDefault(0);
	//eAttr.setInternal(true);
	addAttribute( aLineStyle );

	aPaintStyle = eAttr.create( "paintStyle", "paintStyle", 0);
	eAttr.setStorable(true);
	eAttr.addField("Flat", 0);
	eAttr.addField("Strippled", 1);
	eAttr.setDefault(0);
	//eAttr.setInternal(true);
	addAttribute( aPaintStyle );

	// ---------------------------------------------------------------------------------------------------
	// Font

	aTextPosition = nAttr.create("textPosition", "textPosition", MFnNumericData::k3Double);
	nAttr.setDefault(0.0, 0.0, 0.0);
	addAttribute(aTextPosition);

	// Add text attributes.
	MFnStringData stringFn;
	MObject defaultText = stringFn.create("Main Controller");
	aText = tAttr.create("text", "text", MFnData::kString, defaultText);
	addAttribute(aText);

	aTextFontSize = nAttr.create("textFontSize", "textFontSize", MFnNumericData::kInt, MHWRender::MUIDrawManager::kDefaultFontSize);
	nAttr.setMin(-1);
	nAttr.setDefault(-1);  
	nAttr.setMax(99);    
	addAttribute(aTextFontSize);

	unsigned int nFont = MHWRender::MUIDrawManager::getFontList(BaseLocData::m_fFontList);
	if (nFont == 0)
	{
		perror("No font available!");
	}
	aFontFaceName = eAttr.create("fontFaceName", "fontFaceName", 0);
	for (unsigned int i = 0; i < nFont; i++)
	{
		MString str = BaseLocData::m_fFontList[i];
		eAttr.addField(str, (short)i);
	}
	addAttribute(aFontFaceName);

	aTextAlignment = eAttr.create("textAlignment", "textAlignment", MHWRender::MUIDrawManager::kLeft);
	eAttr.addField("left", MHWRender::MUIDrawManager::kLeft);
	eAttr.addField("center", MHWRender::MUIDrawManager::kCenter);
	eAttr.addField("right", MHWRender::MUIDrawManager::kRight);
	eAttr.setDefault(MHWRender::MUIDrawManager::kCenter);
	addAttribute(aTextAlignment);

	aTextIncline = eAttr.create("textIncline", "textIncline", MHWRender::MUIDrawManager::kInclineNormal);
	eAttr.addField("normal", MHWRender::MUIDrawManager::kInclineNormal);
	eAttr.addField("italic", MHWRender::MUIDrawManager::kInclineItalic);
	addAttribute(aTextIncline);    

	aTextWeight = eAttr.create("textWeight", "textWeight", MHWRender::MUIDrawManager::kWeightBold);
	eAttr.addField("light", MHWRender::MUIDrawManager::kWeightLight);
	eAttr.addField("normal", MHWRender::MUIDrawManager::kWeightNormal);
	eAttr.addField("demiBold", MHWRender::MUIDrawManager::kWeightDemiBold);
	eAttr.addField("bold", MHWRender::MUIDrawManager::kWeightBold);
	eAttr.addField("black", MHWRender::MUIDrawManager::kWeightBlack);
	eAttr.setDefault(MHWRender::MUIDrawManager::kWeightNormal);
	addAttribute(aTextWeight);

	aTextStretch = nAttr.create("textStretch", "textStretch", MFnNumericData::kInt, MHWRender::MUIDrawManager::kStretchUnstretched);
	nAttr.setMin(50);
	nAttr.setDefault(100);
	nAttr.setMax(200);
	addAttribute(aTextStretch);

	aTextLine = eAttr.create("textLine", "textLine", 0);
	eAttr.addField("none", 0);
	eAttr.addField("overline", MHWRender::MUIDrawManager::kLineOverline);
	eAttr.addField("underline", MHWRender::MUIDrawManager::kLineUnderline);
	eAttr.addField("strikeout", MHWRender::MUIDrawManager::kLineStrikeoutLine);
	addAttribute(aTextLine);  

	aTextBoxSize = nAttr.create("textBoxSize", "textBoxSize", MFnNumericData::k2Int);
	nAttr.setDefault(0, 0);
	nAttr.setDefault(100,20);
	addAttribute(aTextBoxSize);

	aTextBoxColor = nAttr.create("textBoxColor", "textBoxColor", MFnNumericData::k3Float);
	nAttr.setDefault(0.0f, 0.0f, 0.0f);
	nAttr.setUsedAsColor(true);
	addAttribute(aTextBoxColor);


	// ---------------------------------------------------------------------------------------------------
	// VP 1.0


	aInLocPosA = mAttr.create( "locatorPosA", "locatorPosA", MFnMatrixAttribute::kDouble );
	mAttr.setChannelBox(false);
	mAttr.setWritable(true);
	mAttr.setReadable(false);
	mAttr.setStorable(false);
	mAttr.setKeyable(false);
	addAttribute( aInLocPosA );

	aInLocPosB = mAttr.create( "locatorPosB", "locatorPosB", MFnMatrixAttribute::kDouble );
	mAttr.setChannelBox(false);
	mAttr.setWritable(true);
	mAttr.setReadable(false);
	mAttr.setStorable(false);
	mAttr.setKeyable(false);
	addAttribute( aInLocPosB );

	// Add text attributes.
	MObject defaultText_path = stringFn.create( "Unknown path" );
	aPresetFolderPath = tAttr.create("presetFolderPath", "presetFolderPath", MFnData::kString, defaultText_path);
	tAttr.setKeyable(false);
	tAttr.setChannelBox(false);
	//tAttr.setUsedAsFilename(true);
	addAttribute(aPresetFolderPath);

	aInPointArray = tAttr.create( "inPointArray", "inPointArray", MFnPointArrayData::kPointArray );
	tAttr.setStorable(true);
	tAttr.setInternal(true);
	addAttribute( aInPointArray );

	aInTriangleArray = tAttr.create( "inTriangleArray", "inTriangleArray", MFnPointArrayData::kPointArray );
	tAttr.setStorable(true);
	tAttr.setInternal(true);
	addAttribute( aInTriangleArray );

	aBoundingBoxA = nAttr.create( "boundingBoxA", "boundingBoxA", MFnNumericData::k3Float );
	nAttr.setStorable(true);
	nAttr.setInternal(true);
	addAttribute( aBoundingBoxA );

	aBoundingBoxB = nAttr.create( "boundingBoxB", "boundingBoxB", MFnNumericData::k3Float );
	nAttr.setStorable(true);
	nAttr.setInternal(true);
	addAttribute( aBoundingBoxB );


	return MS::kSuccess;
}