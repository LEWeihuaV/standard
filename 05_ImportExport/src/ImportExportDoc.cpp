// ImportExportDoc.cpp : implementation of the CImportExportDoc class
//


#include "stdafx.h"
#include "ImportExportApp.h"

#include "ImportExportDoc.h"

#include <ImportExport/ImportExport.h>

#include <AISDialogs.h>
#include "res/resource.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <ChFi2d_ChamferAPI.hxx>

#include <BSplCLib.hxx>
#include <BSplCLib_EvaluatorFunction.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ChFi2d_FilletAPI.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <gp_Pnt.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomConvert.hxx>



#ifdef _DEBUG
//#define new DEBUG_NEW  // by cascade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc

IMPLEMENT_DYNCREATE(CImportExportDoc, OCC_3dDoc)

BEGIN_MESSAGE_MAP(CImportExportDoc, OCC_3dDoc)
	//{{AFX_MSG_MAP(CImportExportDoc)
	ON_COMMAND(ID_FILE_IMPORT_BREP, OnFileImportBrep)
	ON_COMMAND(ID_FILE_IMPORT_IGES, OnFileImportIges)
	ON_COMMAND(ID_FILE_EXPORT_IGES, OnFileExportIges)
	ON_COMMAND(ID_FILE_IMPORT_STEP, OnFileImportStep)
	ON_COMMAND(ID_FILE_EXPORT_STEP, OnFileExportStep)
	ON_COMMAND(ID_FILE_EXPORT_VRML, OnFileExportVrml)
	ON_COMMAND(ID_FILE_EXPORT_STL, OnFileExportStl)
	ON_COMMAND(ID_BOX, OnBox)
	ON_COMMAND(ID_Cylinder, OnCylinder)
	ON_COMMAND(ID_OBJECT_REMOVE, OnObjectRemove)
	ON_COMMAND(ID_OBJECT_ERASE, OnObjectErase)
	ON_COMMAND(ID_OBJECT_DISPLAYALL, OnObjectDisplayall)
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_CAD_CHFI2D, &CImportExportDoc::OnCadChfi2d)
	ON_COMMAND(ID_BSPLCLIB_BSPLCLIBINSTANCE, &CImportExportDoc::OnBsplclibBsplclibinstance)
	ON_COMMAND(ID_CAD_FILLETWIRE, &CImportExportDoc::OnCadFilletwire)
        ON_COMMAND(ID_CAD_CREATEFACE, &CImportExportDoc::OnCadCreateface)
        ON_COMMAND(ID_CAD_NURBS, &CImportExportDoc::OnCadNurbs)
        END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc construction/destruction

CImportExportDoc::CImportExportDoc()
	: OCC_3dDoc(false)
{
	/*
		// TRIHEDRON
		Handle(AIS_Trihedron) aTrihedron;
		Handle(Geom_Axis2Placement) aTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
		aTrihedron=new AIS_Trihedron(aTrihedronAxis);
		myAISContext->Display(aTrihedron);
	*/

	m_pcoloredshapeList = new CColoredShapes();
}

CImportExportDoc::~CImportExportDoc()
{
	if (m_pcoloredshapeList) delete m_pcoloredshapeList;
}


/////////////////////////////////////////////////////////////////////////////
// CSerializeDoc serialization

void CImportExportDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Put the current CColoredShape in the archive
		ar << m_pcoloredshapeList;
	}
	else
	{
		// Read from the archive the current CColoredShape
		ar >> m_pcoloredshapeList;

		// Display the new object
		m_pcoloredshapeList->Display(myAISContext);
	}
}


/*
void CImportExportDoc::OnWindowNew3d()
{
	((CImportExportApp*)AfxGetApp())->CreateView3D(this);
}
*/

//  nCmdShow could be :    ( default is SW_RESTORE ) 
// SW_HIDE   SW_SHOWNORMAL   SW_NORMAL   
// SW_SHOWMINIMIZED     SW_SHOWMAXIMIZED    
// SW_MAXIMIZE          SW_SHOWNOACTIVATE   
// SW_SHOW              SW_MINIMIZE         
// SW_SHOWMINNOACTIVE   SW_SHOWNA           
// SW_RESTORE           SW_SHOWDEFAULT      
// SW_MAX    

// use pViewClass = RUNTIME_CLASS( CImportExportView3D ) for 3D Views

