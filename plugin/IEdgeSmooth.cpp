#include "IEdgeSmooth.h"

static IEdgeSmoothMacroEmitter macroEmitter;

//The 3dsMax2013 SDK replaces 'end' with 'p_end'.
#ifdef MAX2013_FP
#define FP_END p_end
#else
#define FP_END end
#endif


//================================================
//Function Publishing declarations
//================================================
IEdgeSmooth edgesmooth_fn (
	EDGESMOOTH_FN_INTERFACE,
	_M("iedgesmooth"),
	IDS_CLASS_NAME,
	GetEdgeSmoothDesc(),

	FP_STATIC_METHODS,
		IEdgeSmooth::es_isSoft, _M("IsSoft"), 0, TYPE_BOOL, 0, 2,
			_M("node"), 0, TYPE_INODE,
			_M("edges"), 0, TYPE_BITARRAY,
		IEdgeSmooth::es_isHard, _M("IsHard"), 0, TYPE_BOOL, 0, 2,
			_M("node"), 0, TYPE_INODE,
			_M("edges"), 0, TYPE_BITARRAY,
		IEdgeSmooth::es_canApplyToSel, _M("CanApplyToSel"), 0, TYPE_BOOL, 0, 0,
		IEdgeSmooth::es_apply, _M("Apply"), 0, TYPE_VOID, 0, 3,
			_M("soften"), 0, TYPE_BOOL,
			_M("node"), 0, TYPE_INODE, f_keyArgDefault, NULL, 
			_M("edges"), 0, TYPE_BITARRAY, f_keyArgDefault, NULL,
	FP_END
);

IEdgeSmoothActions edgesmooth_actions (
	EDGESMOOTH_ACT_INTERFACE,
	_M("edgesmooth_actions"),
	IDS_CLASS_NAME,
	GetEdgeSmoothDesc(),

	FP_ACTIONS, 
	kActionMainUIContext,
		IEdgeSmoothActions::es_applyHard, _M("MakeHard"), IDS_MAKE_HARD, 0, 
			f_isEnabled, IEdgeSmoothActions::es_isEnabled,
			f_menuText, IDS_MAKE_HARD,
			f_toolTip, IDS_MAKE_HARD_TT,
			f_buttonText, IDS_MAKE_HARD,
			f_icon, _M("edgesmooth"), 1,
			f_macroEmitter, &macroEmitter,
			//f_shortCut, FNOINVERT | FSHIFT, 72, //Shortcut does not seem to work when trying to use it...
		FP_END,	
		IEdgeSmoothActions::es_applySoft, _M("MakeSoft"), IDS_MAKE_SOFT, 0,
			f_isEnabled, IEdgeSmoothActions::es_isEnabled,
			f_menuText, IDS_MAKE_SOFT,
			f_toolTip, IDS_MAKE_SOFT_TT, 
			f_buttonText, IDS_MAKE_SOFT,
			f_icon, _M("edgesmooth"), 2,
			f_macroEmitter, &macroEmitter,
			//f_shortCut, FNOINVERT | FSHIFT, 83, //Shortcut does not seem to work when trying to use it...
		FP_END,
	FP_END
);


//================================================
//Function Publishing implementations
//================================================

void IEdgeSmooth::init() { }

void IEdgeSmooth::Apply(bool soften, INode* node, BitArray* edges)
{
	if (node == NULL && edges == NULL)
		EdgeSmooth::Apply(soften);
	else
		EdgeSmooth::Apply(soften, node, edges);
}


void IEdgeSmoothActions::init() { }

void _showErrorMsgBox(MSTR msg)
{
	MessageBox(GetCOREInterface()->GetMAXHWnd(), msg, _M("EdgeSmooth error"), MB_ICONERROR | MB_OK);
}

FPStatus IEdgeSmoothActions::ApplySoft()
{
	try
	{
		EdgeSmooth::Apply(true);
		return FPS_OK;
	}
	catch (MAXException e)
	{
		_showErrorMsgBox(e.message);
		return FPS_FAIL;
	}
}

FPStatus IEdgeSmoothActions::ApplyHard()
{
	 try {
		EdgeSmooth::Apply(false); 
		return FPS_OK;
	} catch (MAXException e) {
		_showErrorMsgBox(e.message);
		return FPS_FAIL;
	}
}



//================================================
// Macro Emitter implementation
//================================================

void IEdgeSmoothMacroEmitter::EmitMacro (FPInterface *fpi, FPFunctionDef *fd)
{
	MacroRecorder* mr = GetCOREInterface()->GetMacroRecorder();
	if (mr->Enabled())
	{
		switch (fd->ID)
		{
			case IEdgeSmoothActions::es_applySoft:
				mr->FunctionCall(_M("EdgeSmooth.Apply"), 1, 0, mr_bool, true);
				break;
			case IEdgeSmoothActions::es_applyHard:
				mr->FunctionCall(_M("EdgeSmooth.Apply"), 1, 0, mr_bool, false);
				break;
			default:
				return;
		}

		mr->EmitScript();
	}
}
