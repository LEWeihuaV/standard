// ImportExportDoc.cpp : implementation of the CImportExportDoc class
//

#include "ImportExportApp.h"
#include "stdafx.h"

#include "ImportExportDoc.h"

#include <ImportExport/ImportExport.h>

#include "res/resource.h"
#include <AISDialogs.h>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <ChFi2d_ChamferAPI.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <BRepLib.hxx>
#include <BRepMesh_DiscretRoot.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BSplCLib.hxx>
#include <BSplCLib_EvaluatorFunction.hxx>
#include <ChFi2d_FilletAPI.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <XBRepMesh.hxx>
#include <gp_Pnt.hxx>
#include<Geom2d_Ellipse.hxx>
using namespace std;

#ifdef _DEBUG
// #define new DEBUG_NEW  // by cascade
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
ON_COMMAND(ID_BSPLCLIB_BSPLCLIBINSTANCE,
           &CImportExportDoc::OnBsplclibBsplclibinstance)
ON_COMMAND(ID_CAD_FILLETWIRE, &CImportExportDoc::OnCadFilletwire)
ON_COMMAND(ID_OCCT_TMATHVECTOR, &CImportExportDoc::OnOcctTmathvector)
ON_COMMAND(ID_OCCT_TXBREPMESH, &CImportExportDoc::OnOcctTxbrepmesh)
ON_COMMAND(ID_OCCT_TUTORIAL, &CImportExportDoc::OnOcctTutorial)
ON_COMMAND(ID_OCCT_CHFI2D, &CImportExportDoc::OnOcctChfi2d)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc construction/destruction

CImportExportDoc::CImportExportDoc() : OCC_3dDoc(false) {
  /*
          // TRIHEDRON
          Handle(AIS_Trihedron) aTrihedron;
          Handle(Geom_Axis2Placement) aTrihedronAxis=new
     Geom_Axis2Placement(gp::XOY()); aTrihedron=new
     AIS_Trihedron(aTrihedronAxis); myAISContext->Display(aTrihedron);
  */

  m_pcoloredshapeList = new CColoredShapes();
}

CImportExportDoc::~CImportExportDoc() {
  if (m_pcoloredshapeList)
    delete m_pcoloredshapeList;
}

/////////////////////////////////////////////////////////////////////////////
// CSerializeDoc serialization