void CImportExportDoc::ActivateFrame(CRuntimeClass* pViewClass, int nCmdShow)
{
	POSITION position = GetFirstViewPosition();
	while (position != (POSITION)NULL)
	{
		CView* pCurrentView = (CView*)GetNextView(position);
		if (pCurrentView->IsKindOf(pViewClass))
		{
			ASSERT_VALID(pCurrentView);
			CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
			ASSERT(pParentFrm != (CFrameWnd*)NULL);
			// simply make the frame window visible
			pParentFrm->ActivateFrame(nCmdShow);
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc diagnostics

#ifdef _DEBUG
void CImportExportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImportExportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc commands


void CImportExportDoc::OnFileImportBrep()
{
	Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadBREP();
	for (int i = 1; i <= aSeqOfShape->Length(); i++)
	{
		m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
		m_pcoloredshapeList->Display(myAISContext);
	}
	Fit();
}

void CImportExportDoc::OnFileImportIges()
{
	Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadIGES();
	for (int i = 1; i <= aSeqOfShape->Length(); i++)
	{
		m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
		m_pcoloredshapeList->Display(myAISContext);
	}
	Fit();
}
void CImportExportDoc::OnFileExportIges()
{
	CImportExport::SaveIGES(myAISContext);
}

void CImportExportDoc::OnFileImportStep()
{
	Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadSTEP();
	for (int i = 1; i <= aSeqOfShape->Length(); i++)
	{
		m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
		m_pcoloredshapeList->Display(myAISContext);
	}
	Fit();
}
void CImportExportDoc::OnFileExportStep()
{
	CImportExport::SaveSTEP(myAISContext);
}


void CImportExportDoc::OnFileExportVrml()
{
	CImportExport::SaveVRML(myAISContext);
}

void CImportExportDoc::OnFileExportStl()
{
	CImportExport::SaveSTL(myAISContext);
}

void  CImportExportDoc::Popup(const Standard_Integer  x,
	const Standard_Integer  y,
	const Handle(V3d_View)& aView)
{
	Standard_Integer PopupMenuNumber = 0;
	myAISContext->InitSelected();
	if (myAISContext->MoreSelected())
		PopupMenuNumber = 1;

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_Popup3D));
	CMenu* pPopup = menu.GetSubMenu(PopupMenuNumber);

	ASSERT(pPopup != NULL);
	if (PopupMenuNumber == 1) // more than 1 object.
	{
		bool OneOrMoreInShading = false;
		for (myAISContext->InitSelected(); myAISContext->MoreSelected(); myAISContext->NextSelected())
			if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(), 1)) OneOrMoreInShading = true;
		if (!OneOrMoreInShading)
			pPopup->EnableMenuItem(5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	}

	POINT winCoord = { x , y };
	Handle(WNT_Window) aWNTWindow =
		Handle(WNT_Window)::DownCast(aView->Window());
	ClientToScreen((HWND)(aWNTWindow->HWindow()), &winCoord);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, winCoord.x, winCoord.y,
		AfxGetMainWnd());


}

void CImportExportDoc::OnBox()
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next()) {
		myAISContext->Remove(aListIterator.Value(), Standard_False);
	}

	BRepPrimAPI_MakeBox B(200., 150., 100.);

	m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, B.Shape());

	m_pcoloredshapeList->Display(myAISContext);
	Fit();

	// document has been modified
	SetModifiedFlag(TRUE);
}

void CImportExportDoc::OnCylinder()
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next()) {
		myAISContext->Remove(aListIterator.Value(), Standard_False);
	}

	BRepPrimAPI_MakeCylinder C(50., 200.);

	m_pcoloredshapeList->Add(Quantity_NOC_GREEN, C.Shape());

	m_pcoloredshapeList->Display(myAISContext);
	Fit();

	// document has been modified
	SetModifiedFlag(TRUE);
}
void CImportExportDoc::OnObjectRemove()

{
	for (GetAISContext()->InitSelected(); GetAISContext()->MoreSelected(); GetAISContext()->NextSelected()) {
		Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(GetAISContext()->SelectedInteractive());
		if (!aShape.IsNull()) {
			m_pcoloredshapeList->Remove(aShape->Shape());
		}
	}
	OCC_3dBaseDoc::OnObjectRemove();
}

void CImportExportDoc::OnObjectErase()

