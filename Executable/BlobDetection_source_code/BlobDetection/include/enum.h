/*
 * enum.h
 *
 *  Created on: May 30, 2016
 *
 *      Author: pratik
*/


#ifndef ENUM_H_
#define ENUM_H_

enum return_type {
	Success = 0,
	TError,
	FileWriteError,
	OutOfBoundError,
	MemoryOverFlow,
	MemoryNotAllocated

};

enum pixel_type {
	_PBG = 0,
	_PFG,
	_PMARKER,

	_PMAX = 255
};

enum page_type {

	_Normal = 0,
	_UpSideDown,
	_LeftRotated,
	_RightRotated,

	_Horizontal,
	_Vertical
};


enum component_type {
	NotClassified = 0,
	SmallNoise,

	Image,
	ProbableImage,
	ProbableImageNoise,

	TextCharacter,
	TextProbableCharacter,

	TextLine,
	TextLineHorizontal,
	TextLineVertical,

	Word,
	Page,

	LineSmallNoise,

	HorizontalLine,
	HorizontalLineSmallNoise,

	VerticalLine,
	VerticalLineSmallNoise,
};

enum binaryzation {
	NormalOtsu = 0,
	LowerOtsu,
	UpperOtsu,
};

enum style {
	Bold = 0,
	Italic,
	BoldItalic,
	Regular
};

enum alignment {
	LeftAligned = 0,
	RightAligned,
	CenterAligned,
	Justify
};


#endif