void CImportExportDoc::Serialize(CArchive &ar) {
  if (ar.IsStoring()) {
    // Put the current CColoredShape in the archive
    ar << m_pcoloredshapeList;
  } else {
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

void CImportExportDoc::ActivateFrame(CRuntimeClass *pViewClass, int nCmdShow) {
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL) {
    CView *pCurrentView = (CView *)GetNextView(position);
    if (pCurrentView->IsKindOf(pViewClass)) {
      ASSERT_VALID(pCurrentView);
      CFrameWnd *pParentFrm = pCurrentView->GetParentFrame();
      ASSERT(pParentFrm != (CFrameWnd *)NULL);
      // simply make the frame window visible
      pParentFrm->ActivateFrame(nCmdShow);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc diagnostics

#ifdef _DEBUG
void CImportExportDoc::AssertValid() const { CDocument::AssertValid(); }

void CImportExportDoc::Dump(CDumpContext &dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc commands

void CImportExportDoc::OnFileImportBrep() {
  Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadBREP();
  for (int i = 1; i <= aSeqOfShape->Length(); i++) {
    m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
    m_pcoloredshapeList->Display(myAISContext);
  }
  Fit();
}

void CImportExportDoc::OnFileImportIges() {
  Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadIGES();
  for (int i = 1; i <= aSeqOfShape->Length(); i++) {
    m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
    m_pcoloredshapeList->Display(myAISContext);
  }
  Fit();
}
void CImportExportDoc::OnFileExportIges() {
  CImportExport::SaveIGES(myAISContext);
}

void CImportExportDoc::OnFileImportStep() {
  Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadSTEP();
  for (int i = 1; i <= aSeqOfShape->Length(); i++) {
    m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
    m_pcoloredshapeList->Display(myAISContext);
  }
  Fit();
}
void CImportExportDoc::OnFileExportStep() {
  CImportExport::SaveSTEP(myAISContext);
}

void CImportExportDoc::OnFileExportVrml() {
  CImportExport::SaveVRML(myAISContext);
}

void CImportExportDoc::OnFileExportStl() {
  CImportExport::SaveSTL(myAISContext);
}

void CImportExportDoc::Popup(const Standard_Integer x, const Standard_Integer y,
                             const Handle(V3d_View) & aView) {
  Standard_Integer PopupMenuNumber = 0;
  myAISContext->InitSelected();
  if (myAISContext->MoreSelected())
    PopupMenuNumber = 1;

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu *pPopup = menu.GetSubMenu(PopupMenuNumber);

  ASSERT(pPopup != NULL);
  if (PopupMenuNumber == 1) // more than 1 object.
  {
    bool OneOrMoreInShading = false;
    for (myAISContext->InitSelected(); myAISContext->MoreSelected();
         myAISContext->NextSelected())
      if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(), 1))
        OneOrMoreInShading = true;
    if (!OneOrMoreInShading)
      pPopup->EnableMenuItem(5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
  }

  POINT winCoord = {x, y};
  Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast(aView->Window());
  ClientToScreen((HWND)(aWNTWindow->HWindow()), &winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, winCoord.x,
                         winCoord.y, AfxGetMainWnd());
}

void CImportExportDoc::OnBox() {
  AIS_ListOfInteractive aList;
  myAISContext->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive aListIterator;
  for (aListIterator.Initialize(aList); aListIterator.More();
       aListIterator.Next()) {
    myAISContext->Remove(aListIterator.Value(), Standard_False);
  }

  BRepPrimAPI_MakeBox B(200., 150., 100.);

  m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, B.Shape());

  m_pcoloredshapeList->Display(myAISContext);
  Fit();

  // document has been modified
  SetModifiedFlag(TRUE);
}

void CImportExportDoc::OnCylinder() {
  AIS_ListOfInteractive aList;
  myAISContext->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive aListIterator;
  for (aListIterator.Initialize(aList); aListIterator.More();
       aListIterator.Next()) {
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
  for (GetAISContext()->InitSelected(); GetAISContext()->MoreSelected();
       GetAISContext()->NextSelected()) {
    Handle(AIS_Shape) aShape =
        Handle(AIS_Shape)::DownCast(GetAISContext()->SelectedInteractive());
    if (!aShape.IsNull()) {
      m_pcoloredshapeList->Remove(aShape->Shape());
    }
  }
  OCC_3dBaseDoc::OnObjectRemove();
}

void CImportExportDoc::OnObjectErase()

{
  for (GetAISContext()->InitSelected(); GetAISContext()->MoreSelected();
       GetAISContext()->NextSelected()) {
    Handle(AIS_Shape) aShape =
        Handle(AIS_Shape)::DownCast(GetAISContext()->SelectedInteractive());
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

void CImportExportDoc::OnCadChfi2d() {
  // TODO: �ڴ���������������
  // TODO: �ڴ���������������
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
    for (aListIterator.Initialize(aList); aListIterator.More();
         aListIterator.Next()) {
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
    std::cout << "Chamfer operation succeeded and shape displayed."
              << std::endl;
  } else {
    std::cerr << "Chamfer operation failed." << std::endl;
  }
}

void CImportExportDoc::OnBsplclibBsplclibinstance() {
  // TODO: �ڴ���������������
  // �����
  gp_Pnt p1(0.0, 0.0, 0.0);  // ֱ�ߵ����
  gp_Pnt p2(10.0, 0.0, 0.0); // ֱ�ߵ��յ㣬���ߵ����
  gp_Pnt p3(15.0, 5.0, 0.0); // ���ߵĿ��Ƶ�
  gp_Pnt p4(20.0, 0.0, 0.0); // ���ߵ��յ�

  // ����ֱ�߶�
  TopoDS_Edge lineEdge = BRepBuilderAPI_MakeEdge(p1, p2);

  // ����һ���򵥵� Bezier ���߶Σ����ӵ� p2 �� p4
  TColgp_Array1OfPnt curvePoints(1, 3);
  curvePoints(1) = p2; // ֱ�ߵ��յ㣬���ߵ����
  curvePoints(2) = p3; // ���Ƶ�
  curvePoints(3) = p4; // ���ߵ��յ�

  Handle(Geom_BezierCurve) bezierCurve = new Geom_BezierCurve(curvePoints);
  TopoDS_Edge curveEdge = BRepBuilderAPI_MakeEdge(bezierCurve);

  // ��ֱ�߶κ����߶���ϳɶ���� (Wire)
  BRepBuilderAPI_MakeWire wireBuilder;
  wireBuilder.Add(lineEdge);
  wireBuilder.Add(curveEdge);

  // ��ȡ���յ� Wire ����
  TopoDS_Wire wire = wireBuilder.Wire();
  displayShape(wire, Quantity_NOC_GREEN);
}

void CImportExportDoc::displayShape(const TopoDS_Shape &shape,
                                    const Quantity_Color &color) {
  // �������ʾ�Ķ���
  AIS_ListOfInteractive aList;
  myAISContext->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive aListIterator;
  for (aListIterator.Initialize(aList); aListIterator.More();
       aListIterator.Next()) {
    myAISContext->Remove(aListIterator.Value(), Standard_False);
  }

  // ���� AIS_Shape ����������ɫ
  Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
  aisShape->SetColor(color);

  // ��ʾ��״
  myAISContext->Display(aisShape, Standard_True);

  // ������ͼ����Ӧ��״
  Fit();
  SetModifiedFlag(TRUE);

  std::cout << "Shape displayed successfully." << std::endl;
}

void CImportExportDoc::displayShapes(
    const std::vector<TopoDS_Shape> &shapes,
    const std::vector<Quantity_Color> &colors) {
  AIS_ListOfInteractive aList;
  myAISContext->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive aListIterator;
  for (aListIterator.Initialize(aList); aListIterator.More();
       aListIterator.Next()) {
    myAISContext->Remove(aListIterator.Value(), Standard_False);
  }

  if (shapes.size() != colors.size()) {
    std::cerr << "The number of shapes and colors must be the same."
              << std::endl;
    return;
  }

  // ѭ���������е���״
  for (size_t i = 0; i < shapes.size(); ++i) {
    // ���� AIS_Shape ����������ɫ
    Handle(AIS_Shape) aisShape = new AIS_Shape(shapes[i]);
    aisShape->SetColor(colors[i]);

    // ��ʾ��״
    myAISContext->Display(aisShape, Standard_True);
  }

  // ������ͼ����Ӧ������״
  Fit();
  SetModifiedFlag(TRUE);

  std::cout << "All shapes displayed successfully." << std::endl;
}

void CImportExportDoc::OnCadFilletwire() {
  //// TODO: �ڴ���������������
  //// ������������
  // gp_Pnt p1(0, 0, 0);
  // gp_Pnt p2(10, 0, 0);
  // gp_Pnt p3(10, 10, 0);

  // TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(p1);
  // TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(p2);
  // TopoDS_Vertex V3 = BRepBuilderAPI_MakeVertex(p3);

  //// ����һ����
  // TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(V1, V2);
  // TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(V2, V3);

  //// ����һ���߶ι��ɵıպ���Ȧ
  // TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge1, edge2);

  //// ����һ����
  // TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

  //// ����2D���Ƕ��󲢳�ʼ��
  // BRepFilletAPI_MakeFillet2d fillet2d(face);

  //// ��ӵ��ǣ����ǰ뾶Ϊ 1.0
  // TopoDS_Edge filletEdge = fillet2d.AddFillet(V2, 1.0);

  //// ���ڣ��޸ĵ��ǵİ뾶Ϊ 2.0
  // TopoDS_Edge modifiedFilletEdge = fillet2d.ModifyFillet(filletEdge, 2.0);

  //// �����޸ĺ�Ľ����״
  // TopoDS_Shape result = fillet2d.Shape();

  //// ��ʾ��״
  // displayShape(result, Quantity_NOC_GREEN);

  // ������
  gp_Pnt p1(0.0, 0.0, 0.0);
  gp_Pnt p2(1.0, 0.0, 0.0);
  gp_Pnt p3(1.0, 1.0, 0.0);
  TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
  TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(p2, p3);
  // ����һ��ƽ��
  gp_Pln plane(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));
  // ʹ�� ChFi2d_FilletAPI ����Բ��
  ChFi2d_FilletAPI filletAPI(edge1, edge2, plane);
  // ��ʼ���㷨
  filletAPI.Init(edge1, edge2, plane);
  // ����Բ�ǰ뾶
  Standard_Real radius = 0.2;
  // ִ��Բ�ǲ���
  if (filletAPI.Perform(radius)) {
    std::cout << "Fillet created successfully." << std::endl;
    // ��ȡ��һ�����
    gp_Pnt commonPoint = p2; // ����
    TopoDS_Edge resultEdge = filletAPI.Result(commonPoint, edge1, edge2);
    // ��������Զ� resultEdge ���н�һ������������ƻ�洢
    std::cout << "Resulting fillet edge created." << std::endl;
    // std::vector<TopoDS_Shape> shapes = { shape1, shape2, shape3 };
    // std::vector<Quantity_Color> colors = { Quantity_NOC_RED,
    // Quantity_NOC_GREEN, Quantity_NOC_BLUE }; displayShapes(shapes, colors);
    std::vector<TopoDS_Shape> shapes = {resultEdge, edge1, edge2};
    std::vector<Quantity_Color> colors = {Quantity_NOC_RED, Quantity_NOC_GREEN,
                                          Quantity_NOC_BLUE};
    displayShapes(shapes, colors);
  }
}

void CImportExportDoc::OnOcctTmathvector() {
  // TODO: �ڴ���������������
  math_Vector vecotrOne(2, 2);
  math_Vector vectorTwo(0, 2);
  math_Vector result = vecotrOne - vectorTwo;
  std::cout << result;
  // �����ת��Ϊ�ַ���
  std::ostringstream oss;
  oss << result; // ������� math_Vector �����ص���������
  CString resultStr = CString(oss.str().c_str());

  // ������Ϣ����ʾ���
  AfxMessageBox(resultStr);
}

void CImportExportDoc::OnOcctTxbrepmesh() {
  // TODO: �ڴ���������������
  // ����һ�������壬�߳�Ϊ1.0
  TopoDS_Shape cube = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0);

  // STEP �ļ�·��
  const std::string stepFilePath = "C:\\Users\\leweihua\\Downloads\\Lidar.STEP";

  // ���� STEP ��ȡ��
  STEPControl_Reader reader;

  // ��ȡ STEP �ļ�
  if (reader.ReadFile(stepFilePath.c_str()) != IFSelect_RetDone) {
    return;
  }

  // ת���� OCCT ��״
  Standard_Integer nbs = reader.NbRootsForTransfer();
  for (Standard_Integer i = 1; i <= nbs; i++) {
    reader.TransferRoot(i);
  }
  TopoDS_Shape shape = reader.OneShape();

  // ������ɢ������
  Standard_Real deflection = 0.01; // �����ƫ��
  Standard_Real angle = 5.0;       // ����ĽǶ�ƫ��

  BRepMesh_DiscretRoot *algo = nullptr;
  // ������ɢ������
  Standard_Integer status = XBRepMesh::Discret(shape, deflection, angle, algo);

  // �����ɢ��״̬
  if (status == 0) {
    string result = "��������ɢ���ɹ���";
    // �����ת��Ϊ�ַ���
    std::ostringstream oss;
    oss << result; // ������� math_Vector �����ص���������
    CString resultStr = CString(oss.str().c_str());

    // ������Ϣ����ʾ���
    AfxMessageBox(resultStr);

    // ������ɢ������������ȡ���ɵ�����
    // algo->GetMesh() �ȷ����������ڻ�ȡ���ɵ�����
    // ��ȡ���ɵ�����
    TopoDS_Shape meshShape = algo->Shape(); // ���������������

    // �����������ɫ�����磬��ɫ��
    Quantity_Color meshColor(0.0, 0.0, 1.0, Quantity_TOC_RGB);

    // ��ʾ����
    displayShape(meshShape, meshColor);
  } else {
    string result = "��ɢ��ʧ�ܣ��������: ";
    // �����ת��Ϊ�ַ���
    std::ostringstream oss;
    oss << result; // ������� math_Vector �����ص���������
    CString resultStr = CString(oss.str().c_str());
  }

  // ����
  delete algo;
}

void CImportExportDoc::OnOcctTutorial() {
  // TODO: �ڴ���������������
  Standard_Real myWidth = 10, myThickness = 20, myHeight = 20;
  gp_Pnt aPnt1(-myWidth / 2, 0, 0);
  gp_Pnt aPnt2(-myWidth / 2, -myThickness / 4, 0);
  gp_Pnt aPnt3(0, -myThickness / 2., 0);
  gp_Pnt aPnt4(myWidth / 2, -myThickness / 4, 0);
  gp_Pnt aPnt5(myWidth / 2, 0, 0);

  // gp_Pnt aPnt1(-myWidth / 2., 0, 0);
  // gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
  // gp_Pnt aPnt3(0, -myThickness / 2., 0);
  // gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
  // gp_Pnt aPnt5(myWidth / 2., 0, 0);

  Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
  Handle(Geom_TrimmedCurve) aArcOfCircle =
      GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
  Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

  TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
  TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);
  TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);

  TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3);

  // gp_Pnt aOrigin(0, 0, 0);
  // gp_Dir xDir(1, 0, 0);
  // gp_Ax1 xAxis(origin, xDir);
  gp_Ax1 xAxis = gp::OX();
  gp_Trsf aTrsf;
  aTrsf.SetMirror(xAxis);
  BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
  TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
  TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

  BRepBuilderAPI_MakeWire myWire;
  myWire.Add(aWire);
  myWire.Add(aMirroredWire);
  TopoDS_Wire myWireProfile = myWire.Wire();

  TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
  gp_Vec aPrismVec(0, 0, myHeight);
  TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

  // Body : Apply Fillets
  BRepFilletAPI_MakeFillet mkFillet(myBody);
  TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
  while (anEdgeExplorer.More()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
    // Add edge to fillet algorithm
    mkFillet.Add(myThickness / 12., anEdge);
    anEdgeExplorer.Next();
  }

  myBody = mkFillet.Shape();

  // Body : Add the Neck
  gp_Pnt neckLocation(0, 0, myHeight);
  gp_Dir neckAxis = gp::DZ();
  gp_Ax2 neckAx2(neckLocation, neckAxis);

  Standard_Real myNeckRadius = myThickness / 8.;
  Standard_Real myNeckHeight = myHeight / 10.;

  BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
  TopoDS_Shape myNeck = MKCylinder.Shape();

  myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

  // Body : Create a Hollowed Solid
  TopoDS_Face faceToRemove;
  Standard_Real zMax = -1;

  for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More();
       aFaceExplorer.Next()) {
    TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
    // Check if <aFace> is the top face of the bottle�s neck
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
    if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
      Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
      gp_Pnt aPnt = aPlane->Location();
      Standard_Real aZ = aPnt.Z();
      if (aZ > zMax) {
        zMax = aZ;
        faceToRemove = aFace;
      }
    }
  }

  TopTools_ListOfShape facesToRemove;
  facesToRemove.Append(faceToRemove);
  BRepOffsetAPI_MakeThickSolid aSolidMaker;
  aSolidMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50,
                                   1.e-3);
  myBody = aSolidMaker.Shape();

  // Threading : Create Surfaces
  Handle(Geom_CylindricalSurface) aCyl1 =
      new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
  Handle(Geom_CylindricalSurface) aCyl2 =
      new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

  // Threading : Define 2D Curves
  gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
  gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
  gp_Ax2d anAx2d(aPnt, aDir);

  Standard_Real aMajor = 2. * M_PI;
  Standard_Real aMinor = myNeckHeight / 10;

  Handle(Geom2d_Ellipse) anEllipse1 =
      new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
  Handle(Geom2d_Ellipse) anEllipse2 =
      new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
  Handle(Geom2d_TrimmedCurve) anArc1 =
      new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
  Handle(Geom2d_TrimmedCurve) anArc2 =
      new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
  gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
  gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

  Handle(Geom2d_TrimmedCurve) aSegment =
      GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
  // Threading : Build Edges and Wires
  TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
  TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
  TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
  TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
  TopoDS_Wire threadingWire1 =
      BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
  TopoDS_Wire threadingWire2 =
      BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
  BRepLib::BuildCurves3d(threadingWire1);
  BRepLib::BuildCurves3d(threadingWire2);

  // Create Threading
  BRepOffsetAPI_ThruSections aTool(Standard_True);
  aTool.AddWire(threadingWire1);
  aTool.AddWire(threadingWire2);
  aTool.CheckCompatibility(Standard_False);

  TopoDS_Shape myThreading = aTool.Shape();

  // Building the Resulting Compound
  TopoDS_Compound aRes;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aRes);
  aBuilder.Add(aRes, myBody);
  aBuilder.Add(aRes, myThreading);

  std::vector<TopoDS_Shape> shapes = {threadingWire1, threadingWire2};
  std::vector<Quantity_Color> colors = {Quantity_NOC_RED, Quantity_NOC_BLUE1};
  displayShapes(shapes, colors);
  //displayShape(aRes, Quantity_NOC_RED);
}

