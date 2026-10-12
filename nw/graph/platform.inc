
BEGIN_CODE_SEG	Macro
IFDEF _WINNT
	_DATA Segment public use32 'DATA'
ELSE
	_TEXT Segment public use32 'CODE'
ENDIF
	EndM

END_CODE_SEG	Macro
IFDEF _WINNT
	_DATA EndS
ELSE
	_TEXT EndS
ENDIF
	EndM

