//
//  AETemplates.h
//  primGen
//
//  Created by Hunyadi János on 2015. 01. 03..
//  Copyright (c) 2015. Janos Hunyadi. All rights reserved.
//

#ifndef MELSCRIPTS_H
#define MELSCRIPTS_H

#include <maya/MString.h>


MString mel_AETemplate()
{
	MString s_aeTemplate = MString() + "global proc AEBaseLocTemplate( string $nodeName )" +
		"{" +
		"	editorTemplate -beginScrollLayout;" +
		"	editorTemplate -beginLayout \"Locator presets\" -collapse 0;" +
		"	editorTemplate -addControl \"presets\";" +
		"	editorTemplate -addControl \"iconType\";" +
		"	editorTemplate -addControl \"2DIconType\";" +
		"	editorTemplate -addControl \"radius\";" +
		"	editorTemplate -addControl \"lineWidth\";" +
		"	editorTemplate -addControl \"lineColor\";" +
		"	editorTemplate -addControl \"lineAlpha\";" +
		"	editorTemplate -addControl \"polygonColor\";" +
		"	editorTemplate -addControl \"polygonAlpha\";" +
		"	editorTemplate -addControl \"displayLocatorId\";" +
		"	editorTemplate -addControl \"displayLocator\";" +
		"	editorTemplate -addControl \"displayLocatorPivot\";" +
		"	editorTemplate -addControl \"displayText\";" +
		"	editorTemplate -addControl \"drawOnTop\";" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -beginLayout \"Transforms\" -collapse 0;" +
		"	editorTemplate -addControl \"offsetX\";" +
		"	editorTemplate -addControl \"offsetY\";" +
		"	editorTemplate -addControl \"offsetZ\";" +
		"	editorTemplate -addControl \"rotateX\";" +
		"	editorTemplate -addControl \"rotateY\";" +
		"	editorTemplate -addControl \"rotateZ\";" +
		"	editorTemplate -addControl \"scaleX\";" +
		"	editorTemplate -addControl \"scaleY\";" +
		"	editorTemplate -addControl \"scaleZ\";" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -beginLayout \"Draw style\" -collapse 0;" +
		"	editorTemplate -addControl \"lineStyle\";" +
		"	editorTemplate -addControl \"paintStyle\";" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -beginLayout \"Text\" -collapse 0;" +
		"	editorTemplate -addControl \"textPosition\";" +
		"	editorTemplate -addControl \"textFontSize\";" +
		"	editorTemplate -addControl \"text\";" +
		"	editorTemplate -addControl \"fontFaceName\";" +
		"	editorTemplate -addControl \"textAlignment\";" +
		"	editorTemplate -addControl \"textIncline\";" +
		"	editorTemplate -addControl \"textWeight\";" +
		"	editorTemplate -addControl \"textStretch\";" +
		"	editorTemplate -addControl \"textLine\";" +
		"	editorTemplate -addControl \"textBoxSize\";" +
		"	editorTemplate -addControl \"textBoxColor\";" +
		"	editorTemplate -addControl \"textBoxTransparency\";" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -beginLayout \"Misc\" -collapse 0;" +
		"	editorTemplate -addControl \"locID\";" +
		"	editorTemplate -endLayout;" +
		"	" +
		"	editorTemplate -beginLayout \"Plug-in Info\" -collapse 0;" +
		"	editorTemplate -addControl \"presetFolderPath\";" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -suppress \"division\";" +
		"	editorTemplate -suppress \"displayCards\";" +
		"	editorTemplate -suppress \"fadeByDistance\";" +
		"	editorTemplate -suppress \"fadeDistance\";" +
		"	editorTemplate -suppress \"locatorPos\";" +
		"	editorTemplate -suppress \"inPointArray\";" +
		"	editorTemplate -suppress \"inTriangleArray\";" +
		"	AEdependNodeTemplate $nodeName;" +
		"	editorTemplate -addExtraControls;" +
		"	editorTemplate -endScrollLayout;" +
		"}";



	return s_aeTemplate;

}

MString mel_createShelf()
{

	MString s_aeTemplate = MString() + "int $cc_doesShelfExist = `shelfLayout -query -ex \"CreativeCase\"`;"
		"if ($cc_doesShelfExist == true)"
		"{"
		"    string $shelfButtons[] = `shelfLayout -q -ca \"CreativeCase\"`;"
		"    int $ex_b01 = 0;"
		"    "
		"    for( $i=0; $i<size($shelfButtons); ++$i )"
		"    {"
		"        if( `control -exists $shelfButtons[$i]` == true)"
		"        {"
		"            if (`control -q -docTag $shelfButtons[$i]` == \"bl_createBlButton\") {$ex_b01 = 1;}"
		"        }"
		"    }"
		"    "
		"    if ($ex_b01 == 0) {shelfButton -p \"CreativeCase\" -dtg \"bl_createBlButton\" -annotation \"Creates a BaseLocator\" -image1 \"BaseLoc.png\" -command \"BaseLocCommand\";}"
		"    "
		"}"
		"	"
		"if ($cc_doesShelfExist == false)"
		"{"
		"		shelfLayout -cellWidth 33 -cellHeight 33 -p $gShelfTopLevel CreativeCase;"
		"		shelfButton -p \"CreativeCase\" -dtg \"bl_createBlButton\" -annotation \"Creates a BaseLocator\" -image1 \"BaseLoc.png\" -command \"BaseLocCommand\";"
		"}";

	return s_aeTemplate;
}

#endif