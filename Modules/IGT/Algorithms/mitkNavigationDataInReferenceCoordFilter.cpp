/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "mitkNavigationDataInReferenceCoordFilter.h"



//matrix convert
#include "mitkAffineTransform3D.h"
#include "mitkMatrixConvert.h"
#include "vtkQuaternion.h"

mitk::NavigationDataInReferenceCoordFilter::NavigationDataInReferenceCoordFilter() : mitk::NavigationDataToNavigationDataFilter()
{

}


mitk::NavigationDataInReferenceCoordFilter::~NavigationDataInReferenceCoordFilter()
{
}

void mitk::NavigationDataInReferenceCoordFilter::GenerateData()
{
  DataObjectPointerArraySizeType numberOfInputs = this->GetNumberOfInputs();

  if (numberOfInputs == 0)
    return;

  this->CreateOutputsForAllInputs();


  vtkSmartPointer<vtkMatrix4x4> refMatrix = vtkMatrix4x4::New();

  if (m_RefToolIndex < this->GetNumberOfInputs())
  {


    // get reference tool matrix according to RefToolIndex
     const mitk::NavigationData *nd_ref = this->GetInput(m_RefToolIndex);
     refMatrix = NavigationDataToVtkMatrix4x4(nd_ref);
  }
 
  

  // generate output
  for (unsigned int i = 0; i < numberOfInputs; ++i)
  {
    const mitk::NavigationData *nd = this->GetInput(i);
    assert(nd);

    mitk::NavigationData *output = this->GetOutput(i);
    assert(output);

    //cal matrix in ref
    vtkSmartPointer<vtkMatrix4x4> in_matrix = NavigationDataToVtkMatrix4x4(nd);
    vtkSmartPointer<vtkMatrix4x4> res_matrix = vtkMatrix4x4::New();
    GetReferenceMatrix4x4(in_matrix, refMatrix, res_matrix);
    // test ref itself should be i
    if (i == m_RefToolIndex && !res_matrix->IsIdentity())
    {
      MITK_ERROR << "ref itself not Identity";

    }

    output->Graft(nd); // copy all information from input to output
    output->SetPosition(vtkMatrix4x4ToPosition(res_matrix));
    output->SetOrientation(vtkMatrix4x4ToQuaternion(res_matrix));
    output->SetDataValid(nd->IsDataValid());
  }
}

mitk::AffineTransform3D::Pointer mitk::NavigationDataInReferenceCoordFilter::NavigationDataToTransform(
  const mitk::NavigationData *nd)
{
  mitk::AffineTransform3D::Pointer affineTransform = mitk::AffineTransform3D::New();
  affineTransform->SetIdentity();

  // calculate the transform from the quaternions
  static itk::QuaternionRigidTransform<double>::Pointer quatTransform =
    itk::QuaternionRigidTransform<double>::New();

  mitk::NavigationData::OrientationType orientation = nd->GetOrientation();
  // convert mitk::ScalarType quaternion to double quaternion because of itk bug
  vnl_quaternion<double> doubleQuaternion(orientation.x(), orientation.y(), orientation.z(), orientation.r());
  quatTransform->SetIdentity();
  quatTransform->SetRotation(doubleQuaternion);
  quatTransform->Modified();

  /* because of an itk bug, the transform can not be calculated with float data type.
      To use it in the mitk geometry classes, it has to be transfered to mitk::ScalarType which is float */
  static AffineTransform3D::MatrixType m;
  mitk::TransferMatrix(quatTransform->GetMatrix(), m);
  affineTransform->SetMatrix(m);

  /*set the offset by convert from itkPoint to itkVector and setting offset of transform*/
  mitk::Vector3D pos;
  pos.SetVnlVector(nd->GetPosition().GetVnlVector());
  affineTransform->SetOffset(pos);

  affineTransform->Modified();
  return affineTransform;
}

vtkMatrix4x4* mitk::NavigationDataInReferenceCoordFilter::NavigationDataToVtkMatrix4x4(const mitk::NavigationData *nd)
{
  mitk::AffineTransform3D::Pointer affineTransform = nd->GetAffineTransform3D();
  vtkMatrix4x4* vtkmatrix4x4 = vtkMatrix4x4::New();
  TransferItkTransformToVtkMatrix(affineTransform.GetPointer(), vtkmatrix4x4);
  return vtkmatrix4x4;
}

// mitk::NavigationData::Pointer mitk::NavigationDataInReferenceCoordFilter::vtkMatrix4x4ToNavigationData(
//   vtkMatrix4x4 *matrix4x4)
// {
//
// }

mitk::Point3D mitk::NavigationDataInReferenceCoordFilter::vtkMatrix4x4ToPosition(vtkMatrix4x4 *matrix4x4)
{
  mitk::Point3D p;
  FillVector3D(p, matrix4x4->GetElement(0, 3), matrix4x4->GetElement(1, 3), matrix4x4->GetElement(2, 3));
  return p;
}

mitk::NavigationData::OrientationType mitk::NavigationDataInReferenceCoordFilter::vtkMatrix4x4ToQuaternion(
  vtkMatrix4x4 *matrix4x4)
{
  
  double quat[4];
  double matrix3x3[3][3];
  matrix3x3[0][0] = matrix4x4->GetElement(0, 0);matrix3x3[0][1] = matrix4x4->GetElement(0, 1);matrix3x3[0][2] = matrix4x4->GetElement(0, 2);
  matrix3x3[1][0] = matrix4x4->GetElement(1, 0);matrix3x3[1][1] = matrix4x4->GetElement(1, 1);matrix3x3[1][2] = matrix4x4->GetElement(1, 2);
  matrix3x3[2][0] = matrix4x4->GetElement(2, 0);matrix3x3[2][1] = matrix4x4->GetElement(2, 1);matrix3x3[2][2] = matrix4x4->GetElement(2, 2);
  vtkMatrix3x3ToQuaternion(matrix3x3, quat);
  return mitk::NavigationData::OrientationType{quat[1], quat[2], quat[3], quat[0]};
}

void mitk::NavigationDataInReferenceCoordFilter::vtkMatrix4x4ToMatrix3x3(const vtkMatrix4x4 *matrix4x4,
                                                                         double matrix3x3[3][3])
{
  matrix3x3[0][0] = matrix4x4->GetElement(0, 0);
  matrix3x3[0][1] = matrix4x4->GetElement(0, 1);
  matrix3x3[0][2] = matrix4x4->GetElement(0, 2);
  matrix3x3[1][0] = matrix4x4->GetElement(1, 0);
  matrix3x3[1][1] = matrix4x4->GetElement(1, 1);
  matrix3x3[1][2] = matrix4x4->GetElement(1, 2);
  matrix3x3[2][0] = matrix4x4->GetElement(2, 0);
  matrix3x3[2][1] = matrix4x4->GetElement(2, 1);
  matrix3x3[2][2] = matrix4x4->GetElement(2, 2);
}


void mitk::NavigationDataInReferenceCoordFilter::ReinitFilter()
{

  // // clear this class source and target points
  // m_SourceLandmarksFromNavigationDatas->Clear();
  // m_TargetLandmarksFromNavigationDatas->Clear();
  //
  // //clear superclass source and target points
  // m_TargetPoints.clear();
  // m_SourcePoints.clear();
  //
  // this->Modified();
}