{
	for (GetAISContext()->InitSelected(); GetAISContext()->MoreSelected(); GetAISContext()->NextSelected()) {
		Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(GetAISContext()->SelectedInteractive());
		if (!aShape.IsNull()) {
			m_pcoloredshapeList->Remove(aShape->Shape());
		}
	}
	OCC_3dBaseDoc::OnObjectErase();
}

void CImportExportDoc::OnObjectDisplayall()

{
	OCC_3dBaseDoc::OnObjectDisplayall();
}

void CImportExportDoc::OnCadChfi2d()
{
	// TODO: 在此添加命令处理程序代码
	// TODO: 在此添加命令处理程序代码
	gp_Pnt p1(0.0, 0.0, 0.0);
	gp_Pnt p2(10.0, 0.0, 0.0);
	gp_Pnt p3(10.0, 0.0, 0.0);
	gp_Pnt p4(10.0, 10.0, 0.0);

	TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
	TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(p3, p4);

	ChFi2d_ChamferAPI chamferAPI(edge1, edge2);

	if (chamferAPI.Perform()) {
		TopoDS_Edge chamferedEdge = chamferAPI.Result(edge1, edge2, 2.0, 2.0);

		// Clear the displayed objects
		AIS_ListOfInteractive aList;
		myAISContext->DisplayedObjects(aList);
		AIS_ListIteratorOfListOfInteractive aListIterator;
		for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next()) {
			myAISContext->Remove(aListIterator.Value(), Standard_False);
		}

		// Create AIS_Shape objects for each edge with different colors
		Handle(AIS_Shape) aisEdge1 = new AIS_Shape(edge1);
		aisEdge1->SetColor(Quantity_NOC_RED); // Set color for the first edge

		Handle(AIS_Shape) aisEdge2 = new AIS_Shape(edge2);
		aisEdge2->SetColor(Quantity_NOC_BLUE1); // Set color for the second edge

		Handle(AIS_Shape) aisChamferedShape = new AIS_Shape(chamferedEdge);
		aisChamferedShape->SetColor(Quantity_NOC_GREEN);

		myAISContext->Display(aisEdge1, Standard_True);
		myAISContext->Display(aisEdge2, Standard_True);
		myAISContext->Display(aisChamferedShape, Standard_True);

		Fit();
		SetModifiedFlag(TRUE);
		std::cout << "Chamfer operation succeeded and shape displayed." << std::endl;
	}
	else {
		std::cerr << "Chamfer operation failed." << std::endl;
	}
}


void CImportExportDoc::OnBsplclibBsplclibinstance()
{
	// TODO: 在此添加命令处理程序代码
	// 定义点
	gp_Pnt p1(0.0, 0.0, 0.0);   // 直线的起点
	gp_Pnt p2(10.0, 0.0, 0.0);  // 直线的终点，曲线的起点
	gp_Pnt p3(15.0, 5.0, 0.0);  // 曲线的控制点
	gp_Pnt p4(20.0, 0.0, 0.0);  // 曲线的终点

	// 创建直线段
	TopoDS_Edge lineEdge = BRepBuilderAPI_MakeEdge(p1, p2);

	// 创建一条简单的 Bezier 曲线段，连接点 p2 和 p4
	TColgp_Array1OfPnt curvePoints(1, 3);
	curvePoints(1) = p2; // 直线的终点，曲线的起点
	curvePoints(2) = p3; // 控制点
	curvePoints(3) = p4; // 曲线的终点

	Handle(Geom_BezierCurve) bezierCurve = new Geom_BezierCurve(curvePoints);
	TopoDS_Edge curveEdge = BRepBuilderAPI_MakeEdge(bezierCurve);

	// 将直线段和曲线段组合成多段线 (Wire)
	BRepBuilderAPI_MakeWire wireBuilder;
	wireBuilder.Add(lineEdge);
	wireBuilder.Add(curveEdge);

	// 获取最终的 Wire 对象
	TopoDS_Wire wire = wireBuilder.Wire();
	displayShape(wire, Quantity_NOC_GREEN);
}


void CImportExportDoc::displayShape(const TopoDS_Shape& shape, const Quantity_Color& color)
{
	// 清除已显示的对象
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next()) {
		myAISContext->Remove(aListIterator.Value(), Standard_False);
	}

	// 创建 AIS_Shape 对象并设置颜色
	Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
	aisShape->SetColor(color);

	// 显示形状
	myAISContext->Display(aisShape, Standard_True);

	// 调整视图以适应形状
	Fit();
	SetModifiedFlag(TRUE);

	std::cout << "Shape displayed successfully." << std::endl;
}

