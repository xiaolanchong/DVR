//======================================================================================//
//                                         DVR                                          //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Deployment JScript file for DVRClient applications

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   09.06.2005
    
    Features:	1)Delete comments from wxDesigner (optional)
    			2)Replace the wxDesigner .xrc resource file's XML elements.
    			  Stub "unknown" controls will become toolbars 
    			  & other non-supported controls.
    			  Copy result file to ../BIN/<ConfigDir>/res/DVRClient & ../res/Generated
    			3) add to splitter bar dummy static window (just shut up xrc loader )
   	
   	Warning: 	Works only on Windows
   	
   	*Nix notes:	when DVRClient executable file has been created, get it to the <InstallFolder> and
   				do following actions:
   				1)copy	"res/Generated/DVRClient.xrc" to 
   					   	"<InstallFolder>/res/DVRClient/DVRClient.xrc"
   				2)copy	"res/DVRClient_wdr" dir to
   					   	"<InstallFolder>/res/DVRClient"
   				3)copy	"res/Generated/<LocaleDir>/DVRClientUI.mo" to
   					   	"<InstallFolder>/res/DVRClient/<LocaleDir>/DVRClientUI.mo"
   				  for each locale
   				  current locales: ru
   				4)
   				All names are case sensitive!
*/                                                                                      //
//======================================================================================//

var fso = new ActiveXObject("Scripting.FileSystemObject");
main();

function main()
{
	objArgs = WScript.Arguments;
	if(objArgs.length != 3)
	{
		WScript.Echo("Transform.js <InputXRCFile> <OutputXRCFile> <OutputStringFile>" );
	}

	var srcXRCFileName 		=  	objArgs(0);
	var dstXRCFileName 		=	objArgs(1);
	var dstStringFileName 	= 	objArgs(2);
	Transform( srcXRCFileName, dstXRCFileName, dstStringFileName );
}

function LoadDOM(file)
{
   var dom;
   try {
     dom = MakeDOM(null);
     dom.load(file);
   }
   catch (e) {
     alert(e.description);
   }
   return dom;
}

function MakeDOM(progID)
{
  if (progID == null) {
    progID = "msxml2.DOMDocument.3.0";
  }

  var dom;
  try {
    dom = new ActiveXObject(progID);
    dom.async = false;
    dom.validateOnParse = false;
    dom.resolveExternals = false;
  }
  catch (e) {
    alert(e.description);
  }
  return dom;
}

function alert(str)
{
  WScript.Echo(str);
}

function Transform(srcXRCFileName, dstXRCFileName, dstStringFileName)
{
	var dom = LoadDOM(srcXRCFileName);	
	RemoveComments(dom);
	ReplaceControls(dom);
	AttachPanelsToSplitter(dom)
	SetAdditionalStyleForAllToolbars(dom)
	ExtractStrings( dom, dstStringFileName );
	dom.save(dstXRCFileName);
}

//=========================== XML Transformation ===========================
	
function ReplaceControls( dom ) 
{
	ReplaceNodes(dom, "Stub_ToolBar_ViewMode", "ToolBar_ViewMode" );
//	ReplaceNodes(dom, "Stub_ToolBar_VideoControl", "ToolBar_VideoControl" );
//	ReplaceNodes(dom, "Stub_ToolBar_Video1", "ToolBar_Video1" );
//	ReplaceNodes(dom, "Stub_ToolBar_Archive1", "ToolBar_Archive1" );
	ReplaceNodes(dom, "Stub_ToolBar_Video_Settings", "ToolBar_Video_Settings" );
	ReplaceNodes(dom, "Stub_ToolBar_Archive_VideoControl", "ToolBar_Archive_VideoControl" );
} 

function ReplaceNodes(dom, StubControlName, RealControlName)
{
	var SrcNodeList = dom.selectNodes("//*[@name=\"" + StubControlName+ "\"]");
	var DstNode = dom.selectSingleNode("//*[@name=\"" + RealControlName+ "\"]");
	if( SrcNodeList.length == 1 )
		SrcNodeList.item(0).parentNode.replaceChild(DstNode,SrcNodeList.item(0));
	else
	{
		for(var i = 0; i < SrcNodeList.length; ++i)
		{
			var SrcNodeClone = DstNode.cloneNode(true);
			var par = SrcNodeList.item(i).parentNode;
			par.replaceChild(SrcNodeClone, SrcNodeList.item(i));
		}
		//HACK : Toolbar node has no parent
		dom.documentElement.removeChild(DstNode);
	}	
}

function AttachPanelsToSplitter( dom )
{
	var nodeSplitter 	= dom.selectSingleNode( "//*[@name=\"ID_SPLITTER\"]" );
	var nodeLeftPanel = dom.createElement( "object" );
	nodeLeftPanel.setAttribute( "class", "wxStaticText" );
	nodeLeftPanel.setAttribute( "name", "ID_649827394581_FullRandom") ;
	
	nodeSplitter.appendChild( nodeLeftPanel );
}

function SetAdditionalStyleForAllToolbars( dom )
{
	var ToolbarNodeList = dom.selectNodes("//object[@class=\"wxToolBar\"]");
	for(var i = 0; i < ToolbarNodeList.length; ++i)
	{
		var nodeStyle = dom.createElement("style");
		nodeStyle.text = "wxTB_FLAT|wxTB_NODIVIDER";
		ToolbarNodeList[i].appendChild( nodeStyle );
	}
}

function RemoveComments(dom)
{
	// remove comments from xml document - 2nd & 3rd nodes
	dom.removeChild( dom.childNodes(1) );
	dom.removeChild( dom.childNodes(1) );
}

//=========================Resource extracting ===============================

function ExtractStrings( dom, dstStringFileName )
{
	var sTempFileName = dstStringFileName;
	var f = fso.CreateTextFile(sTempFileName, true, false);
	
	var Dict = new ActiveXObject("Scripting.Dictionary");

	// label 	+static 	 , 
	// tooltip 	+tooltip  , 
	// item		+combobox item 
	// title 	+dialog caption
	nodeList = dom.selectNodes("//(label | tooltip | item | title)");
	for(var i=0; i < nodeList.length; ++i)
	{
		var s = nodeList[i].text;
		//f.WriteLine(s);
		if( !Dict.Exists(s) )
			Dict.add (s, "");
	}
	
	a = (new VBArray(Dict.Keys())).toArray();

	for(i in a)
	{
		var s = a[i];
		f.WriteLine("msgid" + " " + "\"" + s + "\"");	
		f.WriteLine("msgstr" + " " + "\"" + s + "\"");
		f.WriteLine("");
	}
	
   	f.Close();
}


