GetDB	Macro   oper,reg,var
		oper	reg,0FFh
		org		$-1
	var	db		0FFh
			EndM

GetDBPub Macro   oper,reg,var
		oper	reg,0FFh
		org		$-1
	var	db		0FFh
  public var
			EndM

GetDW	Macro	oper,reg,var
		oper	reg,0FACEBEDAh
		org		$-4
	var	dd		0FACEBEDAh
			EndM

GetDWPub	Macro	oper,reg,var
		oper	reg,0FACEBEDAh
		org		$-4
	var	dd		0FACEBEDAh
	public	var
			EndM

UseDW	Macro	oper,arg1,arg2,var
		oper	arg1,arg2
		org		$-4
	var	dd		0FACEBEDAh
			EndM