void CImportExportDoc::displayShapes(const std::vector<TopoDS_Shape>& shapes, const std::vector<Quantity_Color>& colors)
{
	if (shapes.size() != colors.size()) {
		std::cerr << "The number of shapes and colors must be the same." << std::endl;
		return;
	}

	// 循环遍历所有的形状
	for (size_t i = 0; i < shapes.size(); ++i) {
		// 创建 AIS_Shape 对象并设置颜色
		Handle(AIS_Shape) aisShape = new AIS_Shape(shapes[i]);
		aisShape->SetColor(colors[i]);

		// 显示形状
		myAISContext->Display(aisShape, Standard_True);
	}

	// 调整视图以适应所有形状
	Fit();
	SetModifiedFlag(TRUE);

	std::cout << "All shapes displayed successfully." << std::endl;
}


void CImportExportDoc::OnCadFilletwire()
{
	// TODO: 在此添加命令处理程序代码
	// 创建两个顶点
	//gp_Pnt p1(0, 0, 0);
	//gp_Pnt p2(10, 0, 0);
	//gp_Pnt p3(10, 10, 0);

	//TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(p1);
	//TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(p2);
	//TopoDS_Vertex V3 = BRepBuilderAPI_MakeVertex(p3);

	//// 创建一条边
	//TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(V1, V2);
	//TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(V2, V3);

	//// 创建一条线段构成的闭合线圈
	//TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge1, edge2);

	//// 创建一个面
	//TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

	//// 创建2D倒角对象并初始化
	//BRepFilletAPI_MakeFillet2d fillet2d(face);

	//// 添加倒角，倒角半径为 1.0
	//TopoDS_Edge filletEdge = fillet2d.AddFillet(V2, 1.0);

	//// 现在，修改倒角的半径为 2.0
	//TopoDS_Edge modifiedFilletEdge = fillet2d.ModifyFillet(filletEdge, 2.0);

	//// 生成修改后的结果形状
	//TopoDS_Shape result = fillet2d.Shape();

	// 创建边
	gp_Pnt p1(0.0, 0.0, 0.0);
	gp_Pnt p2(1.0, 0.0, 0.0);
	gp_Pnt p3(1.0, 1.0, 0.0);
	TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
	TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(p2, p3);
	// 创建一个平面
	gp_Pln plane(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
	// 使用 ChFi2d_FilletAPI 创建圆角
	ChFi2d_FilletAPI filletAPI(edge1, edge2, plane);
	// 初始化算法
	filletAPI.Init(edge1, edge2, plane);
	// 设置圆角半径
	Standard_Real radius = 0.2;
	// 执行圆角操作
	if (filletAPI.Perform(radius)) {
		std::cout << "Fillet created successfully." << std::endl;
		// 获取第一个结果
		gp_Pnt commonPoint = p2; // 交点
		TopoDS_Edge resultEdge = filletAPI.Result(commonPoint, edge1, edge2);
		// 在这里可以对 resultEdge 进行进一步处理，比如绘制或存储
		std::cout << "Resulting fillet edge created." << std::endl;
		//std::vector<TopoDS_Shape> shapes = { shape1, shape2, shape3 };
		//std::vector<Quantity_Color> colors = { Quantity_NOC_RED, Quantity_NOC_GREEN, Quantity_NOC_BLUE };
		//displayShapes(shapes, colors);
		std::vector<TopoDS_Shape> shapes = { resultEdge, edge1, edge2 };
		std::vector<Quantity_Color> colors = { Quantity_NOC_RED, Quantity_NOC_GREEN, Quantity_NOC_BLUE };
		displayShapes(shapes, colors);
	}
}

void CImportExportDoc::OnCadCreateface() {
  // TODO: 在此添加命令处理程序代码
  // 定义边界
  TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));
  TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0));
  TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 1, 0), gp_Pnt(0, 0, 0));

  // 连接为封闭的边界
  TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge1, edge2, edge3, edge4);

  // 使用边界生成面
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  displayShape(face, Quantity_NOC_BLUE);
}

void CImportExportDoc::OnCadNurbs() {

}