void CImportExportDoc::OnOcctChfi2d() {
  // TODO: �ڴ���������������
  // �������ε��ĸ�����
  gp_Pnt p1(0.0, 0.0, 0.0);
  gp_Pnt p2(1.0, 0.0, 0.0);
  gp_Pnt p3(1.0, 1.0, 0.0);
  gp_Pnt p4(0.0, 1.0, 0.0);

  // �������ε�������
  TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
  TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(p2, p3);
  TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(p3, p4);
  TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(p4, p1);

  // ����������ϳ�һ�����ο�
  TopoDS_Wire rectangle = BRepBuilderAPI_MakeWire(edge1, edge2, edge3, edge4);

  // ����Բ�ǵ�ƽ��
  gp_Pln plane(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)));

  // ��������ʼ�� ChFi2d_FilletAPI
  ChFi2d_FilletAPI filletAPI(edge1, edge2, plane);
  filletAPI.Init(edge1, edge2, plane);

  // ����Բ�ǰ뾶
  Standard_Real radius = 0.2;

  // ִ��Բ�ǲ���
  if (filletAPI.Perform(radius)) {
    std::cout << "Fillet created successfully." << std::endl;

    // ��ȡ��һ�����
    gp_Pnt commonPoint = p2; // ����
    TopoDS_Edge resultEdge = filletAPI.Result(commonPoint, edge1, edge2);

    // �������ʾ
    std::vector<TopoDS_Shape> shapes = {rectangle, resultEdge, edge1,
                                        edge2,     edge3,      edge4};
    std::vector<Quantity_Color> colors = {
        Quantity_NOC_BLUE1, Quantity_NOC_RED,   Quantity_NOC_GREEN,
        Quantity_NOC_GREEN, Quantity_NOC_GREEN, Quantity_NOC_GREEN};
    displayShapes(shapes, colors);
  }
}
